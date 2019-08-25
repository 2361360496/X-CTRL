#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

/*ʵ������ǰҳ�������*/
static MillisTaskManager mtm_MainMenu(2);

/*��ͼλ������*/
#define PosCtrlInfo (screen.width()*0.25-15)
#define PosOptions  (screen.width()*0.75-15)
#define PosCursorY 55

/*ʵ����CtrlͼƬ�ؼ�*/
static LightGUI::Bitmap<SCREEN_CLASS> Bitmap_Ctrl(&screen, (uint8_t*)gImage_Bitmap_Ctrl, 0, 0, 30, 30);

/*ʵ����OptionͼƬ�ؼ�*/
static LightGUI::Bitmap<SCREEN_CLASS> Bitmap_Option(&screen, (uint8_t*)gImage_Bitmap_Option, 0, 0, 30, 30);

/*ʵ�������ؼ�*/
static LightGUI::Cursor<SCREEN_CLASS> MenuCursor(&screen, PosCtrlInfo, PosCursorY, 30, 2);

static float CursorPosNow = PosCtrlInfo;
static float CursorPosLast = PosCtrlInfo;
static uint8_t CursorSelect = 0;
static const float CursorTarget[] = {PosCtrlInfo, PosOptions};

/**
  * @brief  ���λ�ø�������
  * @param  ��
  * @retval ��
  */
static void Task_CursorUpdate()
{
    /*����λ��*/
    CursorPosNow = CursorTarget[CursorSelect % __Sizeof(CursorTarget)];
    
    if((CursorPosNow - CursorPosLast) != 0)
        CursorPosLast += (CursorPosNow - CursorPosLast) * 0.2;

    MenuCursor.setPosition(CursorPosLast, PosCursorY);//���ÿؼ�λ��
}

/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{    
    /*�������붯��*/
    for(float i = 0.7; i < 1.0; i += 0.05)
    {
        Bitmap_Ctrl.setPosition_f(0.5 - 0.25 * i, 0.6);
        Bitmap_Option.setPosition_f(0.5 + 0.25 * i, 0.6);
        if((ecd != 0 || btEcd || btLEFT || btRIGHT))
        {
            i = 1.0;
            Bitmap_Ctrl.setPosition_f(0.25, 0.6);
            Bitmap_Option.setPosition_f(0.75, 0.6);
            break;
        }
    }

    /*��굯������*/
    for(float y = screen.height(); y > PosCursorY; y --)
    {
        MenuCursor.setPosition(CursorPosLast, y);
        delay(10);
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
    for(float y = PosCursorY; y < screen.height() + 1; y ++)
    {
        MenuCursor.setPosition(CursorPosLast, y);
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
    if(btLEFT && CursorSelect > 0)CursorSelect--;
    if(btRIGHT && CursorSelect < 1)CursorSelect++;

    if((btEcd || btUP || btDOWN))
    {
        if(CursorPosNow == PosCtrlInfo)
        {
            CursorPosLast = PosCtrlInfo;
            if(State_Handshake)
                page.PageChangeTo(PAGE_Handshake);
            else
                page.PageChangeTo(PAGE_CtrlInfo);
        }
        else if(CursorPosNow == PosOptions)
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
void PageRegister_MainMenu(uint8_t ThisPage)
{
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_EncoderRotate, EncoderEvent);
}
