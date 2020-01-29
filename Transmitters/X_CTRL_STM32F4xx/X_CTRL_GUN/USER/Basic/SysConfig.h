#ifndef __SYSCONFIG_H
#define __SYSCONFIG_H

#define ADC_MaxValue 4095
#define ADC_HalfValue (ADC_MaxValue/2)
//#define USE_Encoder

//*************GPIO**************//

#define NULL_PIN PD0

#define KEY_UP_Pin      PC0
#define KEY_DOWN_Pin    PC1
#define KEY_SEL_Pin     PC2
#define KEY_CH_Pin      PC3

#define JSL_X_Pin PA0
#define JSL_Y_Pin PA0
#define JSR_X_Pin PA1
#define JSR_Y_Pin PA1

#define BattSensor_Pin PB0
#define BattCharge_Pin PB1

#define JSL_X_ADC()         ADC_HalfValue
#define JSL_Y_ADC()         analogRead_DMA(JSL_Y_Pin)
#define JSR_X_ADC()         (ADC_MaxValue-analogRead_DMA(JSR_X_Pin))
#define JSR_Y_ADC()         ADC_HalfValue
#define ADL_ADC()           ADC_MaxValue
#define ADR_ADC()           ADC_MaxValue
#define BattSensor_ADC()    analogRead_DMA(BattSensor_Pin)
#define Temp_ADC()          ADC_DMA_GetValue(ADC_Channel_TempSensor)

#ifdef USE_Encoder
#define EncoderLED_Pin  NULL_PIN
#define EncoderKey_Pin  NULL_PIN
#define EncoderA_Pin    NULL_PIN
#define EncoderB_Pin    NULL_PIN
#endif

#define Buzz_Pin            PA2
#define Motor_Pin           PA3
#define SPDT_Switch_Pin     PC13
#define HC165_OUT_Pin       PB12
#define HC165_CP_Pin        PB13
#define HC165_PL_Pin        PB14

#define IS_EnableCtrl()     digitalRead_FAST(SPDT_Switch_Pin)
#define IS_BattCharge()     !digitalRead_FAST(BattCharge_Pin)

#define NRF_MOSI_Pin PB5
#define NRF_MISO_Pin PB4
#define NRF_SCK_Pin PB3
#define NRF_CE_Pin PC7
#define NRF_CSN_Pin PA15
#define NRF_IRQ_Pin PB9
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

#define BT_PowerCtrl_Pin    PC6
#define BT_State_Pin        PB2
#define BT_EN_Pin           PB15

#define SDIO_D0_Pin PC8
#define SDIO_D1_Pin PC9
#define SDIO_D2_Pin PC10
#define SDIO_D3_Pin PC11
#define SDIO_SCK_Pin PC12
#define SDIO_CMD_Pin PD2

#define SD_CS_Pin   SDIO_D3_Pin
#define SD_SCK_Pin  SDIO_SCK_Pin
#define SD_MOSI_Pin SDIO_CMD_Pin
#define SD_MISO_Pin SDIO_D0_Pin
#define SD_CD_Pin   PA8

#define TFT_CS      PA4
#define TFT_RST     PC5
#define TFT_DC      PC4
#define TFT_MOSI    PA7
#define TFT_CLK     PA5
#define TFT_LED     PA6

/*�����͵�ѹ2.75V�����4.15V*/
#define BattEmptyVoltage 2.75f
#define BattFullVoltage 4.15f

/*����������ʱ��*/
#define TIM_ControlTask TIM7

/*��������ʱ��*/
#define TIM_BuzzTone    TIM1

/*����ͨ�Ŷ�ʱ�ж϶�ʱ��*/
#define TIM_Handshake   TIM4

/*����ģ�鴮�����*/
#define XFS_SERIAL Serial3

/*HMI�������*/
#define HMI_SERIAL Serial3

/*Lua�������*/
#define Lua_SERIAL Serial

/*������Ϣ�������*/
#define Debug_SERIAL Serial

/*����ͨ�Ŵ������*/
#define Bluetooth_SERIAL Serial3

/*�޲���ʱ�䳬ʱ����(����)*/
#define NoOperationTimeout (10*60)

/*����DEBUG*/
#define DEBUG_FUNC() Serial.printf("Time: %0.3fms, Function: %s\r\n", micros()/1000.0f, __FUNCTION__)

#endif
