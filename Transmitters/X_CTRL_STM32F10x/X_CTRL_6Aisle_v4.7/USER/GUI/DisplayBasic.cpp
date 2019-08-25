#include "FileGroup.h"
#include "GUI_Private.h"

/*ʵ������ʾ������*/
SCREEN_CLASS screen(-1);

/*ʵ����GUIҳ�������*/
PageManager page(PAGE_MAX, EVENT_MAX);

/*��Ļ��������*/
void Task_ScreenDisplay()
{
	screen.display();
}

/**
  * @brief  ��ʾ����ʼ��
  * @param  ��
  * @retval ��
  */
void Init_Display()
{
    __LoopExecute(screen.begin(SSD1306_SWITCHCAPVCC), 100);
    screen.setTextSize(1);
    screen.setTextColor(screen.White);
    screen.display();
}

/**
  * @brief  ҳ��ע��
  * @param  ��
  * @retval ��
  */
static void Init_Pages()
{
    PageRegister_MainMenu(PAGE_MainMenu);
    PageRegister_CtrlInfo(PAGE_CtrlInfo);
    PageRegister_Options(PAGE_Options);
    PageRegister_SetJoystick(PAGE_SetJoystick);
    PageRegister_About(PAGE_About);
    PageRegister_Handshake(PAGE_Handshake);
}


/**
  * @brief  GUI��ʼ��
  * @param  ��
  * @retval ��
  */
void Init_GUI()
{
    screen.clearDisplay();              //����
    Init_StatusBar();                   //״̬����ʼ��
    Init_Pages();                       //ҳ���ʼ��
}

/**
  * @brief  �ڸ�ҳ����ʱ�������ֶ�״̬����ˢ��
  * @param  ms:��ʱ������
  * @retval ��
  */
void PageDelay(uint32_t ms)
{
    uint32_t Stop_TimePoint = millis() + ms;
    while(millis() < Stop_TimePoint)
        Thread_StatusBar();
}

/**
  * @brief  GUI�߳�
  * @param  ��
  * @retval ��
  */
void Thread_GUI()
{
    Thread_StatusBar();
    page.Running();
}
