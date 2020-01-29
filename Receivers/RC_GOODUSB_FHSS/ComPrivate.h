#ifndef __COMPRIVATE_H
#define __COMPRIVATE_H

#include "NRF.h"
#include "RC_Protocol.h"

/*************** 从机配置 ***************/
//#define TIM_Handshake TIM3 //握手通信定时中断定时器
#define Slave_ID random(0,0xFF)
#define Slave_Description "GOOD USB"
#define Slave_Type X_COMMON
#define Slave_SupportPassback true
#define Slave_SupportFHSS     true
#define NRF_ComProcess_TimeMs 10

typedef Protocol_Common_t Protocol_CtrlType;

typedef struct {
  uint8_t Address[5];
  uint8_t Speed;
  uint8_t Freq;
} NRF_Config_TypeDef;

extern NRF_Basic nrf;
extern NRF_TRM nrfTRM;
extern NRF_FHSS nrfFHSS;
extern NRF_Config_TypeDef NRF_Cfg;
extern uint8_t NRF_TxBuff[32];
extern uint8_t NRF_RxBuff[32];
extern Protocol_Handshake_t HandshakePack_Attached;
extern Protocol_CtrlType RC_X;
extern ConnectState_t ConnectState;
extern bool State_PassBack;
extern bool State_FHSS;

/*************** 广播配置 ***************/
#define BroadcastHead_MasterWait    (uint16_t)0x55AA
#define BroadcastHead_SlaveWait     (uint16_t)0xAA55
#define BroadcastHead_MasterAttach  (uint16_t)0xF0F0
#define BroadcastHead_SlaveAttach   (uint16_t)0x0F0F
#define BroadcastAddr 18,54,98,66,23
#define BroadcastFreq 43
#define BroadcastSpeed nrf.SPEED_250Kbps

#endif
