#include "FileGroup.h"
#include "FileSystem.h"
#include "GUI_Private.h"
#include "FifoQueue.h"
#include "wav_decoder.h"

/*ʵ������ǰҳ�������*/
static MillisTaskManager mtm_WavPlayer(2);

String WavFilePath;
static bool lastState_BuzzSound;

#define WaveResolution  512
#define FIFO_Size   4096

static uint8_t WavBuffer[FIFO_Size];
static FifoQueue<uint8_t> WavFifo(WavBuffer, FIFO_Size);

static File wavFile;
static WAV_TypeDef wav;
static float WavVolume = 1.0f;

extern "C" {
    void TIM2_IRQHandler()
    {
        TIM_ClearITPendingBit(PIN_MAP[Buzz_Pin].TIMx, TIM_IT_Update);
        if(WavFifo.available() < 8)
        {
            return;
        }
        else
        {
            Wav_Next_16Bit2Channel(&wav);
            int data = wav.CurrentData.LeftVal * WavVolume * WaveResolution / 2 / 32768;
            pwmWrite(Buzz_Pin, constrain(WaveResolution / 2 + data, 0, WaveResolution));
        }
    }
}

/*ProgressBar(T *obj, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t dir, uint8_t style)*/
#define PRG_POS_Y (screen.height()-10)
#define PRG_WIDTH (screen.width()-10)
static LightGUI::ProgressBar<SCREEN_CLASS> WavPrg(&screen, 5, PRG_POS_Y, PRG_WIDTH, 6, 0, 0);

static void Task_UpdateProgress()
{
    WavPrg.setProgress((float)wav.DataPosition/wav.DataSize);
    TextSetDefault();
    screen.setCursor(5, WavPrg.Y - TEXT_HEIGHT_2 - 2);
    int sec = wav.DataPosition / wav.Header.BytePerSecond;
    int min = sec / 60;
    screen.printfX("%02d:%02d ", min, sec % 60);
    
    if(wav.IsEnd)
        page.PagePop();
}

static void Task_WavBufferUpdate()
{
    uint8_t buffer[4];
    if(WavFifo.available() < FIFO_Size - 16)
    {
        wavFile.read(buffer, 4);
        for(int i = 0; i < 4; i++)
        {
            WavFifo.write(buffer[i]);
        }
    }
}

static uint8_t Wav_TestInterface(HWAVEFILE handle, uint8_t size, uint8_t **buffer)
{
    static uint8_t BUFFER[40];
    int bufferPos = 0;
    
    while(size --)
    {
        BUFFER[bufferPos ++] = WavFifo.read();
    }
    *buffer = (uint8_t *)BUFFER;
    return 0;
}

static void Init_WaveTest()
{
    pinMode(Buzz_Pin, INPUT);
    lastState_BuzzSound = State_BuzzSound;
    State_BuzzSound = OFF;

    wavFile = SD.open(WavFilePath);

    if(!wavFile)
    {
        page.PagePop();
        TextSetDefault();
        TextMidPrint(0.5f, 0.7f, "wav error!");
        PageDelay(200);
        return;
    }

    WavFifo.flush();
    while(WavFifo.available() < FIFO_Size - 16)
    {
        Task_WavBufferUpdate();
    }

    Wav_StructInit(&wav, Wav_TestInterface);
    Wav_Open(&wav);

    PWM_Init(Buzz_Pin, WaveResolution, 48000);
    TIM_Cmd(PIN_MAP[Buzz_Pin].TIMx, DISABLE);
    TIM_ITConfig(PIN_MAP[Buzz_Pin].TIMx, TIM_IT_Update, ENABLE);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);
    NVIC_EnableIRQ(TIM2_IRQn);
    TIM_ARRPreloadConfig(PIN_MAP[Buzz_Pin].TIMx, DISABLE);
    TIM_Cmd(PIN_MAP[Buzz_Pin].TIMx, ENABLE);
}

static void WavVolumeCtrl(float step)
{
    WavVolume += step;
    __LimitValue(WavVolume, 0.1f, 2.0f);
}

/********** ���� ************/
/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    CloseStatusBar();
    ClearPage();
    TextSetDefault();
    TextMidPrint(0.5f, 0.5f, (char*)WavFilePath.c_str());
    Init_WaveTest();
    
    mtm_WavPlayer.TaskRegister(0, Task_WavBufferUpdate, 0);
    mtm_WavPlayer.TaskRegister(1, Task_UpdateProgress, 400);
}

/**
  * @brief  ҳ��ѭ���¼�
  * @param  ��
  * @retval ��
  */
static void Loop()
{
    mtm_WavPlayer.Running(millis());
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
    wavFile.close();

    NVIC_DisableIRQ(TIM2_IRQn);
    TIM_Cmd(PIN_MAP[Buzz_Pin].TIMx, DISABLE);
    pinMode(Buzz_Pin, OUTPUT);
    digitalWrite(Buzz_Pin, LOW);
    State_BuzzSound = lastState_BuzzSound;
    ClearPage();
    
    Init_StatusBar();
}

/**
  * @brief  �����¼�
  * @param  ��
  * @retval ��
  */
static void Event(int event, void * param)
{
    if(EVENT_ButtonPress)
    {
        if(btUP)
        {
            WavVolumeCtrl(+0.01f);
        }
        if(btDOWN)
        {
            WavVolumeCtrl(-0.01f);
        }
        if(btBACK)
        {
            page.PagePop();
        }
    }
}

/**
  * @brief  ҳ��ע��
  * @param  ThisPage:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_WavPlayer(uint8_t ThisPage)
{
    page.PageRegister(ThisPage, Setup, Loop, Exit, Event);
}
