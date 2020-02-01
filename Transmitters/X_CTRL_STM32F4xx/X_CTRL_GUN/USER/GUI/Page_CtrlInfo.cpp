#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "ComPrivate.h"
#include "IMU_Private.h"

/*ʵ������ǰҳ����������������ȼ�*/
static MillisTaskManager mtm_CtrlInfo(4, true);

typedef struct{
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
}Area_t;

const Area_t PB_AREA = {
    5, StatusBar_Height + Page_Height / 2 - 10,
    screen.width() - 10, Page_Height / 2 + 5
};

#define ELEMENT_WIDTH  (PB_AREA.w/2)
#define ELEMENT_HEIGHT (PB_AREA.h/4)
#define ELEMENT_X(i)   (i < 4 ? PB_AREA.x : (PB_AREA.x + ELEMENT_WIDTH))
#define ELEMENT_Y(i)   (PB_AREA.y + 5 + (i % 4) * ELEMENT_HEIGHT)


/*ʵ�����������ؼ�����*/
LightGUI::ScrollBar<SCREEN_CLASS>* BarChannel_Grp[8];


/**
  * @brief  ��������������
  * @param  ��
  * @retval ��
  */
static void Task_BarChannelUpdate()
{
    if(!BarChannel_Grp[0])
        return;
    
    for(int i = 0; i < __Sizeof(BarChannel_Grp); i++)
    {
        int16_t chVal = RCX::ChannelReadValue(i);
        float prg = __Map(chVal, -RCX_ChannelData_Max, RCX_ChannelData_Max, 0.0f, 1.0f);
        BarChannel_Grp[i]->setProgress(prg);
    }
}

/**
  * @brief  ʹ��ʱ���������
  * @param  ��
  * @retval ��
  */
static void Task_PrintUseTime()
{
    screen.setCursor(TextMid(0.8, 10), StatusBar_Height + 9);
    screen.setTextColor(screen.White, screen.Black);
    screen.setTextSize(1);
    uint16_t hh = (millis() / (3600 * 1000)) % 100;
    uint16_t mm = (millis() / (60 * 1000)) % 60;
    uint16_t ss = (millis() / 1000) % 60;
    screen.printfX("%02d:%02d:%02d", hh, mm, ss);
}

/**
  * @brief  ��ʾͨ�ûش�����
  * @param  ��
  * @retval ��
  */
static void DisplayCommonInfo()
{
    
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

static void DrawCtrlObjImg()
{
    uint16_t *gImage_Bitmap_x;

    uint8_t objType = RCX::GetObjectType();
    if(objType == RCX::CAR_ServoSteering)
        gImage_Bitmap_x = (uint16_t*)gImage_Bitmap_RC;
    else if(objType == RCX::CAR_DifferentalSteering)
        gImage_Bitmap_x = (uint16_t*)gImage_Bitmap_DS;
    else
        gImage_Bitmap_x = (uint16_t*)gImage_Bitmap_Common;

    screen.drawRGBBitmap(15, StatusBar_Height + 3, gImage_Bitmap_x, 30, 30);
}

static void DrawBarChannel()
{
    TextSetDefault();
    for(int i = 0; i < __Sizeof(BarChannel_Grp); i++)
    {
        screen.setCursor(ELEMENT_X(i) + 2, ELEMENT_Y(i));
        screen.printf("CH%d", i);
        if(!BarChannel_Grp[i])
        {
            BarChannel_Grp[i] = new LightGUI::ScrollBar<SCREEN_CLASS>(
                    &screen,
                    ELEMENT_X(i) + 2 + TEXT_WIDTH_1 * 3,
                    ELEMENT_Y(i),
                    ELEMENT_WIDTH - 4 - TEXT_WIDTH_1 * 3,
                    TEXT_HEIGHT_1,
                    0,
                    2
                );
            BarChannel_Grp[i]->Color_SB = screen.Yellow;
        }
        BarChannel_Grp[i]->display();
    }
}

/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    nrf.SetRF_Enable(true);
    RCX::ChannelReset();
    ClearPage();
    SetupTimePoint = millis();//��¼ʱ���

    DrawCtrlObjImg();
    
    screen.drawRoundRect(PB_AREA.x, PB_AREA.y, PB_AREA.w, PB_AREA.h, 5, screen.Blue);
//    TextSetDefault();
//    TextMidPrint(0.5f, ((PB_AREA.y) / (float)screen.height()), " CHANNELS ");
    
    DrawBarChannel();

    mtm_CtrlInfo.TaskRegister(0, Task_BarChannelUpdate, 20);
    mtm_CtrlInfo.TaskRegister(1, Task_CheckSignal, 20);
    mtm_CtrlInfo.TaskRegister(2, Task_PrintPassback, 30);
    mtm_CtrlInfo.TaskRegister(3, Task_PrintUseTime, 1000);

    State_RF = ON;
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
    nrf.SetRF_Enable(false);
    XFS_Speak("���ƽ���ر�");
}

/**
  * @brief  ҳ���¼�
  * @param  ��
  * @retval ��
  */
static void Event(int event, void* param)
{
    if(event == EVENT_ButtonLongPressed)
    {
        if(param == &btBACK)
        {
            page.PageStackClear();
            page.PagePush(PAGE_MainMenu);
        }
    }
    
    if(event == EVENT_ButtonDoubleClick)
    {
        if(param == &btOK)
        {
            IMU_CalibrateStart();
        }
    }
}

/**
  * @brief  ������Ϣҳ��ע��
  * @param  pageID:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_CtrlInfo(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
