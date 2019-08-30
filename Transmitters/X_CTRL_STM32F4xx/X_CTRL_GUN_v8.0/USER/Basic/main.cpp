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
  * @Upgrade:  2019.4.21  - v4.7 X-CTRL�������ֲ��X-CTRL GUN(STM32F405RGT6)
  * @Upgrade:  2019.4.22  - v4.8 ���Lua��������SD�ļ�ϵͳ��ֲ���
  * @Upgrade:  2019.4.24  - v4.9 ���CPUռ���ʼ���
  * @Upgrade:  2019.4.28  - v5.0 ֧��ģ��XBox360�񶯻ش�
  * @Upgrade:  2019.4.29  - v5.1 ֧���û�����Զ������ݰ���ʽ
  * @Upgrade:  2019.4.30  - v5.2 ��Ӳ˵�ѡ����˸Ч��
  * @Upgrade:  2019.5.1   - v5.3 ֧�ֶ�̬�˵�ͼ�֧꣬�ֿ��������
  * @Upgrade:  2019.5.3   - v5.4 �ڲ�Flash����������֧�֣�������˵����彥����Ч��ADC���ݶ�ȡ����ΪDMAģʽ
  * @Upgrade:  2019.6.18  - v5.5 ���°����¼���
  * @Upgrade:  2019.6.20  - v5.6 ����Searching���������Arduboy��ܣ���Aboutҳ�����ʵ�С��Ϸ
  * @Upgrade:  2019.6.21  - v5.7 ֧��Lua���ư���״̬
  * @Upgrade:  2019.6.22  - v5.8 ֧�ֵ��籣��������Ϣ
  * @Upgrade:  2019.6.23  - v5.9 �޸��������ñ����������ӵ�BUG
  * @Upgrade:  2019.6.28  - v6.0 ֧������ҡ�˷������������
  * @Upgrade:  2019.6.29  - v6.1 ����ͻ��˲������޸���ص���������BUG
  * @Upgrade:  2019.6.30  - v6.2 �޸�ͼ�ο�����Ļ���������������BUG
  * @Upgrade:  2019.7.3   - v6.3 ����UI�ֱ�������Ӧ���������ӿؼ�����ɫЧ��
  * @Upgrade:  2019.7.6   - v6.4 �޸Ķ�̬�˵�Ч��������SD����������Զ��л���ҳ�棬�޸�CPUռ������ʾ����100%��BUG�����CPU�¶���ʾ
  * @Upgrade:  2019.7.9   - v6.5 �޸�����ģ��ͨ��ʧ�ܵ�BUG
  * @Upgrade:  2019.7.13  - v6.6 �޸�ҡ�˸��г̷�����ӳ��б�ʷ�������BUG
  * @Upgrade:  2019.7.15  - v6.7 ��ӳ��ڲ���Ͱγ���ʾ��
  * @Upgrade:  2019.7.16  - v6.8 �Ż�BuzzMusic�Լ������������޸�����ҳ��������޷�����Ӧ��Ļ�ֱ��ʵ�BUG
  * @Upgrade:  2019.7.17  - v6.9 �Ƴ�PowerSaving������PageJump��ǩ
  * @Upgrade:  2019.7.18  - v7.0 ���SD���Ȳ��֧�֣�����豸����γ���ʾ��
  * @Upgrade:  2019.7.19  - v7.1 �޸���ջһ����ADC��ֹͣ���������⣬Ҫ����ʹ��IRAM1�ڴ棡
  * @Upgrade:  2019.7.20  - v7.2 ���Ƶ��ɨ�蹦�ܣ����Ӳ�������֧�֣��޸���������ʱ������δ��յ�BUG
  * @Upgrade:  2019.7.23  - v7.3 ��WString�������sprintf��Ա֧��(���԰�)
  * @Upgrade:  2019.7.26  - v7.4 �Ż����������˳���ʽ��֧�ֲ������˳�
  * @Upgrade:  2019.8.3   - v7.5 ΢��״̬������Ӧ���֣��������غ꣬��������ҳ��
  * @Upgrade:  2019.8.22  - v7.6 �Ż���������ɣ���ӳ�ʱ���޲�������
  * @Upgrade:  2019.8.23  - v7.7 ���µײ�⣬ADC DMA֧��ע��ģʽ
  * @Upgrade:  2019.8.28  - v7.8 Aboutҳ����֧������Ӧ��Ļ�ֱ��ʣ���Ӽ����ļ�����������WAV������
  * @Upgrade:  2019.8.29  - v7.9 �ļ��������֧��ֱ������Lua�ű�������������󱨸�
  * @Upgrade:  2019.8.29  - v8.0 �ļ���������Ӽ�����ʾ����WAV����������ʱ����״̬����֤��Ƶ��ʧ�� 
  */

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

    Init_GUI(1); //GUI��ʼ��

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
    Thread_HMI();//HMI�߳�
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
