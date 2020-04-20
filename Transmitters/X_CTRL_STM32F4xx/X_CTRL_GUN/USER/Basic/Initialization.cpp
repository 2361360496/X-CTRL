#include "FileGroup.h"
#include "ComPrivate.h"
#include "cm_backtrace.h"

#define StorageDataReg(data)\
do{\
    Debug_SERIAL.printf("Reg value: %s (addr=0x%x size=%d)\r\n", #data, &data, sizeof(data));\
    EEPROM_Register(&(data), sizeof(data));\
}while(0)

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
    DEBUG_FUNC();
    /*ע����Ҫ���索����ı���*/
    StorageDataReg(CTRL);                   //ң������Ϣ
    StorageDataReg(DR_ST_Value);            //ת���޷���Ϣ
    StorageDataReg(DR_TH_Value);            //�����޷���Ϣ
    StorageDataReg(NRF_Cfg);                //NRF������Ϣ
    StorageDataReg(NRF_AddressConfig);      //NRF��ַ
    
    return EEPROM_Handle(EEPROM_Chs::ReadData);
}

/**
  * @brief  HC05(����)��ʼ��
  * @param  ��
  * @retval ��
  */
void Init_HC05()
{
    DEBUG_FUNC();
    hc05.Init(115200);          //������115200
    hc05.Power(CTRL.Bluetooth.Enable);//����ʹ��
}

/**
  * @brief  ң������ʼ��
  * @param  ��
  * @retval ��
  */
void Init_X_CTRL()
{
    DEBUG_FUNC();
    cm_backtrace_init(_X_CTRL_NAME, _X_CTRL_VERSION , __DATE__);
    Init_Display();         //��ʼ����ʾ��
    Init_Sensors();         //��ʼ��������
    Init_HC05();            //��ʼ������
    
    if(Init_SD())
        Init_BvPlayer();
    
    Init_Value();           //��ʼ������

    IS_KEY_PRESSED(KEY_DOWN_Pin, CTRL.State.Sound = OFF);//���°�ť��������

    IS_KEY_PRESSED(KEY_SEL_Pin, CTRL.State.LuaScript = ON);

    /*�ж��Ƿ�ɹ���ʼ��ͨ�ţ������Ŷ�Ӧ����ʾ��*/
    Com_Init() 
    ? BuzzMusic(MC_Type::MC_StartUp) 
    : BuzzMusic(MC_Type::MC_Error);
}
