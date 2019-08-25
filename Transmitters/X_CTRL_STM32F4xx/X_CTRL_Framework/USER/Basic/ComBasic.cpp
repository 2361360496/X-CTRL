#include "FileGroup.h"
#include "ComPrivate.h"

/*ʵ����NRF����*/
NRF nrf(NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin, NRF_CE_Pin, NRF_CSN_Pin);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

/*NRF�������ݻ�����*/
uint8_t NRF_TxBuff[32];

/*NRF�������ݻ�����*/
uint8_t NRF_RxBuff[32];

/*��������ʹ��*/
bool State_RF = OFF;

/*�ش�ʹ��*/
bool State_PassBack = OFF;

/*����ʹ��*/
bool State_Handshake = ON;

/**
  * @brief  NRF��ʼ��
  * @param  ��
  * @retval true�ɹ� falseʧ��
  */
bool Init_NRF()
{
    /*�ܳ�ʼ��*/
    nrf.init(nrf.TXRX_MODE, NRF_TxBuff, sizeof(NRF_TxBuff), sizeof(NRF_RxBuff));

    /*�����������*/
    return nrf.IsDetect();
}

/**
  * @brief  ���ݷ�������
  * @param  ��
  * @retval ��
  */
void Task_TransferData()
{
    if(State_RF == OFF) return;
}
