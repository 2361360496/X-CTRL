#include "FileGroup.h"
#include "ComPrivate.h"
#include "SpeedCtrl.h"

#define RefSpeed (MotorSpeed_target - MotorSpeed_MID)

bool EnableSlider = true;
bool IsSpeedUp = false;

typedef struct
{
    float Speed;
    float Acc;
} SpeedAccList_TypeDef;

static const SpeedAccList_TypeDef SpeedUp_List[] =
{
    0.15, 2,
    0.3, 1,
    0.5, 0.5,
    0.75, 0.25,
    1, 0.2
};

static float CheakAccList(const SpeedAccList_TypeDef *list, uint16_t size, float speed)
{
    for(uint8_t i = 0; i < size; i++)
    {
        if(speed < (MotorSpeed_MID + RefSpeed * list[i].Speed))
        {
            return list[i].Acc;
        }
    }
    return 0.0;
}

void Task_SpeedGenerator()
{
    if(EnableSlider)
    {
        if(IsSpeedUp)
        {
            if(MotorSpeed < MotorSpeed_target - 1)
                MotorSpeed += CheakAccList(SpeedUp_List, __Sizeof(SpeedUp_List), MotorSpeed);
            else if(MotorSpeed > MotorSpeed_target + 1)
                MotorSpeed -= 0.5;
        }
        else
        {
            if(MotorSpeed < MotorSpeed_target)
                MotorSpeed += 0.02;
            else if(MotorSpeed > MotorSpeed_target)
                MotorSpeed = MotorSpeed_target;
        }
    }
    else
        MotorSpeed = MotorSpeed_target;
}

void SpeedUp_Handler()
{
    MotorSpeed_target = fmap(RC_X.Left.Y * L_limit, 0, CtrlOutput_MaxValue, MotorSpeed_MID, MotorSpeed_MAX);
    mtm.TaskList[TP_SpeedGenerator].IntervalTime = 50 * (1.0 - R_limit);
    EnableSlider = true;
}

/*����ӳ��*/
static float LinearMap(float value, float oriMin, float oriMax, float nmin, float nmax)
{
    float k = 0;
    if (oriMax != oriMin)
    {
        k = (value - oriMin) / (oriMax - oriMin);
    }
    return nmin + k * (nmax - nmin);
}

#define LinearBrakeSpeedRange       2
#define MaxSpeed                    50
float GetLinearBrakeValue(float val)
{
    float startVal = fmap(ElsSpeed, 0, MaxSpeed, 400, 0);
    return fmap(constrain(val, 0, 1000), 0, 1000, 600, 1000);
}
void Brake_Handler()
{
    float brakeStr = LinearMap(RC_X.Left.Y, -100, -CtrlOutput_MaxValue, 0, 1000);
    float actualBrakeStre = GetLinearBrakeValue(brakeStr);
    if(ElsSpeed < LinearBrakeSpeedRange)
    {
        actualBrakeStre = brakeStr;
    }
    MotorSpeed_target = LinearMap(actualBrakeStre, 0, 1000, MotorSpeed_MID, MotorSpeed_MIN);
    mtm.TaskList[TP_SpeedGenerator].IntervalTime = 2;
    EnableSlider = true;
}
