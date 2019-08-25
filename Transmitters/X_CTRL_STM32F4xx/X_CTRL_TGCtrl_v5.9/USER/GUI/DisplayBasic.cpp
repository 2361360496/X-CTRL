#include "FileGroup.h"
#include "GUI_Private.h"
#include "Fonts/FreeMono12pt7b.h"

/*ʵ������ʾ������*/
SCREEN_CLASS screen(TFT_CS, TFT_DC, TFT_RST);//, TFT_CLK, TFT_MOSI);

/*ʵ����GUIҳ�������*/
PageManager page(PAGE_MAX, EVENT_MAX);

/**
  * @brief  ��ʾ����ʼ��
  * @param  ��
  * @retval ��
  */
void Init_Display()
{
    pinMode(TFT_LED, OUTPUT);
    digitalWrite(TFT_LED, LOW);
    delay(100);

    screen.begin();

    screen.setRotation(3);
    screen.fillScreen(screen.Black);
    screen.setTextColor(screen.White, screen.Black);
    screen.setTextWrap(false);
    screen.setTextSize(1);

    digitalWrite(TFT_LED, HIGH);
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
    PageRegister_SetGravity(PAGE_SetGravity);
    PageRegister_About(PAGE_About);
    PageRegister_Handshake(PAGE_Handshake);
    PageRegister_Game(PAGE_Game);
}


/**
  * @brief  GUI��ʼ��
  * @param  ��
  * @retval ��
  */
void Init_GUI()
{
    Draw_RGBBitmap_Logo(0.5, 0.5, 30);  //����LOGO����
    Init_HMI();                         //��ʼ��HMI
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
