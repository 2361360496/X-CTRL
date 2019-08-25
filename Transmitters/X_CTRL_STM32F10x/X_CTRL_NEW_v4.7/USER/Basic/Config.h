#ifndef __CONFIG_H
#define __CONFIG_H

#define ADC_MaxValue 4095
#define btLEFT btBACK
#define btRIGHT btOK

//*************GPIO**************//
#define JSL_X_Pin PA0
#define JSL_Y_Pin PA1
#define JSR_X_Pin PA2
#define JSR_Y_Pin PA3
#define ADL_Pin PA4
#define ADR_Pin PA5
#define BattSensor_Pin PA6

#define JSL_X_ADC()         analogRead_DMA(JSL_X_Pin)
#define JSL_Y_ADC()         analogRead_DMA(JSL_Y_Pin)
#define JSR_X_ADC()         analogRead_DMA(JSR_X_Pin)
#define JSR_Y_ADC()         analogRead_DMA(JSR_Y_Pin)
#define ADL_ADC()           analogRead_DMA(ADL_Pin)
#define ADR_ADC()           analogRead_DMA(ADR_Pin)
#define BattSensor_ADC()    analogRead_DMA(BattSensor_Pin)

#define EncoderLED_Pin PB1 //Use TIM3
#define EncoderKey_Pin PB9
#define EncoderA_Pin PB8
#define EncoderB_Pin PB7

#define Buzz_Pin PB12
#define SPDT_Switch_Pin PA15

#define HC165_OUT_Pin		PB4
#define HC165_CP_Pin		PB5
#define HC165_PL_Pin		PB6

#define NRF_MOSI_Pin PB15
#define NRF_MISO_Pin PB14
#define NRF_SCK_Pin PB13
#define NRF_CE_Pin PA12
#define NRF_CSN_Pin PA11
#define NRF_IRQ_Pin -1
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

#define BT_PowerCtrl_Pin PC13
#define BT_State_Pin PC14
#define BT_EN_Pin PC15

#define TFT_CS     -1
#define TFT_RST    -1
#define TFT_DC     PB0
#define TFT_MOSI	PB3
#define TFT_CLK		PA8

/*�����͵�ѹ2.75V�����4.15V*/
#define BattEmptyVoltage 2.75
#define BattFullVoltage 4.15

#endif
