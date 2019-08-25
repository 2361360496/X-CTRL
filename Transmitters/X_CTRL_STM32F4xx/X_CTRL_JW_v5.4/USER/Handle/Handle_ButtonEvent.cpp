#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"
#include "NotesToFreq.h"

using namespace NotesToFreq;

uint16_t NTF[]={
    M1,M2,M3,M4,M5,M6,M7,H1,H2
};

/*ʵ������������*/
ButtonEvent btUP;
ButtonEvent btDOWN;
ButtonEvent btOK;
ButtonEvent btBACK;
ButtonEvent btEcd;

uint8_t KEY_Y[] = {KEY_Y1_Pin, KEY_Y2_Pin, KEY_Y3_Pin};
uint8_t KEY_X[] = {KEY_X1_Pin, KEY_X2_Pin, KEY_X3_Pin};
ButtonEvent btMK[sizeof(KEY_Y) * sizeof(KEY_X)];
uint8_t MK_Remap[] = {
    4,5,3,
    1,2,0,
    7,8,6
};

/**
  * @brief  ��ȡ��������ϵ�һ������
  * @param  keyID:��ID��
  * @retval ����״̬
  */
bool ReadMatrixKeys(uint8_t keyID)
{
    uint8_t ky = keyID / sizeof(KEY_Y);
    uint8_t kx = keyID % sizeof(KEY_X);
    for(uint8_t y = 0; y < sizeof(KEY_Y); y++)
    {
        (y == ky) ? digitalWrite(KEY_Y[y], HIGH) : digitalWrite(KEY_Y[y], LOW);
    }
    return digitalRead(KEY_X[kx]);
}

void When_MK_ButtonPress()
{
    for(uint8_t k = 0; k < __Sizeof(btMK); k++)
    {
        if(btMK[k] == btMK[k].Press)
        {
            BuzzTone(NTF[k]);
            break;
        }
    }
}

void When_MK_ButtonLongPress()
{
}

void When_MK_ButtonRelease()
{
    BuzzTone(0);
}

/**
  * @brief  ���������¼�
  * @param  ��
  * @retval ��
  */
static void When_ButtonPress_Page()
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
  * @brief  ��ʼ�������¼�
  * @param  ��
  * @retval ��
  */
void Init_BottonEvent()
{
    /*������ʼ��*/
    pinMode(KEY_Y1_Pin, OUTPUT);
    pinMode(KEY_Y2_Pin, OUTPUT);
    pinMode(KEY_Y3_Pin, OUTPUT);
    pinMode(KEY_X1_Pin, INPUT_PULLDOWN);
    pinMode(KEY_X2_Pin, INPUT_PULLDOWN);
    pinMode(KEY_X3_Pin, INPUT_PULLDOWN);

    /*��ת�������������ų�ʼ��*/
    pinMode(EncoderKey_Pin, INPUT_PULLUP);

    /*���������¼�����*/
    btUP.	EventAttach_Press(When_ButtonPress_Page);
    btDOWN.	EventAttach_Press(When_ButtonPress_Page);
    btOK.	EventAttach_Press(When_ButtonPress_Page);
    btBACK.	EventAttach_Press(When_ButtonPress_Page);
    btEcd.	EventAttach_Press(When_ButtonPress_Page);

    /*���������¼�����*/
    btUP.	EventAttach_LongPress(When_ButtonLongPress_Page);
    btDOWN.	EventAttach_LongPress(When_ButtonLongPress_Page);
    btOK.	EventAttach_LongPress(When_ButtonLongPress_Page);
    btBACK.	EventAttach_LongPress(When_ButtonLongPress_Page);
    btEcd.	EventAttach_LongPress(When_ButtonLongPress_Page);
    
   /*�����ͷ��¼�����*/
    btUP.	EventAttach_Release(When_ButtonRelease_Page);
    btDOWN.	EventAttach_Release(When_ButtonRelease_Page);
    btOK.	EventAttach_Release(When_ButtonRelease_Page);
    btBACK.	EventAttach_Release(When_ButtonRelease_Page);
    btEcd.	EventAttach_Release(When_ButtonRelease_Page);
    
    for(uint8_t k = 0; k < __Sizeof(btMK); k++)
    {
        btMK[k].EventAttach_Press(When_MK_ButtonPress);
        btMK[k].EventAttach_LongPress(When_MK_ButtonLongPress);
        btMK[k].EventAttach_Release(When_MK_ButtonRelease);
    }
}

/**
  * @brief  �����¼�����
  * @param  ��
  * @retval ��
  */
void ButtonEventMonitor()
{   
    for(uint8_t k = 0; k < __Sizeof(btMK); k++)
    {
        btMK[k].EventMonitor(ReadMatrixKeys(MK_Remap[k]));
    }

    /*�����¼�����*/
    btEcd.  EventMonitor(!digitalRead(EncoderKey_Pin));
}
