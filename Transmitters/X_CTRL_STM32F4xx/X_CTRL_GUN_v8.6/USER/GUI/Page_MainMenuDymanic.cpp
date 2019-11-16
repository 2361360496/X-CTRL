#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

/*ʵ������ǰҳ�������*/
static MillisTaskManager mtm_MainMenu(4);

/*��ͼλ������*/
#define PosCtrlInfo 0.25f
#define PosOptions  0.75f
#define PosCursorY  0.8f

/*ʵ�������ؼ�*/
static LightGUI::Cursor<SCREEN_CLASS> MenuCursor(&screen, PosCursorY * screen.height(), 100, 60, 2);

#define CursorCloseTo(pos) (ABS(CursorPosNow - pos)<0.01f)

static float CursorPosNow = PosCtrlInfo;
static float CursorPosLast = PosCtrlInfo;
static uint8_t CursorSelect = 0;
static uint8_t CursorSelect_Stop = 0;
static const float CursorTargetNow[] = {PosCtrlInfo, PosOptions};
static uint32_t StayPageTimePoint;
static bool IsSpoke;

/**
  * @brief  ���λ�ø�������
  * @param  ��
  * @retval ��
  */
static void Task_CursorUpdate()
{
    /*����λ��*/
    CursorPosNow = CursorTargetNow[CursorSelect % __Sizeof(CursorTargetNow)];

    if(!CursorCloseTo(CursorPosLast))
        CursorPosLast += (CursorPosNow - CursorPosLast) * 0.3f;

    MenuCursor.setPosition_f(CursorPosLast, PosCursorY); //���ÿؼ�λ��
}

static void Task_UpdateTextOpacity()
{
    static const uint8_t OpacityStep = 40;
    static uint8_t Opacity = 0;
    if(CursorSelect_Stop == 0)
    {
        if(Opacity < 0xff - OpacityStep)
        {
            Opacity += OpacityStep;
        }
        else
        {
            Opacity = 0xff;
        }
    }
    else if(CursorSelect_Stop == 1)
    {
        if(Opacity > OpacityStep)
        {
            Opacity -= OpacityStep;
        }
        else
        {
            Opacity = 0;
        }
    }

    screen.setTextColor(screen.White, screen.Black);
    screen.setCursor(17, StatusBar_POS + 15);
    screen.setOpacityX(Opacity);
    screen.printfX("PLAY");

    screen.setCursor(76, StatusBar_POS + 15);
    screen.setOpacityX(255 - Opacity);
    screen.printfX("CONFIG");

    screen.setOpacityX(255);
}

static void Task_PlayVideoFast_1()
{
    player1.UpdatePlayerState();

    if(CursorCloseTo(PosCtrlInfo))
    {
//        player1.UpdatePlayerState();
        CursorSelect_Stop = 0;
    }
    else if(CursorCloseTo(PosOptions))
    {
//        player2.UpdatePlayerState();
        CursorSelect_Stop = 1;
    }
}

static void Task_PlayVideoFast_2()
{
    player2.UpdatePlayerState();
}

/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    ClearPage();
    StayPageTimePoint = millis();//��¼ҳ�����ʱ���

    IsSpoke = false;

    /*��굯������*/
    for(float y = 1.1f; y > PosCursorY; y -= 0.01f)
    {
        MenuCursor.setPosition_f(CursorPosLast, y);
        PageDelay(10);
    }

    player1.UpdatePlayerState();
    player2.UpdatePlayerState();
    /*����ע��*/
    mtm_MainMenu.TaskRegister(0, Task_CursorUpdate, 30);
    mtm_MainMenu.TaskRegister(1, Task_PlayVideoFast_1, 50);
    mtm_MainMenu.TaskRegister(2, Task_PlayVideoFast_2, 50);
    mtm_MainMenu.TaskRegister(3, Task_UpdateTextOpacity, 50, OFF);
}

/**
  * @brief  ҳ��ѭ���¼�
  * @param  ��
  * @retval ��
  */
static void Loop()
{
    if(!IsSpoke && millis() - StayPageTimePoint > 10000)
    {
        if(BattUsage < 30)
            XFS_Speak("���棺��ص����͡�");

        XFS_Speak(
            "��ǰ��ص�ѹ%0.1f����ʣ�����%0.0f%%",
            BattVoltage,
            fmap(BattVoltage, BattEmptyVoltage, BattFullVoltage, 0.0, 100.0)
        );

        if(BattUsage < 20)
            XFS_Speak("���������");
        else if(BattUsage < 30)
            XFS_Speak("�뼰ʱ���");

        IsSpoke = true;
    }
    mtm_MainMenu.Running(millis());
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
    /*������䶯��*/
    for(float y = PosCursorY; y < 1.1f; y += 0.01f)
    {
        MenuCursor.setPosition_f(CursorPosLast, y);
        PageDelay(10);
    }
}

/**
  * @brief  ���������¼�
  * @param  ��
  * @retval ��
  */
static void ButtonPressEvent()
{
    if(btDOWN && CursorSelect > 0)CursorSelect--;
    if(btUP && CursorSelect < 1)CursorSelect++;

    if(btEcd || btOK)
    {
        if(CursorCloseTo(PosCtrlInfo))
        {
            CursorPosLast = PosCtrlInfo;
            if(State_Handshake)
                page.PageChangeTo(PAGE_Handshake);
            else
                page.PageChangeTo(PAGE_CtrlInfo);
        }
        else if(CursorCloseTo(PosOptions))
        {
            CursorPosLast = PosOptions;
            page.PageChangeTo(PAGE_Options);
        }
    }

}

/**
  * @brief  ���������¼�
  * @param  ��
  * @retval ��
  */
static void ButtonLongPressEvent()
{
}

/**
  * @brief  ��ת�������¼�
  * @param  ��
  * @retval ��
  */
static void EncoderEvent()
{
    if(ecd < 0 && CursorSelect > 0)
        CursorSelect--;

    if(ecd > 0 && CursorSelect < 1)
        CursorSelect++;
}

/**
  * @brief  ���˵�ҳ��ע��
  * @param  ThisPage:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_MainMenuDymanic(uint8_t ThisPage)
{
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_EncoderRotate, EncoderEvent);
}
