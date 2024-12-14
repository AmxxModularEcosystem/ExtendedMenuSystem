#if defined _ems_src__objects_menu_extension_instance_included
    #endinput
#endif
#define _ems_src__objects_menu_extension_instance_included

#include <amxmodx>
#include <json>
#include <ems>
#include <ParamsController>
#include "EMS/ArrayMap"
#include "EMS/Objects/MenuExtension"

enum _:S_MenuExtensionInstance {
    T_MenuExtension:MenuExtensionInstance_MenuExtension,
    Trie:MenuExtensionInstance_Params,
}

MenuExtensionInstance_CallMakeTitle(const extensionInstance[S_MenuExtensionInstance], title[]) {
    MenuExtension_CallMakeTitle(extensionInstance[MenuExtensionInstance_MenuExtension], title, extensionInstance[MenuExtensionInstance_Params]);
}

MenuExtensionInstance_CallCreate(const extensionInstance[S_MenuExtensionInstance], const menu) {
    MenuExtension_CallCreate(extensionInstance[MenuExtensionInstance_MenuExtension], menu, extensionInstance[MenuExtensionInstance_Params]);
}

MenuExtensionInstance_CallMakeItem(const extensionInstance[S_MenuExtensionInstance], title[], command[]) {
    MenuExtension_CallMakeItem(extensionInstance[MenuExtensionInstance_MenuExtension], title, command, extensionInstance[MenuExtensionInstance_Params]);
}

MenuExtensionInstance_Make(const T_MenuExtension:extension, const Trie:params) {
    new instance[S_MenuExtensionInstance];

    if (extension == Invalid_MenuExtension) {
        abort(AMX_ERR_PARAMS, "extension is an invalid menu extension handler (%d).", extension);
        return instance;
    }
    instance[MenuExtensionInstance_MenuExtension] = extension;

    if (params == Invalid_Trie) {
        instance[MenuExtensionInstance_Params] = TrieCreate();
    } else {
        instance[MenuExtensionInstance_Params] = params;
    }

    return instance;
}

MenuExtensionInstance_MakeFromJson(const JSON:extensionJson, instance[S_MenuExtensionInstance]) {
    if (json_is_string(extensionJson)) {
        new extensionKey[EMS_EXTENSION_KEY_MAX_LEN];
        json_get_string(extensionJson, extensionKey, charsmax(extensionKey));
        
        instance = MenuExtensionInstance_Make(
            .extension = MenuExtension_Find(extensionKey, .orFail = true),
            .params = Invalid_Trie
        );
    } else if (json_is_object(extensionJson)) {
        new extensionKey[EMS_EXTENSION_KEY_MAX_LEN];
        json_object_get_string(extensionJson, "Extension", extensionKey, EMS_EXTENSION_KEY_MAX_LEN);

        new T_MenuExtension:extension = MenuExtension_Find(extensionKey, .orFail = true);
        instance = MenuExtensionInstance_Make(
            .extension = extension,
            .params = MenuExtension_ReadParams(extension, extensionJson)
        );
    } else {
        abort(AMX_ERR_PARAMS, "extensionJson is not a json object or string.");
    }
}

Array:MenuExtensionInstance_MakeListFromJson(const JSON:extensionsJson) {
    if (!json_is_array(extensionsJson)) {
        abort(AMX_ERR_PARAMS, "extensionsJson is not a json array.");
        return Invalid_Array;
    }

    new Array:instances = ArrayCreate(S_MenuExtensionInstance, 1);

    for (new i = 0, ii = json_array_get_count(extensionsJson); i < ii; ++i) {
        new instance[S_MenuExtensionInstance];
        new JSON:extensionJson = json_array_get_value(extensionsJson, i);
        MenuExtensionInstance_MakeFromJson(extensionJson, instance);
        json_free(extensionJson);
        ArrayPushArray(instances, instance);
    }

    return instances;
}

Array:JsonObject_GetMenuExtensionInstances(const JSON:jsonObject, const key[], const bool:dotNot = false) {
    new JSON:extensionsJson = json_object_get_value(jsonObject, key, dotNot);
    new Array:instances = MenuExtensionInstance_MakeListFromJson(extensionsJson);
    json_free(extensionsJson);
    return instances;
}
