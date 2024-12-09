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

T_MenuController:MenuController_Find(const controllerKey[], const bool:orFail = false) {
    if (!ArrayMapKeyExists(MenuControllers, controllerKey)) {
        if (orFail) {
            abort(AMX_ERR_PARAMS, "Controller '%s' not found.", controllerKey);
        }
        return Invalid_MenuController;
    }

    return ArrayMapGetIndex(MenuControllers, controllerKey);
}

MenuController_Get(const T_MenuController:controller, controllerObject[S_MenuController]) {
    ArrayMapGetArray(MenuControllers, controller, controllerObject);
}

T_MenuController:MenuController_Construct(const controllerKey[], const callback) {
    new controllerObject[S_MenuController];

    copy(controllerObject[MenuController_Key], charsmax(controllerObject[MenuController_Key]), controllerKey);
    controllerObject[MenuController_Callback] = callback;
    controllerObject[MenuController_Params] = ArrayCreate(1, 1);

    return ArrayMapPushArray(MenuControllers, controllerObject, controllerObject[MenuController_Key]);
}

MenuController_AddParams(const T_MenuController:controller, const Array:newParams) {
    new controllerObject[S_MenuController];
    MenuController_Get(controller, controllerObject);

    for (new i =, ii = ArraySize(newParams); i < ii; ++i) {
        ArrayPushCell(controllerObject[MenuController_Params], ArrayGetCell(newParams, i));
    }
}

MenuController_MakeCallback(const pluginIndex, const callback[]) {
    // (const playerIndex, const menu, const Trie:params)
    new fwd = CreateOneForward(pluginIndex, callback, FP_CELL, FP_CELL, FP_CELL);

    if (fwd == INVALID_HANDLE) {
        abort(AMX_ERR_PARAMS, "Failed to create forward.");
        return;
    }

    return fwd;
}

MenuController_Use(const T_MenuController:controller, const playerIndex, const menu, const Trie:params) {
    new controllerObject[S_MenuController];
    MenuController_Get(controller, controllerObject);

    // TODO: Вместо создания пунктов напрямую, использовать какую-нить обёртку
    ExecuteForward(controllerObject[MenuController_Callback], _, playerIndex, menu, params);
}
