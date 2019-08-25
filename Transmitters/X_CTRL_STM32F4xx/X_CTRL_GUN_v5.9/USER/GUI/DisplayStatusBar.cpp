#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

/*״̬��ʱ��Ƭ���������������ȼ�*/
static MillisTaskManager mtm_StatusBar(3, true);

/*��ص�����ʾ�ؼ�*/
static LightGUI::ProgressBar<SCREEN_CLASS> PB_Batt(&screen, 0);

bool State_DisplayCPU_Usage = false;

#define TextPosBase_Y (StatusBar_POS-15)

/**
  * @brief  ��ص�����ʾ��������
  * @param  ��
  * @retval ��
  */
static void Task_DrawBattUsage()
{
    static uint8_t usage;//��ʾ�ĵ�ص���

    __ValueCloseTo(usage, BattUsage, 1);//����ʾ�ĵ�������ʵ�ʵĵ������ﵽ������Ч��
    
    if(digitalRead(BattCharge_Pin) == LOW && usage == (uint8_t)BattUsage)
    {
        usage = 0;
    }

    if(usage < 30)
        PB_Batt.Color_PB = screen.Red;//����<30%�����Ϊ��ɫ
    else if(usage >= 90)
        PB_Batt.Color_PB = screen.Green;//����>90%�����Ϊ��ɫ
    else
        PB_Batt.Color_PB = screen.White;//�������������Ϊ��ɫ

    PB_Batt.setProgress(usage / 100.0);//���¿ؼ�
    
    screen.drawFastVLine(
        PB_Batt.X + PB_Batt.Width,
        PB_Batt.Y + 2,
        PB_Batt.Height - 4,
        screen.White
    );
}

/**
  * @brief  NRF�շ�״̬��ʾ����
  * @param  ��
  * @retval ��
  */
static void Task_DrawNrfTxRxState()
{
    screen.fillRect(69, StatusBar_POS - 10, 5, 7, screen.Black);
    if(page.NowPage == PAGE_CtrlInfo)
    {
        //screen.drawFastVLine(71, 2, 7, screen.White);
        if(nrf.GetTxRxState() == nrf.RX_MODE)
        {
            screen.drawLine(71, StatusBar_POS - 4, 71 - 2, StatusBar_POS - 4 - 2, screen.White);
            screen.drawLine(71, StatusBar_POS - 4, 71 + 2, StatusBar_POS - 4 - 2, screen.White);
        }
        else //(nrf.GetTxRxState() == nrf.TX_MODE)
        {
            screen.drawLine(71, StatusBar_POS - 10, 71 - 2, StatusBar_POS - 10 + 2, screen.White);
            screen.drawLine(71, StatusBar_POS - 10, 71 + 2, StatusBar_POS - 10 + 2, screen.White);
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
    if(State_DisplayCPU_Usage)
    {
        if(CPU_Usage > 90.0f)
            screen.setTextColor(screen.Red, screen.Black);
        else if(CPU_Usage > 50.0f)
            screen.setTextColor(screen.Yellow, screen.Black);
        else if(CPU_Usage > 10.0f)
            screen.setTextColor(screen.White, screen.Black);
        else 
            screen.setTextColor(screen.Green, screen.Black);
        
        screen.setCursor(10, StatusBar_POS + 2);
        screen.printfX("CPU:%0.2f%%", CPU_Usage);
    }   
    
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

    screen.setCursor(2, TextPosBase_Y);
    screen.printfX("%3d%%   ", NRF_SignalStrength);

    /* ��ʾ��������״̬ */
    screen.setCursor(33, TextPosBase_Y);
    if(State_Bluetooth)
    {
        if(hc05.GetState())
            screen.setTextColor(screen.Green, screen.Black);
        else
            screen.setTextColor(screen.Yellow, screen.Black);

        screen.printfX(StrBtc[Bluetooth_ConnectObject]);
    }
    else
        screen.fillRect(33, StatusBar_POS - 16, 20, 16, screen.Black);

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
        screen.setTextColor(screen.Red, screen.Black);
    }
    screen.setCursor(56, TextPosBase_Y);
    screen.printfX("RF");

    /* ��ʾ��ص�ѹ */
    screen.setCursor(78, TextPosBase_Y);
    screen.setTextSize(0);
    screen.setTextColor(screen.White, screen.Black);
    screen.printfX("%1.1fV", BattVoltage);

//    /* ����һ��ʱ��󽵵͵�ص���ˢ���� */
//    if(millis() > 3000)
//        mtm_StatusBar.TaskSetIntervalTime((uint8_t)0, 500);
}

/**
  * @brief  ��ʼ��״̬��
  * @param  ��
  * @retval ��
  */
void Init_StatusBar()
{
    /*״̬����£����*/
    for(float i = 0.0f; i < 1.0f; i += 0.001f)
    {
        screen.drawFastHLine(0, StatusBar_POS, screen.width() / 2 * i, screen.White);
        screen.drawFastHLine(screen.width() - screen.width() / 2 * i - 1, StatusBar_POS, screen.width() / 2 * i, screen.White);
    }
    
    pinMode(BattCharge_Pin, INPUT_PULLUP);

    /*��ص�����ʾ�ؼ���������*/
    PB_Batt.X = 105;
    PB_Batt.Y = StatusBar_POS - 12;
    PB_Batt.Width = 20;
    PB_Batt.Height = 10;

    /*����ע��*/
    mtm_StatusBar.TaskRegister(0, Task_DrawBattUsage, 20);
    mtm_StatusBar.TaskRegister(1, Task_1000msUpdate, 1000);
    mtm_StatusBar.TaskRegister(2, Task_DrawNrfTxRxState, 123);
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
