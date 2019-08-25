#include "FileGroup.h"
#include "GUI_Private.h"
#include "Fonts/FreeMono12pt7b.h"

/*ʵ������ʾ������*/
TFT_S6B33B2 screen(TFT_PORT_START_Pin, TFT_RST_Pin, TFT_CS_Pin, TFT_RS_Pin, TFT_RW_Pin, TFT_RD_Pin);

/*ʵ����GUIҳ�������*/
PageManager page(PAGE_MAX, EVENT_MAX);

/**
  * @brief  ��ʾ����ʼ��
  * @param  ��
  * @retval ��
  */
void Init_Display()
{
    pinMode(TFT_BLK_Pin, OUTPUT);
    digitalWrite(TFT_BLK_Pin, LOW);
    
    screen.begin();
	screen.setRotation(1);
    screen.fillScreen(screen.Black);
    screen.setTextColor(screen.White, screen.Black);
    screen.setTextWrap(false);
    screen.setTextSize(1);
    
    digitalWrite(TFT_BLK_Pin, HIGH);
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
    PageRegister_SetBluetooth(PAGE_SetBluetooth);
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
    Draw_RGBBitmap_Logo(0.5, 0.5, 30);  //����LOGO����
    ClearDisplay();                     //����
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
