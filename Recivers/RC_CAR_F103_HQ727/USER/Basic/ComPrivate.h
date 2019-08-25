#ifndef __COMPRIVATE_H
#define __COMPRIVATE_H

#include "NRF.h"
#include "RC_Protocol.h"

/*************** 从机配置 ***************/
#define Slave_ID 0x07
#define Slave_Description "HQ727"
#define Slave_Type CAR_ServoSteering
#define Slave_PassbackSupport true

/*************** 广播配置 ***************/
#define BroadcastHead_MasterWait    (uint16_t)0x55AA
#define BroadcastHead_SlaveWait     (uint16_t)0xAA55
#define BroadcastHead_MasterAttach  (uint16_t)0xF0F0
#define BroadcastHead_SlaveAttach   (uint16_t)0x0F0F
#define BroadcastAddr 18,54,98,66,23
#define BroadcastFreq 43
#define BroadcastSpeed nrf.SPEED_250Kbps

typedef Protocol_CarSS_t Protocol_CtrlType;

extern NRF nrf;
extern uint8_t NRF_TxBuff[32];
extern uint8_t NRF_RxBuff[32];
extern Protocol_CtrlType RC_X;
extern ConnectState_t ConnectState;
extern bool PassBack_State;

#endif
