#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

/*ʵ������������*/
ButtonEvent btUP;
ButtonEvent btDOWN;
ButtonEvent btOK;
ButtonEvent btBACK;
ButtonEvent btEcd;

static ButtonEvent swSPDT;

const uint8_t KEY_Y[] = {KEY_Y1_Pin, KEY_Y2_Pin, KEY_Y3_Pin};
const uint8_t KEY_X[] = {KEY_X1_Pin, KEY_X2_Pin, KEY_X3_Pin};
ButtonEvent btMK[9];

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

/**
  * @brief  ���������¼�
  * @param  ��
  * @retval ��
  */
static void When_ButtonPress_Page()
{
    BuzzTone(500, 20);//���Ų�����(500Hz, ����20ms)

    /*��λ��Ӧ��CTRL������־λ*/
    if (btUP == ButtonEvent_Type::Press)
        CTRL.Key |= BT_UP;
    if (btDOWN == ButtonEvent_Type::Press)
        CTRL.Key |= BT_DOWN;
    if (btOK == ButtonEvent_Type::Press)
        CTRL.Key |= BT_OK;
    if (btBACK == ButtonEvent_Type::Press)
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
    if (btUP == ButtonEvent_Type::NoPress)
        CTRL.Key &= ~BT_UP;
    if (btDOWN == ButtonEvent_Type::NoPress)
        CTRL.Key &= ~BT_DOWN;
    if (btOK == ButtonEvent_Type::NoPress)
        CTRL.Key &= ~BT_OK;
    if (btBACK == ButtonEvent_Type::NoPress)
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
    /*�������ų�ʼ��*/
    pinMode(SPDT_Switch_Pin, INPUT_PULLDOWN);
    
    /*������ʼ��*/
    pinMode(KEY_Y1_Pin, OUTPUT);
    pinMode(KEY_Y2_Pin, OUTPUT);
    pinMode(KEY_Y3_Pin, OUTPUT);
    pinMode(KEY_X1_Pin, INPUT_PULLDOWN);
    pinMode(KEY_X2_Pin, INPUT_PULLDOWN);
    pinMode(KEY_X3_Pin, INPUT_PULLDOWN);

#ifdef USE_Encoder
    /*��ת�������������ų�ʼ��*/
    pinMode(EncoderKey_Pin, INPUT_PULLUP);
#endif

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
    
    /*����״̬�ı��¼�����*/
    swSPDT.EventAttach_Change(When_ChangeSPDT);
    
//    for(uint8_t k = 0; k < __Sizeof(btMK); k++)
//    {
//        btMK[k].EventAttach_Press(When_ButtonPress_Page);
//        btMK[k].EventAttach_LongPress(When_ButtonLongPress_Page);
//        btMK[k].EventAttach_Release(When_ButtonRelease_Page);
//    }
}

/**
  * @brief  �����¼�����
  * @param  ��
  * @retval ��
  */
void ButtonEventMonitor()
{
    enum{
        L_Left = 7,
        L_Right = 6,
        L_Up = 3,
        L_Down = 4,
        R_Left = 2,
        R_Right = 5,
        R_Up = 0,
        R_Down = 1
    };
    /*�����¼�����*/
    btUP.   EventMonitor(ReadMatrixKeys(R_Left));
    btDOWN. EventMonitor(ReadMatrixKeys(R_Right));
    btOK.   EventMonitor(ReadMatrixKeys(L_Right));
    btBACK. EventMonitor(ReadMatrixKeys(L_Left));
    
    swSPDT. EventMonitor(digitalRead(SPDT_Switch_Pin));
    
    for(uint8_t k = 0; k < __Sizeof(btMK); k++)
    {
        btMK[k].EventMonitor(ReadMatrixKeys(k));
    }
}
