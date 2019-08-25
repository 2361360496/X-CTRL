/** @Author: _VIFEXTech
  * @Describtion: ��ͨ��ң�������򣬻���X-CTRL���
  */

#include "FileGroup.h"

/*�����������ȼ������*/
enum TaskPriority{
    TP_SensorUpdate,
    TP_TransferData,
    TP_MusicPlayerRunning,
    TP_MAX
};

/*����������ʱ��*/
#define ControlTask_Timer TIM2

/*��ʱ��Ƭ���������������ȼ�*/
static MillisTaskManager ControlTask(TP_MAX, true);

/*�����߳�*/
static void Thread_Control();

/**
  * @brief  ϵͳ��ʼ��
  * @param  ��
  * @retval ��
  */
static void SystemSetup()
{
    Init_X_CTRL();  //��ʼ��ң����

    /*������������ע��*/
    ControlTask.TaskRegister(TP_SensorUpdate,       Task_SensorUpdate,          5);     //��������ȡ����ִ������5ms
    ControlTask.TaskRegister(TP_TransferData,       Task_TransferData,          10);    //���ݷ�������ִ������10ms
    ControlTask.TaskRegister(TP_MusicPlayerRunning, Task_MusicPlayerRunning,    20);    //���ֲ�������ִ������20ms
    
    Timer_Init(TIM3, 25*1000, Task_ScreenDisplay, 2, 0);        //��Ļ��������,25ms��������@40fps
    TIM_Cmd(TIM3, ENABLE);
    
    Timer_Init(ControlTask_Timer, 1000, Thread_Control, 1, 1);  //��������(�����߳�)��Ӳ����ʱ���󶨣�ʱ��Ƭ1ms�������ȼ�1�������ȼ�1
    TIM_Cmd(ControlTask_Timer, ENABLE);                         //��ʱ��ʹ��

    Init_GUI(); //GUI��ʼ��
}

/**
  * @brief  ���߳�
  * @param  ��
  * @retval ��
  */
static void Thread_Main()
{
    Thread_GUI();//GUI�߳�
}

/**
  * @brief  �����߳�(��������)
  * @param  ��
  * @retval ��
  */
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
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //NVIC�ж�������
    GPIO_JTAG_Disable();                            //�ر�JTAG
    ADCx_DMA_Init();                                //ADC DMA ��ʼ��
    Delay_Init();
    SystemSetup();                                  //ϵͳ��ʼ��
    for(;;)Thread_Main();                           //���߳�ִ��
}
