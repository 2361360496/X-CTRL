#include "FileGroup.h"
#include "GUI_Private.h"

/*ʵ������ʾ������*/
SCREEN_CLASS screen(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);

/*ʵ����GUIҳ�������*/
PageManager page(PAGE_MAX, EVENT_MAX);

/**
  * @brief  ��ʾ����ʼ��
  * @param  ��
  * @retval ��
  */
void Init_Display()
{
    screen.initR(INITR_MINI128x64);
    screen.fillScreen(screen.Black);
    screen.setTextColor(screen.White, screen.Black);
    screen.setTextWrap(false);
    screen.setTextSize(1);
}

/**
  * @brief  ҳ��ע��
  * @param  ��
  * @retval ��
  */
static void Init_Pages()
{
    PAGE_REG(MainMenu);
    PAGE_REG(CtrlInfo);
    PAGE_REG(Options);
    PAGE_REG(SetJoystick);
    PAGE_REG(SetBluetooth);
    PAGE_REG(SetGravity);
    PAGE_REG(About);
    PAGE_REG(Handshake);
    PAGE_REG(FreqGraph);
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
    screen.clear();                     //����
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
