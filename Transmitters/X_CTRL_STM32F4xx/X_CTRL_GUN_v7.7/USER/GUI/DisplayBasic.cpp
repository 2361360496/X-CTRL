#include "FileGroup.h"
#include "GUI_Private.h"
#include "Fonts/FreeMono12pt7b.h"

/*ʵ������ʾ������*/
TFT_SSD1283A screen(TFT_CS, TFT_RST, TFT_DC/*, TFT_SDA, TFT_SCK*/);

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

    screen.begin();
    screen.setRotation(1);
    screen.fillScreen(screen.Black);
    screen.setTextColor(screen.White, screen.Black);
    screen.setTextWrap(false);
    screen.setTextSize(1);

    delay(200);
    digitalWrite(TFT_LED, HIGH);
}

/**
  * @brief  ҳ��ע��
  * @param  ��
  * @retval ��
  */
static void Init_Pages()
{
    if(State_SD_Enable && State_BV_Enable)
        PageRegister_MainMenuDymanic(PAGE_MainMenu);
    else
        PageRegister_MainMenu(PAGE_MainMenu);

    PageRegister_CtrlInfo(PAGE_CtrlInfo);
    PageRegister_Options(PAGE_Options);
    PageRegister_SetJoystick(PAGE_SetJoystick);
    PageRegister_SetBluetooth(PAGE_SetBluetooth);
    PageRegister_SetGravity(PAGE_SetGravity);
    PageRegister_About(PAGE_About);
    PageRegister_Handshake(PAGE_Handshake);
    PageRegister_Game(PAGE_Game);
    PageRegister_FreqGraph(PAGE_FreqGraph);

    page.PageChangeTo(PAGE_MainMenu);
}


/**
  * @brief  GUI��ʼ��
  * @param  ��
  * @retval ��
  */
void Init_GUI(uint8_t step)
{
    if(step == 0)
    {
        Draw_RGBBitmap_Logo(0.5, 0.5, 30);  //����LOGO����
        Init_HMI();                         //��ʼ��HMI
    }
    else
    {
        ClearDisplay();                     //����
        Init_StatusBar();                   //״̬����ʼ��
        Init_Pages();                       //ҳ���ʼ��
    }
}

/**
  * @brief  �ڸ�ҳ����ʱ�������ֶ�״̬����ˢ��
  * @param  ms:��ʱ������
  * @retval ��
  */
void PageDelay(uint32_t ms)
{
    uint32_t time = millis();
    while(millis() - time < ms)
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
