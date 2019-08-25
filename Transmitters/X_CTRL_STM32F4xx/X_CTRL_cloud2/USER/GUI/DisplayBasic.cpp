#include "FileGroup.h"
#include "GUI_Private.h"

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
	PageRegister_Ctrl(PAGE_CTRL);
	PageRegister_Main(PAGE_MAIN);
}


/**
  * @brief  GUI��ʼ��
  * @param  ��
  * @retval ��
  */
void Init_GUI()
{
    ClearDisplay();                     //����
    Init_Pages();                       //ҳ���ʼ��
}

/**
  * @brief  GUI�߳�
  * @param  ��
  * @retval ��
  */
void Thread_GUI()
{
    page.Running();
}
