#include "FileGroup.h"
#include "XFS.h"
#include <stdio.h>
#include <stdarg.h>
#include "FifoQueue.h"

/*ʵ���������ϳɶ���*/
static XFS5152CE xfs(&Serial);

/*ʵ���������ϳɶ��У����г���5*/
static FifoQueue<String> SpeakStr(5);

/*�����ϳ������־λ*/
#define AllowRunXFS (State_Bluetooth && Bluetooth_ConnectObject == BC_Type::BC_XFS)

/**
  * @brief  �����ϳɴ����жϻص�
  * @param  ��
  * @retval ��
  */
static void XFS_USART_Callback()
{
    xfs.ChipStatus = USART_ReceiveData(USART1);
}

/**
  * @brief  ��ʼ�������ϳ�
  * @param  ��
  * @retval ��
  */
void Init_XFS()
{
    Serial.attachInterrupt(XFS_USART_Callback);
    xfs.SetReader(xfs.Reader_XiaoYan);
//  xfs.SetLanguage(xfs.Language_English);
    xfs.SetStyle(xfs.Style_Continue);
    xfs.SetArticulation(xfs.Articulation_Word);
}

/**
  * @brief  ��������ϳɶ���
  * @param  ��
  * @retval ��
  */
void XFS_Clear()
{
    SpeakStr.flush();
}

/**
  * @brief  ���������ϳɶ���
  * @param  ��
  * @retval ��
  */
void XFS_Speak(String str)
{
    if(!AllowRunXFS || !hc05.GetState())
        return;

    SpeakStr.write(str);
}

/**
  * @brief  ���������ϳɶ��У�֧���Զ����ʽ�����
  * @param  ��
  * @retval ��
  */
int XFS_Speak(const char *__restrict __format, ...)
{
    if(!AllowRunXFS || !hc05.GetState())
        return 0;
    
    char printf_buff[100];

    va_list args;
    va_start(args, __format);
    int ret_status = vsnprintf(printf_buff, sizeof(printf_buff), __format, args);
    va_end(args);
    SpeakStr.write(String(printf_buff));

    return ret_status;
}

/**
  * @brief  �����ϳɶ���ɨ������
  * @param  ��
  * @retval ��
  */
void Task_XFS_ListCheck()
{
    if(!AllowRunXFS)
        return;
    
    /*�������ϳɿ��У����Ҷ������д��ϳɵ�����ʱ*/
    if(xfs.ChipStatus == xfs.ChipStatus_Idle && SpeakStr.available())
        xfs.StartSynthesis(SpeakStr.read());//�ϳ�����
}
