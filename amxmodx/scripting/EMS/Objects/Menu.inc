#if defined _ems_src__objects_menu_included
    #endinput
#endif
#define _ems_src__objects_menu_included

#include <amxmodx>
#include <json>
#include <ems>
#include <regex>
#include <CommandAliases>
#include "EMS/Objects/MenuControllerInstance"
#include "EMS/Objects/MenuExtensionInstance"
#include "EMS/Objects/MenuController"
#include "EMS/Objects/MenuExtension"
#include "EMS/Utils"

enum T_Menu { Invalid_Menu = INVALID_HANDLE }

enum _:S_Menu {
    Menu_Title[EMS_MENU_TITLE_MAX_LEN],
    Menu_Command[EMS_MENU_COMMAND_MAX_LEN],

    Menu_ControllerInstance[S_MenuControllerInstance],
    Array:Menu_ExtensionInstances,
}

static Array:Menus = Invalid_Array;
static Trie:MenuCommands = Invalid_Trie;
static MenuGlobalCommandsHandlerName[64];

Menu_Init(const menuGlobalCommandsHandlerName[]) {
    CommandAliases_Init();

    Menus = ArrayCreate(S_Menu, 1);
    MenuCommands = TrieCreate();
    copy(MenuGlobalCommandsHandlerName, charsmax(MenuGlobalCommandsHandlerName), menuGlobalCommandsHandlerName);

    MenuController_Init();
    MenuExtension_Init();
}

Menu_Get(const T_Menu:menu, menuObject[S_Menu]) {
    ArrayGetArray(Menus, _:menu, menuObject);
}

T_Menu:Menu_FindByCommand(const command[]) {
    new T_Menu:menu = Invalid_Menu;
    TrieGetCell(MenuCommands, command, menu);
    return menu;
}

Menu_Build(const playerIndex, const T_Menu:menu) {
    log_amx("[DBG] Menu_Build(%n, %d)", playerIndex, menu);
    new menuObject[S_Menu];
    Menu_Get(menu, menuObject);

    new title[EMS_MENU_TITLE_MAX_LEN];
    Menu_CallMakeTitle(menuObject, title);

    new menuHandler = menu_create(title, "@Menu_Handle");
    Menu_CallCreate(menuObject, menuHandler);

    // TODO Посмотреть будут ли такие проблемы на версии 1.10.0. Если нет, то переходить на неё.
    new controllerInstance[S_MenuControllerInstance];
    controllerInstance[MenuControllerInstance_MenuController] = menuObject[Menu_ControllerInstance][MenuControllerInstance_MenuController];
    controllerInstance[MenuControllerInstance_Params] = menuObject[Menu_ControllerInstance][MenuControllerInstance_Params];
    MenuControllerInstance_Use(controllerInstance, playerIndex, menuHandler);

    return menuHandler;
}

@Menu_Handle(const playerIndex, const menu, const item) {
    new command[EMS_MENU_COMMAND_MAX_LEN];
    menu_item_getinfo(menu, item, _, command, charsmax(command));

    if (command[0] != EOS) {
        client_cmd(playerIndex, command);
    }

    menu_destroy(menu);
}

Menu_CallMakeTitle(const menuObject[], title[]) {
    if (menuObject[Menu_ExtensionInstances] == Invalid_Array) {
        return;
    }

    for (new i = 0, ii = ArraySize(menuObject[Menu_ExtensionInstances]); i < ii; ++i) {
        new extensionInstance[S_MenuExtensionInstance];
        ArrayGetArray(menuObject[Menu_ExtensionInstances], i, extensionInstance);
        MenuExtensionInstance_CallMakeTitle(extensionInstance, title);
    }
}

Menu_CallCreate(const menuObject[], const menu) {
    if (menuObject[Menu_ExtensionInstances] == Invalid_Array) {
        return;
    }

    for (new i = 0, ii = ArraySize(menuObject[Menu_ExtensionInstances]); i < ii; ++i) {
        new extensionInstance[S_MenuExtensionInstance];
        ArrayGetArray(menuObject[Menu_ExtensionInstances], i, extensionInstance);
        MenuExtensionInstance_CallCreate(extensionInstance, menu);
    }
}

Menu_CallMakeItem(const menuObject[], title[], command[]) {
    if (menuObject[Menu_ExtensionInstances] == Invalid_Array) {
        return;
    }

    for (new i = 0, ii = ArraySize(menuObject[Menu_ExtensionInstances]); i < ii; ++i) {
        new extensionInstance[S_MenuExtensionInstance];
        ArrayGetArray(menuObject[Menu_ExtensionInstances], i, extensionInstance);
        MenuExtensionInstance_CallMakeItem(extensionInstance, title, command);
    }
}

Menu_LoadFromFolder(folderPath[]) {
    new filePath[PLATFORM_MAX_PATH], fileHandler, FileType:fileType;
    fileHandler = open_dir(folderPath, filePath, charsmax(filePath), fileType);
    if (!fileHandler) {
        abort(AMX_ERR_PARAMS, "Can't open folder '%s'.", folderPath);
        return;
    }

    new Regex:jsonFileRegex;
    jsonFileRegex = regex_compile("(.+).json$", .flags = "i");

    do {
        if (filePath[0] == '!' || filePath[0] == '.') {
            continue;
        }

        if (fileType == FileType_Directory) {
            Menu_LoadFromFolder(fmt("%s/%s", folderPath, filePath));
        } else if (fileType == FileType_File) {
            if (regex_match_c(filePath, jsonFileRegex) > 0) {
                regex_substr(jsonFileRegex, 1, filePath, charsmax(filePath));

                Menu_LoadFromFile(fmt("%s/%s.json", folderPath, filePath));
            }
        }
    } while (next_file(fileHandler, filePath, charsmax(filePath), fileType));

    regex_free(jsonFileRegex);
    close_dir(fileHandler);
}

T_Menu:Menu_LoadFromFile(const filePath[]) {
    new JSON:menuJson = json_parse(filePath, true, true);
    new T_Menu:menuIndex = Menu_LoadFromJson(menuJson);
    json_free(menuJson);

    return menuIndex;
}

T_Menu:Menu_LoadFromJson(const JSON:menuJson) {
    if (!json_is_object(menuJson)) {
        abort(AMX_ERR_PARAMS, "menuJson is not a json object.");
        return Invalid_Menu;
    }

    new menu[S_Menu];

    json_object_get_string(menuJson, "Command", menu[Menu_Command], charsmax(menu[Menu_Command]));
    if (menu[Menu_Command][0] == EOS) {
        abort(AMX_ERR_PARAMS, "`Command` field in menu object is empty.");
        return Invalid_Menu;
    }

    JsonObject_ReadMultilineString(menuJson, "Title", menu[Menu_Title], charsmax(menu[Menu_Title]));
    menu[Menu_ExtensionInstances] = JsonObject_GetMenuExtensionInstances(menuJson, "Extensions");

    if (json_object_has_value(menuJson, "Items", JSONArray)) {
        menu[Menu_ControllerInstance] = JsonObject_iGetDefaultMenuControllerInstance(menuJson, "Items");
    } else if (json_object_has_value(menuJson, "Controller")) {
        menu[Menu_ControllerInstance] = JsonObject_iGetMenuControllerInstance(menuJson, "Controller");
    } else {
        abort(AMX_ERR_PARAMS, "Menu object has no `Items` or `Controller` fields.");
        return Invalid_Menu;
    }

    new T_Menu:menuIndex = T_Menu:ArrayPushArray(Menus, menu);

    Menu_AddCommand(menuIndex, menu[Menu_Command]);

    if (json_object_has_value(menuJson, "ChatCommands", JSONArray)) {
        new JSON:chatCommandsJson = json_object_get_value(menuJson, "ChatCommands");

        for (new i = 0, ii = json_array_get_count(chatCommandsJson); i < ii; ++i) {
            new chatCommand[EMS_MENU_COMMAND_MAX_LEN];
            json_array_get_string(chatCommandsJson, i, chatCommand, charsmax(chatCommand));
            
            CommandAliases_Add(chatCommand, menu[Menu_Command]);
        }

        json_free(chatCommandsJson);
    }

    return menuIndex;
}

Menu_AddCommand(const T_Menu:menu, const command[]) {
    TrieSetCell(MenuCommands, command, menu);
    register_clcmd(command, MenuGlobalCommandsHandlerName);
}

Menu_MakeEventHandler(const E_MenuEvent:event, const pluginIndex, const callback[]) {
    new fwd = INVALID_HANDLE;
    switch (event) {
        case MenuEvent_MakeTitle:
            // (title[], const Trie:params)
            fwd = CreateOneForward(pluginIndex, callback, FP_STRING, FP_CELL);
        case MenuEvent_Create:
            // (const menu, const Trie:params)
            fwd = CreateOneForward(pluginIndex, callback, FP_CELL, FP_CELL);
        case MenuEvent_Create:
            // (title[], command[], const Trie:params)
            fwd = CreateOneForward(pluginIndex, callback, FP_STRING, FP_STRING, FP_CELL);
        default: {
            abort(AMX_ERR_PARAMS, "Invalid menu event (%d).", event);
            return;
        }
    }

    if (fwd == INVALID_HANDLE) {
        abort(AMX_ERR_PARAMS, "Failed to create forward.");
        return;
    }

    return fwd;
}
