#include "FileGroup.h"
#include "ComPrivate.h"

#define StorageDataReg(data) EEPROM_Register(&(data), sizeof(data))//ע�ᴢ�����

#define IS_KEY_PRESSED(key,func) \
do{\
   if(!digitalRead(key))\
   {\
       delay(10);\
       if(!digitalRead(key))\
           func;\
   }\
}while(0)

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
    StorageDataReg(DR_ST_Value);            //ת���޷���Ϣ
    StorageDataReg(DR_TH_Value);            //�����޷���Ϣ
    StorageDataReg(NRF_Cfg);                //NRF������Ϣ
    StorageDataReg(State_BuzzSound);        //������ʹ�ܿ���
    StorageDataReg(State_PassBack);         //�ش�ʹ�ܿ���
    StorageDataReg(State_Handshake);        //����ʹ�ܿ���
    StorageDataReg(State_Bluetooth);        //����ʹ�ܿ���
    StorageDataReg(State_NoOperationMonitor);//��ʱ���޲������ʹ�ܿ���
    StorageDataReg(Bluetooth_ConnectObject);//���ƶ���
    StorageDataReg(NRF_AddressConfig);      //NRF��ַ
    StorageDataReg(JS_L_SlopeStart);
    StorageDataReg(JS_L_SlopeEnd);
    StorageDataReg(JS_R_SlopeStart);
    StorageDataReg(JS_R_SlopeEnd);

    if(EEPROM_Handle(EEPROM_Chs::ReadData) == false)//��ȡ��Ϣ�����ʧ�ܣ�ʹ��Ĭ�ϲ�����ʼ��
    {
        CTRL.Info.CtrlObject = X_COMMON;    //ͨ�ö���
        ConnectState.Pattern = Pattern_NRF; //ʹ��NRFң��

        JS_L.Xmin = 0;
        JS_L.Xmid = ADC_MaxValue / 2;
        JS_L.Xmax = ADC_MaxValue;

        JS_L.Ymin = 1703;
        JS_L.Ymid = 2020;
        JS_L.Ymax = 2929;

        JS_R.Xmin = 324;
        JS_R.Xmid = 2017;
        JS_R.Xmax = 3917;

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
    Init_Sensors();         //��ʼ��������
    Init_HC05();            //��ʼ������
    Init_XBox360Sim();      //��ʼ��XBox360ģ��

    IS_KEY_PRESSED(KEY_DOWN_Pin, State_BuzzSound = OFF);//���°�ť��������

    IS_KEY_PRESSED(KEY_SEL_Pin, State_LuaScript = ON);

    if(ConnectState.Pattern == Pattern_NRF)//�Ƿ�ѡ��ʹ��NRFң��
    {
        bool NRF_IsDetect = false;
        __LoopExecute(NRF_IsDetect = Init_NRF(), 100);//NRF��ʼ��100��
        NRF_IsDetect ? BuzzMusic(MC_Type::MC_StartUp) : BuzzMusic(MC_Type::MC_Error);//�ж��Ƿ�ɹ���ʼ��NRF�������Ŷ�Ӧ����ʾ��
    }
}
