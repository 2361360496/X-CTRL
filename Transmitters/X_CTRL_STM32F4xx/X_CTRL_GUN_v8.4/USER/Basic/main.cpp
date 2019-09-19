#include "FileGroup.h"

/*�����������ȼ������*/
enum TaskPriority
{
    TP_ClacSystemUsage,
    TP_SensorUpdate,
    TP_TransferData,
    TP_MPU6050Read,
    TP_MotorRunning,
    TP_MusicPlayerRunning,
    TP_XFS_ListCheck,
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
    if(State_DisplayCPU_Usage)
        CPU_Usage = ControlTask.GetCPU_Usage();
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
    ControlTask.TaskRegister(TP_SensorUpdate,       Task_SensorUpdate,          10);    //��������ȡ����ִ������10ms
    ControlTask.TaskRegister(TP_TransferData,       Task_TransferData,          10);    //���ݷ�������ִ������10ms
    ControlTask.TaskRegister(TP_MPU6050Read,        Task_MPU6050Read,           20);    //��̬��������ִ������20ms
    ControlTask.TaskRegister(TP_MotorRunning,       Task_MotorRunning,          10);    //���������ִ������10ms
    ControlTask.TaskRegister(TP_MusicPlayerRunning, Task_MusicPlayerRunning,    20);    //���ֲ�������ִ������20ms
    ControlTask.TaskRegister(TP_XFS_ListCheck,      Task_XFS_ListCheck,         500);   //�����ϳɶ���ɨ���������ȼ�4��ִ������500ms
    ControlTask.TaskRegister(TP_ClacSystemUsage,    Task_ClacCPU_Usage,         1000);
    Timer_Init(TIM_ControlTask, 1000, Thread_Control, 1, 1);  //��������(�����߳�)��Ӳ����ʱ���󶨣�ʱ��Ƭ1ms�������ȼ�1�������ȼ�1
    TIM_Cmd(TIM_ControlTask, ENABLE);                         //��ʱ��ʹ��

    Init_GUI(0); //LOGO + HMI

    if(Init_SD())
        Init_BvPlayer();

    if(State_LuaScript)
        Init_LuaScript();
    
    Init_GUI(1); //GUI��ʼ��
}

/**
  * @brief  ���߳�
  * @param  ��
  * @retval ��
  */
static void Thread_Main()
{
    Thread_GUI();//GUI�߳�
    //Thread_HMI();//HMI�߳�
    Thread_SD_Monitor();//SD�Ȳ�μ���߳�
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
    SystemSetup();                                  //ϵͳ��ʼ��
    for(;;)Thread_Main();                           //���߳�ִ��
}
