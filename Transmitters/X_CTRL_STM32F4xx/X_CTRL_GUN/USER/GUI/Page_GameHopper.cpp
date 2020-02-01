#include "FileGroup.h"
#include "DisplayPrivate.h"

#include "common.h"

/*���ڼ��CPUռ����*/
static MillisTaskManager mtm_Game(1);

#if ARDUBOY_LIB_VER != 10101
#error
#endif // It may work even if you use other version.

/*  Defines  */

enum MODE
{
    LOGO_MODE = 0,
    TITLE_MODE,
    GAME_MODE
};

/*  Typedefs  */

typedef struct
{
    void (*initFunc)(void);
    bool (*updateFunc)(void);
    void (*drawFunc)(void);
} MODULE_FUNCS;

/*  Global Variables  */

MyArduboy arduboy;

/*  Local Variables  */

static const MODULE_FUNCS moduleTable[] =
{
    { initLogo,  updateLogo,  drawLogo },
    { initTitle, updateTitle, drawTitle },
    { initGame,  updateGame,  drawGame },
};

static MODE mode = LOGO_MODE;

/*  For Debugging  */

#ifdef DEBUG
bool    dbgPrintEnabled = true;
char    dbgRecvChar = '\0';
uint8_t dbgCaptureMode = 0;

static void dbgCheckSerialRecv(void)
{
    int recv;
    while ((recv = Serial.read()) != -1)
    {
        switch (recv)
        {
        case 'd':
            dbgPrintEnabled = !dbgPrintEnabled;
            Serial.print("Debug output ");
            Serial.println(dbgPrintEnabled ? "ON" : "OFF");
            break;
        case 'c':
            dbgCaptureMode = 1;
            break;
        case 'C':
            dbgCaptureMode = 2;
            break;
        }
        if (recv >= ' ' && recv <= '~') dbgRecvChar = recv;
    }
}

static void dbgScreenCapture()
{
    if (dbgCaptureMode)
    {
        Serial.write((const uint8_t *)arduboy.getBuffer(), WIDTH * HEIGHT / 8);
        if (dbgCaptureMode == 1) dbgCaptureMode = 0;
    }
}
#endif

static void Task_GameRunning()
{
#ifdef DEBUG
    dbgCheckSerialRecv();
#endif
    if (!(arduboy.nextFrame())) return;
    bool isDone = moduleTable[mode].updateFunc();
    moduleTable[mode].drawFunc();
#ifdef DEBUG
    dbgScreenCapture();
    dbgRecvChar = '\0';
#endif
    arduboy.display();
    if (isDone)
    {
        mode = (mode == TITLE_MODE) ? GAME_MODE : TITLE_MODE;
        moduleTable[mode].initFunc();
        dprint("mode=");
        dprintln(mode);
    }
}

/*---------------------------------------------------------------------------*/

/********** ���� ************/
/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    ClearPage();
    mode = LOGO_MODE;
#ifdef DEBUG
    Serial.begin(115200);
#endif
    arduboy.beginNoLogo();
    arduboy.setFrameRate(60);
    moduleTable[LOGO_MODE].initFunc();

    mtm_Game.TaskRegister(0, Task_GameRunning, 5);
}

/**
  * @brief  ҳ��ѭ���¼�
  * @param  ��
  * @retval ��
  */
static void Loop()
{
    mtm_Game.Running(millis());
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
    ClearPage();
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
        if(btBACK && mode == TITLE_MODE)
        {
            page.PagePop();
        }
    }
}

/**
  * @brief  ҳ��ע��
  * @param  pageID:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_GameHopper(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
