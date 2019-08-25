#include "FileGroup.h"
#include "GUI_Private.h"

/*�ֿ�*/
#include "Fonts/FreeSerif9pt7b.h"

/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    screen.setTextColor(screen.White);
    screen.setTextSize(0);
    screen.setFont(&FreeSerif9pt7b);
    screen.setCursor(20, 32);//10,18
    screen.print("VIFEXTech");

    screen.setFont();
    screen.setTextSize(1);
    screen.setCursor(33, 40);
    screen << "X-CTRL " << _X_CTRL_VERSION;
    screen.setCursor(13, 50);
    screen << __DATE__ << " Build";
}

/**
  * @brief  ҳ��ѭ���¼�
  * @param  ��
  * @retval ��
  */
static void Loop()
{
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
}

/**
  * @brief  ���������¼�
  * @param  ��
  * @retval ��
  */
static void ButtonPressEvent()
{
    page.PageChangeTo(PAGE_Options);
}

/**
  * @brief  �������Ϣҳ��ע��
  * @param  ThisPage:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_About(uint8_t ThisPage)
{
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
//    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
//    page.PageRegister_Event(ThisPage, EVENT_EncoderRotate, EncoderEvent);
}
