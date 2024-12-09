#if defined _ems_src__objects_menu_extension_included
    #endinput
#endif
#define _ems_src__objects_menu_extension_included

#include <amxmodx>
#include <json>
#include <ems>
#include <ParamsController>
#include "EMS/ArrayMap"

enum _:S_MenuExtension {
    MenuExtension_Key[EMS_EXTENSION_KEY_MAX_LEN],
    Array:MenuExtension_Params,

    MenuExtension_OnMakeTitle, // = INVALID_HANDLE
    MenuExtension_OnCreate, // = INVALID_HANDLE
    MenuExtension_OnMakeItem, // = INVALID_HANDLE
}

static ArrayMap(MenuExtensions);

MenuExtension_Init() {
    MenuExtensions = ArrayMapCreate(S_MenuExtension);
}

MenuExtension_Get(const T_MenuExtension:extension, extensionObject[S_MenuExtension]) {
    ArrayMapGetArray(MenuExtensions, extension, extensionObject);
}

T_MenuExtension:MenuExtension_Find(const extensionKey[], const bool:orFail = false) {
    if (!ArrayMapKeyExists(MenuExtensions, extensionKey)) {
        if (orFail) {
            abort(AMX_ERR_PARAMS, "Extension '%s' not found.", extensionKey);
        }

        return Invalid_MenuExtension;
    }

    return ArrayMapGetIndex(MenuExtensions, extensionKey);
}

MenuExtension_CallMakeTitle(const T_MenuExtension:extension, title[], const Trie:params) {
    new extensionObject[S_MenuExtension];
    MenuExtension_Get(extension, extensionObject);

    if (extensionObject[MenuExtension_OnMakeTitle] != INVALID_HANDLE) {
        ExecuteForward(extensionObject[MenuExtension_OnMakeTitle], _, title, params);
    }
}

bool:MenuExtension_CallCreate(const T_MenuExtension:extension, const menu, const Trie:params) {
    new extensionObject[S_MenuExtension];
    MenuExtension_Get(extension, extensionObject);

    if (extensionObject[MenuExtension_OnMakeTitle] != INVALID_HANDLE) {
        ExecuteForward(extensionObject[MenuExtension_OnCreate], ret, menu, params);
    }
}

bool:MenuExtension_CallMakeItem(const T_MenuExtension:extension, title[], command[], const Trie:params) {
    new extensionObject[S_MenuExtension];
    MenuExtension_Get(extension, extensionObject);

    if (extensionObject[MenuExtension_OnMakeTitle] != INVALID_HANDLE) {
        new ret = 0;
        ExecuteForward(extensionObject[MenuExtension_OnMakeItem], ret, title, command, params);

        return !ret;
    }
}

MenuExtension_MenuEventToExtField(const E_MenuEvent:event) {
    switch (event) {
        case MenuEvent_MakeTitle:
            return MenuExtension_OnMakeTitle;
        case MenuEvent_Create:
            return MenuExtension_OnCreate;
        case MenuEvent_MakeItem:
            return MenuExtension_OnMakeItem;
    }
    abort(0, "Invalid menu event (%d).", event);
}
