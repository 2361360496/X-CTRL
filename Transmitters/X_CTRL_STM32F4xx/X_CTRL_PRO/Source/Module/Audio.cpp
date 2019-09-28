#include "FileGroup.h"
#include "Module.h"
#include "TasksManage.h"
#include "FifoQueue.h"
#include "wav_decoder.h"
#include "SdFat.h"

#define DAC_CH1_Pin PA4
#define DAC_CH2_Pin PA5

#define IS_DAC_Pin(Pin) (Pin==DAC_CH1_Pin||Pin==DAC_CH2_Pin)

static void Audio_Init()
{
    pinMode(AudioMute_Pin, OUTPUT);
    
    if(IS_DAC_Pin(Speaker_Pin))
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
        DAC_InitTypeDef dacConfig;
        DAC_StructInit(&dacConfig);
        dacConfig.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
        dacConfig.DAC_Trigger = DAC_Trigger_None;
        dacConfig.DAC_WaveGeneration = DAC_WaveGeneration_None;
        if(Speaker_Pin == DAC_CH1_Pin)
        {
            DAC_Init(DAC_Channel_1, &dacConfig);
            DAC_Cmd(DAC_Channel_1, ENABLE);
        }
        else if(Speaker_Pin == DAC_CH2_Pin)
        {
            DAC_Init(DAC_Channel_2, &dacConfig);
            DAC_Cmd(DAC_Channel_2, ENABLE);
        }
        pinMode(Speaker_Pin, OUTPUT_AF);
    }
}

void Audio_SetEnable(bool en)
{
    __ExecuteOnce(Audio_Init());
    
    digitalWrite(AudioMute_Pin, en);
}

#define FIFO_Size 6000
char waveBuff[FIFO_Size];
FifoQueue<char> WaveBuffer(sizeof(waveBuff), waveBuff);

File wavFile;
WAV_TypeDef wav;
int WavAvailable;

TaskHandle_t TaskHandle_WavPlayer;
EventGroupHandle_t CreatedEventGroup = NULL;
bool Reqed = false;


#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)

static void SendDataToDAC()
{
    WavAvailable = WaveBuffer.available();
    if(WavAvailable < 16)
    {
//        if(!Reqed)
//        {
//            BaseType_t xResult;
//            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//            /* ������ vTaskMsgPro �����¼���־ */
//            xResult = xEventGroupSetBitsFromISR(CreatedEventGroup, /* �¼���־���� */
//                                                BIT_0 , /* ���� bit0 */
//                                                &xHigherPriorityTaskWoken);
//            Reqed = true;
//        }
        return;
    }
    else
    {
        Wav_Next_16Bit2Channel(&wav);
        DAC_SetChannel1Data(DAC_Align_12b_R, ((wav.CurrentData.LeftVal + 32768) >> 6) * 0.7f);
    }
}

static uint8_t Wav_TestInterface(HWAVEFILE handle, uint8_t size, uint8_t **buffer)
{
    static uint8_t BUFFER[32];
    int bufferPos = 0;
    while(size --)
    {
        BUFFER[bufferPos ++] = WaveBuffer.read();
    }
    *buffer = BUFFER;
    return 0;
}

static void Wav_BufferUpdate()
{
    while(WaveBuffer.available() < FIFO_Size - 16)
    {
        uint8_t buffer[4];
        wavFile.read(buffer, sizeof(buffer));
        for(int i = 0; i < sizeof(buffer); i++)
        {
            WaveBuffer.write(buffer[i]);
        }
    }
}

static void Init_WaveTest()
{
    wavFile = SD.open("UNTALG.wav");
    while(WaveBuffer.available() < FIFO_Size - 16)
    {
        Wav_BufferUpdate();
    }

    Wav_StructInit(&wav, Wav_TestInterface);
    Wav_Open(&wav);

    TimerSet(TIM2, 1000000 / wav.Header.SampleFreq, SendDataToDAC);
    TIM_Cmd(TIM2, ENABLE);
}

void Task_WavPlayer(void *pvParameters)
{
    pinMode(SD_CD_Pin, INPUT_PULLUP);
    
    if(digitalRead(SD_CD_Pin))
    {
        while(1);
    }
    
    if(!SD.begin(SD_CS_Pin, SD_SCK_MHZ(50))) 
    {
        while(1);
    }
    Audio_SetEnable(true);
    Init_WaveTest();
    
    CreatedEventGroup = xEventGroupCreate();
    if(!CreatedEventGroup)
    {
        while(1);
    }
    
    EventBits_t uxBits;
    for(;;)
    {
//        uxBits = xEventGroupWaitBits(CreatedEventGroup, /* �¼���־���� */
//                                     BIT_0 | BIT_1, /* �ȴ� bit0 �� bit1 ������ */
//                                     pdTRUE, /* �˳�ǰ bit0 �� bit1 ������������� bit0 �� bit1�������òű�ʾ���˳���*/
//                                     pdFALSE, /* ����Ϊ pdTRUE��ʾ�ȴ� bit1�� bit0 ��������*/
//                                     1000); /* �ȴ��ӳ�ʱ�� */
//        if(uxBits == BIT_0)
//        {
//            Wav_BufferUpdate();
//            Reqed = true;
//        }
        Wav_BufferUpdate();
    }
}
