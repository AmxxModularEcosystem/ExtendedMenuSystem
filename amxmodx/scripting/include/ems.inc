#if defined _ems_included
    #endinput
#endif
#define _ems_included

#include <amxmodx>

/**
 * Максимальная длина заголовка меню.
 */
#define EMS_MENU_TITLE_MAX_LEN 256

/**
 * Максимальная длина команды вызова меню.
 * 
 * Распространяется как на консольную команду, так и на чат-команды.
 */
#define EMS_MENU_COMMAND_MAX_LEN 64

/**
 * Максимальная длина команды пункта меню.
 */
#define EMS_MENU_ITEM_COMMAND_MAX_LEN 256

/**
 * Максимальная длина уникального строкового идентификатора контроллера.
 */
#define EMS_CONTROLLER_KEY_MAX_LEN 64

/**
 * Максимальная длина уникального строкового идентификатора расширения.
 */
#define EMS_EXTENSION_KEY_MAX_LEN 64

/**
 * Максимальная длина названия функции-обработчика события.
 */
#define EMS_EVENT_HANDLER_MAX_LEN 64

/**
 * Корневая папка для всех конфигов плагина в amxmodx/configs/plugins/.
 */
stock const EMS_CONFIG_ROOT_DIRECTORY[] = "ExtendedMenuSystem";

/**
 * Путь до папки с файлами меню.
 */
stock const EMS_CONFIG_MENUS_DIRECTORY[] = "Menus";

/**
 * Название контроллера меню по умолчанию.
 * 
 * Контроллер будет зарегистрирован системой перед вызовом EMS_OnInit.
 * Контроллер будет использоваться при указании поля Items в обьекте меню.
 */
stock const EMS_DEFAULT_CONTROLLER_KEY[] = "Default";

/**
 * Название типа параметра списка пунктов меню.
 * 
 * Тип изначально создаётся под контроллер по умолчанию, но можно использовать и в других местах.
 * На выходе отдаёт Array со структурами S_Param_MenuItem.
 */
stock const EMS_MENU_ITEMS_PARAM_TYPE[] = "EMS-MenuItems";

/**
 * Структура пункта меню для контроллера по умолчанию.
 */
enum _:S_Param_MenuItem {
    Param_MenuItem_Title[EMS_MENU_TITLE_MAX_LEN],
    Param_MenuItem_Command[EMS_MENU_ITEM_COMMAND_MAX_LEN],
}

/**
 * Тэг хендлера контроллера меню.
 */
enum T_MenuController { Invalid_MenuController = INVALID_HANDLE }

/**
 * Тэг хендлера расширения меню.
 */
enum T_MenuExtension { Invalid_MenuExtension = INVALID_HANDLE }

/**
 * События меню.
 */
enum E_MenuEvent {
    /**
     * Вызывается при создании заголовка меню.
     * 
     * @note Передаваемые параметры: (title[], const Trie:params)
     * @param title Текст заголовка меню. Мог быть изменён другими расширениями.
     * @param params Параметры расширения, для которого был вызван обработчик.
     * 
     * @noreturn
     */
    MenuEvent_MakeTitle,

    /**
     * Вызывается после создания обьекта меню.
     * 
     * @note Передаваемые параметры: (const menu, const Trie:params)
     * @param menu Хендлер меню, полученный из натива menu_create.
     * @param params Параметры расширения, для которого был вызван обработчик.
     * 
     * @noreturn
     */
    MenuEvent_Create,

    /**
     * Вызывается перед добавлением пункта в меню.
     * 
     * @note Передаваемые параметры: (title[], command[], const Trie:params)
     * @param title Текст создаваемого пункта. Мог быть изменён другими расширениями.
     * @param command Команда, которая будет вызвана при выборе этого пункта. Могла быть изменена другими расширениями.
     * @param params Параметры расширения, для которого был вызван обработчик.
     * 
     * @noreturn
     */
    MenuEvent_MakeItem,
}

/**
 * Вызывается после инициализации системы, но до загрузки меню.
 * 
 * @note Форвард вызывается внутри plugin_precache.
 * @note Все контроллеры и расширения должны регистрироваться внутри вызова этого форварда.
 * 
 * @noreturn
 */
forward EMS_OnInit();

/**
 * Вызывается после полной загрузки системы.
 * 
 * @note Форвард вызывается внутри plugin_precache.
 * @note К моменту вызова форварда все расширения зарегистрированы и все меню загружены.
 * 
 * @noreturn
 */
forward EMS_OnLoaded();

/**
 * Регистрация расширения меню.
 * 
 * @param extensionKey  Уникальный строковый идентификатор расширения.
 * 
 * @return Индекс расширения.
 */
native T_MenuExtension:EMS_MenuExtension_Register(const extensionKey[]);

/**
 * TODO
 */
native EMS_MenuExtension_AddParams(const T_MenuExtension:extension, any:...);

/**
 * Регистрация обработчика событий меню для расширения.
 * 
 * @param extension Индекс расширения.
 * @param event Событие.
 * @param callback Название публичной функции-обработчика.
 * 
 * @noreturn
 */
native EMS_MenuExtension_RegisterEventHandler(const T_MenuExtension:extension, const E_MenuEvent:event, const callback[]);

/**
 * Регистрация контроллера меню.
 * 
 * @param controllerKey  Уникальный строковый идентификатор контроллера.
 * @param callback       Обработчик контроллера - (const playerIndex, const menu, const Trie:params).
 * 
 * @return Индекс контроллера.
 */
native T_MenuController:EMS_MenuController_Register(const controllerKey[], const callback[]);

/**
 * TODO
 */
native EMS_MenuController_AddParams(const T_MenuController:extension, any:...);
