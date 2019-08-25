#include "FileGroup.h"
#include "GUI_Private.h"

MillisTaskManager mtm_HMI(5);

bool HMI_State = OFF;

typedef struct HMI_Protocal {
    uint16_t DataHead;
    uint8_t Page;
    uint8_t Widget;
    uint8_t Data;
} HMI_Protocal_TypeDef;

HMI_Protocal_TypeDef HMI_Data;
const uint16_t HMI_DataHead = 0xAA55;

void HMI_end()
{
    HMI.write(0xFF);
    HMI.write(0xFF);
    HMI.write(0xFF);
}

float RPMtoAgl(float rpm)
{
    if (rpm < 40.0)return (270.0 + rpm / 40.0 * 90.0);
    else return (rpm / 40.0 * 90.0 - 90.0);
}

void HMI_ShowGear(uint8_t Gear)
{
    HMI.printf("nGear.val=%d", Gear), HMI_end();
}

void HMI_ShowKph(int spd)
{
    HMI.printf("tSPD.txt=\"%02d\"", spd), HMI_end();
}

void HMI_ShowRPM(int rpm)
{
    static int Agl, Agl_b;
    Agl = RPMtoAgl(rpm);
    if (Agl != Agl_b)
    {
        HMI.printf("zRPM.val=%d", Agl), HMI_end();
        Agl_b = Agl;
    }
}

void HMI_ShowRPM_Cvs(float rpm, float rpm_cvs)
{
    float RC;
    if (rpm > rpm_cvs)RC = 1.0;
    else if(rpm_cvs == 0.0)RC = 0.0;
    else RC = 1.0 - (rpm_cvs - rpm) / rpm_cvs;
    HMI.printf("jRPM_Cvs.val=%0.0f", 100.0 * RC), HMI_end();
}

void HMI_ShowDST(float distance)
{
    HMI.printf("DST.txt=\"%0.2fkm\"", distance / 1000.0), HMI_end();
}

void HMI_Test()
{
    float i;
    HMI_ShowKph(88);
    HMI_ShowGear(8);
    for (i = 0.0; i < 1.0; i += 0.01)
    {
        HMI_ShowRPM(110.0 * i);
        HMI_ShowRPM_Cvs(i, 1.0);
        delay(5);
    }
    delay(100);

    for (i = 1.0; i > 0.0; i -= 0.01)
    {
        HMI_ShowRPM(110.0 * i);
        HMI_ShowRPM_Cvs(i, 1.0);
        delay(5);
    }
    HMI_ShowKph(0);
    HMI_ShowGear(1);
}

static DigitalFilter dfKPH(13);

double RC_Distance = 0.0;

static void Task_20msUpdate()
{
    int kph = dfKPH.getSmooth(CarSS_Passback.Channel_1);
    RC_Distance += (double)CarSS_Passback.Channel_1 / 3.6 * 0.02;
    HMI_ShowDST(RC_Distance);
    HMI_ShowKph(kph);
    HMI_ShowRPM(kph);
    HMI_ShowRPM_Cvs(fmap(abs(JS_L.Y) * (CTRL.KnobLimit.L / 1000.0), 0, CtrlOutput_MaxValue, 0.0, 1.0), 1.0);
}

static void Task_100msUpdate()
{
    HMI_ShowGear(map(CTRL.KnobLimit.L, 0, CtrlOutput_MaxValue - 10, 1, 9));
}

void HMI_EventHandler()
{
}

void Init_HMI()
{
    HMI.setTimeout(10);
    mtm_HMI.TaskRegister(0, Task_20msUpdate, 20);
    mtm_HMI.TaskRegister(1, Task_100msUpdate, 100);
	if(!HMI_State) return;
    ProgressBar pbCntHMI(screen, 12, 40, 100, 5, 0);
    uint32_t TimeOut = millis() + 3000;

    while(!hc05.GetState() && millis() < TimeOut)
    {
        pbCntHMI.setProgress(1.0 - (TimeOut - millis()) / 3000.0);
        if(!digitalRead(EncoderKey_Pin) || millis() == TimeOut) goto Exit;
    }
    pbCntHMI.Color_PB = screen.Green;
    pbCntHMI.setProgress(1.0);
    HMI.print("dim=dims");
    HMI_end();
    HMI.print("page DashBoard");
    HMI_end();
    delay(200);
    HMI_Test();
Exit:
    delay(0);
}

void HMI_Running()
{
    if(HMI_State && ConnectState.Pattern == Pattern_NRF)
    {
        if(hc05.GetState()) mtm_HMI.Running(millis());
        if(HMI.available())
        {
            HMI.readBytes((uint8_t*)&HMI_Data, sizeof(HMI_Data));
            if(HMI_Data.DataHead == HMI_DataHead)
            {
                HMI_EventHandler();
                HMI.flush();
            }
        }
    }
}
