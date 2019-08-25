#ifndef __FILEGROUP_H
#define	__FILEGROUP_H

/** 
  *@Illustrate: A Cross-platform migration Project | Arduino for STM32F10x 
  *@Author: FASTSHIFT
  *@Website: https://github.com/FASTSHIFT/Arduino-For-Keil
  */

//************* Arduino Library **************//
#include "Arduino.h"
#include "HardwareSerial.h"
#include "Stream.h"
#include "SPI.h"
#include "WString.h"
#include "Wire.h"
#include "Tone.h"
#include "WMath.h"

//************* STM32 Core Library **************//
#include "delay.h"
#include "exti.h"
#include "gpio.h"
#include "adc.h"
#include "pwm.h"
#include "timer.h"

//********** User Libraries *********//
#include "NRF.h"
#include "MillisTaskManager.h"
#include "RC_Protocol.h"
#include "CommonMacro.h"

//************* Objects *************//
extern NRF nrf;
extern MillisTaskManager mtm;

//*********** GPIO Define ***********//
#define CH4_Pin		PA3//TIM2 CH4 ADC
#define CH3_Pin		PA2//TIM2 CH3 ADC
#define CH2_Pin		PA1//TIM2 CH2 ADC
#define CH1_Pin		PA0//TIM2 CH1 ADC
#define CH6_Pin		PB7//TIM4 CH2
#define CH5_Pin		PB6//TIM4 CH1

#define LED_Pin		        PA15
#define SPD_DET_Pin         CH5_Pin
#define Buzz_Pin            PC0
#define Motor_Pin	        CH4_Pin
#define BattDet_Pin         CH3_Pin
#define HandshakeEnable_Pin CH2_Pin

#define NRF_IRQ_Pin		PB2
#define	NRF_MISO_Pin	PA6
#define NRF_MOSI_Pin	PA7
#define NRF_SCK_Pin		PA5
#define NRF_CSN_Pin		PB1
#define NRF_CE_Pin		PB0
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

//*********** Value ***********//

enum TaskPriority{
    TP_NRF_Handler,
    TP_SpeedGenerator,
    TP_Ctrl,
    TP_BuzzRing,
    TP_InfoReport,
    TP_InfoLED,
    TP_FeedWatchDog,
    TP_MAX
};

namespace EEPROM_Chs {
typedef enum {
    ReadData,
    SaveData,
    ClearData
} EEPROM_Chs_t;
}

//*********** Functions ***********//
void Task_NRF_Handler();
void Task_Ctrl();
void Task_SpeedGenerator();
void Task_BuzzRing();
void Task_InfoReport();
void Task_InfoLED();
void NRF_Init();
void NRF_IRQHandler();
void Init_Motor();
void Init_SpeedDet();
bool HandshakeToMaster(uint8_t setState = 0xff);

bool EEPROM_Register(void *pdata, uint16_t size);
bool EEPROM_Handle(EEPROM_Chs::EEPROM_Chs_t chs);

#endif
