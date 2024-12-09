#include <amxmodx>
#include <ems>
#include <ParamsController>
#include "EMS/CfgUtils"
#include "EMS/Forwards"
#include "EMS/ArrayMap"
#include "EMS/ParamTypes"
#include "EMS/DefaultObjects"
#include "EMS/Objects/Menu"

public plugin_precache() {
    register_plugin("Extended Menu System", "1.0.0-wip", "ArKaNeMaN");
    CfgUtils_SetFolder(EMS_CONFIG_ROOT_DIRECTORY);
    Forwards_Init();
    Menu_Init();

    DefaultObjects_Register();
    Forwards_RegAndCall("EMS_OnInit", ET_IGNORE);

    Menu_LoadFromFolder(CfgUtils_MakePath(EMS_CONFIG_ROOT_DIRECTORY));

    Forwards_RegAndCall("EMS_OnLoaded", ET_IGNORE);
}

public ParamsController_OnRegisterTypes() {
    ParamTypes_Register();
}