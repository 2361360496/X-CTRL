#include "FileGroup.h"
#include "ComPrivate.h"

/*ʧ����ʱ500ms*/
#define ConnectLost_TimeOut 500

/*ʵ����NRF����*/
NRF_Basic nrf(
    NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin,
    NRF_CE_Pin, NRF_CSN_Pin
);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

NRF_TRM  nrfTRM(&nrf);
NRF_FHSS nrfFHSS(&nrf);

/*������*/
CTRL_TypeDef CTRL;

/*NRF�������ݻ�����*/
static uint8_t NRF_TxBuff[32];

/*NRF�������ݻ�����*/
static uint8_t NRF_RxBuff[32];

/*NRF��ַ�����*/
uint8_t NRF_AddressConfig[] =
{
    45, 23, 78, 19, 61, //ADDR:0
    75, 75, 75, 75, 75, //ADDR:1
    12, 34, 56, 78, 90, //ADDR:2
    12, 90, 17, 44, 55, //ADDR:3
    10, 93, 70, 72, 31, //ADDR:4
    0,  0,   0,  0,  0 //ADDR: Handshake
};

/*NRF�ź�ǿ��ָʾ*/
int16_t NRF_SignalStrength = 0;

/*NRF�������ñ�*/
NRF_Config_TypeDef NRF_Cfg = {0, 0, 40};

/*��������ʹ��*/
bool State_RF = OFF;

/*�ش�ʹ��*/
bool State_PassBack = ON;

/*ͨ�ûش�ģʽʹ��*/
bool Enable_CommonPassBack = true;

/*����ʹ��*/
bool State_Handshake = ON;

/*��Ƶʹ��*/
bool State_FHSS = OFF;

/**
  * @brief  ��ʼ��Ĭ��ͨ������
  * @param  ��
  * @retval ��
  */
void Init_DefaultChannel()
{
    RCX::ChannelAttachValueSetEnable(true);
    
    RCX::ChannelAttachValue(0, &CTRL.KnobLimit.L);
    RCX::ChannelAttachValue(1, &CTRL.KnobLimit.R);
    RCX::ChannelAttachValue(2, &CTRL.KnobCab.L);
    RCX::ChannelAttachValue(3, &CTRL.KnobCab.R);
    RCX::ChannelAttachValue(4, &CTRL.Left.X);
    RCX::ChannelAttachValue(5, &CTRL.Left.Y);
    RCX::ChannelAttachValue(6, &CTRL.Right.X);
    RCX::ChannelAttachValue(7, &CTRL.Right.Y);
    RCX::ChannelAttachValue(8, NULL);
    RCX::ChannelAttachValue(9, NULL);
    RCX::ChannelAttachValue(10, NULL);
    RCX::ChannelAttachValue(11, NULL);
}

/**
  * @brief  NRF��ʼ��
  * @param  ��
  * @retval true�ɹ� falseʧ��
  */
bool Init_NRF()
{
    /*Ĭ�ϳ�ʼ��*/
    bool isInit = nrf.Init();

    /*����ģʽ*/
    nrf.TX_Mode();

    /*���õ�ַ*/
    nrf.SetAddress((uint8_t*)NRF_AddressConfig + NRF_Cfg.Address * 5);

    /*����Ƶ��*/
    nrf.SetFreqency(NRF_Cfg.Freq);

    /*��������*/
    nrf.SetSpeed(NRF_Cfg.Speed);

    /*���ݰ�����*/
    nrf.SetPayloadWidth(sizeof(NRF_TxBuff), sizeof(NRF_RxBuff));

    /*������Ƶ*/
    nrf.SetRF_Enable(false);

    /*���¼Ĵ���״̬*/
    nrf.UpdateRegs();

    /*��ʼ��Ĭ��ͨ��*/
    Init_DefaultChannel();

    /*�����������*/
    return isInit;
}

void NRF_TxRx_Process()
{
    if(State_FHSS)
    {
        nrfFHSS.TxProcess(
            NRF_TxBuff,
            State_PassBack ? NRF_RxBuff : NULL
        );
    }
    else
    {
        nrfTRM.TranRecv(NRF_TxBuff, NRF_RxBuff);
    }

    if(State_PassBack)
    {
        RCX::PassbackProcess(NRF_RxBuff);
    }
}

/**
  * @brief  ���ݷ�������
  * @param  ��
  * @retval ��
  */
void Task_TransferData()
{
    if(State_RF == OFF) return;
    
    /*д�밴��״̬*/
    RCX::SetPackKey(CTRL.Key.Value);
    /*�������ݰ������ͻ�����*/
    RCX::LoadPack(NRF_TxBuff);

    if(State_FHSS || State_PassBack)
    {
        NRF_TxRx_Process();
    }
    else
    {
        if(nrf.GetRF_State() != nrf.State_TX)
        {
            nrf.TX_Mode();
        }
        nrf.TranCheck();
        nrf.Tran(NRF_TxBuff);//NRF��������
    }
}
