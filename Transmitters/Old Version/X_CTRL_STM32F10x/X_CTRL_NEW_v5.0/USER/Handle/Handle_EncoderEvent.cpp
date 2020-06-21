#include "FileGroup.h"
#include "GUI_Private.h"

/*��������ʼƵ��2000Hz*/
#define FreqStart 2000

/*ʵ������ת����������*/
EncoderEvent ecd;

/*LED����Ŀ��ֵ*/
static uint16_t BrightSet;

/*��һ����תʱ���*/
static uint32_t LastRotate_TimePoint;

/**
  * @brief  ��ת������LED��������
  * @param  ��
  * @retval ��
  */
static void Task_EncoderLED()
{
    static uint16_t LED_Bright;
    if(millis() - LastRotate_TimePoint > 500)
        BrightSet = 0;

    if(LED_Bright > BrightSet)
        LED_Bright -= 10;
    else if(LED_Bright < BrightSet)
        LED_Bright += 20;

    analogWrite(EncoderLED_Pin, LED_Bright);
}

/**
  * @brief  ��ת�������¼�
  * @param  ��
  * @retval ��
  */
static void When_EncoderRotate()
{
    BrightSet = 300;                    //LED����Ŀ��ֵ�趨300(30%)
    
    static uint16_t Freq = FreqStart;   //������Ƶ��

    if(millis() - LastRotate_TimePoint > 1000)//�Ƿ�ʱ
    {
        Freq = FreqStart;               //���ط�����Ƶ��
    }
    else
    {
        if(ecd > 0)                     //˳ʱ����ת
            Freq += 100;                //Ƶ������100Hz
        
        if(ecd < 0)                     //��ʱ����ת
            if(Freq > 100)              //�޷����100Hz
                Freq -= 100;            //Ƶ�ʽ���100Hz
    }

    LastRotate_TimePoint = millis();    //��¼ʱ���
    BuzzTone(Freq, 5);                  //���������
	
	page.PageEventTransmit(EVENT_EncoderRotate);//��ҳ�����������һ����ת�������¼�
}

/**
  * @brief  ��ʼ����ת�������¼�
  * @param  ��
  * @retval ��
  */
void Init_EncoderEvent()
{
    /*��ʼ������*/
    pinMode(EncoderLED_Pin, PWM);
    analogWrite(EncoderLED_Pin, 0);
    pinMode(EncoderA_Pin, INPUT);
    pinMode(EncoderB_Pin, INPUT);
    
    ecd.EventAttach_Rotate(When_EncoderRotate);//������ת�������¼�
}

/**
  * @brief  ��ת����������
  * @param  ��
  * @retval ��
  */
void EncoderMonitor()
{
    ecd.EventMonitor(digitalRead(EncoderA_Pin), digitalRead(EncoderB_Pin));
    __IntervalExecute(Task_EncoderLED(), 30);//ÿ30ms����һ��LED״̬
}
