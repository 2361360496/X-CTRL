#include "FileGroup.h"
#include "GUI_Private.h"

static MillisTaskManager mtm_SetJoystick(3);

#define FM_Size 35

LightGUI::Joystick<SCREEN_CLASS> JSPos_L(&screen, 0, StatusBar_POS + 4, FM_Size, FM_Size, 4);
LightGUI::Joystick<SCREEN_CLASS> JSPos_R(&screen, screen.width() - FM_Size, StatusBar_POS + 4, FM_Size, FM_Size, 4);

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
    screen.setTextColor(screen.Green, screen.Black);
    screen.setCursor(0, screen.height() - 8);
    screen.printf("% 2.1f,% 2.1f", JSPos_L.Xc, JSPos_L.Yc);

    screen.setCursor(70, screen.height() - 8);
    screen.printf("% 2.1f,% 2.1f", JSPos_R.Xc, JSPos_R.Yc);
}

void Task_JoystickCab()
{
    int16_t ADC_LX = JSL_X_ADC();
    int16_t ADC_LY = JSL_Y_ADC();
    int16_t ADC_RX = JSR_X_ADC();
    int16_t ADC_RY = JSR_Y_ADC();

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
        JSPos_R.setPosition(screen.width() - i * (FM_Size + 2) - 1, JSPos_R.Y);

        LX_sum += JSL_X_ADC();
        LY_sum += JSL_Y_ADC();
        RX_sum += JSR_X_ADC();
        RY_sum += JSR_Y_ADC();
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

    screen.setTextColor(screen.White, screen.Black);
    screen.setCursor(FM_Size + 16, StatusBar_POS + 8);
    screen.print("Turn");
    screen.setCursor(FM_Size + 10, StatusBar_POS + 18);
    screen.print("Around");
    screen.setCursor(FM_Size + 4, StatusBar_POS + 28);
    screen.print("Joystick");

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
        JSPos_R.setPosition(screen.width() - FM_Size + i * (FM_Size + 2), JSPos_R.Y);
    }
    ClearPage();
}

static void ButtonPressEvent()
{
    if(btBACK)
    {
        JS_L = JS_L_BK;
        JS_R = JS_R_BK;
        page.PageChangeTo(PAGE_Options);
    }
    if(btOK)
    {
        page.PageChangeTo(PAGE_Options);
    }
}

void PageRegister_SetJoystick(uint8_t ThisPage)
{
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
//    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
//    page.PageRegister_Event(ThisPage, EVENT_EncoderRotate, EncoderEvent);
}
