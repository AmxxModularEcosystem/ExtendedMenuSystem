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
    Menu_Init("@ClCmd_Menu");

    Forwards_RegAndCall("EMS_OnInit", ET_IGNORE);
    
    Menu_LoadFromFolder(CfgUtils_GetPath("Menus"));

    Forwards_RegAndCall("EMS_OnLoaded", ET_IGNORE);
}

@ClCmd_Menu(const playerIndex) {
    if (
        !is_user_connected(playerIndex)
        || is_user_bot(playerIndex)
        || is_user_hltv(playerIndex)
    ) {
        return PLUGIN_CONTINUE;
    }

    new command[EMS_MENU_COMMAND_MAX_LEN];
    read_argv(0, command, charsmax(command));

    new T_Menu:menu = Menu_FindByCommand(command);
    if (menu == Invalid_Menu) {
        return PLUGIN_CONTINUE;
    }

    menu_display(playerIndex, Menu_Build(playerIndex, menu));
    return PLUGIN_HANDLED;
}

public ParamsController_OnRegisterTypes() {
    ParamTypes_Register();
}

public EMS_OnInit() {
    DefaultObjects_Register();
}

#include "EMS/API/MenuController"
#include "EMS/API/MenuExtension"

public plugin_natives() {
    API_MenuController_Init();
    API_MenuExtension_Init();
}