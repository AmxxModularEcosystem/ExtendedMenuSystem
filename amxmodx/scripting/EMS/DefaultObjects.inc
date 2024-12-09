#if defined _ems_src__default_objects_included
    #endinput
#endif
#define _ems_src__default_objects_included

#include <amxmodx>
#include <ems>
#include "EMS/Objects/MenuController"

static T_MenuController:DefaultControllerHandler = Invalid_MenuController;

DefaultObjects_Register() {
    if (DefaultControllerHandler == Invalid_MenuController) {
        DefaultControllerHandler = EMS_MenuController_Register(EMS_DEFAULT_CONTROLLER_KEY, "@DefaultObjects_MenuController_Default");
        EMS_MenuController_AddParams(DefaultControllerHandler,
            "Items", EMS_MENU_ITEMS_PARAM_TYPE, true
        );
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