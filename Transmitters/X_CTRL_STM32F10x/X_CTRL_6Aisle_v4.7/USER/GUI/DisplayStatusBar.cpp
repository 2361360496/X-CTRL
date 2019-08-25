#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

/*״̬��ʱ��Ƭ���������������ȼ�*/
static MillisTaskManager mtm_StatusBar(3, true);

/*��ص�����ʾ�ؼ�*/
static LightGUI::ProgressBar<SCREEN_CLASS> PB_Batt(&screen, 0);

bool State_DisplayCPU_Usage = false;

/**
  * @brief  ��ص�����ʾ��������
  * @param  ��
  * @retval ��
  */
static void Task_DrawBattUsage()
{
    static uint8_t usage;//��ʾ�ĵ�ص���
    
    __ValueCloseTo(usage, BattUsage, 1);//����ʾ�ĵ�������ʵ�ʵĵ������ﵽ������Ч��

    if(usage < 30)
        PB_Batt.Color_PB = screen.Red;//����<30%�����Ϊ��ɫ
    else if(usage >= 90)
        PB_Batt.Color_PB = screen.Green;//����>90%�����Ϊ��ɫ
    else
        PB_Batt.Color_PB = screen.White;//�������������Ϊ��ɫ

    PB_Batt.setProgress(usage / 100.0);//���¿ؼ�
}

/**
  * @brief  NRF�շ�״̬��ʾ����
  * @param  ��
  * @retval ��
  */
static void Task_DrawNrfTxRxState()
{   
    screen.fillRect(69, 2, 5, 7, screen.Black);
    if(page.NowPage == PAGE_CtrlInfo)
    {
        //screen.drawFastVLine(71, 2, 7, screen.White);       
        if(nrf.GetTxRxState() == nrf.RX_MODE)
        {
            screen.drawLine(71, 8, 71-2, 8-2, screen.White);
            screen.drawLine(71, 8, 71+2, 8-2, screen.White);
        }
        else //(nrf.GetTxRxState() == nrf.TX_MODE)
        {     
            screen.drawLine(71, 2, 71-2, 2+2, screen.White);
            screen.drawLine(71, 2, 71+2, 2+2, screen.White);
        }
    }
}

/**
  * @brief  1000ms��������
  * @param  ��
  * @retval ��
  */
static void Task_1000msUpdate()
{
    /* NRF �ź�ǿ��(0~100%) */
    NRF_SignalStrength = nrf.GetSignalStrength();
    screen.setTextSize(1);
    if(NRF_SignalStrength >= 90)
        screen.setTextColor(screen.Green, screen.Black);
    else if(NRF_SignalStrength >= 60)
        screen.setTextColor(screen.White, screen.Black);
    else if(NRF_SignalStrength >= 20)
        screen.setTextColor(screen.Yellow, screen.Black);
    else if(NRF_SignalStrength < 20)
        screen.setTextColor(screen.Red, screen.Black);

    screen.setCursor(2, 2);
    screen.printf("%3d%%", NRF_SignalStrength);

    /* ��Ƶͨ��״̬ */
    if(State_RF)
    {
        if(!digitalRead(SPDT_Switch_Pin))
            screen.setTextColor(screen.Yellow, screen.Black);
        else
            screen.setTextColor(screen.Green, screen.Black);
    }
    else
    {
        screen.setTextColor(screen.Black, screen.Black);
    }
    screen.setCursor(56, 2);
    screen.print("RF");

    /* ��ʾ��ص�ѹ */
    screen.setCursor(80, 2);
    screen.setTextSize(0);
    screen.setTextColor(screen.Blue, screen.Black);
    screen.printf("%1.1fV", BattVoltage);

    /* ����һ��ʱ��󽵵͵�ص���ˢ���� */
    if(millis() > 3000)
        mtm_StatusBar.TaskSetIntervalTime((uint8_t)0, 100);
}

/**
  * @brief  ��ʼ��״̬��
  * @param  ��
  * @retval ��
  */
void Init_StatusBar()
{
    /*״̬����£����*/
    for(float i = 0.0; i < 1.0; i += 0.001)
    {
        screen.drawFastHLine(0, StatusBar_POS, screen.width() / 2 * i, screen.White);
        screen.drawFastHLine(screen.width() - screen.width() / 2 * i - 1, StatusBar_POS, screen.width() / 2 * i, screen.White);
    }

    /*��ص�����ʾ�ؼ���������*/
    PB_Batt.X = 105;
    PB_Batt.Y = 1;
    PB_Batt.Width = 20;
    PB_Batt.Height = 10;
    screen.drawFastVLine(
        PB_Batt.X + PB_Batt.Width,
        PB_Batt.Y + 2,
        PB_Batt.Height - 4,
        screen.White
    );

    /*����ע��*/
    mtm_StatusBar.TaskRegister(0, Task_DrawBattUsage, 5);
    mtm_StatusBar.TaskRegister(1, Task_1000msUpdate, 1000);
    mtm_StatusBar.TaskRegister(2, Task_DrawNrfTxRxState, 123);
    
    Task_1000msUpdate();
}

/**
  * @brief  ״̬���߳�
  * @param  ��
  * @retval ��
  */
void Thread_StatusBar()
{
    mtm_StatusBar.Running(millis());
}
