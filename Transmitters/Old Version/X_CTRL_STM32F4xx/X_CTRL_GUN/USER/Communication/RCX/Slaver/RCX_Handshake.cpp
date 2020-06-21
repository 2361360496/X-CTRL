#include "../RCX.h"

using namespace RCX;
using namespace Handshake;

static NRF_TRM*  nrfTRM  = NULL;
static NRF_FHSS* nrfFHSS = NULL;
static uint8_t NRF_TxBuff[32];
static uint8_t NRF_RxBuff[32];
static const uint8_t NRF_ComProcess_TimeMs = 10;
static bool NRF_PassBack_Enable = false;
static bool NRF_FHSS_Enable = false;

static Pack_t Master;//�����������ݰ�
static Pack_t Slave;//�ӻ��������ݰ�
static Pack_t Attached;//���������ݰ�         

static uint8_t HankshakeState = State_Idle;

uint8_t Handshake::GetSlaveID()
{
    return Slave.ID;
}

bool Handshake::GetPassBack_Enable()
{
    return NRF_PassBack_Enable;
}

bool Handshake::GetFHSS_Enable()
{
    return NRF_FHSS_Enable;
}

void Handshake::Init(
    NRF_TRM*  trm, NRF_FHSS* fhss, 
    const char* name
)
{
    nrfTRM = trm;
    nrfFHSS = fhss;
    strncpy(Slave.Description, name, sizeof(Slave.Description));
    HankshakeState = State_Prepare;
}

/**
  * @brief  NRFͨ�Ŵ���(T = 10ms)
  * @param  ��
  * @retval ��
  */
static void NRF_ComProcess_Handler()
{
    nrfTRM->RecvTran(NRF_RxBuff, NRF_TxBuff);
}

/**
  * @brief  ������ͨ��,��������
  * @param  *master:�������ְ�ָ��
  * @retval ��
  */
static void ComToMaster(Pack_t *master)
{
    static uint32_t lastTime;
    if(millis() - lastTime >= NRF_ComProcess_TimeMs)
    {
        lastTime = millis();
        NRF_ComProcess_Handler();
    }
    *(Pack_t*)NRF_TxBuff = Slave;
    *master = *(Pack_t*)NRF_RxBuff;
}

/**
  * @brief  ��������״̬��
  * @param  ��
  * @retval true: �ɹ�, false: ʧ��
  */
bool Handshake::Process()
{
    if(nrfTRM == NULL || nrfFHSS == NULL)
        return false;
    
    uint8_t ret = false;
    uint32_t time = 0;

    switch (HankshakeState)
    {
    case State_Prepare://����׼��״̬
        nrfTRM->Basic->SetRF_Enable(false);
        nrfTRM->Basic->SetAddress(RCX_BROADCAST_ADDR);//���ù㲥��ַ
        nrfTRM->Basic->SetFreqency(RCX_BROADCAST_FREQ);//���ù㲥Ƶ��
        nrfTRM->Basic->SetSpeed(RCX_BROADCAST_SPEED);//���ù㲥ͨ���ٶ�
        nrfTRM->Basic->SetAutoRetryTimeout(10);
        nrfTRM->Basic->RX_Mode();
        nrfTRM->Basic->SetRF_Enable(true);

        Slave.BroadcastHead = BroadcastHead_SlaveWait;//����֡ͷʶ����Ϊ�ӻ��ȴ�
#if (RCX_USE_CUSTOM_ID != 0)
        Slave.ID = RCX_USE_CUSTOM_ID;//�����Զ���ӻ�ID
#else
        Slave.ID = random(1, 0xFF);//���ID
#endif

        Slave.Type = RCX_TYPE;//���ôӻ���������
        Slave.EnableFunction.Passback = RCX_USE_PASSBACK;//�����Ƿ�֧�ֻش�
        Slave.EnableFunction.FHSS = RCX_USE_FHSS;//�����Ƿ�֧����Ƶ
        Slave.Speed = 0;//���ôӻ�ͨ���ٶ�
        Slave.CMD = CMD_ReqConnect;//���ôӻ�����:��������

        HankshakeState = State_Search;//״̬����ת������״̬
        break;

    case State_Search://�㲥����״̬
        ComToMaster(&Master);//�ڹ㲥��ַ�������ݣ����ѵ���������Ϣ����HandshakePack_Master
        if ( Master.BroadcastHead == BroadcastHead_MasterAttach //֡ͷʶ����Ϊ������������
                && Master.CMD == CMD_AttachConnect//��������Ϊ�����
                && Master.ID == Slave.ID)//����ID��ӻ�IDһ��
        {
            Attached = Master;//����������������������Ϣ(��ַ����Ƶ���)

            Slave.BroadcastHead = BroadcastHead_SlaveAttach;//֡ͷʶ��������Ϊ�ӻ���Ӧ��
            Slave.CMD = CMD_AgreeConnect;//�ӻ�ͬ������
            HankshakeState = State_ReqConnect;//״̬����ת����������״̬
        }
        break;

    case State_ReqConnect://��������״̬
        time = millis();
        while (millis() - time < 1000) //��������1�����ݰ���������ʾ��������
        {
            ComToMaster(&Master);
        }

        HankshakeState = State_Connected;//״̬����ת������֮��״̬
        break;

    case State_Connected: //����֮��״̬
        nrfTRM->Basic->SetRF_Enable(false);
        nrfTRM->Basic->SetAddress(Attached.Address);//Ӧ���µ�ַ
        nrfTRM->Basic->SetFreqency(Attached.FerqList[0]);//Ӧ����Ƶ��
        nrfTRM->Basic->SetSpeed(Attached.Speed);//Ӧ����ͨ������
        nrfTRM->Basic->SetAutoRetryTimeout(NRF_ComProcess_TimeMs);

        if(RCX_USE_PASSBACK)
            NRF_PassBack_Enable = Attached.EnableFunction.Passback;

        if(RCX_USE_FHSS)
        {
            NRF_FHSS_Enable = Attached.EnableFunction.FHSS;
            nrfFHSS->SetFreqHoppingList(Attached.FerqList, sizeof(Attached.FerqList));
        }

        nrfTRM->Basic->RX_Mode();
        nrfTRM->Basic->SetRF_Enable(true);
        
        HankshakeState = State_Idle;
        
        RCX::SetRxTimeUpdate();

        ret = true;
        break;
    }
    return ret;
}
