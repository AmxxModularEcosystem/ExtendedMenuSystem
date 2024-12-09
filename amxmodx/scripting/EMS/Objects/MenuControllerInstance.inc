#if defined _ems_src__objects_menu_controller_included
    #endinput
#endif
#define _ems_src__objects_menu_controller_included

#include <amxmodx>
#include <json>
#include <ems>
#include <ParamsController>
#include "EMS/ArrayMap"
#include "EMS/Objects/MenuController"
#include "EMS/DefaultObjects"

enum _:S_MenuControllerInstance {
    T_MenuController:MenuControllerInstance_MenuController,
    Trie:MenuControllerInstance_Params,
}

MenuControllerInstance_Use(const instance[S_MenuControllerInstance], const playerIndex, const menu) {
    MenuController_Use(instance[MenuControllerInstance_MenuController], playerIndex, menu, instance[MenuControllerInstance_Params]);
}

MenuControllerInstance_Make(const T_MenuController:controller, const Trie:params) {
    new instance[S_MenuControllerInstance];

    if (controller == Invalid_MenuController) {
        abort(AMX_ERR_PARAMS, "controller is an invalid menu controller handler (%d).", controller);
        return instance;
    }

    if (params == Invalid_Trie) {
        abort(AMX_ERR_PARAMS, "params is an invalid trie handler (%d).", params);
        return instance;
    }
    
    instance[MenuControllerInstance_MenuController] = controller;
    instance[MenuControllerInstance_Params] = params;

    return instance;
}

static MenuControllerInstance_MakeDefaultFromItemsJson(const JSON:itemsJson, instance[S_MenuControllerInstance]) {
    new T_MenuController:controller = DefaultObjects_GetDefaultController();

    new JSON:fakeParamsJson = json_init_object();
    json_object_set_value(fakeParamsJson, "Items", json_deep_copy(itemsJson));
    instance = MenuControllerInstance_Make(controller, MenuController_ReadParams(controller, fakeParamsJson));
    json_free(fakeParamsJson);
}

MenuControllerInstance_MakeFromJson(const JSON:controllerJson, instance[S_MenuControllerInstance]) {
    if (json_is_string(controllerJson)) {
        new controllerKey[EMS_CONTROLLER_KEY_MAX_LEN];
        json_get_string(controllerJson, controllerKey, charsmax(controllerKey));

        new T_MenuController:controller = MenuController_Find(controllerKey, .orFail = true);

        new JSON:fakeParamsJson = json_init_object();
        new Trie:params = MenuController_ReadParams(controller, fakeParams);
        json_free(fakeParamsJson);
        
        instance = MenuControllerInstance_Make(controller, params);
    } else if (json_is_object(controllerJson)) {
        new controllerKey[EMS_CONTROLLER_KEY_MAX_LEN];
        json_get_string(controllerJson, "Controller", controllerKey, EMS_CONTROLLER_KEY_MAX_LEN);

        new T_MenuController:controller = MenuController_Find(controllerKey, .orFail = true);

        instance = MenuControllerInstance_Make(controller, MenuController_ReadParams(controller, controllerJson));
    } else {
        abort(AMX_ERR_PARAMS, "controllerJson is not a json object or string.");
    }
}

JsonObject_GetMenuControllerInstance(const JSON:jsonObject, const key[], instance[S_MenuControllerInstance], const bool:dotNot = false) {
    new JSON:controllerJson = json_object_get_value(jsonObject, key, dotNot);
    instance = MenuControllerInstance_MakeFromJson(controllerJson);
    json_free(controllersJson);
}

JsonObject_GetDefaultMenuControllerInstance(const JSON:jsonObject, const itemsKey[], instance[S_MenuControllerInstance], const bool:dotNot = false) {
    new JSON:itemsJson = json_object_get_value(jsonObject, itemsKey, dotNot);
    MenuControllerInstance_MakeDefaultFromItemsJson(itemsJson, instance);
    json_free(itemsJson);
}
