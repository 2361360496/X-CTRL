#include "FileGroup.h"
#include "GUI_Private.h"

MillisTaskManager mtm_SetJoystick(3);

#define FM_Size 35

JoystickPos JSPos_L(oled, 0, StatusBar_POS + 4, FM_Size, FM_Size, 4);
JoystickPos JSPos_R(oled, oled.width() - FM_Size, StatusBar_POS + 4, FM_Size, FM_Size, 4);

Joystick_t JS_L_BK, JS_R_BK;

void Task_UpdateJsPos()
{
    JSPos_L.setJsPos(
        fmap(JS_L.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, -1.0, 1.0),
        fmap(JS_L.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, -1.0, 1.0)
    );

    JSPos_R.setJsPos(
        fmap(JS_R.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, -1.0, 1.0),
        fmap(JS_R.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, -1.0, 1.0)
    );
}

void Task_ShowJsCoor()
{
    oled.setTextColor(WHITE,BLACK);
    oled.setCursor(0, oled.height() - 8);
    oled.printf("% 2.1f,% 2.1f", JSPos_L.Xc, JSPos_L.Yc);

    oled.setCursor(70, oled.height() - 8);
    oled.printf("% 2.1f,% 2.1f", JSPos_R.Xc, JSPos_R.Yc);
}

void Task_JoystickCab()
{
    int16_t ADC_LX = analogRead_DMA(JSL_X_Pin-2);
    int16_t ADC_LY = analogRead_DMA(JSL_Y_Pin-2);
    int16_t ADC_RX = analogRead_DMA(JSR_X_Pin);
    int16_t ADC_RY = analogRead_DMA(JSR_Y_Pin);

    if(ADC_LX > JS_L.Xmax)JS_L.Xmax = ADC_LX;
    if(ADC_LX < JS_L.Xmin)JS_L.Xmin = ADC_LX;
    if(ADC_LY > JS_L.Ymax)JS_L.Ymax = ADC_LY;
    if(ADC_LY < JS_L.Ymin)JS_L.Ymin = ADC_LY;

    if(ADC_RX > JS_R.Xmax)JS_R.Xmax = ADC_RX;
    if(ADC_RX < JS_R.Xmin)JS_R.Xmin = ADC_RX;
    if(ADC_RY > JS_R.Ymax)JS_R.Ymax = ADC_RY;
    if(ADC_RY < JS_R.Ymin)JS_R.Ymin = ADC_RY;
}

static void Setup()
{
    JS_L_BK = JS_L;
    JS_R_BK = JS_R;

    float LX_sum = 0.0, LY_sum = 0.0, RX_sum = 0.0, RY_sum = 0.0;
    uint32_t cnt = 0;
    for(float i = 0.0; i < 1.0; i += ((1.0 - i) * 0.1 + 0.001))
    {
        JSPos_L.setPosition(-FM_Size + i * (FM_Size + 2), JSPos_L.Y);
        JSPos_R.setPosition(oled.width() - i * (FM_Size + 2) - 1, JSPos_R.Y);
		delay(1);

        LX_sum += analogRead_DMA(JSL_X_Pin-2);
        LY_sum += analogRead_DMA(JSL_Y_Pin-2);
        RX_sum += analogRead_DMA(JSR_X_Pin);
        RY_sum += analogRead_DMA(JSR_Y_Pin);
        cnt++;
    }
    JS_L.Xmid = LX_sum / cnt;
    JS_L.Ymid = LY_sum / cnt;
    JS_R.Xmid = RX_sum / cnt;
    JS_R.Ymid = RY_sum / cnt;

    JS_L.Xmax = ADC_MaxValue / 2 + 200, JS_L.Xmin = ADC_MaxValue / 2 - 200;
    JS_L.Ymax = ADC_MaxValue / 2 + 200, JS_L.Ymin = ADC_MaxValue / 2 - 200;
    JS_R.Xmax = ADC_MaxValue / 2 + 200, JS_R.Xmin = ADC_MaxValue / 2 - 200;
    JS_R.Ymax = ADC_MaxValue / 2 + 200, JS_R.Ymin = ADC_MaxValue / 2 - 200;

    oled.setTextColor(WHITE);
    oled.setCursor(FM_Size + 16, StatusBar_POS + 8);
    oled.print("Turn");
    oled.setCursor(FM_Size + 10, StatusBar_POS + 18);
    oled.print("Around");
    oled.setCursor(FM_Size + 4, StatusBar_POS + 28);
    oled.print("Joystick");

    mtm_SetJoystick.TaskRegister(0, Task_UpdateJsPos, 10);
    mtm_SetJoystick.TaskRegister(1, Task_ShowJsCoor, 30);
    mtm_SetJoystick.TaskRegister(2, Task_JoystickCab, 10);
}

static void Loop()
{
    mtm_SetJoystick.Running(millis());
}

static void Exit()
{
    for(float i = 0.0; i < 1.0; i += ((1.0 - i) * 0.1 + 0.001))
    {
        JSPos_L.setPosition(i * -(FM_Size + 2), JSPos_L.Y);
        JSPos_R.setPosition(oled.width() - FM_Size + i * (FM_Size + 2), JSPos_R.Y);
		delay(1);
    }
    oled.clearPage();
}

static void ButtonPressEvent()
{
    if(btBACK)
    {
        JS_L = JS_L_BK;
        JS_R = JS_R_BK;
        pm.PageChangeTo(PAGE_Options);
    }
    if(btOK)
    {
        pm.PageChangeTo(PAGE_Options);
    }
}

void PageRegister_SetJoystick()
{
#define ThisPage PAGE_SetJoystick
    pm.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    pm.PageRegister_ButtonPressEvent(ThisPage, ButtonPressEvent);
//	pm.PageRegister_ButtonLongPressEvent(ThisPage, ButtonLongPressEvent);
//	pm.PageRegister_EncoderEvent(ThisPage, EncoderEvent);
}
