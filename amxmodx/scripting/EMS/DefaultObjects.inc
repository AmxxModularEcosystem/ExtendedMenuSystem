#if defined _ems_src__default_objects_included
    #endinput
#endif
#define _ems_src__default_objects_included

#include <amxmodx>
#include <ems>
#include "EMS/Objects/MenuController"

static T_MenuController:DefaultControllerHandler = Invalid_MenuController;
static T_MenuController:PlayersControllerHandler = Invalid_MenuController;

DefaultObjects_Register() {
    if (DefaultControllerHandler == Invalid_MenuController) {
        DefaultControllerHandler = EMS_MenuController_Register(EMS_DEFAULT_CONTROLLER_KEY, "@DefaultObjects_MenuController_Default");
        EMS_MenuController_AddParams(DefaultControllerHandler,
            "Items", EMS_MENU_ITEMS_PARAM_TYPE, true
        );
    }

    if (PlayersControllerHandler == Invalid_MenuController) {
        PlayersControllerHandler = EMS_MenuController_Register(EMS_PLAYERS_CONTROLLER_KEY, "@DefaultObjects_MenuController_Players");
        EMS_MenuController_AddParams(PlayersControllerHandler,
            // TODO: title template
            "Title", "String", false,
            "Command", "String", true,
            "ExcludeDead", "Boolean", false
        );
    }
}

@DefaultObjects_MenuController_Players(const playerIndex, const menu, const Trie:params) {
    static titleTemplate[EMS_MENU_TITLE_MAX_LEN], title[EMS_MENU_TITLE_MAX_LEN];
    TrieGetString(params, "Title", titleTemplate, charsmax(titleTemplate));

    if (titleTemplate[0] == EOS) {
        copy(title, charsmax(title), "{i:playerName}");
    }

    static commandTemplate[EMS_MENU_ITEM_COMMAND_MAX_LEN], command[EMS_MENU_ITEM_COMMAND_MAX_LEN];
    TrieGetString(params, "Command", commandTemplate, charsmax(commandTemplate));
    
    log_amx("Title tpl: %s, Command tpl: %s", titleTemplate, commandTemplate);

    
    new bool:excludeDead = false;
    TrieGetCell(params, "ExcludeDead", excludeDead);

    for (new i = 1; i <= MAX_PLAYERS; ++i) {
        if (!is_user_connected(i)) {
            continue;
        }

        if (excludeDead && !is_user_alive(i)) {
            continue;
        }

        copy(title, charsmax(title), titleTemplate);
        copy(command, charsmax(command), commandTemplate);

        static name[32];
        get_user_name(i, name, charsmax(name));
        static index[11];
        num_to_str(i, index, charsmax(index));

        // TODO: мб сделать что-то типа компиляции шаблонов
        replace_all(title, charsmax(title), "{i:playerIndex}", index);
        replace_all(title, charsmax(title), "{i:playerName}", name);

        replace_all(command, charsmax(command), "{i:playerIndex}", index);
        replace_all(command, charsmax(command), "{i:playerName}", name);

        log_amx("Title: %s, Command: %s", title, command);

        // TODO: юзать прослойку с эмитом ивентов
        menu_additem(menu, title, command);
    }
}

T_MenuController:DefaultObjects_GetDefaultController() {
    return DefaultControllerHandler;
}


@DefaultObjects_MenuController_Default(const playerIndex, const menu, const Trie:params) {
    new Array:items = Invalid_Array;
    if (!TrieGetCell(params, "Items", items) || items == Invalid_Array) {
        abort(AMX_ERR_PARAMS, "Failed to get items array.");
        return;
    }

    for (new i = 0, ii = ArraySize(items); i < ii; ++i) {
        new menuItemData[S_Param_MenuItem];        
        ArrayGetArray(items, i, menuItemData);

        // TODO: юзать прослойку с эмитом ивентов
        menu_additem(menu, menuItemData[Param_MenuItem_Title], menuItemData[Param_MenuItem_Command]);
    }
}