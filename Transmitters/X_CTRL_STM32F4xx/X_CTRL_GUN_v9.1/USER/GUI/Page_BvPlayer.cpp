#include "FileGroup.h"
#include "FileSystem.h"
#include "GUI_Private.h"

/*ʵ������ǰҳ�������*/
static MillisTaskManager mtm_BvPlayer(2, true);

String BvFilePath;
static int16_t PlayIntervalTime = 20;

static uint32_t LastButtonPressTime = 0;
#define UpdateLastButtonPressTime() LastButtonPressTime=millis()

/*ProgressBar(T *obj, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t dir, uint8_t style)*/
static LightGUI::ProgressBar<SCREEN_CLASS> BvPrg(&screen, 5, screen.height() - 10, screen.width() - 10, 6, 0, 0);
static int8_t PrgBrightness;

static void Task_UpdateProgress()
{
    if(millis() - LastButtonPressTime > 3000)
    {
        if(PrgBrightness > -1)
            PrgBrightness--;
    }
    else
    {
        PrgBrightness = 15;
    }


    if(PrgBrightness < 0)
        return;

    BvPrg.Color_FM = BvPrg.Color_PB = StrPrinter_ColorMap(screen.Black, screen.White, PrgBrightness);

    BvPrg.Progress = (float)BvPlayer.NowFrame / BvPlayer.EndFrame * 1000.0f;
    BvPrg.display();
}

static void Task_UpdateBvState()
{
    BvPlayer.UpdatePlayerState();
}

static void SetPlayIntervalTime(int16_t time)
{
    PlayIntervalTime += time;
    __LimitValue(PlayIntervalTime, 0, 100);
    mtm_BvPlayer.TaskSetIntervalTime(1, PlayIntervalTime);
}

/********** ���� ************/
/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    CloseStatusBar();
    ClearPage();
    UpdateLastButtonPressTime();
    PrgBrightness = 15;
    if(!BvPlayer.OpenVideo(BvFilePath))
    {
        return;
    }
    BvPlayer.BaseX = (screen.width() - BvPlayer.Head.Width) * 0.5f;
    BvPlayer.BaseY = (screen.height() - BvPlayer.Head.Height) * 0.5f;
    BvPlayer.SetPlayerState(BvPlayer.Always);

    mtm_BvPlayer.TaskRegister(0, Task_UpdateProgress, 100);
    mtm_BvPlayer.TaskRegister(1, Task_UpdateBvState, PlayIntervalTime);
}

/**
  * @brief  ҳ��ѭ���¼�
  * @param  ��
  * @retval ��
  */
static void Loop()
{
    mtm_BvPlayer.Running(millis());
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
    BvPlayer.CloseVideo();
    ClearDisplay();
    Init_StatusBar();
}

/**
  * @brief  ҳ���¼�
  * @param  ��
  * @retval ��
  */
static void Event(int event, void* param)
{
    if(event == EVENT_ButtonPress)
    {
        UpdateLastButtonPressTime();
        if(btOK)
        {
            if(BvPlayer == BvPlayer.Run)
            {
                BvPlayer.SetPlayerState(BvPlayer.Pause);
            }
            else
            {
                BvPlayer.SetPlayerState(BvPlayer.Run);
            }
        }
        if(btUP)
        {
            SetPlayIntervalTime(-2);
        }
        if(btDOWN)
        {
            SetPlayIntervalTime(+2);
        }

        if(btBACK)
        {
            page.PagePop();
        }
    }
    if(event == EVENT_ButtonLongPressRepeat)
    {
        UpdateLastButtonPressTime();
        if(btUP)
        {
            SetPlayIntervalTime(-2);
        }
        if(btDOWN)
        {
            SetPlayIntervalTime(+2);
        }
    }
}

/**
  * @brief  ҳ��ע��
  * @param  ThisPage:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_BvPlayer(uint8_t ThisPage)
{
    /*����*/
    page.PageRegister(ThisPage, Setup, Loop, Exit, Event);
}
