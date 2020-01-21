#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

/*ʵ������ǰҳ�������*/
static MillisTaskManager mtm_MainMenu(2);

/*��ͼλ������*/
#define PosCtrlInfo 0.25f
#define PosOptions  0.75f
#define PosCursorY  0.8f

/*ʵ����CtrlͼƬ�ؼ�*/
static LightGUI::Bitmap<SCREEN_CLASS> Bitmap_Ctrl(&screen, (uint16_t*)gImage_Bitmap_Ctrl, 0, 0, 30, 30);

/*ʵ����OptionͼƬ�ؼ�*/
static LightGUI::Bitmap<SCREEN_CLASS> Bitmap_Option(&screen, (uint16_t*)gImage_Bitmap_Option, 0, 0, 30, 30);

/*ʵ�������ؼ�*/
static LightGUI::Cursor<SCREEN_CLASS> MenuCursor(&screen, PosCursorY * screen.height(), 100, Bitmap_Ctrl.Width, 2);

#define CursorCloseTo(pos) (ABS(CursorPosNow - pos)<0.01f)

static float CursorPosNow = PosCtrlInfo;
static float CursorPosLast = PosCtrlInfo;
static uint8_t CursorSelect = 0;
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
        CursorPosLast += (CursorPosNow - CursorPosLast) * 0.2f;

    MenuCursor.setPosition_f(CursorPosLast, PosCursorY); //���ÿؼ�λ��
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

    /*�������붯��*/
    for(float i = 0.7f; i < 1.0f; i += 0.05f)
    {
        Bitmap_Ctrl.setPosition_f(0.5f - 0.25f * i, 0.6f);
        Bitmap_Option.setPosition_f(0.5f + 0.25f * i, 0.6f);
        if((ecd != 0 || btEcd || btOK || btBACK))
        {
            i = 1.0;
            Bitmap_Ctrl.setPosition_f(PosCtrlInfo, 0.6f);
            Bitmap_Option.setPosition_f(PosOptions, 0.6f);
            break;
        }
    }

    /*��굯������*/
    for(float y = 1.1f; y > PosCursorY; y -= 0.01f)
    {
        MenuCursor.setPosition_f(CursorPosLast, y);
        PageDelay(10);
    }

    /*����ע��*/
    mtm_MainMenu.TaskRegister(0, Task_CursorUpdate, 20);
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
  * @brief  ҳ���¼�
  * @param  ��
  * @retval ��
  */
static void Event(int event, void * param)
{
    if(param == &ecd)
    {
        if(ecd < 0 && CursorSelect > 0)
            CursorSelect--;

        if(ecd > 0 && CursorSelect < 1)
            CursorSelect++;
    }
    
    if(event == EVENT_ButtonPress)
    {
        if(btDOWN && CursorSelect > 0)CursorSelect--;
        if(btUP && CursorSelect < 1)CursorSelect++;

        if(btEcd || btOK)
        {
            if(CursorCloseTo(PosCtrlInfo))
            {
                CursorPosLast = PosCtrlInfo;
                if(State_Handshake)
                    page.PagePush(PAGE_Handshake);
                else
                    page.PagePush(PAGE_CtrlInfo);
            }
            else if(CursorCloseTo(PosOptions))
            {
                CursorPosLast = PosOptions;
                page.PagePush(PAGE_Options);
            }
        }
    }
}

/**
  * @brief  ���˵�ҳ��ע��
  * @param  ThisPage:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_MainMenu(uint8_t ThisPage)
{
    page.PageRegister(ThisPage, Setup, Loop, Exit, Event);
}
