#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

/*ʵ������������*/
ButtonEvent btUP;
ButtonEvent btDOWN;
ButtonEvent btOK;
ButtonEvent btBACK;
ButtonEvent btEcd;
ButtonEvent btA;
ButtonEvent btB;

static ButtonEvent swSPDT;

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
    if (btUP == ButtonEvent_Type::Press)
        CTRL.Key |= BT_UP;
    if (btDOWN == ButtonEvent_Type::Press)
        CTRL.Key |= BT_DOWN;
    if (btOK == ButtonEvent_Type::Press)
        CTRL.Key |= BT_OK;
    if (btA == ButtonEvent_Type::Press)
        CTRL.Key |= BT_L2;
	if (btB == ButtonEvent_Type::Press)
        CTRL.Key |= BT_R2;

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
    if (btUP == ButtonEvent_Type::NoPress)
        CTRL.Key &= ~BT_UP;
    if (btDOWN == ButtonEvent_Type::NoPress)
        CTRL.Key &= ~BT_DOWN;
    if (btOK == ButtonEvent_Type::NoPress)
        CTRL.Key &= ~BT_OK;
    if (btBACK == ButtonEvent_Type::NoPress)
        CTRL.Key &= ~BT_BACK;
	if (btA == ButtonEvent_Type::NoPress)
        CTRL.Key &= ~BT_L2;
	if (btB == ButtonEvent_Type::NoPress)
        CTRL.Key &= ~BT_R2;
    
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
    pinMode(KEY_OK_Pin, INPUT_PULLUP);
    pinMode(KEY_BACK_Pin, INPUT_PULLUP);
    pinMode(KEY_UP_Pin, INPUT_PULLUP);
    pinMode(KEY_DOWN_Pin, INPUT_PULLUP);
	pinMode(KEY_A_Pin, INPUT_PULLUP);
    pinMode(KEY_B_Pin, INPUT_PULLUP);

    /*�������ų�ʼ��*/
    pinMode(SPDT_Switch_Pin, INPUT_PULLUP);

    /*��ת�������������ų�ʼ��*/
    pinMode(EncoderKey_Pin, INPUT_PULLUP);

    /*���������¼�����*/
    btUP.	EventAttach_Press(When_ButtonPress_Page);
    btDOWN.	EventAttach_Press(When_ButtonPress_Page);
    btOK.	EventAttach_Press(When_ButtonPress_Page);
    btBACK.	EventAttach_Press(When_ButtonPress_Page);
    btEcd.	EventAttach_Press(When_ButtonPress_Page);
	btA.	EventAttach_Press(When_ButtonPress_Page);
	btB.	EventAttach_Press(When_ButtonPress_Page);

    /*���������¼�����*/
    btUP.	EventAttach_LongPress(When_ButtonLongPress_Page);
    btDOWN.	EventAttach_LongPress(When_ButtonLongPress_Page);
    btOK.	EventAttach_LongPress(When_ButtonLongPress_Page);
    btBACK.	EventAttach_LongPress(When_ButtonLongPress_Page);
    btEcd.	EventAttach_LongPress(When_ButtonLongPress_Page);
	btA.	EventAttach_LongPress(When_ButtonLongPress_Page);
	btB.	EventAttach_LongPress(When_ButtonLongPress_Page);

    
   /*�����ͷ��¼�����*/
    btUP.	EventAttach_Release(When_ButtonRelease_Page);
    btDOWN.	EventAttach_Release(When_ButtonRelease_Page);
    btOK.	EventAttach_Release(When_ButtonRelease_Page);
    btBACK.	EventAttach_Release(When_ButtonRelease_Page);
    btEcd.	EventAttach_Release(When_ButtonRelease_Page);
	btA.	EventAttach_Release(When_ButtonRelease_Page);
	btB.	EventAttach_Release(When_ButtonRelease_Page);

    /*����״̬�ı��¼�����*/
    swSPDT.EventAttach_Change(When_ChangeSPDT);
}

/**
  * @brief  �����¼�����
  * @param  ��
  * @retval ��
  */
void ButtonEventMonitor()
{ 
    /*�����¼�����*/
    btUP.   EventMonitor(!digitalRead(KEY_UP_Pin));
    btDOWN. EventMonitor(!digitalRead(KEY_DOWN_Pin));
    btOK.   EventMonitor(!digitalRead(KEY_OK_Pin));
    btBACK. EventMonitor(!digitalRead(KEY_BACK_Pin));
	btA.	EventMonitor(!digitalRead(KEY_A_Pin));
	btB.	EventMonitor(!digitalRead(KEY_B_Pin));

    swSPDT. EventMonitor(digitalRead(SPDT_Switch_Pin));
}
