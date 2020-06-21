#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "ComPrivate.h"

/*ʵ������������*/
ButtonEvent btUP;
ButtonEvent btDOWN;
ButtonEvent btOK;
ButtonEvent btBACK;
ButtonEvent btEcd;

static ButtonEvent btDR_ST_UP;
static ButtonEvent btDR_ST_DOWN;
static ButtonEvent btDR_TH_UP;
static ButtonEvent btDR_TH_DOWN;

static ButtonEvent btTRIM_ST_UP;
static ButtonEvent btTRIM_ST_DOWN;
static ButtonEvent btTRIM_TH_UP;
static ButtonEvent btTRIM_TH_DOWN;

static ButtonEvent swSPDT;

int16_t DR_ST_Value = RCX_CHANNEL_DATA_MAX;
int16_t DR_TH_Value = RCX_CHANNEL_DATA_MAX;

static void BuzzRing_DR(bool isST)
{
    if(isST)
        BuzzTone(1000 + DR_ST_Value * 5, 20);
    else
        BuzzTone(1000 + DR_TH_Value * 5, 20);
}

static void DR_EventHandler(ButtonEvent* btn, int event)
{
    const uint8_t ChangeStep = 20;
    if(event == EVENT_ButtonPress || event == EVENT_ButtonLongPressRepeat)
    {
        if (*btn == btDR_ST_UP)
        {
            if(DR_ST_Value < RCX_CHANNEL_DATA_MAX)
            {
                DR_ST_Value += ChangeStep;
                BuzzRing_DR(true);
            }
            else
                MotorVibrate(1.0f, 50);
        }
        else if (*btn == btDR_ST_DOWN)
        {
            if(DR_ST_Value > 0)
            {
                DR_ST_Value -= ChangeStep;
                BuzzRing_DR(true);
            }
            else
                MotorVibrate(1.0f, 50);
        }

        else if (*btn == btDR_TH_UP)
        {
            if(DR_TH_Value < RCX_CHANNEL_DATA_MAX)
            {
                DR_TH_Value += ChangeStep;
                BuzzRing_DR(false);
            }
            else
                MotorVibrate(1.0f, 50);
        }
        else if (*btn == btDR_TH_DOWN)
        {
            if(DR_TH_Value > 0)
            {
                DR_TH_Value -= ChangeStep;
                BuzzRing_DR(false);
            }
            else
                MotorVibrate(1.0f, 50);
        }
    }

}

static void TRIM_EventHandler(ButtonEvent* btn, int event)
{
    uint8_t keyVal;
    if(event == EVENT_ButtonPress)
    {
        BuzzTone(600, 20);
        keyVal = 1;
    }
    else if(event == EVENT_ButtonRelease)
    {
        BuzzTone(900, 20);
        keyVal = 0;
    }
    else 
        return;
    
    if (*btn == btTRIM_ST_UP)
    {
        CTRL.Key.Bit.BT_L2 = keyVal;
    }
    if (*btn == btTRIM_ST_DOWN)
    {
        CTRL.Key.Bit.BT_R2 = keyVal;
    }
    if (*btn == btTRIM_TH_UP)
    {
        CTRL.Key.Bit.BT_L1 = keyVal;
    }
    if (*btn == btTRIM_TH_DOWN)
    {
        CTRL.Key.Bit.BT_R1 = keyVal;
    }
}

/**
  * @brief  ҳ�水���¼�
  * @param  ��
  * @retval ��
  */
static void Page_EventHandler(ButtonEvent* btn, int event)
{
    uint8_t keyVal;
    if(event == EVENT_ButtonPress)
    {
        MotorVibrate(1.0f, 50);
        BuzzTone(500, 20);//���Ų�����(500Hz, ����20ms)
        keyVal = 1;
    }
    else if(event == EVENT_ButtonRelease)
    {
        BuzzTone(700, 20);//���Ų�����(700Hz, ����20ms)
        keyVal = 0;
    }
    else
        goto PageEvent;
    
    /*��Ӧ��CTRL������־λ*/
    if (*btn == btUP)
        CTRL.Key.Bit.BT_UP = keyVal;
    if (*btn == btDOWN)
        CTRL.Key.Bit.BT_DOWN = keyVal;
    if (*btn == btOK)
        CTRL.Key.Bit.BT_OK = keyVal;
    if (*btn == btBACK)
        CTRL.Key.Bit.BT_BACK = keyVal;

PageEvent:
    /*���ݵ�ҳ���¼�*/
    page.PageEventTransmit(event, btn);
}

/**
  * @brief  �����¼�
  * @param  ��
  * @retval ��
  */
static void SPDT_EventHandler(ButtonEvent* btn, int event)
{
    if(*btn == swSPDT && event == EVENT_ButtonChange)
    {
        if(IS_EnableCtrl())//�жϿ���״̬
        {
            BuzzTone(500, 300); //���Ų�����(500Hz, ����300ms)
            XFS_Clear();        //��������������
            XFS_Speak("ң��������");//���������������
        }
        else
        {
            BuzzTone(200, 100); //���Ų�����(200Hz, ����100ms)
            XFS_Clear();        //��������������
            XFS_Speak("ң��������");//���������������
        }
    }
}

/**
  * @brief  ��ʼ�������¼�
  * @param  ��
  * @retval ��
  */
void Init_BottonEvent()
{
    /*������ʼ��*/
    pinMode(KEY_CH_Pin, INPUT_PULLUP);
    pinMode(KEY_SEL_Pin, INPUT_PULLUP);
    pinMode(KEY_UP_Pin, INPUT_PULLUP);
    pinMode(KEY_DOWN_Pin, INPUT_PULLUP);

    /*�������ų�ʼ��*/
    pinMode(SPDT_Switch_Pin, INPUT_PULLUP);

#ifdef USE_Encoder
    /*��ת�������������ų�ʼ��*/
    pinMode(EncoderKey_Pin, INPUT_PULLUP);
#endif

    /*HC165���ų�ʼ��*/
    pinMode(HC165_OUT_Pin, INPUT);
    pinMode(HC165_CP_Pin, OUTPUT);
    pinMode(HC165_PL_Pin, OUTPUT);

    /*�����¼�����*/
    btUP.   EventAttach(Page_EventHandler);
    btDOWN. EventAttach(Page_EventHandler);
    btOK.   EventAttach(Page_EventHandler);
    btBACK. EventAttach(Page_EventHandler);
    btEcd.  EventAttach(Page_EventHandler);

    btDR_ST_UP.     EventAttach(DR_EventHandler);
    btDR_ST_DOWN.   EventAttach(DR_EventHandler);
    btDR_TH_UP.     EventAttach(DR_EventHandler);
    btDR_TH_DOWN.   EventAttach(DR_EventHandler);

    btTRIM_ST_UP.   EventAttach(TRIM_EventHandler);
    btTRIM_ST_DOWN. EventAttach(TRIM_EventHandler);
    btTRIM_TH_UP.   EventAttach(TRIM_EventHandler);
    btTRIM_TH_DOWN. EventAttach(TRIM_EventHandler);

    /*����״̬�ı��¼�����*/
    swSPDT.EventAttach(SPDT_EventHandler);
}

/**
  * @brief  ��ȡHC165��ֵ
  * @param  ��
  * @retval ��
  */
static uint8_t Get_HC165_Value()
{
    digitalWrite_HIGH(HC165_CP_Pin);
    digitalWrite_LOW(HC165_PL_Pin);
    digitalWrite_HIGH(HC165_PL_Pin);
    return shiftIn(HC165_OUT_Pin, HC165_CP_Pin, MSBFIRST);
}

uint8_t ButtonValue;
/**
  * @brief  �����¼�����
  * @param  ��
  * @retval ��
  */
void ButtonEventMonitor()
{
    ButtonValue = Get_HC165_Value();//��ȡHC165�˸����ŵĵ�ƽֵ
    /*enum KeyBit {
        BIT_ST_DRP = 0x80,
        BIT_ST_DRM = 0x10,
        BIT_TH_DRP = 0x40,
        BIT_TH_DRM = 0x20,
        BIT_TH_TRIMP = 0x04,
        BIT_TH_TRIMM = 0x08,
        BIT_ST_TRIMP = 0x01,
        BIT_ST_TRIMM = 0x02
    };*/
    enum KeyBit
    {
        BIT_ST_DRP = 0x01,
        BIT_ST_DRM = 0x02,
        BIT_TH_DRP = 0x04,
        BIT_TH_DRM = 0x08,
        BIT_TH_TRIMP = 0x80,
        BIT_TH_TRIMM = 0x10,
        BIT_ST_TRIMP = 0x40,
        BIT_ST_TRIMM = 0x20
    };

    /*�����¼�����*/
    btUP.   EventMonitor(!digitalRead(KEY_UP_Pin));
    btDOWN. EventMonitor(!digitalRead(KEY_DOWN_Pin));
    btOK.   EventMonitor(!digitalRead(KEY_CH_Pin));
    btBACK. EventMonitor(!digitalRead(KEY_SEL_Pin));

    btDR_ST_UP.     EventMonitor(ButtonValue & BIT_ST_DRP);
    btDR_ST_DOWN.   EventMonitor(ButtonValue & BIT_ST_DRM);
    btDR_TH_UP.     EventMonitor(ButtonValue & BIT_TH_DRP);
    btDR_TH_DOWN.   EventMonitor(ButtonValue & BIT_TH_DRM);

    btTRIM_ST_UP.     EventMonitor(ButtonValue & BIT_ST_TRIMP);
    btTRIM_ST_DOWN.   EventMonitor(ButtonValue & BIT_ST_TRIMM);
    btTRIM_TH_UP.     EventMonitor(ButtonValue & BIT_TH_TRIMP);
    btTRIM_TH_DOWN.   EventMonitor(ButtonValue & BIT_TH_TRIMM);

    swSPDT. EventMonitor(IS_EnableCtrl());
}
