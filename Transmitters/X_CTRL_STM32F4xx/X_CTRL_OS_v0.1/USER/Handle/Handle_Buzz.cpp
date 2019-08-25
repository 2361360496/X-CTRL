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
bool State_BuzzSound = ON;

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

/**
  * @brief  ��ʼ��������
  * @param  ��
  * @retval ��
  */
void Init_Buzz()
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
    State_MusicPlayer = player.Running(millis());
}

/**
  * @brief  ���ֲ���
  * @param  music:���ֱ��
  * @retval ��
  */
void BuzzMusic(uint8_t music)
{	
#define PLAY(m) player.Play(m, __Sizeof(m))
	
    if(!State_BuzzSound)//�Ƿ�ʹ�ܷ�����
        return;

    switch (music)
    {
    case 0:
        PLAY(StartUpMc);
        break;
    case 1:
        PLAY(ErrorMc);
        break;

    case 2:
        PLAY(ConnectMc);
        break;

    case 3:
		PLAY(UnstableConnectMc);
        break;
    
    case 4:
        PLAY(DisconnectMc);
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
