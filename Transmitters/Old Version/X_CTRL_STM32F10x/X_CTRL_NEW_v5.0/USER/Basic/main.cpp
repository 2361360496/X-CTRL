/** @Author: _VIFEXTech
  * @Describe: <X-CTRL> A brand new high performance digital wireless remote control system.
  * @Finished: 2018.10.6  - v1.0 ȫ�µ��������ϵͳ���¼����������Լ�GUI���
  * @Upgrade:  2018.10.12 - v1.1 ����HMI
  * @Upgrade:  2018.10.14 - v1.2 ���ӽ��������Լ�HMI����
  * @Upgrade:  2018.10.19 - v1.3 �����������ӿ���
  * @Upgrade:  2018.10.23 - v1.4 �޸���������Ӧ��ʧ�ܵ�BUG
  * @Upgrade:  2018.10.26 - v1.5 ����MPU6050��̬���㣬����������Ӧ��Ӧ�ٶ�
  * @Upgrade:  2018.11.17 - v1.6 ͬ��RC-CTRL���룬֧��MPU��������ҡ�ˣ����HMI���ؿ��ƣ�����GUI��Ŀ���
  * @Upgrade:  2018.11.21 - v1.7 �Ż�HMI����
  * @Upgrade:  2018.11.23 - v1.8 MainMenu���붯������Ч����֧�ֵ���NRF��ַ������ʧ���ٽ羯��
  * @Upgrade:  2018.11.26 - v1.9 �޸�������ʾ��
  * @Upgrade:  2018.11.28 - v2.0 ���ͨ�ûش�Э��,�ϲ�Task_LoadCtrlMode �� Task_TransferData
  * @Upgrade:  2018.11.30 - v2.1 ֧������NRF��Ƶ������������
  * @Upgrade:  2018.12.17 - v2.2 ��Ƶ��112MHz
  * @Upgrade:  2018.12.18 - v2.3 �����ⲿEEPROM��ʹ��FLASH�������ò�������Ƶ��128MHz
  * @Upgrade:  2018.12.23 - v2.4 ���°����¼�������
  * @Upgrade:  2018.12.29 - v2.5 ���Handshake����Э��
  * @Upgrade:  2019.1.26  - v2.6 ���µײ��
  * @Upgrade:  2019.2.5   - v2.7 ʹ�ø����ٵ�ˢͼ����
  * @Upgrade:  2019.2.10  - v2.8 ������Ļ����������GUI�������϶�
  * @Upgrade:  2019.2.11  - v2.9 ����MenuManager�˵������ܣ��򻯲˵����߼���ƣ���Ӳ������ؿؼ�������ת�������¼�����
  * @Upgrade:  2019.2.15  - v3.0 ����XFS����ģ��֧�֣��Ƴ�������Ϊң�ص�ѡ��
  * @Upgrade:  2019.2.16  - v3.1 ���ģ����DigitalFilter��FifoQueue��֧�ַ���
  * @Upgrade:  2019.2.17  - v3.2 ������룬�Ż�������붯����Handshakeҳ����ӹ������ؼ�
  * @Upgrade:  2019.2.18  - v3.3 ����GUI�ؼ���(LightGUI)
  * @Upgrade:  2019.2.20  - v3.4 ���ӷ�����ʽPageDelay
  * @Upgrade:  2019.2.21  - v3.5 �޸��ش���Ϣ��������ʾ�߳�������BUG
  * @Upgrade:  2019.2.28  - v3.6 ����CommonMacroͨ�ú궨���
  * @Upgrade:  2019.2.28  - v3.7 �Ż����ֳ�ʱ����
  * @Upgrade:  2019.3.8   - v3.8 �Ż�IMU����������������GUI�����϶�
  * @Upgrade:  2019.3.9   - v3.9 ���ʡ��ģʽ
  * @Upgrade:  2019.3.15  - v4.0 ����NRF�⣬���������ֺ��ѽ�ֹ�޸�Ƶ�ʺ�ͨ���ٶȣ��޸��˳����ֺ�NRF��Ȼ������BUG
  * @Upgrade:  2019.3.16  - v4.1 �޸���������״̬�жϵ�BUG
  * @Upgrade:  2019.3.20  - v4.2 �����ŵ����ݴ��淽ʽ
  * @Upgrade:  2019.3.21  - v4.3 ����MTM�⣬֧���趨���ȼ�
  * @Upgrade:  2019.3.22  - v4.4 ����NRF�⣬��ǿͨ�Ż����ϲ�ʱ��ͨ���ȶ��ԣ�����NRF�շ�ģʽ��ʾ
  * @Upgrade:  2019.4.1   - v4.5 ����ҳ��������Լ�LightGUI�⣬������Ƶ���в�����
  * @Upgrade:  2019.4.16  - v4.6 �����ܿ�ܣ����ע�ͣ�����NRF������ͨ���ȶ���
  * @Upgrade:  2019.4.29  - v4.7 ���CPUʹ����
  * @Upgrade:  2019.6.22  - v4.8 ����Searching������֧�ֵ��籣��������Ϣ
  * @Upgrade:  2019.8.22  - v4.9 ���Ƶ��ɨ�蹦�ܣ��޸�Searching����BUG���޸�CPUռ������ʾ����BUG���Ż����ò˵�
  * @Upgrade:  2019.12.24 - v5.0 ����NRF��XFS���ײ�� 
  */

#include "FileGroup.h"

/*�����������ȼ������*/
enum TaskPriority{
    TP_ClacSystemUsage,
    TP_SensorUpdate,
    TP_TransferData,
    TP_MPU6050Read,
    TP_MusicPlayerRunning,
    TP_XFS_ListCheck,
    TP_MAX
};

/*��ʱ��Ƭ���������������ȼ�*/
static MillisTaskManager ControlTask(TP_MAX, true);

/*�����߳�*/
static void Thread_Control();

/*����CPUռ�����*/
float CPU_Usage;
static void Task_ClacCPU_Usage()
{
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
    ControlTask.TaskRegister(TP_SensorUpdate,       Task_SensorUpdate,          5);     //��������ȡ�������ȼ�0��ִ������5ms
    ControlTask.TaskRegister(TP_TransferData,       Task_TransferData,          10);    //���ݷ����������ȼ�1��ִ������10ms
    ControlTask.TaskRegister(TP_MPU6050Read,        Task_MPU6050Read,           20);    //��̬�����������ȼ�2��ִ������20ms
    ControlTask.TaskRegister(TP_MusicPlayerRunning, Task_MusicPlayerRunning,    20);    //���ֲ����������ȼ�3��ִ������20ms
    ControlTask.TaskRegister(TP_XFS_ListCheck,      Task_XFS_ListCheck,         500);   //�����ϳɶ���ɨ���������ȼ�4��ִ������500ms
    ControlTask.TaskRegister(TP_ClacSystemUsage,    Task_ClacCPU_Usage,         1000);
    Timer_Init(TIM_ControlTask, 1000, Thread_Control, 1, 1);  //��������(�����߳�)��Ӳ����ʱ���󶨣�ʱ��Ƭ1ms�������ȼ�1�������ȼ�1
    TIM_Cmd(TIM_ControlTask, ENABLE);                         //��ʱ��ʹ��

    Init_GUI(); //GUI��ʼ��

    XFS_Speak("ϵͳ�Ѿ���"); //����������ϳɶ���
}

/**
  * @brief  ���߳�
  * @param  ��
  * @retval ��
  */
static void Thread_Main()
{
    Thread_GUI();//GUI�߳�
    Thread_HMI();//HMI�߳�
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
    SysClock_Config();                              //����������Ƶ
    SystemSetup();                                  //ϵͳ��ʼ��
    for(;;)Thread_Main();                           //���߳�ִ��
}
