#include "FileGroup.h"
#include "GUI_Private.h"
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

int16_t DR_ST_Value = CtrlOutput_MaxValue;
int16_t DR_TH_Value = CtrlOutput_MaxValue;

static void BuzzRing_DR(bool isST)
{
    if(isST)
        BuzzTone(1000 + DR_ST_Value * 5, 20);
    else
        BuzzTone(1000 + DR_TH_Value * 5, 20);
}

#define ChangeStep 20

static void When_ButtonPress_DR()
{
    if (btDR_ST_UP >= btDR_ST_UP.Press)
    {
        if(DR_ST_Value < CtrlOutput_MaxValue)
        {
            DR_ST_Value += ChangeStep;
            BuzzRing_DR(true);
        }
        else
            MotorVibrate(1.0f, 50);
    }
    if (btDR_ST_DOWN >= btDR_ST_DOWN.Press)
    {
        if(DR_ST_Value > 0)
        {
            DR_ST_Value -= ChangeStep;
            BuzzRing_DR(true);
        }
        else
            MotorVibrate(1.0f, 50);
    }

    if (btDR_TH_UP >= btDR_TH_UP.Press)
    {
        if(DR_TH_Value < CtrlOutput_MaxValue)
        {
            DR_TH_Value += ChangeStep;
            BuzzRing_DR(false);
        }
        else
            MotorVibrate(1.0f, 50);
    }
    if (btDR_TH_DOWN >= btDR_TH_DOWN.Press)
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


static void When_ButtonLongPress_DR()
{
    __IntervalExecute(When_ButtonPress_DR(), 100);
}

static void When_ButtonPress_TRIM()
{
    BuzzTone(600, 20);
    if (btTRIM_ST_UP == btTRIM_ST_UP.NoPress)
    {
        CTRL.Key |= BT_L2;
    }
    if (btTRIM_ST_DOWN == btTRIM_ST_DOWN.NoPress)
    {
        CTRL.Key |= BT_R2;
    }
    if (btTRIM_TH_UP == btTRIM_TH_UP.NoPress)
    {
        CTRL.Key |= BT_L1;
    }
    if (btTRIM_TH_DOWN == btTRIM_TH_DOWN.NoPress)
    {
        CTRL.Key |= BT_R1;
    }
}

static void When_ButtonRelease_TRIM()
{
    BuzzTone(900, 20);
    if (btTRIM_ST_UP == btTRIM_ST_UP.NoPress)
    {
        CTRL.Key &= ~BT_L2;
    }
    if (btTRIM_ST_DOWN == btTRIM_ST_DOWN.NoPress)
    {
        CTRL.Key &= ~BT_R2;
    }
    if (btTRIM_TH_UP == btTRIM_TH_UP.NoPress)
    {
        CTRL.Key &= ~BT_L1;
    }
    if (btTRIM_TH_DOWN == btTRIM_TH_DOWN.NoPress)
    {
        CTRL.Key &= ~BT_R1;
    }
}

/**
  * @brief  ���������¼�
  * @param  ��
  * @retval ��
  */
static void When_ButtonPress_Page()
{
    MotorVibrate(1.0f, 50);
    BuzzTone(500, 20);//���Ų�����(500Hz, ����20ms)

    /*��λ��Ӧ��CTRL������־λ*/
    if (btUP == btUP.Press)
        CTRL.Key |= BT_UP;
    if (btDOWN == btDOWN.Press)
        CTRL.Key |= BT_DOWN;
    if (btOK == btOK.Press)
        CTRL.Key |= BT_OK;
    if (btBACK == btBACK.Press)
        CTRL.Key |= BT_BACK;

    page.PageEventTransmit(EVENT_ButtonPress);//��ҳ�����������һ�����������¼�
}

/**
  * @brief  ���������¼�
  * @param  ��
  * @retval ��
  */
void When_ButtonLongPress_Page()
{
    page.PageEventTransmit(EVENT_ButtonLongPress);//��ҳ�����������һ�����������¼�
}

/**
  * @brief  �����ͷ��¼�
  * @param  ��
  * @retval ��
  */
void When_ButtonRelease_Page()
{
    BuzzTone(700, 20);//���Ų�����(700Hz, ����20ms)

    /*�����Ӧ��CTRL������־λ*/
    if (btUP == btUP.NoPress)
        CTRL.Key &= ~BT_UP;
    if (btDOWN == btDOWN.NoPress)
        CTRL.Key &= ~BT_DOWN;
    if (btOK == btOK.NoPress)
        CTRL.Key &= ~BT_OK;
    if (btBACK == btBACK.NoPress)
        CTRL.Key &= ~BT_BACK;
    
    page.PageEventTransmit(EVENT_ButtonRelease);
}

/**
  * @brief  ����״̬�ı��¼�
  * @param  ��
  * @retval ��
  */
void When_ChangeSPDT()
{
    if(digitalRead(SPDT_Switch_Pin))//�жϿ���״̬
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

    /*��ת�������������ų�ʼ��*/
    pinMode(EncoderKey_Pin, INPUT_PULLUP);

    /*HC165���ų�ʼ��*/
    pinMode(HC165_OUT_Pin, INPUT);
    pinMode(HC165_CP_Pin, OUTPUT);
    pinMode(HC165_PL_Pin, OUTPUT);

    /*���������¼�����*/
    btUP.	EventAttach_Press(When_ButtonPress_Page);
    btDOWN.	EventAttach_Press(When_ButtonPress_Page);
    btOK.	EventAttach_Press(When_ButtonPress_Page);
    btBACK.	EventAttach_Press(When_ButtonPress_Page);
    btEcd.	EventAttach_Press(When_ButtonPress_Page);

    btDR_ST_UP.	    EventAttach_Press(When_ButtonPress_DR);
    btDR_ST_DOWN.	EventAttach_Press(When_ButtonPress_DR);
    btDR_TH_UP.	    EventAttach_Press(When_ButtonPress_DR);
    btDR_TH_DOWN.	EventAttach_Press(When_ButtonPress_DR);
    
    btTRIM_ST_UP.	EventAttach_Press(When_ButtonPress_TRIM);
    btTRIM_ST_DOWN.	EventAttach_Press(When_ButtonPress_TRIM);
    btTRIM_TH_UP.	EventAttach_Press(When_ButtonPress_TRIM);
    btTRIM_TH_DOWN.	EventAttach_Press(When_ButtonPress_TRIM);  

    /*���������¼�����*/
    btUP.	EventAttach_LongPress(When_ButtonLongPress_Page);
    btDOWN.	EventAttach_LongPress(When_ButtonLongPress_Page);
    btOK.	EventAttach_LongPress(When_ButtonLongPress_Page);
    btBACK.	EventAttach_LongPress(When_ButtonLongPress_Page);
    btEcd.	EventAttach_LongPress(When_ButtonLongPress_Page);

    btDR_ST_UP.	    EventAttach_LongPress(When_ButtonLongPress_DR);
    btDR_ST_DOWN.	EventAttach_LongPress(When_ButtonLongPress_DR);
    btDR_TH_UP. 	EventAttach_LongPress(When_ButtonLongPress_DR);
    btDR_TH_DOWN.	EventAttach_LongPress(When_ButtonLongPress_DR);

    
   /*�����ͷ��¼�����*/
    btUP.	EventAttach_Release(When_ButtonRelease_Page);
    btDOWN.	EventAttach_Release(When_ButtonRelease_Page);
    btOK.	EventAttach_Release(When_ButtonRelease_Page);
    btBACK.	EventAttach_Release(When_ButtonRelease_Page);
    btEcd.	EventAttach_Release(When_ButtonRelease_Page);
    
    btTRIM_ST_UP.	EventAttach_Release(When_ButtonRelease_TRIM);
    btTRIM_ST_DOWN.	EventAttach_Release(When_ButtonRelease_TRIM);
    btTRIM_TH_UP.	EventAttach_Release(When_ButtonRelease_TRIM);
    btTRIM_TH_DOWN.	EventAttach_Release(When_ButtonRelease_TRIM);

    /*����״̬�ı��¼�����*/
    swSPDT.EventAttach_Change(When_ChangeSPDT);
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

/**
  * @brief  �����¼�����
  * @param  ��
  * @retval ��
  */
void ButtonEventMonitor()
{ 
    uint8_t ButtonValue = Get_HC165_Value();//��ȡHC165�˸����ŵĵ�ƽֵ
    enum KeyBit {
        BIT_ST_DRP = 0x80,
        BIT_ST_DRM = 0x10,
        BIT_TH_DRP = 0x40,
        BIT_TH_DRM = 0x20,
        BIT_TH_TRIMP = 0x04,
        BIT_TH_TRIMM = 0x08,
        BIT_ST_TRIMP = 0x01,
        BIT_ST_TRIMM = 0x02
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

    swSPDT. EventMonitor(digitalRead(SPDT_Switch_Pin));
}
