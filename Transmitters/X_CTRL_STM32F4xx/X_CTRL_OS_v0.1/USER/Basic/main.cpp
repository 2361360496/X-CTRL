/** @Author: _VIFEXTech
  * @Describe: Time shareing operating system, base on <X-CTRL> Framework.
  */

#include "FileGroup.h"

/*�����������ȼ������*/
enum TaskPriority {
    TP_ClacSystemUsage,
    TP_SensorUpdate,
    TP_MotorRunning,
    TP_MusicPlayerRunning,
    TP_MAX
};

/*ʵ���������������󣬿������ȼ�*/
static MillisTaskManager ControlTask(TP_MAX, true);

/*�����߳�*/
static void Thread_Control();

/*����CPUռ�����*/
float CPU_Usage;
static void Task_ClacCPU_Usage()
{
    CPU_Usage = ControlTask.GetCPU_Usage() * 100;
}

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
    ControlTask.TaskRegister(TP_MotorRunning,       Task_MotorRunning,          10);    //���������ִ������10ms
    ControlTask.TaskRegister(TP_MusicPlayerRunning, Task_MusicPlayerRunning,    20);    //���ֲ������񣬣�ִ������20ms
    ControlTask.TaskRegister(TP_ClacSystemUsage,    Task_ClacCPU_Usage,     	1000);
    Timer_Init(TIM_ControlTask, 1000, Thread_Control, 1, 1);  //��������(�����߳�)��Ӳ����ʱ���󶨣�ʱ��Ƭ1ms�������ȼ�1�������ȼ�1
    TIM_Cmd(TIM_ControlTask, ENABLE);                         //��ʱ��ʹ��

    Init_GUI(); //GUI��ʼ��

    Init_SD();
    
    if(State_LuaScript)
        Init_LuaScript();
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
    Delay_Init();                              		
    ADCx_DMA_Init();                               //ADC DMA ��ʼ��
    //ADCx_Init(ADC1);
    SystemSetup();                                  //ϵͳ��ʼ��
    for(;;)Thread_Main();                           //���߳�ִ��
}
