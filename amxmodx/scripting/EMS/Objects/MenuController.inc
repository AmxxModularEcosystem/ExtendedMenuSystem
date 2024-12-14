#if defined _ems_src__objects_menu_controller_included
    #endinput
#endif
#define _ems_src__objects_menu_controller_included

#include <amxmodx>
#include <json>
#include <ems>
#include <ParamsController>
#include "EMS/ArrayMap"

enum _:S_MenuController {
    MenuController_Key[EMS_CONTROLLER_KEY_MAX_LEN],
    Array:MenuController_Params,
    MenuController_Callback,
}

static ArrayMap(MenuControllers);

MenuController_Init() {
    if (!ArrayMapCreated(MenuControllers)) {
        MenuControllers = ArrayMapCreate(S_MenuController);
    }

    ParamsController_Init();
}

MenuController_Use(const T_MenuController:controller, const playerIndex, const menu, const Trie:params) {
    new controllerObject[S_MenuController];
    MenuController_Get(controller, controllerObject);

    // TODO: Вместо создания пунктов напрямую, использовать какую-нить обёртку
    ExecuteForward(controllerObject[MenuController_Callback], _, playerIndex, menu, params);
}

T_MenuController:MenuController_Construct(const controllerKey[], const callback) {
    new controllerObject[S_MenuController];

    copy(controllerObject[MenuController_Key], charsmax(controllerObject[MenuController_Key]), controllerKey);
    controllerObject[MenuController_Callback] = callback;
    controllerObject[MenuController_Params] = ArrayCreate(1, 1);

    return T_MenuController:ArrayMapPushArray(MenuControllers, controllerObject, controllerObject[MenuController_Key]);
}

MenuController_AddParams(const T_MenuController:controller, const Array:newParams) {
    new controllerObject[S_MenuController];
    MenuController_Get(controller, controllerObject);

    for (new i = 0, ii = ArraySize(newParams); i < ii; ++i) {
        ArrayPushCell(controllerObject[MenuController_Params], ArrayGetCell(newParams, i));
    }
}

T_MenuController:MenuController_Find(const controllerKey[], const bool:orFail = false) {
    if (!ArrayMapKeyExists(MenuControllers, controllerKey)) {
        if (orFail) {
            abort(AMX_ERR_PARAMS, "Controller '%s' not found.", controllerKey);
        }
        return Invalid_MenuController;
    }

    return T_MenuController:ArrayMapGetIndex(MenuControllers, controllerKey);
}

MenuController_Get(const T_MenuController:controller, controllerObject[S_MenuController]) {
    ArrayMapGetArray(MenuControllers, controller, controllerObject);
}

Trie:MenuController_ReadParams(const T_MenuController:controller, const JSON:paramsJson) {
    if (paramsJson == Invalid_JSON) {
        abort(AMX_ERR_PARAMS, "paramsJson is an invalid json handler (%d).", paramsJson);
        return Invalid_Trie;
    }

    new controllerObject[S_MenuController];
    MenuController_Get(controller, controllerObject);

    new Trie:params = TrieCreate();
    if (controllerObject[MenuController_Params] != Invalid_Array) {
        new E_ParamsReadErrorType:errType = ParamsReadError_None;
        new errParamName[PARAM_KEY_MAX_LEN];

        ParamsController_Param_ReadList(
            controllerObject[MenuController_Params], paramsJson, params,
            .iErrType = errType,
            .sErrParamName = errParamName,
            .iErrParamNameLen = charsmax(errParamName)
        );

        switch (errType) {
            case ParamsReadError_RequiredParamNotPresented:
                abort(AMX_ERR_PARAMS, "Controller '%s' parameter '%s' is required but not presented.", controllerObject[MenuController_Key], errParamName);
            case ParamsReadError_ParamValueIsInvalid:
                abort(AMX_ERR_PARAMS, "Controller '%s' parameter '%s' has invalid value.", controllerObject[MenuController_Key], errParamName);
        }
    }

    return params;
}

MenuController_MakeCallback(const pluginIndex, const callback[]) {
    // (const playerIndex, const menu, const Trie:params)
    new fwd = CreateOneForward(pluginIndex, callback, FP_CELL, FP_CELL, FP_CELL);

    if (fwd == INVALID_HANDLE) {
        abort(AMX_ERR_PARAMS, "Failed to create forward.");
    }

    return fwd;
}
