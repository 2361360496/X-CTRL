#include "FileGroup.h"
#include "MusicPlayer.h"
#include "ComPrivate.h"

//#define Volume_MAX 101

//static void MusicPlay_Callback(uint32_t freq, uint16_t volume)
//{
//    PIN_MAP[Buzz_Pin].TIMx->PSC = (F_CPU / Volume_MAX / freq) - 1;
//    pwmWrite(Buzz_Pin, volume);
//}

/*ʵ�������ֲ�����*/
static MusicPlayer player(Buzz_Pin);

/*������ʹ�ܿ���*/
bool State_BuzzSound = OFF;

/*���ֲ�����״ָ̬ʾ*/
static bool State_MusicPlayer = OFF;

/*������*/
static const MusicPlayer_Type::MusicCode_t StartUpMc[] = {
    {523, 80, 100},
    {880, 80, 100},
    {659, 80, 100},
    0
};

/*������ʾ��*/
static const MusicPlayer_Type::MusicCode_t ErrorMc[] = {
    {100, 80, 100},
    {0,   80, 100},
    {100, 80, 100},
    0
};

/*���ӳɹ���*/
static const MusicPlayer_Type::MusicCode_t ConnectMc[] = {
    {1046, 80, 100},
    {1175, 80, 100},
    {1318, 80, 100},
    0
};

/*�Ͽ�������*/
static const MusicPlayer_Type::MusicCode_t DisconnectMc[] = {
    {1318, 80, 100},
    {1175, 80, 100},
    {1046, 80, 100},
    0
};

/*�źŲ��ȶ���ʾ��*/
static const MusicPlayer_Type::MusicCode_t UnstableConnectMc[] = {
    {1046, 80, 100},
    {0, 80, 100},
    {1046, 80, 100},
    0
};

/*�ź�״ָ̬ʾ�ı��¼�*/
static void SignalChangeEvent(uint8_t isConnect)
{
    if(isConnect == 2)
    {
        player.Play(ConnectMc, __Sizeof(ConnectMc));//���ֲ���
        XFS_Speak("������");
        XFS_Speak("�ź�ǿ��%d%%", NRF_SignalStrength);
    }
    else if(isConnect == 1)
    {
        player.Play(UnstableConnectMc, __Sizeof(UnstableConnectMc));
        XFS_Speak("����:�źŲ��ȶ�");
        XFS_Speak("�ź�ǿ��%d%%", NRF_SignalStrength);
    }
    else
    {
        player.Play(DisconnectMc, __Sizeof(DisconnectMc));
        XFS_Speak("���ӶϿ�");
    }
}

/**
  * @brief  ��ʼ��������
  * @param  ��
  * @retval ��
  */
void Init_Buzz()
{
    toneSetTimer(TIM1);
    pinMode(Buzz_Pin, OUTPUT);
    digitalWrite(Buzz_Pin, LOW);
//    PWM_Init(Buzz_Pin, Volume_MAX, 1000);
//    pwmWrite(Buzz_Pin, 0);
}

/**
  * @brief  ���ֲ�������
  * @param  ��
  * @retval ��
  */
void Task_MusicPlayerRunning()
{
    State_MusicPlayer = player.Running(millis());
}

/**
  * @brief  �ź�״̬��������
  * @param  ��
  * @retval ��
  */
void Task_SignalMonitor()
{
    if(!State_BuzzSound)//�Ƿ�ʹ�ܷ�����
        return;
    
    /*����״ָ̬ʾ*/
    static uint8_t ConnectState = 0;
    
    /*��һ�ε�NRF�ź�ǿ��*/
    static int16_t NRF_SignalStrength_Last = 0;
    
    /*���źŲ��ȶ�ʱ��ʱ���*/
    static uint32_t UntableSignalStrength_TimePoint = 0;
    
    /*�ж��ź��Ƿ��ȶ�*/
    if(ABS(NRF_SignalStrength - NRF_SignalStrength_Last) > 10 && NRF_SignalStrength < 90)
    {
        ConnectState = 1;
        UntableSignalStrength_TimePoint = millis();
        NRF_SignalStrength_Last = NRF_SignalStrength;
    }

    if(NRF_SignalStrength < 10 && millis() - UntableSignalStrength_TimePoint > 3000)
        ConnectState = 0;
    else if(NRF_SignalStrength > 90 && millis() - UntableSignalStrength_TimePoint > 5000)
        ConnectState = 2;

    if(!State_MusicPlayer)
    {
        /*���ConnectState����������ı�ʱ����SignalChangeEvent�¼�*/
        __ValueMonitor(ConnectState, SignalChangeEvent(ConnectState));
    }
}

/**
  * @brief  ���ֲ���
  * @param  music:���ֱ��
  * @retval ��
  */
void BuzzMusic(uint8_t music)
{
    if(!State_BuzzSound)//�Ƿ�ʹ�ܷ�����
        return;

    switch (music)
    {
    case 0:
        player.Play(StartUpMc, __Sizeof(StartUpMc));
        break;
    case 1:
        player.Play(ErrorMc, __Sizeof(ErrorMc));
        break;

    case 2:
        player.Play(ConnectMc, __Sizeof(ConnectMc));
        break;

    case 3:
        player.Play(UnstableConnectMc, __Sizeof(UnstableConnectMc));
        break;
    
    case 4:
        player.Play(DisconnectMc, __Sizeof(DisconnectMc));
        break;
    }
}

/**
  * @brief  �����������ָ��Ƶ�ʺͳ���ʱ��ķ����ź�(������ʽ)
  * @param  freq:Ƶ��(Hz)
  * @param  time:����ʱ��(ms)  
  * @retval ��
  */
void BuzzTone(uint32_t freq, uint32_t time)
{
    if(!State_BuzzSound)return;
    tone(Buzz_Pin, freq, time);
}
