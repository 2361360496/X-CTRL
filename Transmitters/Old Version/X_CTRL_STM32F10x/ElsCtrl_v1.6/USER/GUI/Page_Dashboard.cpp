#include "FileGroup.h"
#include "GUI_Private.h"
#include "Fonts/FreeSerif9pt7b.h"

void Task_UpdateDashboard()
{
	TIM_Cmd(TIM_DisplayThread, DISABLE);
	ClearPage();
	screen.setFont(&FreeSerif9pt7b);
	
	screen.setCursor(StatusBarPos + 20, 15);
	screen.setTextColor(screen.White, screen.Black);
	screen.printf("%0.1fkm/h", ElsSpeed);
	
	screen.setFont();
	screen.setCursor(StatusBarPos + 20, 25);
	screen.printf("%0.2fkm", ElsDistance);
	
	TIM_Cmd(TIM_DisplayThread, ENABLE);
}

/********** ���� ************/
/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
	ClearPage();
}

/**
  * @brief  ҳ��ѭ���¼�
  * @param  ��
  * @retval ��
  */
static void Loop()
{
	__IntervalExecute(Task_UpdateDashboard(), 200);
	CTRL.Left.Y = JS_L.Y;
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
	TIM_Cmd(TIM_DisplayThread, ENABLE);
}

/************ �¼����� **********/
/**
  * @brief  ���������¼�
  * @param  ��
  * @retval ��
  */
static void ButtonPressEvent()
{
	if(btUP)
		page.PageChangeTo(PAGE_CtrlInfo);
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
  * @brief  ҳ��ע��
  * @param  ThisPage:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_Dashboard(uint8_t ThisPage)
{
    /*����*/
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    
    /*�¼�*/
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
}
