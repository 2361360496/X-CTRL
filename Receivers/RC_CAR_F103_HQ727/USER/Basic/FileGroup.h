#ifndef __FILEGROUP_H
#define	__FILEGROUP_H

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
#include "MillisTaskManager.h"
#include "CommonMacro.h"

//************* Objects *************//
extern MillisTaskManager mtm;

//*********** GPIO Define ***********//
#define CH1_Pin		PA0//TIM2 CH1
#define CH2_Pin		PA1//TIM2 CH2
#define CH3_Pin		PA2//TIM2 CH3
#define CH4_Pin		PA3//TIM2 CH4
#define CH5_Pin		PB6//TIM4 CH1
#define CH6_Pin		PB7//TIM4 CH2

#define Motor_Pin   CH5_Pin
#define Servo_Pin   CH6_Pin
#define FrontLED_Pin CH1_Pin

#define LED_Pin		PB4
#define SPD_DET_Pin PB5
#define NRF_IRQ_Pin		PB1
#define	NRF_MISO_Pin	PB0
#define NRF_MOSI_Pin	PA7
#define NRF_SCK_Pin		PA6
#define NRF_CSN_Pin		PA5
#define NRF_CE_Pin		PB4
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

//*********** Value ***********//


//*********** Functions ***********//
bool HandshakeToMaster();

void Init_NRF();
void Init_MotorServo();

void Task_NRF_Handler();
void Task_Ctrl();
void Task_InfoLED();
void Task_MID_Setting();

#endif
