#include "FileGroup.h"

/*��ѭ���ܵ����������ȼ�����*/
MillisTaskManager mtm(TP_MAX, true);

void setup()
{
    pinMode(LED_Pin, OUTPUT);
    digitalWrite(LED_Pin, HIGH);
    Init_Value();
    Init_Motor();
    Init_MPU6050();

    toneSetTimer(TIM_BuzzTone);
    __LoopExecute((togglePin(LED_Pin), delay(80)), 4);

    Init_NRF();

#ifdef HandshakeEnable_Pin
    pinMode(HandshakeEnable_Pin, INPUT_PULLUP);
    if(!digitalRead(HandshakeEnable_Pin))//��Ƶ������ʱ��������״̬
    {
        while(!HandshakeToMaster())
            __IntervalExecute(togglePin(LED_Pin), 2000);
    }
#else
    while(!HandshakeToMaster())
        __IntervalExecute(togglePin(LED_Pin), 2000);
#endif

    Init_IWDG(IWDG_Prescaler_64, 625);//���Ź���ʼ��@1s

    mtm.TaskRegister(TP_NRF_Handler, Task_NRF_Handler, 10);
    mtm.TaskRegister(TP_Ctrl, Task_Ctrl, 10);
    mtm.TaskRegister(TP_InfoReport, Task_InfoReport, 100);
    mtm.TaskRegister(TP_InfoLED, Task_InfoLED, 100);
    mtm.TaskRegister(TP_FeedWatchDog, IWDG_ReloadCounter, 500);
}

void loop()
{
    mtm.Running(millis());
}

/**
  * @brief  Main Function
  * @param  None
  * @retval None
  */
int main(void)
{
    Init_BaseClocks(true);
    GPIO_JTAG_Disable();
    Delay_Init();
    ADCx_Init(ADC1);
//  ADCx_DMA_Init();
    setup();
    for(;;)loop();
}
