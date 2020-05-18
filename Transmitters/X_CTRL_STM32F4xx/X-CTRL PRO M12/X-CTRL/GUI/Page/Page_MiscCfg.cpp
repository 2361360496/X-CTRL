#include "Basic/FileGroup.h"
#include "GUI/DisplayPrivate.h"
#include "GUI/lv_settings.h"

static lv_obj_t * appWindow;

static lv_settings menu;

enum item_index{
    IIDX_SW_Sound,
    IIDX_SW_Vibrate,
    IIDX_SW_BigVibrate,
    IIDX_SW_IdleWarn,
    IIDX_MAX
};

static lv_settings::item_t item_grp[IIDX_MAX] =
{
    {.type = menu.TYPE_SW,     .name = "Sound",        .value = "Beep with motor",  .user_data.ptr = &CTRL.State.Sound},
    {.type = menu.TYPE_SW,     .name = "Vibrate",      .value = "linear motor",     .user_data.ptr = &CTRL.State.Vibrate},
    {.type = menu.TYPE_SW,     .name = "Big vibrate",  .value = "rotate motor",     .user_data.ptr = &CTRL.State.BigVibrate},
    {.type = menu.TYPE_SW,     .name = "Idle warn",    .value = "no-operation warn",.user_data.ptr = &CTRL.State.SignWarn},
};

static void Menu_EventHnadler(lv_obj_t * obj, lv_event_t event)
{
    /*Get the caller item*/
    lv_settings::item_t * act_item = (lv_settings::item_t *)lv_event_get_data();
#define IS_ITEM(item_name) (strcmp((item_name), act_item->name) == 0)
    
    if(event == LV_EVENT_VALUE_CHANGED)
    {
    }
    else if(event == LV_EVENT_CLICKED)
    {
    }
}

static void Menu_Init()
{
    menu.create(appWindow, Menu_EventHnadler);
    for(int i = 0; i < __Sizeof(item_grp); i++)
    {
        lv_settings::item_t * item = &item_grp[i];

        menu.add(item);
    }
}

/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    /*����ҳ���Ƶ�ǰ̨*/
    lv_obj_move_foreground(appWindow);
    Menu_Init();
    Menu_AnimOpen(&menu, true);
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
    Menu_AnimOpen(&menu, false);
    menu.del();
    lv_obj_clean(appWindow);
    if(!EEPROM_SaveAll())
    {
        Audio_PlayMusic(MC_Type::MC_UnstableConnect);
    }
}

/**
  * @brief  ҳ���¼�
  * @param  event:�¼����
  * @param  param:�¼�����
  * @retval ��
  */
static void Event(int event, void* btn)
{
    if(btn == &btBACK)
    {
        if(event == ButtonEvent_Type::EVENT_ButtonPress)
        {
            if(!menu.back())
            {
                page.PagePop();
            }
        }
    }

    if(btn == &btOK)
    {
        if(event == ButtonEvent_Type::EVENT_ButtonPress || event == ButtonEvent_Type::EVENT_ButtonLongPressRepeat)
        {
            menu.click();
        }
    }

    if(event == ButtonEvent_Type::EVENT_ButtonPress || event == ButtonEvent_Type::EVENT_ButtonLongPressRepeat)
    {
        lv_coord_t dist = (event == ButtonEvent_Type::EVENT_ButtonLongPressRepeat) ? 5 : 1;
        if(btn == &btUP)
        {
            menu.move(-dist);
        }
        if(btn == &btDOWN)
        {
            menu.move(+dist);
        }
        if(btn == &btUPL)
        {
            menu.scroll(-20);
        }
        if( btn == &btDOWNL)
        {
            menu.scroll(+20);
        }
    }
}

/**
  * @brief  ҳ��ע��
  * @param  pageID:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_MiscCfg(uint8_t pageID)
{
    appWindow = AppWindow_GetCont(pageID);
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
