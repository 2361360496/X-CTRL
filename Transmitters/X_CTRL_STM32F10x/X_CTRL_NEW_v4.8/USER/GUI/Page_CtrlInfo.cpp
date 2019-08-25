#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

/*ʵ������ǰҳ����������������ȼ�*/
static MillisTaskManager mtm_CtrlInfo(4, true);

/*����������:40*/
#define PBx_L 40

/*���������:6*/
#define PBx_W 6

/*������X��Y����ʼ����*/
#define PBY_START (screen.width()/2-(PBx_W/2))
#define PBX_START (screen.width()/2-(PBx_L/2))

/*�������X��Y����ֹ����*/
#define PBLY_END 0
#define PBLX_END 10

/*�ҹ�����X��Y����ֹ����*/
#define PBRY_END (screen.width() - PBx_W)
#define PBRX_END (screen.width() - 10 - PBx_L)

/*ʵ�����������ؼ�����*/
LightGUI::ScrollBar<SCREEN_CLASS> PB_LY(&screen, PBY_START, StatusBar_POS + 4, PBx_W, PBx_L, 1, 4);
LightGUI::ScrollBar<SCREEN_CLASS> PB_RY(&screen, PBY_START, StatusBar_POS + 4, PBx_W, PBx_L, 1, 4);
LightGUI::ScrollBar<SCREEN_CLASS> PB_LX(&screen, PBX_START, screen.height() - PBx_W, PBx_L, PBx_W, 0, 4);
LightGUI::ScrollBar<SCREEN_CLASS> PB_RX(&screen, PBX_START, screen.height() - PBx_W, PBx_L, PBx_W, 0, 4);

/**
  * @brief  ��������������
  * @param  ��
  * @retval ��
  */
static void ScrollBar_Open()
{
    for(double i = 0.0; i < 1.0; i += (((1.0 - i) * 0.1) + 0.001))
    {
        PB_LY.setPosition(PBY_START - i * (PBY_START - PBLY_END), PB_LY.Y);
        PB_RY.setPosition(PBY_START + i * (PBRY_END - PBY_START), PB_RY.Y);
        PB_LX.setPosition(PBX_START - i * (PBX_START - PBLX_END), PB_LX.Y);
        PB_RX.setPosition(PBX_START + i * (PBRX_END - PBX_START), PB_RX.Y);

        PB_LY.setProgress(i * 0.5);
        PB_RY.setProgress(i * 0.5);
        PB_LX.setProgress(1.0 - i * 0.5);
        PB_RX.setProgress(i * 0.5);
    }
}

/**
  * @brief  �������رն���
  * @param  ��
  * @retval ��
  */
static void ScrollBar_Close()
{
    for(double i = 1.0; i > 0.0; i -= ((i * 0.1) + 0.001))
    {
        PB_LY.setPosition(PBY_START - i * (PBY_START - PBLY_END), PB_LY.Y);
        PB_RY.setPosition(PBY_START + i * (PBRY_END - PBY_START), PB_RY.Y);
        PB_LX.setPosition(PBX_START - i * (PBX_START - PBLX_END), PB_LX.Y);
        PB_RX.setPosition(PBX_START + i * (PBRX_END - PBX_START), PB_RX.Y);

        PB_LY.setProgress(i * 0.5);
        PB_RY.setProgress(i * 0.5);
    }
}

/**
  * @brief  ��������������
  * @param  ��
  * @retval ��
  */
static void Task_DrawProgress()
{
    PB_LX.setProgress(fmap(CTRL.Left.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 0.0, 1.01));
    PB_LY.setProgress(fmap(CTRL.Left.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 0.0, 1.01));
    PB_RX.setProgress(fmap(CTRL.Right.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 0.0, 1.01));
    PB_RY.setProgress(fmap(CTRL.Right.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 0.0, 1.01));
}

/**
  * @brief  ʹ��ʱ���������
  * @param  ��
  * @retval ��
  */
static void Task_PrintUseTime()
{
    screen.setCursor(70, 20);
    screen.setTextColor(screen.White, screen.Black);
    screen.setTextSize(1);
    uint16_t hh = (millis() / (3600 * 1000)) % 100;
    uint16_t mm = (millis() / (60 * 1000)) % 60;
    uint16_t ss = (millis() / 1000) % 60;
    screen.printf("%02d:%02d:%02d", hh, mm, ss);
}

/*static float TopSpeed = 0.0;
static void DisplayCarSSInfo()
{
    if(CarSS_Passback.Channel_1 >= 200.0)
        return;
    if(CarSS_Passback.Channel_1 > TopSpeed) 
        TopSpeed = CarSS_Passback.Channel_1;

    screen.setCursor(60, 35);
    screen.setTextColor(screen.White, screen.Black);
    screen.setTextSize(2);
    screen.printf("%5.1f", CarSS_Passback.Channel_1);

    screen.setCursor(17, 45);
    screen.setTextColor(screen.Yellow, screen.Black);
    screen.setTextSize(1);
    screen.printf("%5.1f", TopSpeed);
}*/

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
        /*if(CTRL.Info.CtrlObject == CAR_ServoSteering)
            DisplayCarSSInfo();
        else if(CTRL.Info.CtrlObject == X_COMMON)*/
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
    SetupTimePoint = millis();//��¼ʱ���
    PB_LX.Color_SB = screen.Yellow;
    PB_LY.Color_SB = screen.Yellow;
    PB_RX.Color_SB = screen.Yellow;
    PB_RY.Color_SB = screen.Yellow;
    ScrollBar_Open();

    uint16_t *gImage_Bitmap_x;

    if(CTRL.Info.CtrlObject == CAR_ServoSteering)
        gImage_Bitmap_x = (uint16_t*)gImage_Bitmap_RC;
    else if(CTRL.Info.CtrlObject == CAR_DifferentalSteering)
        gImage_Bitmap_x = (uint16_t*)gImage_Bitmap_DS;
    else
        gImage_Bitmap_x = (uint16_t*)gImage_Bitmap_Common;

    screen.drawRGBBitmap(15, 15, gImage_Bitmap_x, 30, 30);

    mtm_CtrlInfo.TaskRegister(0, Task_DrawProgress, 20);
    mtm_CtrlInfo.TaskRegister(1, Task_CheckSignal, 20);
    mtm_CtrlInfo.TaskRegister(2, Task_PrintPassback, 30);
    mtm_CtrlInfo.TaskRegister(3, Task_PrintUseTime, 1000);
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
    ScrollBar_Close();
    ClearPage();
    State_RF = OFF;
    XFS_Speak("���ƽ���ر�");
}

/**
  * @brief  ���������¼�
  * @param  ��
  * @retval ��
  */
static void ButtonPressEvent()
{
}

/**
  * @brief  ���������¼�
  * @param  ��
  * @retval ��
  */
static void ButtonLongPressEvent()
{
    if(btEcd)
        page.PageChangeTo(PAGE_MainMenu);
}

/**
  * @brief  ��ת�������¼�
  * @param  ��
  * @retval ��
  */
static void EncoderEvent()
{
}

/**
  * @brief  ������Ϣҳ��ע��
  * @param  ThisPage:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_CtrlInfo(uint8_t ThisPage)
{
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_EncoderRotate, EncoderEvent);
}
