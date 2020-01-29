#include "FileGroup.h"

/*�����������ȼ������*/
enum TaskPriority
{
    TP_TransferData,
    TP_ClacSystemUsage,
    TP_SensorUpdate,
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
static void Task_ClacSystemUsage()
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
    Serial.begin(115200);
    Serial.println(_X_CTRL_NAME","_X_CTRL_VERSION","__DATE__","__TIME__);
    Init_X_CTRL();  //��ʼ��ң����

    /*������������ע��*/
    ControlTask.TaskRegister(TP_TransferData,       Task_TransferData,          10);    //���ݷ�������ִ������10ms
    ControlTask.TaskRegister(TP_SensorUpdate,       Task_SensorUpdate,          10);    //��������ȡ����ִ������10ms
    ControlTask.TaskRegister(TP_MPU6050Read,        Task_MPU6050Read,           20);    //��̬��������ִ������20ms
    ControlTask.TaskRegister(TP_MotorRunning,       Task_MotorRunning,          10);    //���������ִ������10ms
    ControlTask.TaskRegister(TP_MusicPlayerRunning, Task_MusicPlayerRunning,    20);    //���ֲ�������ִ������20ms
    ControlTask.TaskRegister(TP_XFS_ListCheck,      Task_XFS_ListCheck,         500);   //�����ϳɶ���ɨ������ִ������500ms
    ControlTask.TaskRegister(TP_ClacSystemUsage,    Task_ClacSystemUsage,       1000);
    Timer_SetInterruptBase(TIM_ControlTask, 10, 10, Thread_Control, 1, 1);  //��������(�����߳�)��Ӳ����ʱ���󶨣������ȼ�1�������ȼ�1
    Timer_SetInterruptTimeUpdate(TIM_ControlTask, 1000); //ʱ��Ƭ1ms
    TIM_Cmd(TIM_ControlTask, ENABLE);//��ʱ��ʹ��

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
    Delay_Init();
    SystemSetup();                                  //ϵͳ��ʼ��
    for(;;)Thread_Main();                           //���߳�ִ��
}
