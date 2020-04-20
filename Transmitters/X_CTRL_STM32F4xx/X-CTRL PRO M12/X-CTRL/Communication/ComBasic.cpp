#include "Basic/FileGroup.h"
#include "ComPrivate.h"
#include "BSP/IMU_Private.h"

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

/*ͨ��ʹ��*/
static bool Com_Enable = OFF;

void Com_SetEnable(bool en)
{
    Com_Enable = en;
    Com_PassbackProcess(en ? PBS_Setup : PBS_Exit);
}

bool Com_GetEnable()
{
    return Com_Enable;
}

/**
  * @brief  ��ʼ��Ĭ��ͨ������
  * @param  ��
  * @retval ��
  */
void Com_SetDefaultChannel()
{
    RCX::ChannelSetAttachEnable(true);
    
    RCX::ChannelSetAttach(0, &CTRL.KnobLimit.L);
    RCX::ChannelSetAttach(1, &CTRL.KnobLimit.R);
    RCX::ChannelSetAttach(2, NULL);
    RCX::ChannelSetAttach(3, NULL);
    RCX::ChannelSetAttach(4, &CTRL.JS_L.X.Val);
    RCX::ChannelSetAttach(5, &CTRL.JS_L.Y.Val);
    RCX::ChannelSetAttach(6, &CTRL.JS_R.X.Val);
    RCX::ChannelSetAttach(7, &CTRL.JS_R.Y.Val);
}

/**
  * @brief  ͨ�ų�ʼ��
  * @param  ��
  * @retval true�ɹ� falseʧ��
  */
bool Com_Init()
{
    /*Ĭ�ϳ�ʼ��*/
    bool isInit = nrf.Init();

    /*����ģʽ*/
    nrf.TX_Mode();

    /*���ݰ�����*/
    nrf.SetPayloadWidth(sizeof(NRF_TxBuff), sizeof(NRF_RxBuff));

    /*������Ƶ*/
    nrf.SetRF_Enable(false);

    /*���¼Ĵ���״̬*/
    nrf.UpdateRegs();

    /*��ʼ��Ĭ��ͨ��*/
    Com_SetDefaultChannel();

    /*�����������*/
    return isInit;
}

static void Com_TxRxProcess()
{
    if(CTRL.State.FHSS)
    {
        nrfFHSS.TxProcess(
            NRF_TxBuff,
            CTRL.State.PassBack ? NRF_RxBuff : NULL
        );
    }
    else
    {
        nrfTRM.TranRecv(NRF_TxBuff, NRF_RxBuff);
    }

    if(CTRL.State.PassBack)
    {
        RCX::LoadRxPack(NRF_RxBuff);
        Com_PassbackProcess(RCX::GetRxSignalLost() ? PBS_Error : PBS_Loop);
    }
}

/**
  * @brief  ���ݷ�������
  * @param  ��
  * @retval ��
  */
void Com_Update()
{
    if(!Com_Enable)
        return;
    
    /*д�밴��״̬*/
    RCX::SetTxPackKey(CTRL.Key.Value);
    /*�������ݰ������ͻ�����*/
    RCX::LoadTxPack(NRF_TxBuff);

    if(CTRL.State.FHSS || CTRL.State.PassBack)
    {
        Com_TxRxProcess();
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
