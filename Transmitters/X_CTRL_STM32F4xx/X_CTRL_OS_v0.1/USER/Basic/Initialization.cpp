#include "FileGroup.h"
#include "ComPrivate.h"

Protocol_Common_t CTRL;

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
    StorageDataReg(State_BuzzSound);        //������ʹ�ܿ���
	
    if(EEPROM_Handle(EEPROM_Chs::ReadData) == false)//��ȡ��Ϣ�����ʧ�ܣ�ʹ��Ĭ�ϲ�����ʼ��
    {
        CTRL.Info.CtrlObject = X_COMMON;    //ͨ�ö���

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
  * @brief  ң������ʼ��
  * @param  ��
  * @retval ��
  */
void Init_X_CTRL()
{
    Init_Display();         //��ʼ����ʾ��
    Init_Buzz();            //��ʼ��������
    Init_Motor();           //��ʼ���񶯵��
    Init_Value();           //��ʼ������
    Init_BottonEvent();     //��ʼ�������¼�
    Init_EncoderEvent();    //��ʼ����ת�������¼�
	
    IS_KEY_PRESSED(KEY_DOWN_Pin, State_BuzzSound = OFF);//������ת��������ť��������
    
    IS_KEY_PRESSED(KEY_SEL_Pin, State_LuaScript = ON);
	
	BuzzMusic(0);
}
