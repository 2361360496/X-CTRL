#include "FileGroup.h"
#include "ComPrivate.h"

/*ʵ����NRF����*/
NRF nrf(NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin, NRF_CE_Pin, NRF_CSN_Pin);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

ShellClient client("JW_Controller", NRF_SendPacket);
uint8_t Client_SlaverCount = 0;

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

void NRF_Handle()
{
	uint8_t status =  nrf.Recv(NRF_RxBuff);
	if(status & nrf.RX_DR)
	{
		client.PacketReceived((char *)NRF_RxBuff);
	}
	client.Handle();
}

bool NRF_SendPacket(char *buffer)
{
	volatile uint32_t nowMS = millis();
	nrf.TX_Mode((uint8_t *)buffer);
	
	while(millis() - nowMS < 100)
	{
		delay(20);
		uint8_t status = nrf.Tran((uint8_t *)buffer);
		if(status & nrf.TX_DS)
		{
			nrf.RX_Mode();
			return true;
		}
		else if(status & nrf.MAX_RT)
		{
			nrf.RX_Mode();
			return false;
		}
	}
	nrf.RX_Mode();
	return false;
}


void Init_Client()
{
	/* ����׼������ */
//	HandshakeRun(HandshakeState_Prepare);
//	/* ��ʱ����5000ms */
//	uint32_t time = millis();
//	
//	uint8_t slaverCnt = 0;
//	/* �����ӻ� */
//	while(millis() - time < 3000)
//	{
//		/*��ȡ�ӻ��б�����*/
//		slaverCnt = HandshakeRun(HandshakeState_Search);
//	}
//	
//	Client_SlaverCount = slaverCnt;
}

/**
  * @brief  NRF��ʼ��
  * @param  ��
  * @retval true�ɹ� falseʧ��
  */
bool Init_NRF()
{
    /*�ܳ�ʼ��*/
    nrf.init(nrf.TX_MODE, NRF_TxBuff, sizeof(NRF_TxBuff), sizeof(NRF_RxBuff));
	nrf.SetAddress(NRF_ADDR);
	nrf.SetSpeed(nrf.SPEED_2Mbps);
	nrf.SetFreqency(NRF_CLIENTCH);

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
