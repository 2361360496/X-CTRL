#include "FileGroup.h"

#define LimitValue(x,min,max) (x=constrain(x,min,max))

MPU6050 mpu;
Servo SteerServo;
Servo BrushlessMotor;
Servo BrushlessMotor2;
uint16_t ServoAngle, ServoAngle_MID = (ServoAngle_MIN + ServoAngle_MAX) / 2;
uint16_t MotorSpeed, MotorSpeed_MID = 60;

void Task_MID_Setting()
{
    //MotorSpeed_MID = map(RC_CAR.KnobCab.R, -1000, 1000, MotorSpeed_MIN, MotorSpeed_MAX);

    if(RC_CAR.Key & BT_L2)ServoAngle_MID++;
    else if(RC_CAR.Key & BT_R2)ServoAngle_MID--;
    LimitValue(ServoAngle_MID, ServoAngle_MIN, ServoAngle_MAX);

    static int8_t LED_pwm;
    if(RC_CAR.Key & BT_UP)LED_pwm += 10;
    else if(RC_CAR.Key & BT_DOWN)LED_pwm -= 10;
    LimitValue(LED_pwm, 0, 100);
    analogWrite(LED_Pin, LED_pwm);

//    if(RC_CAR.Key & BT_L1)ServoAngle_Limit-=0.05;
//    else if(RC_CAR.Key & BT_R1)ServoAngle_Limit+=0.05;
//	LimitValue(ServoAngle_Limit, 0.0, 1.0);
}

void Task_CarCtrl()
{
    if(ConnectState.IsConnect && ConnectState.IsCorrect)
    {
        RC_CAR.MotorSpeed *= RC_CAR.KnobLimit.L / 1000.0;
        if(RC_CAR.MotorSpeed >= 0) MotorSpeed = map(RC_CAR.MotorSpeed, 0, 1000, MotorSpeed_MID, MotorSpeed_MAX);
        else MotorSpeed = map(RC_CAR.MotorSpeed, -1000, 0, MotorSpeed_MIN, MotorSpeed_MID);

        RC_CAR.SteerAngle *= RC_CAR.KnobLimit.R / 1000.0;

        if(RC_CAR.SteerAngle >= 0) ServoAngle = map(RC_CAR.SteerAngle, 0, 1000, ServoAngle_MID, ServoAngle_MAX);
        else ServoAngle = map(RC_CAR.SteerAngle, -1000, 0, ServoAngle_MIN, ServoAngle_MID);

    }
    else
    {
        MotorSpeed = MotorSpeed_MID;
        ServoAngle = ServoAngle_MID;
    }
    BrushlessMotor.write(MotorSpeed);
    BrushlessMotor2.write(MotorSpeed);
    SteerServo.write(ServoAngle);
}

void Task_BuzzRing_Block()
{
	togglePin(PA4);
	enum {Sound_Start = 1, Sound_Connect, Sound_Disonnect, Sound_Error};
	static uint8_t Sound_Now,Sound_Last;
	if(ConnectState.IsConnect && ConnectState.IsCorrect)
    {
        Sound_Now = Sound_Connect;
        if(Sound_Now != Sound_Last)
        {
            toneBlock(Buzz_Pin, 1046, 80);
            toneBlock(Buzz_Pin, 1175, 80);
            toneBlock(Buzz_Pin, 1318, 80);
			Sound_Last = Sound_Now;
        }
    }
    else if(!ConnectState.IsConnect && ConnectState.IsCorrect)
    {
        Sound_Now = Sound_Disonnect;
        if(Sound_Now != Sound_Last)
        {
            toneBlock(Buzz_Pin, 100, 80);
            delay(80);
            toneBlock(Buzz_Pin, 100, 80);
			Sound_Last = Sound_Now;
        }
    }
}
