#ifndef __FILEGROUP_H
#define	__FILEGROUP_H

//*************** STM32 Core *****************//
#include "delay.h"
#include "pwm.h"
#include "adc.h"
#include "gpio.h"
#include "exti.h"
#include "timer.h"

//************* Arduino API **************//
#include "Arduino.h"
#include "WMath.h"
#include "Tone.h"

//************* User Libraries ************//
#include "ButtonEvent.h"
#include "EncoderEvent.h"
#include "MillisTaskManager.h"
#include "RC_Protocol.h"
#include "CommonMacro.h"
#include "Config.h"
#include "shellclient.h"
#include "nrf.h"

namespace EEPROM_Chs {
typedef enum {
    ReadData,
    SaveData,
    ClearData
} EEPROM_Chs_t;
}

//************* Object & Struct & Value**************///*Object*/
extern ButtonEvent btEcd;
extern EncoderEvent ecd;
extern ButtonEvent btMK[9];

/*Struct*/
extern Joystick_t JS_L;
extern Joystick_t JS_R;
extern ShellClient client;

/*Value*/
extern bool State_BuzzSound;
extern float BattUsage;
extern float BattVoltage;

//****************** Functions********************//
void BuzzMusic(uint8_t music);
void BuzzTone(uint32_t freq, uint32_t time);
void BuzzTone(uint32_t freq);
void ButtonEventMonitor();

#define Client_Timeout				100
#define Client_CallTimeout		3000
void Client_Handle();
uint32_t Client_Call(const char *fname, int pSize, void *params, void *ret);
typedef struct
{
	float Yaw;								/* ����ֵ ��λ degree */
	float Pitch;							/* ����ֵ ��λ degree */
	float Roll;								/* ���ֵ ��λ degree */
} AttitudePoint_TypeDef;
/* ָʾһ����ĳλ�õ�ָ��λ�õĹ켣/�ڵ� 
 * ----------------------------------------------
 * 
 */
typedef struct
{
	char Name[12];												/* ָʾ�ڵ������ */
	AttitudePoint_TypeDef Attitude;				/* ָʾ�ڵ��ȶ�ʱ����̬ */
	float SettleTime;											/* ָʾ�ڵ�ͣ��ʱ�� */
	uint8_t TrackAnimationIndex;					/* ָʾʹ����Դ�б��е���һ������ʵ�����л����˽ڵ� */
} TrackPoint_TypeDef;

/* ָʾһ����������
 * ----------------------------------------------
 * 
 */
typedef struct
{
	char Name[11];												/* ָʾ���������� */
	uint8_t TrackTypeID;									/* ָʾ�ö���ʵ��ʹ�õĻ������������� ID */
	float Duration;												/* ָʾ�ö���ʵ���ĳ���ʱ�� */
	float Params[4];											/* ָʾ�ö���ʵ��������Ҫʹ�õĲ��� */
} TrackAnimation_TypeDef;


void EncoderMonitor();
bool EEPROM_Handle(EEPROM_Chs::EEPROM_Chs_t chs);
bool EEPROM_Register(void *pdata, uint16_t size);

void Init_X_CTRL();
bool Init_Value();
void Init_Buzz();
void Init_BottonEvent();
void Init_Display();
void Init_EncoderEvent();
void Init_GUI();
bool Init_SD();
void Init_Client();

void Thread_GUI();
void Task_SensorUpdate();
void Task_MusicPlayerRunning();

#endif
