#include "Basic/FileGroup.h"
#include "MusicPlayer/MusicPlayer.h"

using namespace MusicPlayer_Type;

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

static void MusicPlayer_Callback(uint32_t freq, uint16_t volume)
{
//    tone(Buzz_Pin, freq);
    Motor_Tone(freq);
}

/*ʵ�������ֲ�����*/
static MusicPlayer player(MusicPlayer_Callback);

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

/**
  * @brief  ��ʼ��������
  * @param  ��
  * @retval ��
  */
void Buzz_Init()
{
    DEBUG_FUNC_LOG();
}

/**
  * @brief  ���ֲ�������
  * @param  ��
  * @retval ��
  */
void Buzz_Update()
{
    player.Running(millis());
}

/**
  * @brief  ���ֲ���
  * @param  music:���ֱ��
  * @retval ��
  */
void Buzz_PlayMusic(uint8_t music)
{
    if(!CTRL.State.Sound) //�Ƿ�ʹ�ܷ�����
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
void Buzz_Tone(uint32_t freq, uint32_t time)
{
    if(!CTRL.State.Sound)
        return;
    
    Motor_Tone(freq, time);
}
