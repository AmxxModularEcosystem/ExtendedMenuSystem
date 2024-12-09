#include <amxmodx>
#include <ems>
#include <ParamsController>
#include "EMS/Objects/MenuController"

API_MenuController_Init() {
    register_native("MenuController_Register", "@_MenuController_Register");
    register_native("MenuController_AddParams", "@_MenuController_AddParams");
}

T_MenuController:@_MenuController_Register(const pluginIndex) {
    enum {Arg_Key = 1, Arg_Callback};

    new controllerKey[EMS_CONTROLLER_KEY_MAX_LEN];
    new controllerCallback[EMS_EVENT_HANDLER_MAX_LEN];

    get_string(Arg_Key, controllerKey, charsmax(controllerKey));
    get_string(Arg_Callback, controllerCallback, charsmax(controllerCallback));

    return MenuController_Register(controllerKey, MenuController_MakeCallback(pluginIndex, controllerCallback));
}

@_MenuController_AddParams(const pluginIndex, const paramsCount) {
    enum {Arg_Controller = 1, Arg_Params};
    
    new T_MenuController:controller = get_param(Arg_Controller);
    new Array:newParams = ParamsController_Param_ListFromNativeParams(Arg_Params, paramsCount);

    MenuController_AddParams(controller, newParams);

    ArrayDestroy(newParams);
}