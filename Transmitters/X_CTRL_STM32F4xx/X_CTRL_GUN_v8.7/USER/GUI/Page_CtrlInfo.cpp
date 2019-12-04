#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

/*ʵ������ǰҳ����������������ȼ�*/
static MillisTaskManager mtm_CtrlInfo(4, true);

/*���������*/
#define PBx_Width (screen.width()*0.7)

/*�������߶�*/
#define PBx_Height 10

/*������X��Y����ʼ����*/
#define PBY_START (StatusBar_POS+50)
#define PBX_START ((screen.width()-PBx_Width)/2+15)

/*���������*/
#define PBx_INTERVAL 3

/*ʵ�����������ؼ�����*/
LightGUI::ScrollBar<SCREEN_CLASS> BarThr(&screen, PBX_START, PBY_START, PBx_Width, PBx_Height, 0, 8);
LightGUI::ScrollBar<SCREEN_CLASS> BarStr(&screen, PBX_START, PBY_START + PBx_Height + PBx_INTERVAL, PBx_Width, PBx_Height, 0, 8);

LightGUI::ProgressBar<SCREEN_CLASS> BarLimit_L(&screen, PBX_START, PBY_START + (PBx_Height + PBx_INTERVAL) * 2, PBx_Width, PBx_Height, 0);
LightGUI::ProgressBar<SCREEN_CLASS> BarLimit_R(&screen, PBX_START, PBY_START + (PBx_Height + PBx_INTERVAL) * 3, PBx_Width, PBx_Height, 0);


/**
  * @brief  ��������������
  * @param  ��
  * @retval ��
  */
static void Task_DrawProgress()
{
    BarThr.setScroll(fmap(CTRL.Left.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 0.0, 1.01));
    BarStr.setScroll(fmap(CTRL.Right.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 0.0, 1.01));

    BarLimit_L.setProgress(CTRL.KnobLimit.L / (float)CtrlOutput_MaxValue);
    BarLimit_R.setProgress(CTRL.KnobLimit.R / (float)CtrlOutput_MaxValue);
}

/**
  * @brief  ʹ��ʱ���������
  * @param  ��
  * @retval ��
  */
static void Task_PrintUseTime()
{
    screen.setCursor(TextMid(0.8, 10), StatusBar_POS + 9);
    screen.setTextColor(screen.White, screen.Black);
    screen.setTextSize(1);
    uint16_t hh = (millis() / (3600 * 1000)) % 100;
    uint16_t mm = (millis() / (60 * 1000)) % 60;
    uint16_t ss = (millis() / 1000) % 60;
    screen.printfX("%02d:%02d:%02d", hh, mm, ss);
}

/*�����ش����ݰ���־λ*/
bool Lock_CommonPassback = false;
/*�ش����ݰ����³�ʼ����־λ*/
bool Is_CommonInfoInit = false;

/**
  * @brief  ��ʾͨ�ûش�����
  * @param  ��
  * @retval ��
  */
static void DisplayCommonInfo()
{
    /*�����ش����ݰ�����ֹNRF�޸�*/
    Lock_CommonPassback = true;

    if(!Is_CommonInfoInit)//�Ƿ��ѳ�ʼ��
    {
        /*����Ĭ�ϻش�����*/
        Common_Passback.X = 70;
        Common_Passback.Y = 42;
        Common_Passback.TextSize = 1;
        Common_Passback.TextColor = screen.White;
        Common_Passback.BackColor = screen.Black;
        Common_Passback.Text[0] = '\0';
        __TypeExplain(Protocol_Passback_CommonDisplay_t, NRF_RxBuff) = Common_Passback;
        Is_CommonInfoInit = true;
    }
    else
    {
        /*�ж��Ƿ�λ�ںϷ�����*/
        if(Common_Passback.Y > StatusBar_POS && Common_Passback.TextSize < 5)
        {
            screen.setCursor(Common_Passback.X, Common_Passback.Y);
            screen.setTextColor(Common_Passback.TextColor, Common_Passback.BackColor);
            screen.setTextSize(Common_Passback.TextSize);
            screen.print(Common_Passback.Text);
        }
    }

    /*�����ش����ݰ�*/
    Lock_CommonPassback = false;
}

/**
  * @brief  ��ӡ�ش���������
  * @param  ��
  * @retval ��
  */
void Task_PrintPassback()
{
    if(State_PassBack && Enable_CommonPassBack)
    {
        DisplayCommonInfo();
    }
}

/*��ʼ��ҳ��ʱ���*/
static uint32_t SetupTimePoint;

/*�ⲿ����:�ź�ǿ�ȼ�������*/
extern void Task_SignalMonitor();

/**
  * @brief  ����ź�ǿ������
  * @param  ��
  * @retval ��
  */
static void Task_CheckSignal()
{
    /*�ڽ����ҳ��5000ms�������ź�ǿ�ȼ�������*/
    if(millis() - SetupTimePoint > 5000)
        Task_SignalMonitor();
}

/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    ClearPage();
    SetupTimePoint = millis();//��¼ʱ���
    BarThr.Color_SB = screen.Yellow;
    BarStr.Color_SB = screen.Yellow;
    BarLimit_L.Color_PB = screen.White;
    BarLimit_R.Color_PB = screen.White;

    const uint8_t TextStartX = 8;
    screen.setTextColor(screen.White, screen.Black);
    screen.setCursor(TextStartX, BarThr.Y - (TEXT_HEIGHT_2 - PBx_Height) / 2);
    screen.printfX("THR");
    screen.setCursor(TextStartX, BarStr.Y - (TEXT_HEIGHT_2 - PBx_Height) / 2);
    screen.printfX("STR");
    screen.setCursor(TextStartX, BarLimit_L.Y - (TEXT_HEIGHT_2 - PBx_Height) / 2);
    screen.printfX("LTL");
    screen.setCursor(TextStartX, BarLimit_R.Y - (TEXT_HEIGHT_2 - PBx_Height) / 2);
    screen.printfX("LTR");

    uint16_t *gImage_Bitmap_x;

    if(CTRL.Info.CtrlObject == CAR_ServoSteering)
        gImage_Bitmap_x = (uint16_t*)gImage_Bitmap_RC;
    else if(CTRL.Info.CtrlObject == CAR_DifferentalSteering)
        gImage_Bitmap_x = (uint16_t*)gImage_Bitmap_DS;
    else
        gImage_Bitmap_x = (uint16_t*)gImage_Bitmap_Common;

    screen.drawRGBBitmap(15, StatusBar_POS + 3, gImage_Bitmap_x, 30, 30);

    mtm_CtrlInfo.TaskRegister(0, Task_DrawProgress, 20);
    mtm_CtrlInfo.TaskRegister(1, Task_CheckSignal, 20);
    mtm_CtrlInfo.TaskRegister(2, Task_PrintPassback, 30);
    mtm_CtrlInfo.TaskRegister(3, Task_PrintUseTime, 1000);

    BarLimit_R.display();
    BarLimit_L.display();

    State_RF = ON;
    Is_CommonInfoInit = false;
}

/**
  * @brief  ҳ��ѭ���¼�
  * @param  ��
  * @retval ��
  */
static void Loop()
{
    mtm_CtrlInfo.Running(millis());
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
    ClearPage();
    State_RF = OFF;
    XFS_Speak("���ƽ���ر�");
}

/**
  * @brief  ҳ���¼�
  * @param  ��
  * @retval ��
  */
static void Event(int event, void * param)
{
    if(event == EVENT_ButtonLongPressed)
    {
        if(btBACK)
            page.PagePop();
    }
}

/**
  * @brief  ������Ϣҳ��ע��
  * @param  ThisPage:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_CtrlInfo(uint8_t ThisPage)
{
    page.PageRegister(ThisPage, Setup, Loop, Exit, Event);
}
