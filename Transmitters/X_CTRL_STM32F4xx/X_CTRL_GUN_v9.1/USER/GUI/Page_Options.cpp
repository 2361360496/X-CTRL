#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

/*ʵ������ǰҳ�������*/
static MillisTaskManager mtm_Options(2);

/*Ŀ¼����*/
#define ItemStartY (StatusBar_POS+4)
#define ItemStartX (TEXT_WIDTH_1+8)
#define ItemEndX   (screen.width()-35)

/*ʵ�������ؼ�����*/
static LightGUI::Cursor<SCREEN_CLASS> ItemCursor(&screen, ItemStartX, screen.height(), 8, 8);

/*ʵ�������ؿؼ�����*/
//static LightGUI::BoolSwitch<SCREEN_CLASS> Switch(&screen, screen.width() - 35, ItemStartY + 1 * 10, 15, 9, 2);

/*�Զ���ѡ���ַ���*/
String StrBtc[BC_Type::BC_END] = {"HMI", "XFS", "PHE"};
static String StrModel[] = {"COMMON", "CAR-DS", "CAR-SS", "CUSTOM"};
static String StrSpeed[] = {"250Kbps", "1Mbps", "2Mbps"};

/*��ǰѡ����ʾ��λ��*/
static int16_t ItemDispalyPos;

/*ѡ����˸��־λ*/
static bool ItemFlashUpdating = false;

/*�����˳���˸״̬��־λ*/
static bool ItemFlashUpdating_ReqExit = false;

/*��ͣ��˸��־λ*/
static bool ItemFlashUpdating_Pause = false;

/*ѡ���������ռ�*/
namespace ItemNum
{
/*ѡ��ö��*/
enum
{
    Mode,
    Btc,
    Addr,
    Freq,
    Speed,
    PassBack,
    Handshake,
    FHSS,
    Sound,
    Vibration,
    Bluetooth,
    CPU_Usage,
    NoOperationMonitor,
    SetJoystick,
    SetBlueTooth,
    SetGravity,
    FreqGraph,
    FileExplorer,
    About,
    SaveExit,
    MAX
};
}

/*ʵ�����˵�����������*/
/*
 *ItemNum::MAX -> ѡ��������
 *           7 -> ��ҳ��ʾѡ������
 *       &page -> ҳ���������ַ
 */
static MenuManager menu(
    ItemNum::MAX,
    (screen.height() - StatusBar_POS) / TEXT_HEIGHT_2 - 1,
    &page
);

/*ʵ�����������ؼ�����*/
static LightGUI::ScrollBar<SCREEN_CLASS> Scroll(
    &screen,
    screen.width() + 5,
    StatusBar_POS + 4,
    4,
    menu.ItemNumDisplay_Max * TEXT_HEIGHT_2,
    1,
    menu.ItemNumDisplay_Max * TEXT_HEIGHT_2 * (menu.ItemNumDisplay_Max / (float)menu.ItemNum_Max)
);

/**
  * @brief  ���ָ����ѡ������
  * @param  pos:ָ����ѡ������
  * @retval ��
  */
static void ClearItemPos(uint16_t pos)
{
    screen.fillRect(
        ItemStartX,
        ItemStartY + pos * TEXT_HEIGHT_2,
        Scroll.X - ItemStartX,
        TEXT_HEIGHT_2,
        screen.Black
    );
}

/**
  * @brief  ��ָ��������Ʋ������ؿؼ�
  * @param  x:X��ʼ����
  * @param  y:Y��ʼ����
  * @param  w:�ؼ����
  * @param  h:�ؼ��߶�
  * @param  r:��Բ�뾶
  * @param  sw:����״̬
  * @retval ��
  */
static void DrawBoolSwitch(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, bool sw)
{
    screen.drawRoundRect(x, y, w, h, r, screen.White);
    if(sw)
    {
        screen.fillRect(x + 1, y + 1, w - 2, h - 2, screen.Green);
        screen.fillCircle(x + w - 3 - r, (y + y + h) / 2, r, screen.White);
    }
    else
    {
        screen.fillRect(x + 1, y + 1, w - 2, h - 2, screen.Black);
        screen.fillCircle(x + 2 + r, (y + y + h) / 2, r, screen.White);
    }
}

/**
  * @brief  ��ָ���л��Ʋ�������
  * @param  pos:��
  * @param  sw:����״̬
  * @param  x:X����
  * @retval ��
  */
static void DrawItemSwitch(uint8_t pos, bool sw, int16_t x = ItemEndX)
{
    DrawBoolSwitch(x, ItemStartY + pos * TEXT_HEIGHT_2 + 3, 15, 9, 2, sw);
}

/**
  * @brief  ���¹��λ��
  * @param  ��
  * @retval ��
  */
static void UpadateCursor()
{
    ItemDispalyPos = menu.GetItemDispalyPos();
}

/**
  * @brief  ����ѡ���
  * @param  ��
  * @retval ��
  */
static void UpdateItemTop()
{
    menu.UpdateItemStrBuffer();
    Scroll.setScroll(1.0f - ((float)menu.ItemTop / (float)(menu.ItemNum_Max - menu.ItemNumDisplay_Max)));
    screen.setTextColor(screen.White, screen.Black);
    for(int16_t i = 0; i < menu.ItemNumDisplay_Max; i++)
    {
        ClearItemPos(i);
        screen.setCursor(ItemStartX, ItemStartY + i * TEXT_HEIGHT_2);
        screen.printfX(menu.ItemStrBuffer[i]);

        int16_t nowItemNum = (menu.ItemTop + i) % menu.ItemNum_Max;
        if(menu.ItemList[nowItemNum].Type == menu.TYPE_Bool)
        {
            DrawItemSwitch(i, *((bool*)menu.ItemList[nowItemNum].Data));
        }
        else if(menu.ItemList[nowItemNum].Type == menu.TYPE_PageJump)
        {
            screen.setCursor(ItemEndX + 6, ItemStartY + i * TEXT_HEIGHT_2);
            screen.printfX(">");
        }
    }
}

/**
  * @brief  ����ָ����ѡ��
  * @param  pos:��
  * @retval ��
  */
static void UpdateItemPos(uint8_t pos)
{
    menu.UpdateItemStrBuffer();
    screen.setTextColor(screen.White, screen.Black);
    ClearItemPos(pos);
    screen.setCursor(ItemStartX, ItemStartY + pos * TEXT_HEIGHT_2);
    screen.printfX(menu.ItemStrBuffer[pos]);

    int16_t nowItemNum = (menu.ItemTop + pos) % menu.ItemNum_Max;
    if(menu.ItemList[nowItemNum].Type == menu.TYPE_Bool)
        DrawItemSwitch(pos, *((bool*)menu.ItemList[nowItemNum].Data));
}

/**
  * @brief  ��˸ָ����ѡ��
  * @param  pos:��
  * @retval ��
  */
static void FlashItemPos(uint8_t pos, uint32_t time = 500)
{
    static bool isDisplay = true;
    __IntervalExecute(
        (
            (isDisplay ? UpdateItemPos(pos) : ClearItemPos(pos)),
            isDisplay = !isDisplay
        ),
        time
    );

    if(ItemFlashUpdating_Pause)
    {
        isDisplay = true;
    }

    if(ItemFlashUpdating_ReqExit)
    {
        UpdateItemPos(pos);
        isDisplay = true;
        ItemFlashUpdating = ItemFlashUpdating_ReqExit = false;
    }
}

/**
  * @brief  ѡ������¼�
  * @param  ��
  * @retval ��
  */
static void When_ItemSelect_IncDec(int8_t step)
{
    String str_temp;
    switch(menu.ItemSelect)
    {
    case ItemNum::Mode:
        __ValueStep(CTRL.Info.CtrlObject, step, __Sizeof(StrModel));
        menu.UpdateItem(ItemNum::Mode, ".Model:" + StrModel[CTRL.Info.CtrlObject]);
        break;

    case ItemNum::Btc:
        __ValueStep(Bluetooth_ConnectObject, step, BC_Type::BC_END);
        menu.UpdateItem(ItemNum::Btc, ".Btc:" + StrBtc[Bluetooth_ConnectObject]);
        break;

    case ItemNum::Addr:
        __ValueStep(NRF_Cfg.Address, step, (sizeof(NRF_AddressConfig) / 5));
        str_temp.sprintf(".Address:%d", NRF_Cfg.Address);
        menu.UpdateItem(ItemNum::Addr, str_temp);
        nrf.SetAddress((uint8_t*)NRF_AddressConfig + NRF_Cfg.Address * 5);
        break;

    case ItemNum::Freq:
        __ValueStep(NRF_Cfg.Freq, step, 128);
        str_temp.sprintf(".Freq:%dMHz", 2400 + NRF_Cfg.Freq);
        menu.UpdateItem(ItemNum::Freq, str_temp);
        nrf.SetFreqency(NRF_Cfg.Freq);
        break;

    case ItemNum::Speed:
        __ValueStep(NRF_Cfg.Speed, step, 3);
        menu.UpdateItem(ItemNum::Speed, ".Speed:" + StrSpeed[NRF_Cfg.Speed]);
        nrf.SetSpeed(NRF_Cfg.Speed);
        break;
    }
}

/**
  * @brief  ѡ�ѡ���¼�
  * @param  ��
  * @retval ��
  */
static void When_ItemSelect()
{
    switch(menu.ItemSelect)
    {
    case ItemNum::Mode:
        ItemFlashUpdating = true;//������˸״̬
        break;

    case ItemNum::Btc:
        ItemFlashUpdating = true;
        break;

    case ItemNum::Addr:
        ItemFlashUpdating = true;
        break;

    case ItemNum::Freq:
        /*������ʹ��ʱ�������¼�����ֹ����*/
        if(State_Handshake)
        {
            BuzzTone(100, 20);
            break;
        }
        ItemFlashUpdating = true;
        break;

    case ItemNum::Speed:
//        /*������ʹ��ʱ�������¼�����ֹ����*/
//        if(State_Handshake)
//        {
//            BuzzTone(100, 20);
//            break;
//        }
        ItemFlashUpdating = true;
        break;

    case ItemNum::SaveExit:
        if(EEPROM_Handle(EEPROM_Chs::SaveData))
        {
            BuzzTone(2000, 50);
        }
        else
        {
            BuzzTone(50, 50);
        }
        page.PagePop();
        break;

    case ItemNum::Bluetooth:
        hc05.Power(!State_Bluetooth);
        break;
    }
}

/**
  * @brief  �˵���������
  * @param  ��
  * @retval ��
  */
static void Task_MenuUpdate()
{
    if(menu.ItemSelectUpdating)
    {
        UpadateCursor();
        menu.ItemSelectUpdating = false;
    }

    if(menu.ItemTopUpdating)
    {
        UpdateItemTop();
        menu.ItemTopUpdating = false;
    }

    if(menu.ItemXUpdating)
    {
        UpdateItemPos(ItemDispalyPos);
        menu.ItemXUpdating = false;
    }

    if(ItemFlashUpdating)
    {
        FlashItemPos(ItemDispalyPos, 250);
    }
}

/*��ǰ���λ��*/
static float CursorPosNow = ItemStartY;

/*��һ�εĹ��λ��*/
static float CursorPosLast = ItemStartY + menu.ItemNumDisplay_Max * 10;

/**
  * @brief  ���λ�ø�������
  * @param  ��
  * @retval ��
  */
static void Task_DrawCursorUpdate()
{
    CursorPosNow = ItemStartY + ItemDispalyPos * TEXT_HEIGHT_2 + 5;

    if((CursorPosNow - CursorPosLast) != 0)
    {
        CursorPosLast += (CursorPosNow - CursorPosLast) * 0.4f;
    }

    ItemCursor.setPosition(ItemStartX - 8, CursorPosLast);
}

/**
  * @brief  �Զ�����ص�����
  * @param  ��
  * @retval ��
  */
static void DrawCursor_Callback()
{
    screen.setTextColor(screen.Yellow, screen.Black);
    screen.setCursor(ItemCursor.X, ItemCursor.Y);
    screen.print('>');
}



/*********************************** Events ************************************/

/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    XFS_Speak("��������");
    ClearPage();
    CursorPosNow = ItemStartY;
    CursorPosLast = screen.height();
    Scroll.Color_FM = Scroll.Color_SB = screen.White;
    if(page.LastPage == PAGE_MainMenu)
    {
        ItemDispalyPos = menu.ItemSelect = menu.ItemTop = 0;
        menu.ItemXUpdating = menu.ItemSelectUpdating = false;
    }

    menu.ItemTopUpdating = true;

    /*�˵�ѡ��ѡ���¼���*/
    menu.CallbackFunc_ItemSelectEvent = When_ItemSelect;
    /*�˵�ѡ��ע��*/
    menu.UpdateItem(ItemNum::Mode,    ".Model:"   + StrModel[CTRL.Info.CtrlObject]);
    menu.UpdateItem(ItemNum::Btc,     ".Btc:"     + StrBtc[Bluetooth_ConnectObject]);
    menu.UpdateItem(ItemNum::Addr,    ".Address:" + String(NRF_Cfg.Address));
    menu.UpdateItem(ItemNum::Freq,    ".Freq:"    + String(2400 + nrf.GetFreqency()) + "MHz");
    menu.UpdateItem(ItemNum::Speed,   ".Baud:"   + StrSpeed[nrf.GetSpeed()]);
    menu.UpdateItem(ItemNum::PassBack,   ".PassBack",   menu.TYPE_Bool, (int)&State_PassBack);
    menu.UpdateItem(ItemNum::Handshake,  ".Handshake",  menu.TYPE_Bool, (int)&State_Handshake);
    menu.UpdateItem(ItemNum::FHSS,       ".FHSS",       menu.TYPE_Bool, (int)&State_FHSS);
    menu.UpdateItem(ItemNum::Sound,      ".Sound",      menu.TYPE_Bool, (int)&State_BuzzSound);
    menu.UpdateItem(ItemNum::Vibration,  ".Vibration",  menu.TYPE_Bool, (int)&State_MotorVibrate);
    menu.UpdateItem(ItemNum::Bluetooth,  ".Bluetooth",  menu.TYPE_Bool, (int)&State_Bluetooth);
    menu.UpdateItem(ItemNum::CPU_Usage,   ".CPU Load", menu.TYPE_Bool, (int)&State_DisplayCPU_Usage);
    menu.UpdateItem(ItemNum::NoOperationMonitor, ".OpaMonit", menu.TYPE_Bool, (int)&State_NoOperationMonitor);
    menu.UpdateItem(ItemNum::SetJoystick,  ".Joystick", menu.TYPE_PageJump, PAGE_SetJoystick);
    menu.UpdateItem(ItemNum::SetBlueTooth, ".BlueTooth", menu.TYPE_PageJump, PAGE_SetBluetooth);
    menu.UpdateItem(ItemNum::SetGravity,   ".Gravity",  menu.TYPE_PageJump, PAGE_SetGravity);
    menu.UpdateItem(ItemNum::FreqGraph,   ".Spectrum", menu.TYPE_PageJump, PAGE_FreqGraph);
    menu.UpdateItem(ItemNum::FileExplorer, ".Explorer", menu.TYPE_PageJump, PAGE_FileExplorer);
    menu.UpdateItem(ItemNum::About,        ".About",    menu.TYPE_PageJump, PAGE_About);
    menu.UpdateItem(ItemNum::SaveExit,     ".Save & Exit",    menu.TYPE_None, 0);

    menu.UpdateItemStrBuffer();
    for(uint8_t i = 0; i < menu.ItemNumDisplay_Max; i++)
    {
        for(int16_t j = -20; j < ItemStartX - 2; j++)
        {
            screen.setOpacityX(__Map(j, -20, ItemStartX - 3, 0, 255));
            screen.setTextColor(screen.White, screen.Black);
            screen.setCursor(j, ItemStartY + i * TEXT_HEIGHT_2);
            screen.printfX(" " + menu.ItemStrBuffer[i]);
            screen.setOpacityX(255);
            PageDelay(1);
        }
        BuzzTone(1000 + 100 * i, 5);
        PageDelay(10);
    }

    Scroll.setScroll(1.0);
    for(uint8_t i = screen.width() + 5; i > screen.width() - 7; i--)
    {
        Scroll.setPosition(i, Scroll.Y);
        PageDelay(10);
    }

    /*ע���Զ�����ص�����*/
    ItemCursor.DisplayCallbackFunction = DrawCursor_Callback;

    /*ע������*/
    mtm_Options.TaskRegister(0, Task_MenuUpdate, 50);
    mtm_Options.TaskRegister(1, Task_DrawCursorUpdate, 20);
}

/**
  * @brief  ҳ��ѭ���¼�
  * @param  ��
  * @retval ��
  */
static void Loop()
{
    mtm_Options.Running(millis());
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
    if(page.NextPage == PAGE_MainMenu)
        XFS_Speak("�˳�����");

    /*���������󻬶�����*/
    for(float i = 1.0f; i > 0.0f; i -= ((i * 0.01f) + 0.001f))
    {
        Scroll.Color_FM = Scroll.Color_SB = StrPrinter_ColorMap(screen.Black, screen.White, (uint8_t)(i * 15));
        Scroll.setPosition(screen.width()*i, Scroll.Y);
        PageDelay(2);
    }
    ClearPage();
}

/**
  * @brief  ҳ���¼�
  * @param  ��
  * @retval ��
  */
static void Event(int event, void * param)
{
    if(event == EVENT_ButtonPress || event == EVENT_ButtonLongPressRepeat)
    {
        if(btUP)
        {
            if(ItemFlashUpdating)
            {
                ItemFlashUpdating_Pause = true;
                When_ItemSelect_IncDec(+1);
            }
            else
                menu.SelectMove(+1);//�˵��������
        }
        if(btDOWN)
        {
            if(ItemFlashUpdating)
            {
                ItemFlashUpdating_Pause = true;
                When_ItemSelect_IncDec(-1);
            }
            else
                menu.SelectMove(-1);//�˵��������
        }
    }
    if(event == EVENT_ButtonPress)
    {
        if(btOK || btEcd)
        {
            if(ItemFlashUpdating)
                ItemFlashUpdating_ReqExit = true;
            else
                menu.ItemSelectEvent();//����ѡ�ѡ���¼�
        }

        if(btBACK)
        {
            if(ItemFlashUpdating)
                ItemFlashUpdating_ReqExit = true;
            else
                page.PagePop();//ҳ����ת�����˵�
        }
    }
    if(event == EVENT_ButtonRelease)
    {
        ItemFlashUpdating_Pause = false;
    }
}

/**
  * @brief  ����ҳ��ע��
  * @param  ThisPage:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_Options(uint8_t ThisPage)
{
    page.PageRegister(ThisPage, Setup, Loop, Exit, Event);
}
