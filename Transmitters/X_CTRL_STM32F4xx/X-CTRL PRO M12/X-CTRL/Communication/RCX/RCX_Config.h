/** @Describe: X-CTRLר��Э����
  * @Author: _VIFEXTech
  * @Finished: UNKNOWTIME - v1.0 Э���ܻ������
  * @Upgrade:  UNKNOWTIME - v1.1 �޸Ĳ��ֽṹ��
  * @Upgrade:  2018.12.25 - v1.2 ȥ��TimePoint_Tran, �������Э����ʱ�ӻ�׼
  * @Upgrade:  2018.12.26 - v1.3 ����Э����Գɹ�
  * @Upgrade:  2019.2.13  - v1.4 ʹ�õ��ֽڶ��룬��֤��ƽ̨������
  * @Upgrade:  2020.1.20  - v1.5 �����Ƶ����
  * @Upgrade:  2020.1.30  - v1.6 Э����ع�����Э������ΪRCX�����ݾ�Э��
  * @Upgrade:  2020.3.25  - v1.7 ���ƿ�ܣ��ش�Э���뷢��Э��ͳһ��ȥ��CtrlObjectö��
  * @Upgrade:  2020.3.27  - v1.8 ���ͨ����ת����
  */
#ifndef __RCX_CONFIG_H
#define __RCX_CONFIG_H

/*�Զ���*/
#define RCX_IS_MASTER             1
#define RCX_NAME                  "X-CTRL"
#define RCX_TYPE                  RCX::GetTxObjectType()
#define RCX_CONNECT_LOST_TIMEOUT  500
#define RCX_SIGNAL_LOST_TIMEOUT   6000

/*ͨ��*/
#define RCX_CHANNEL_NUM         8
#define RCX_CHANNEL_DATA_MAX    1000
#define RCX_CHANNEL_DATA_LIMIT  1

/*����ʹ��*/
#define RCX_USE_CUSTOM_ID       0
#define RCX_USE_CRC             1
#define RCX_USE_PASSBACK        1
#define RCX_USE_FHSS            1
#define RCX_USE_CH_DEF          0

#endif
