#include "FileGroup.h"
#include "ComPrivate.h"
#include "GUI_Private.h"

/*ʵ������������*/
ButtonEvent btUP(1, 500);
ButtonEvent btDOWN(1, 500);
ButtonEvent btOK(1, 500);
ButtonEvent btBACK(1, 500);
ButtonEvent btL1(1, 500);
ButtonEvent btR1(1, 500);
ButtonEvent btL2(1, 500);
ButtonEvent btR2(1, 500);
ButtonEvent btEcd(1, 500);
ButtonEvent swSPDT;

/**
  * @brief  ���������¼�
  * @param  ��
  * @retval ��
  */
void When_ButtonPress()
{
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
    if (btL1 == btL1.Press)
        CTRL.Key |= BT_L1;
    if (btR1 == btR1.Press)
        CTRL.Key |= BT_R1;
    if (btL2 == btL2.Press)
        CTRL.Key |= BT_L2;
    if (btR2 == btR2.Press)
        CTRL.Key |= BT_R2;

    page.PageEventTransmit(EVENT_ButtonPress);//��ҳ�����������һ�����������¼�
}

/**
  * @brief  ���������¼�
  * @param  ��
  * @retval ��
  */
void When_ButtonLongPress()
{
    page.PageEventTransmit(EVENT_ButtonLongPress);
}

/**
  * @brief  �����ͷ��¼�
  * @param  ��
  * @retval ��
  */
void When_ButtonRelease()
{
    BuzzTone(700, 20);

    if (btUP == btUP.NoPress)
        CTRL.Key &= ~BT_UP;
    if (btDOWN == btDOWN.NoPress)
        CTRL.Key &= ~BT_DOWN;
    if (btOK == btOK.NoPress)
        CTRL.Key &= ~BT_OK;
    if (btBACK == btBACK.NoPress)
        CTRL.Key &= ~BT_BACK;
    if (btL1 == btL1.NoPress)
        CTRL.Key &= ~BT_L1;
    if (btR1 == btR1.NoPress)
        CTRL.Key &= ~BT_R1;
    if (btL2 == btL2.NoPress)
        CTRL.Key &= ~BT_L2;
    if (btR2 == btR2.NoPress)
        CTRL.Key &= ~BT_R2;
}

/**
  * @brief  ����״̬�ı��¼�
  * @param  ��
  * @retval ��
  */
void When_ChangeSPDT()
{
    if(digitalRead(SPDT_Switch_Pin))
    {
        BuzzTone(500, 300);
    }
    else
    {
        BuzzTone(200, 100);
    }
}

/**
  * @brief  ��ʼ�������¼�
  * @param  ��
  * @retval ��
  */
void Init_BottonEvent()
{
    /*�������ų�ʼ��*/
    pinMode(SPDT_Switch_Pin, INPUT_PULLDOWN);
    
    /*�������ų�ʼ��*/
    pinMode(Button_OK_Pin, INPUT_PULLUP);
    pinMode(Button_BACK_Pin, INPUT_PULLUP);
    pinMode(Button_UP_Pin, INPUT_PULLUP);
    pinMode(Button_DOWN_Pin, INPUT_PULLUP);
    pinMode(Button_L_Pin, INPUT_PULLUP);
    pinMode(Button_R_Pin, INPUT_PULLUP);
    
    /*���������¼�����*/
    btUP.	EventAttach_Press(When_ButtonPress);
    btDOWN.	EventAttach_Press(When_ButtonPress);
    btOK.	EventAttach_Press(When_ButtonPress);
    btBACK.	EventAttach_Press(When_ButtonPress);
    btL1.	EventAttach_Press(When_ButtonPress);
    btL2.	EventAttach_Press(When_ButtonPress);
    btR1.	EventAttach_Press(When_ButtonPress);
    btR2.	EventAttach_Press(When_ButtonPress);
    btEcd.	EventAttach_Press(When_ButtonPress);

    /*���������¼�����*/
    btUP.	EventAttach_LongPress(When_ButtonLongPress);
    btDOWN.	EventAttach_LongPress(When_ButtonLongPress);
    btOK.	EventAttach_LongPress(When_ButtonLongPress);
    btBACK.	EventAttach_LongPress(When_ButtonLongPress);
    btL1.	EventAttach_LongPress(When_ButtonLongPress);
    btL2.	EventAttach_LongPress(When_ButtonLongPress);
    btR1.	EventAttach_LongPress(When_ButtonLongPress);
    btR2.	EventAttach_LongPress(When_ButtonLongPress);
    btEcd.	EventAttach_LongPress(When_ButtonLongPress);

    /*�����ͷ��¼�����*/
    btUP.	EventAttach_Release(When_ButtonRelease);
    btDOWN.	EventAttach_Release(When_ButtonRelease);
    btOK.	EventAttach_Release(When_ButtonRelease);
    btBACK.	EventAttach_Release(When_ButtonRelease);
    btL1.	EventAttach_Release(When_ButtonRelease);
    btL2.	EventAttach_Release(When_ButtonRelease);
    btR1.	EventAttach_Release(When_ButtonRelease);
    btR2.	EventAttach_Release(When_ButtonRelease);
    btEcd.	EventAttach_Release(When_ButtonRelease);

    /*����״̬�ı��¼�����*/
    swSPDT.EventAttach_Change(When_ChangeSPDT);
}

void ButtonEventMonitor()
{
    /*�����¼�����*/
    swSPDT. EventMonitor(digitalRead(SPDT_Switch_Pin));
    
    /*�����¼�����*/
    btUP.   EventMonitor(digitalRead(Button_UP_Pin));
    btDOWN. EventMonitor(digitalRead(Button_DOWN_Pin));
    btOK.   EventMonitor(digitalRead(Button_OK_Pin));
    btBACK. EventMonitor(digitalRead(Button_BACK_Pin));
    btL2.   EventMonitor(digitalRead(Button_L_Pin));
    btR2.   EventMonitor(digitalRead(Button_R_Pin));
}
