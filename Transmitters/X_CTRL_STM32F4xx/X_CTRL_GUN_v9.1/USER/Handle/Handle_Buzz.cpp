#include "FileGroup.h"
#include "MusicPlayer.h"
#include "ComPrivate.h"

using namespace MusicPlayer_Type;

//#define Volume_MAX 101

//static void MusicPlay_Callback(uint32_t freq, uint16_t volume)
//{
//    PIN_MAP[Buzz_Pin].TIMx->PSC = (F_CPU / Volume_MAX / freq) - 1;
//    pwmWrite(Buzz_Pin, volume);
//}

#define PLAY_MUSIC(mc) player.Play(mc,__Sizeof(mc))
#define ADD_MUSIC(mc) {mc,__Sizeof(mc)}

/*����Ƶ�ʶ�Ӧ��*/
typedef enum
{
    L1 = 262,
    L1h = 277,
    L2 = 294,
    L2h = 311,
    L3 = 330,
    L4 = 349,
    L4h = 370,
    L5 = 392,
    L5h = 415,
    L6 = 440,
    L6h = 466,
    L7 = 494,
    M1 = 523,
    M1h = 554,
    M2 = 587,
    M2h = 622,
    M3 = 659,
    M4 = 698,
    M4h = 740,
    M5 = 784,
    M5h = 831,
    M6 = 880,
    M6h = 932,
    M7 = 988,
    H1 = 1046,
    H1h = 1109,
    H2 = 1175,
    H2h = 1245,
    H3 = 1318,
    H4 = 1397,
    H4h = 1480,
    H5 = 1568,
    H5h = 1661,
    H6 = 1760,
    H6h = 1865,
    H7 = 1976,
} Tone_TypeDef;

/*ʵ�������ֲ�����*/
static MusicPlayer player(Buzz_Pin);

/*������ʹ�ܿ���*/
bool State_BuzzSound = ON;

/*���ֲ�����״ָ̬ʾ*/
static bool State_MusicPlayer = OFF;

/*������*/
static const MusicCode_t MC_StartUp[] =
{
    {M1, 80, 100},
    {M6, 80, 100},
    {M3, 80, 100},
    0
};

/*������ʾ��*/
static const MusicCode_t MC_Error[] =
{
    {100, 80, 100},
    {0,   80, 100},
    {100, 80, 100},
    0
};

/*���ӳɹ���*/
static const MusicCode_t MC_Connect[] =
{
    {H1, 80, 100},
    {H2, 80, 100},
    {H3, 80, 100},
    0
};

/*�Ͽ�������*/
static const MusicCode_t MC_Disconnect[] =
{
    {H3, 80, 100},
    {H2, 80, 100},
    {H1, 80, 100},
    0
};

/*�źŲ��ȶ���ʾ��*/
static const MusicCode_t MC_UnstableConnect[] =
{
    {H1, 80, 100},
    {0, 80, 100},
    {H1, 80, 100},
    0
};

/*���������ʾ��*/
static const MusicCode_t MC_BattChargeStart[] =
{
    {L1, 80, 100},
    {L3, 80, 100},
    0
};

/*���ر���ʾ��*/
static const MusicCode_t MC_BattChargeEnd[] =
{
    {L3, 80, 100},
    {L1, 80, 100},
    0
};

/*�豸������ʾ��*/
static const MusicCode_t MC_DeviceInsert[] =
{
    /*C4,A3,F3,F4*/
    {M1, 180, 100},
    {L6, 80, 100},
    {L4, 80, 100},
    {M4, 160, 100},
    0
};

/*�豸�γ���ʾ��*/
static const MusicCode_t MC_DevicePullout[] =
{
    /*A4,F4,E4*/
    {L6, 80, 100},
    {L4, 80, 100},
    {L3, 80, 100},
    0
};

/*�޲�����ʾ��*/
static const MusicPlayer_Type::MusicCode_t MC_NoOperationWarning[] = {
    {4000, 40, 100},
    {0, 80, 100},
    {4000, 40, 100},
    {0, 80, 100},
    {4000, 40, 100},
    0
};

/*�б��������Ͷ���*/
typedef struct
{
    const MusicCode_t *mc;
    uint16_t length;
} MC_List_t;

/*��ʾ����ַ�����б�*/
static const MC_List_t MC_List[MC_Type::MC_MAX] =
{
    ADD_MUSIC(MC_StartUp),
    ADD_MUSIC(MC_Error),
    ADD_MUSIC(MC_Connect),
    ADD_MUSIC(MC_Disconnect),
    ADD_MUSIC(MC_UnstableConnect),
    ADD_MUSIC(MC_BattChargeStart),
    ADD_MUSIC(MC_BattChargeEnd),
    ADD_MUSIC(MC_DeviceInsert),
    ADD_MUSIC(MC_DevicePullout),
    ADD_MUSIC(MC_NoOperationWarning)
};

/*�ź�״ָ̬ʾ�ı��¼�*/
static void SignalChangeEvent(uint8_t isConnect)
{
    if(isConnect == 2)
    {
        BuzzMusic(MC_Type::MC_Connect);
        XFS_Speak("������");
        XFS_Speak("�ź�ǿ��%d%%", NRF_SignalStrength);
    }
    else if(isConnect == 1)
    {
        BuzzMusic(MC_Type::MC_UnstableConnect);
        XFS_Speak("����:�źŲ��ȶ�");
        XFS_Speak("�ź�ǿ��%d%%", NRF_SignalStrength);
    }
    else
    {
        BuzzMusic(MC_Type::MC_Disconnect);
        XFS_Speak("���ӶϿ�");
    }
}

/**
  * @brief  ��ʼ��������
  * @param  ��
  * @retval ��
  */
static void Init_Buzz()
{
    toneSetTimer(TIM_BuzzTone);
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
    __ExecuteOnce(Init_Buzz());
    
    State_MusicPlayer = player.Running(millis());
}

/**
  * @brief  �ź�״̬��������
  * @param  ��
  * @retval ��
  */
void Task_SignalMonitor()
{
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
    if(!State_BuzzSound) //�Ƿ�ʹ�ܷ�����
        return;

    if(music >= MC_Type::MC_MAX) //��������Ƿ�Խ��
        return;

    /*��������������*/
    player.Play(MC_List[music].mc, MC_List[music].length);
}

/**
  * @brief  �����������ָ��Ƶ�ʺͳ���ʱ��ķ����ź�(������ʽ)
  * @param  freq:Ƶ��(Hz)
  * @param  time:����ʱ��(ms)
  * @retval ��
  */
void BuzzTone(uint32_t freq, uint32_t time)
{
    if(!State_BuzzSound)
        return;

    tone(Buzz_Pin, freq, time);
}
