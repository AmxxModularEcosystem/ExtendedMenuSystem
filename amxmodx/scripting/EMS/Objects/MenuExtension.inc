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

Trie:MenuExtension_ReadParams(const T_MenuExtension:extension, const JSON:paramsJson) {
    if (paramsJson == Invalid_JSON) {
        abort(AMX_ERR_PARAMS, "paramsJson is an invalid json handler (%d).", paramsJson);
        return Invalid_Trie;
    }

    new extensionObject[S_MenuExtension];
    MenuExtension_Get(extension, extensionObject);

    new Trie:params = TrieCreate();
    if (extensionObject[MenuExtension_Params] != Invalid_Array) {
        new E_ParamsReadErrorType:errType = ParamsReadError_None;
        new errParamName[PARAM_KEY_MAX_LEN];

        ParamsController_Param_ReadList(
            extensionObject[MenuExtension_Params], paramsJson, params,
            .iErrType = errType,
            .sErrParamName = errParamName,
            .iErrParamNameLen = charsmax(errParamName)
        );

        switch (errType) {
            case ParamsReadError_RequiredParamNotPresented:
                abort(AMX_ERR_PARAMS, "Extension '%s' parameter '%s' is required but not presented.", extensionObject[MenuExtension_Key], errParamName);
            case ParamsReadError_ParamValueIsInvalid:
                abort(AMX_ERR_PARAMS, "Extension '%s' parameter '%s' has invalid value.", extensionObject[MenuExtension_Key], errParamName);
        }
    }

    return params;
}

T_MenuExtension:MenuExtension_Find(const extensionKey[], const bool:orFail = false) {
    if (!ArrayMapKeyExists(MenuExtensions, extensionKey)) {
        if (orFail) {
            abort(AMX_ERR_PARAMS, "Extension '%s' not found.", extensionKey);
        }

        return Invalid_MenuExtension;
    }

    return T_MenuExtension:ArrayMapGetIndex(MenuExtensions, extensionKey);
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
        ExecuteForward(extensionObject[MenuExtension_OnCreate], _, menu, params);
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
