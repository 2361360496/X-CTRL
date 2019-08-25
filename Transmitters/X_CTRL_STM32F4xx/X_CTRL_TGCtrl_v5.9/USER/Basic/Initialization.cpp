#include "FileGroup.h"
#include "ComPrivate.h"

#define StorageDataReg(data) EEPROM_Register(&(data), sizeof(data))//ע�ᴢ�����

/**
  * @brief  ������ʼ��
  * @param  ��
  * @retval true�ɹ� falseʧ��
  */
bool Init_Value()
{
    /*ע����Ҫ���索����ı���*/
    StorageDataReg(JS_L);                   //��ҡ����Ϣ
    StorageDataReg(JS_R);                   //��ҡ����Ϣ
    StorageDataReg(CTRL.Info);              //������������Ϣ
    StorageDataReg(NRF_Cfg);                //NRF������Ϣ
    StorageDataReg(State_BuzzSound);        //������ʹ�ܿ���
    StorageDataReg(State_PassBack);         //�ش�ʹ�ܿ���
    StorageDataReg(State_Handshake);        //����ʹ�ܿ���
    StorageDataReg(State_Bluetooth);        //����ʹ�ܿ���
    StorageDataReg(State_PowerSaving);      //�͹���ʹ�ܿ���
    StorageDataReg(Bluetooth_ConnectObject);//���ƶ���
    StorageDataReg(NRF_AddressConfig);      //NRF��ַ

    if(EEPROM_Handle(EEPROM_Chs::ReadData) == false)//��ȡ��Ϣ�����ʧ�ܣ�ʹ��Ĭ�ϲ�����ʼ��
    {
        CTRL.Info.CtrlObject = X_COMMON;    //ͨ�ö���
        ConnectState.Pattern = Pattern_NRF; //ʹ��NRFң��

        JS_L.Xmin = 0;
        JS_L.Xmid = ADC_MaxValue / 2;
        JS_L.Xmax = ADC_MaxValue;

        JS_L.Ymin = 0;
        JS_L.Ymid = ADC_MaxValue / 2;
        JS_L.Ymax = ADC_MaxValue;

        JS_R.Xmin = 0;
        JS_R.Xmid = ADC_MaxValue / 2;
        JS_R.Xmax = ADC_MaxValue;

        JS_R.Ymin = 0;
        JS_R.Ymid = ADC_MaxValue / 2;
        JS_R.Ymax = ADC_MaxValue;
        return false;
    }
    return true;
}

/**
  * @brief  HC05(����)��ʼ��
  * @param  ��
  * @retval ��
  */
void Init_HC05()
{
    hc05.Init(115200);          //������115200
    hc05.Power(State_Bluetooth);//����ʹ��
}

/**
  * @brief  ң������ʼ��
  * @param  ��
  * @retval ��
  */
void Init_X_CTRL()
{
    Init_Display();         //��ʼ����ʾ��
    Init_Buzz();            //��ʼ��������
    Init_Motor();           //��ʼ���񶯵��
    Init_MPU6050();         //��ʼ��MPU6050
    Init_Value();           //��ʼ������
    Init_BottonEvent();     //��ʼ�������¼�
    Init_EncoderEvent();    //��ʼ����ת�������¼�
    Init_HC05();            //��ʼ������
    Init_XBox360Sim();      //��ʼ��XBox360ģ��

//    IS_KEY_PRESSED(KEY_DOWN_Pin, State_BuzzSound = OFF);//������ת��������ť��������

    IS_KEY_PRESSED(KEY_BACK_Pin, State_LuaScript = ON);

    if(ConnectState.Pattern == Pattern_NRF)//�Ƿ�ѡ��ʹ��NRFң��
    {
        bool NRF_IsDetect = false;
        __LoopExecute(NRF_IsDetect = Init_NRF(), 100);//NRF��ʼ��100��
        NRF_IsDetect ? BuzzMusic(0) : BuzzMusic(1);//�ж��Ƿ�ɹ���ʼ��NRF�������Ŷ�Ӧ����ʾ��
    }
}
