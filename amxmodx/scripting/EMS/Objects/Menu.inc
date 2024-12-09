#if defined _ems_src__objects_menu_included
    #endinput
#endif
#define _ems_src__objects_menu_included

#include <amxmodx>
#include <json>
#include <ems>
#include <regex>
#include "EMS/Objects/MenuControllerInstance"
#include "EMS/Objects/MenuExtensionInstance"
#include "EMS/Objects/MenuController"
#include "EMS/Objects/MenuExtension"

enum T_Menu { Invalid_Menu = INVALID_HANDLE }

enum _:S_Menu {
    Menu_Title[EMS_MENU_TITLE_MAX_LEN],
    Menu_Command[EMS_MENU_COMMAND_MAX_LEN],

    Menu_ControllerInstance[S_MenuControllerInstance],
    Array:Menu_ExtensionInstances,
}

static Array:Menus = Invalid_Array;
static Trie:MenuCommands = Invalid_Trie;

Menu_Init() {
    Menus = ArrayCreate(S_Menu, 1);
    MenuCommands = TrieCreate();

    MenuController_Init();
    MenuExtension_Init();
}

Menu_Get(const T_Menu:menu, const menuObject[S_Menu]) {
    ArrayGetArray(Menus, menu, menuObject);
}

Menu_Build(const playerIndex, const T_Menu:menu) {
    new menuObject[S_Menu];
    Menu_Get(menu, menuObject);

    new title[EMS_MENU_TITLE_MAX_LEN];
    Menu_CallMakeTitle(menuObject, title);

    new menuHandler = menu_create(title, "TODO");
    Menu_CallCreate(menuObject, menu);

    MenuControllerInstance_Use(menuObject[Menu_ControllerInstance], playerIndex, menuHandler);

    return menuHandler;
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

Menu_LoadFromFolder(const folderPath[]) {
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

Menu_LoadFromFile(const filePath[]) {
    new JSON:menuJson = json_parse(filePath, true, true);
    Menu_LoadFromJson(menuJson);
    json_free(menuJson);

    return menuIndex;
}

Menu_LoadFromJson(const JSON:menuJson) {
    if (!json_is_object(menuJson)) {
        abort(AMX_ERR_PARAMS, "menuJson is not a json object.");
        return;
    }

    new menu[S_Menu];

    json_object_get_string(menuJson, "Command", menu[Menu_Command], charsmax(menu[Menu_Command]));
    if (menu[Menu_Command][0] == EOS) {
        abort(AMX_ERR_PARAMS, "`Command` field in menu object is empty.");
        return;
    }

    JsonObject_ReadMultilineString(menuJson, "Title", menu[Menu_Title], charsmax(menu[Menu_Title]));
    menu[Menu_ExtensionInstances] = JsonObject_GetMenuExtensionInstances(menuJson, "Extensions");

    if (json_object_has_value(menuJson, "Items", JSONArray)) {
        JsonObject_GetDefaultMenuControllerInstance(menuJson, "Items", menu[Menu_ControllerInstance]);
    } else if (json_object_has_value(menuJson, "Controller")) {
        JsonObject_GetMenuControllerInstance(menuJson, "Controller", menu[Menu_ControllerInstance]);
    } else {
        abort(AMX_ERR_PARAMS, "Menu object has no `Items` or `Controller` fields.");
        return;
    }

    new menuIndex = ArrayPushArray(Menus, menu);

    TrieSetCell(MenuCommands, menu[Menu_Command], menuIndex);

    if (json_object_has_value(menuJson, "ChatCommands", JSONArray)) {
        new JSON:chatCommandsJson = json_object_get_value(menuJson, "ChatCommands");

        for (new i = 0, ii = json_array_get_count(chatCommandsJson); i < ii; ++i) {
            new chatCommand[EMS_MENU_COMMAND_MAX_LEN];
            json_array_get_string(chatCommandsJson, i, chatCommand, charsmax(chatCommand));
            TrieSetCell(MenuCommands, chatCommand, menuIndex);
        }

        json_free(chatCommandsJson);
    }

    return menuIndex;
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
