/** @Author: _VIFEXTech
  * @Describe: <ElsCtrl> Electric long skateboard wireless controller, base on <X-CTRL> Framework. 
  * @Finished: 2019.3.26 v1.0 ӵ�л�����ͨ�Ź����Լ��򵥵�GUIҳ�棬֧�ֶ���Ѳ��
  * @Upgrade:  2019.3.28 v1.1 Ϊҡ������˲�����֧��ҡ��У׼�����籣�����ݣ��Զ�Ϩ��
  * @Upgrade:  2019.4.3  v1.2 ��Ϩ��ʱ����һ���˳�����Ѳ��ģʽ���Ż���ѡ�ӻ��б������ʾ
  * @Upgrade:  2019.4.8  v1.3 �Ż���������������ݼ����������޸�Ϊ����ģʽ�����ֳɹ���ע��������ֵ�ַƵ����Ϣ�������ڲ�FLASH��
  * @Upgrade:  2019.6.27 v1.4 ��Ϩ�����Զ��ر�ˢ����ʱ���Խ�ʡ����,���³�ʱ���޲�����ʾ��
  * @Upgrade:  2019.7.7  v1.5 ����ٶȺ;���ش�
  * @Upgrade:  2019.7.8  v1.6 �����˲���
  */
  
#include "FileGroup.h"

static MillisTaskManager ControlTask(3, true);
static void Thread_Control();

static void SystemSetup()
{
    Init_ElsCtrl();
    
    Timer_Init(TIM_DisplayThread, 25 * 1000/*25ms@40FPS*/, Task_ScreenDisplay, 2, 0);
    TIM_Cmd(TIM_DisplayThread, ENABLE);

    ControlTask.TaskRegister(0, Task_TransferData, 10);
    ControlTask.TaskRegister(1, Task_SensorUpdate, 10);
    ControlTask.TaskRegister(2, Task_MusicPlayerRunning, 20);    
    Timer_Init(TIM_ControlThread, 1000, Thread_Control, 1, 0);    
    TIM_Cmd(TIM_ControlThread, ENABLE);

    Init_GUI();
}

static void Thread_Main()
{
    Thread_GUI();
}

static void Thread_Control()
{
    ControlTask.Running(millis());
}


/**
  * @brief  Main Function
  * @param  None
  * @retval None
  */
int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    GPIO_JTAG_Disable();
    Delay_Init();
//    ADCx_Init(ADC1);
    ADCx_DMA_Init();
    SystemSetup();
    for(;;)Thread_Main();
}
