#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "ComPrivate.h"

/*ʵ������ǰҳ�������*/
static MillisTaskManager mtm_SetJoystick(3);

/*ҡ�˿ؼ����С*/
#define FM_Size 35

/*ʵ����ҡ�˿ؼ�����*/
static LightGUI::Joystick<SCREEN_CLASS> JSPos_L(&screen, 0, StatusBar_Height + 4, FM_Size, FM_Size, 4);
static LightGUI::Joystick<SCREEN_CLASS> JSPos_R(&screen, screen.width() - FM_Size, StatusBar_Height + 4, FM_Size, FM_Size, 4);

/*ҡ�����ݱ���*/
static RCX::Joystick_t JS_L_BK, JS_R_BK;

/*�������������*/
#define Coor_Y (screen.height()-20)
#define Coor_X_JSL 7
#define Coor_X_JSR (screen.width()/2+Coor_X_JSL)
#define Coor_W (screen.width()/2-15)
#define Coor_H (screen.height() - StatusBar_Height - FM_Size - 20 - 18)

bool UpdateCurveReq_L = true;
bool UpdateCurveReq_R = true;
uint8_t NowAdjValue = 0;
double *AdjGroup[4] = {&JS_L_SlopeStart, &JS_L_SlopeEnd, &JS_R_SlopeStart, &JS_R_SlopeEnd};

void AdjCurve(double step)
{
    *(AdjGroup[NowAdjValue]) += step;
    __LimitValue(*(AdjGroup[NowAdjValue]), 0.0, 9.9f);
    if(NowAdjValue <= 1)
    {
        UpdateCurveReq_L = true;
    }
    else
    {
        UpdateCurveReq_R = true;
    }
}

void DrawCoordinate(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, int16_t triSize, String xAsisText, String yAsisText)
{
    /*X��*/
    screen.drawFastHLine(x, y, w, color);
    screen.fillTriangle(x + w - triSize, y + triSize, x + w - triSize, y, x + w, y, color);

    /*Y��*/
    screen.drawFastVLine(x, y - h, h, color);
    screen.fillTriangle(x - triSize, y - h + triSize, x, y - h + triSize, x, y - h, color);

    /*XY���ǩ*/
    screen.setTextColor(color);
    screen.setTextSize(1);
    /*ԭ��*/
//  screen.setCursor(x - TEXT_WIDTH_1, y);
//  screen.print('0');
    screen.setCursor(x - TEXT_WIDTH_1, y - h - TEXT_HEIGHT_1 - 1);
    screen.print(yAsisText);
    screen.setCursor(x + w + 2, y);
    screen.print(xAsisText);
}

void DrawCurve(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t linecolor,
               uint16_t text1color, uint16_t text2color, uint16_t dotcolor, uint16_t bgcolor,
               double startK, double endK)
{
    /*����ɫ*/
    screen.fillRect(x, y - h + 1, w, h, bgcolor);

    for(int16_t i = 0; i < w; i ++)
    {
        /*�����*/
        if(dotcolor != bgcolor)
        {
            for(int16_t j = 0; j < h; j++)
            {
                if(i && j && i % 8 == 0 && j % 8 == 0)
                {
                    screen.drawPixel(x + i, y - j, dotcolor);
                }
            }
        }
        /*����*/
        int16_t y1 = NonlinearMap(i, 0, w, 0, h, startK, endK);
        if(y1 >= 0)
            screen.drawPixel(x + i, y - y1, linecolor);
    }

    /*���б��ֵ*/
    screen.setTextColor(text1color);
    screen.setTextSize(1);
    screen.setCursor(x + 1, y - 7);
    screen.printf("%0.1f", startK);

    /*�յ�б��ֵ*/
    screen.setTextColor(text2color);
    screen.setCursor(x + w - 17, y - h + 1);
    screen.printf("%0.1f", endK);
}

/**
  * @brief  ����ҡ�˿ؼ�״̬����
  * @param  ��
  * @retval ��
  */
void Task_UpdateJsPos()
{
    JSPos_L.setJsPos(
        __Map(JS_L.X, -RCX_ChannelData_Max, RCX_ChannelData_Max, -1.0, 1.0),
        __Map(JS_L.Y, -RCX_ChannelData_Max, RCX_ChannelData_Max, -1.0, 1.0)
    );

    JSPos_R.setJsPos(
        __Map(JS_R.X, -RCX_ChannelData_Max, RCX_ChannelData_Max, -1.0, 1.0),
        __Map(JS_R.Y, -RCX_ChannelData_Max, RCX_ChannelData_Max, -1.0, 1.0)
    );
}

/**
  * @brief  ����ҡ����������
  * @param  ��
  * @retval ��
  */
void Task_ShowJsCoor()
{
    screen.setTextColor(screen.Green, screen.Black);
    screen.setCursor(0, screen.height() - (TEXT_HEIGHT_1 + 2));
    screen.printf("% 2.1f,% 2.1f", JSPos_L.Xc, -JSPos_L.Yc);

    screen.setCursor(screen.width() / 2, screen.height() - (TEXT_HEIGHT_1 + 2));
    screen.printf("% 2.1f,% 2.1f", JSPos_R.Xc, -JSPos_R.Yc);
}

/**
  * @brief  ҡ��У׼����
  * @param  ��
  * @retval ��
  */
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

/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    JS_L_BK = JS_L;
    JS_R_BK = JS_R;

    /*ҡ����ֵУ׼*/
    float LX_sum = 0.0, LY_sum = 0.0, RX_sum = 0.0, RY_sum = 0.0;
    uint32_t cnt = 0;
    /*ҡ�˿ؼ����һ��붯��*/
    for(float i = 0.0f; i < 1.0f; i += ((1.0f - i) * 0.1f + 0.001f))
    {
        JSPos_L.setPosition(-FM_Size + i * (FM_Size + 2), JSPos_L.Y);
        JSPos_R.setPosition(screen.width() - i * (FM_Size + 2) + 1, JSPos_R.Y);
        PageDelay(1);

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

    JS_L.Xmax = ADC_MaxValue / 2 + ADC_MaxValue / 20, JS_L.Xmin = ADC_MaxValue / 2 - ADC_MaxValue / 20;
    JS_L.Ymax = ADC_MaxValue / 2 + ADC_MaxValue / 20, JS_L.Ymin = ADC_MaxValue / 2 - ADC_MaxValue / 20;
    JS_R.Xmax = ADC_MaxValue / 2 + ADC_MaxValue / 20, JS_R.Xmin = ADC_MaxValue / 2 - ADC_MaxValue / 20;
    JS_R.Ymax = ADC_MaxValue / 2 + ADC_MaxValue / 20, JS_R.Ymin = ADC_MaxValue / 2 - ADC_MaxValue / 20;

    screen.setTextColor(screen.White, screen.Black);
    screen.setCursor(TextMid(0.5f, 4), StatusBar_Height + TEXT_HEIGHT_1);
    screen.print("Turn");

    screen.setCursor(TextMid(0.5f, 6), StatusBar_Height + TEXT_HEIGHT_1 + 10);
    screen.print("Around");

    screen.setCursor(TextMid(0.5f, 8), StatusBar_Height + TEXT_HEIGHT_1 + 20);
    screen.print("Joystick");

    DrawCoordinate(Coor_X_JSL, Coor_Y, Coor_W, Coor_H, screen.White, 2, "x", "y");
    DrawCoordinate(Coor_X_JSR, Coor_Y, Coor_W, Coor_H, screen.White, 2, "x", "y");
    UpdateCurveReq_L = UpdateCurveReq_R = true;
    NowAdjValue = 0;

    /*����ע��*/
    mtm_SetJoystick.TaskRegister(0, Task_UpdateJsPos, 10);
    mtm_SetJoystick.TaskRegister(1, Task_ShowJsCoor, 50);
    mtm_SetJoystick.TaskRegister(2, Task_JoystickCab, 10);
}

/**
  * @brief  ҳ��ѭ���¼�
  * @param  ��
  * @retval ��
  */
static void Loop()
{
    mtm_SetJoystick.Running(millis());

    if(UpdateCurveReq_L)
    {
        DrawCurve(
            Coor_X_JSL + 1,
            Coor_Y - 1,
            Coor_W - 1,
            Coor_H - 1,
            screen.Yellow,
            NowAdjValue == 0 ? screen.Red : screen.Green,
            NowAdjValue == 1 ? screen.Red : screen.Green,
            screen.White,
            screen.Black,
            JS_L_SlopeStart,
            JS_L_SlopeEnd
        );
        UpdateCurveReq_L = false;
    }
    if(UpdateCurveReq_R)
    {
        DrawCurve(
            Coor_X_JSR + 1,
            Coor_Y - 1,
            Coor_W - 1,
            Coor_H - 1,
            screen.Yellow,
            NowAdjValue == 2 ? screen.Red : screen.Green,
            NowAdjValue == 3 ? screen.Red : screen.Green,
            screen.White,
            screen.Black,
            JS_R_SlopeStart,
            JS_R_SlopeEnd
        );
        UpdateCurveReq_R = false;
    }
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
    /*ҡ�˿ؼ����һ�������*/
    for(float i = 0.0f; i < 1.0f; i += ((1.0f - i) * 0.1f + 0.001f))
    {
        JSPos_L.setPosition(i * -(FM_Size + 2), JSPos_L.Y);
        JSPos_R.setPosition(screen.width() - FM_Size + i * (FM_Size + 2), JSPos_R.Y);
        PageDelay(1);
    }
    ClearPage();
}

/**
  * @brief  ���������¼�
  * @param  ��
  * @retval ��
  */
static void Event(int event, void* param)
{
    if(event == EVENT_ButtonPress || event == EVENT_ButtonLongPressRepeat)
    {
        if(event == EVENT_ButtonPress)
        {
            if(btOK)
            {
                __ValueStep(NowAdjValue, 1, 4);
                UpdateCurveReq_L = UpdateCurveReq_R = true;
            }
            if(btBACK)
            {
                __ValueStep(NowAdjValue, -1, 4);
                UpdateCurveReq_L = UpdateCurveReq_R = true;
            }
        }
        else
        {
            if(btBACK)
            {
                JS_L = JS_L_BK;
                JS_R = JS_R_BK;
                page.PagePop();
            }
            if(btOK)
            {
                page.PagePop();
            }
        }

        if(btUP)
        {
            AdjCurve(0.1f);
        }
        if(btDOWN)
        {
            AdjCurve(-0.1f);
        }
    }
}

/**
  * @brief  ҡ������ҳ��ע��
  * @param  pageID:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_SetJoystick(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
