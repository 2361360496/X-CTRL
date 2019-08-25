#include "FileGroup.h"
#include "GUI_Private.h"

HMI::Widget text(&HMI_SERIAL, "tSPD.val=");


/********** ���� ************/
static void Setup()
{
    //ҳ���ʼ���¼�
    text.SetValue("%d",0);
}

static void Loop()
{
    //ҳ��ѭ���¼�
}

static void Exit()
{
    //ҳ���˳��¼�
}

/************ �¼����� **********/
static void ButtonPressEvent()
{
    //���������¼�
}

static void ButtonLongPressEvent()
{
    //���������¼�
}

static void EncoderEvent()
{
    //��ת�������¼�
}

/************** ҳ��ע�� **************/
void PageRegister_MainMenu(uint8_t ThisPage)
{
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_EncoderRotate, EncoderEvent);
}

