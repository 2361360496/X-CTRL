#include "FileGroup.h"
#include "ComPrivate.h"

#define HeadCode_XBox (uint16_t)0x0036

#pragma pack (1)
typedef struct {
    uint16_t HeadCode;
    uint8_t LED_Pattern[4];
    uint8_t Motor[2];
} Protocol_XBox360_Passback_t;
#pragma pack ()

static Protocol_XBox360_Passback_t xbox;

static void XBox360Sim_PassBack_Handler(uint8_t *rxbuff)
{   
    xbox = *((Protocol_XBox360_Passback_t*)rxbuff);
    
    if(xbox.HeadCode != HeadCode_XBox)
        return;

    int motorPwm = constrain(xbox.Motor[0] + xbox.Motor[1], 0, 150);
    analogWrite(
        Motor_Pin,
        __Map(
            motorPwm,
            0,
            150,
            350,
            1000
        )
    );
}

void Init_XBox360Sim()
{
    AddUserCustomPassBack_Callback(XBox360Sim_PassBack_Handler);
}
