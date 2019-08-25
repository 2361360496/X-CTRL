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
    StorageDataReg(State_BuzzSound);        //������ʹ�ܿ���
	
    if(EEPROM_Handle(EEPROM_Chs::ReadData) == false)//��ȡ��Ϣ�����ʧ�ܣ�ʹ��Ĭ�ϲ�����ʼ��
    {
        JS_L.Xmin = 900;
        JS_L.Xmid = 2100;
        JS_L.Xmax = 3235;
        
        JS_L.Ymin = 930;
        JS_L.Ymid = 2084;
        JS_L.Ymax = 3393;

        JS_R.Xmin = 804;
        JS_R.Xmid = 2044;
        JS_R.Xmax = 3140;
        
        JS_R.Ymin = 862;
        JS_R.Ymid = 2072;
        JS_R.Ymax = 3354;
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
	Init_Value();           //��ʼ������
	Init_BottonEvent();     //��ʼ�������¼�
	Init_EncoderEvent();    //��ʼ����ת�������¼�
	Init_Client();	
	
	Serial.begin(115200);

}
