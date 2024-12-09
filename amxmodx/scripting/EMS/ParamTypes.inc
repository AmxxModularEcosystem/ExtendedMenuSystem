#if defined _ems_src__default_controller_included
    #endinput
#endif
#define _ems_src__default_controller_included

#include <amxmodx>
#include <json>
#include <ems>
#include <ParamsController>

ParamTypes_Register() {
    ParamsController_RegSimpleType(EMS_MENU_ITEMS_PARAM_TYPE, "@ParamType_MenuItems");
}

@ParamType_MenuItems(const JSON:jValue) {
    new Array:items = ArrayCreate(S_Param_MenuItem, 1);

    if (!json_is_array(jValue)) {
        log_amx("Value of '%s' param type must be an array.", EMS_MENU_ITEMS_PARAM_TYPE);
        return false;
    }

    for (new i = 0, ii = json_array_get_count(jValue); i < ii; ++i) {
        new JSON:item = json_array_get_value(jValue, i);

        if (!json_is_object(item)) {
            json_free(item);
            log_amx("Value of '%s' param type must be an array of objects.", EMS_MENU_ITEMS_PARAM_TYPE);
            return false;
        }

        new menuItem[S_Param_MenuItem];

        json_object_get_string(item, "Title", menuItem[Param_MenuItem_Title], charsmax(menuItem[Param_MenuItem_Title]));
        json_object_get_string(item, "Command", menuItem[Param_MenuItem_Command], charsmax(menuItem[Param_MenuItem_Command]));
        json_free(item);

        ArrayPushArray(items, menuItem);
    }

    return ParamsController_SetCell(items);
}