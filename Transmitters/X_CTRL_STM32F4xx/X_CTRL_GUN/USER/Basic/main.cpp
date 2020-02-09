#include "FileGroup.h"

/*ʵ���������������󣬿������ȼ�*/
MillisTaskManager MainTask(TP_MAX, true);

/*���߳�*/
static void MainTask_Process();

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
    MainTask.TaskRegister(TP_TransferData,       Task_TransferData,          10);    //���ݷ�������ִ������10ms
    MainTask.TaskRegister(TP_SensorUpdate,       Task_SensorUpdate,          10);    //��������ȡ����ִ������10ms
    MainTask.TaskRegister(TP_IMU_Process,        Task_IMU_Process,           20);    //��̬��������ִ������20ms
    MainTask.TaskRegister(TP_MotorRunning,       Task_MotorRunning,          10);    //���������ִ������10ms
    MainTask.TaskRegister(TP_MusicPlayerRunning, Task_MusicPlayerRunning,    20);    //���ֲ�������ִ������20ms
    MainTask.TaskRegister(TP_XFS_ListCheck,      Task_XFS_ListCheck,         500);   //�����ϳɶ���ɨ������ִ������500ms
    MainTask.TaskRegister(TP_CPUInfoUpdate,      Task_CPUInfoUpdate,         1000);
    Timer_SetInterruptBase(TIM_ControlTask, 10, 10, MainTask_Process, 1, 1);  //��������(�����߳�)��Ӳ����ʱ���󶨣������ȼ�1�������ȼ�1
    Timer_SetInterruptTimeUpdate(TIM_ControlTask, 1000); //ʱ��Ƭ1ms
    TIM_Cmd(TIM_ControlTask, ENABLE);//��ʱ��ʹ��

    Init_GUI(0); //LOGO

    if(Init_SD())
        Init_BvPlayer();

    if(CTRL.State.LuaScript)
        Init_LuaScript();
    
    Init_GUI(1); //GUI��ʼ��
}

/**
  * @brief  ���߳�(��������)
  * @param  ��
  * @retval ��
  */
static void MainTask_Process()
{
    MainTask.Running(millis());
}

/**
  * @brief  ���߳�
  * @param  ��
  * @retval ��
  */
static void SubTask_Process()
{
    Thread_GUI();//GUI�߳�
    Thread_SD_Monitor();//SD�Ȳ�μ���߳�
}

/**
  * @brief  Main Function
  * @param  None
  * @retval None
  */
int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();
    SystemSetup();
    for(;;)SubTask_Process();
}
