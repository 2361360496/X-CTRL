#ifndef __XC_TYPE_H
#define __XC_TYPE_H

#include "stdint.h"

#pragma pack(1)
/*����*/
typedef struct
{
    double Start;
    double End;
} CurveSlope_TypeDef;

/*У׼*/
typedef struct
{
    int16_t Val;
    int16_t Min;
    int16_t Mid;
    int16_t Max;
    bool Reverse;
    CurveSlope_TypeDef Curve;
} Channel_TypeDef;

/*ҡ��*/
typedef struct
{
    Channel_TypeDef X;
    Channel_TypeDef Y;
} Joystick_TypeDef;

/*����*/
typedef struct
{
    int16_t X;
    int16_t Y;
} Axis_TypeDef;

/*����*/
typedef struct
{
    int16_t L;
    int16_t R;
} Direction_TypeDef;

/*ң����״̬*/
typedef struct
{
    /*ҡ��*/
    Joystick_TypeDef JS_L;
    Joystick_TypeDef JS_R;

    /*����*/
    union
    {
        struct
        {
            uint8_t BT_UP : 1;
            uint8_t BT_DOWN : 1;
            uint8_t BT_BACK : 1;
            uint8_t BT_OK : 1;
            uint8_t BT_L1 : 1;
            uint8_t BT_L2 : 1;
            uint8_t BT_R1 : 1;
            uint8_t BT_R2 : 1;
        } Bit;
        uint8_t Value;
    } Key;
    
    /*����ʽ����*/
    int16_t Switch[4];
    
    /*ͨ��������Ϣ*/
    bool CH_Reverse[8];
    uint8_t CH_AttachMap[8];

//    /*�޷�*/
//    Direction_TypeDef KnobLimit;

    /*����ʹ��*/
    struct
    {
        bool Passback;
        bool Handshake;
        bool FHSS;
        bool Sound;
        bool Vibrate;
        bool IMU;
        bool BigVibrate;
        bool IdleWarn;
        bool SignWarn;
        bool JostickFilter;
    } State;
    
    struct
    {
        uint8_t Speed;
        uint8_t Freq;
        uint8_t Addr[5];
    } RF_Config;

    /*�����Ϣ*/
    struct
    {
        float Usage;
        float Voltage;
    } Battery;

    /*CPU��Ϣ*/
    struct
    {
        float Usage;
        float Temperature;
        bool  Enable;
    } CPU;

    /*����*/
    struct
    {
        uint8_t ConnectObject;
        bool Enable;
    } Bluetooth;
} CTRL_TypeDef;

#pragma pack()

#endif
