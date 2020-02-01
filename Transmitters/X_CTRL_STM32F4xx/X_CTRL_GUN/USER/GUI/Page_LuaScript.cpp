#include "FileGroup.h"
#include "FileSystem.h"
#include "DisplayPrivate.h"
#include "LuaGroup.h"

/*Lua�ļ�·��*/
String LuaFilePath;

static void ScreenPrint(const char* s)
{
    TextSetDefault();
    if(screen.getCursorY() > screen.height() - TEXT_HEIGHT_1 || screen.getCursorY() < TEXT_HEIGHT_1)
    {
        ClearDisplay();
        screen.setCursor(0, TEXT_HEIGHT_1);
    }
    screen.print(s);
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
    TextSetDefault();
    screen.setTextWrap(true);
    
    luaScript.registerStrOutput(ScreenPrint);
    Lua_Begin();
    
    ScreenPrint("File:");
    ScreenPrint(LuaFilePath.c_str());
    ScreenPrint("\nExecuting...");
}

/**
  * @brief  ҳ��ѭ���¼�
  * @param  ��
  * @retval ��
  */
static void Loop()
{
    luaScript.doFile(LuaFilePath.c_str());
    ScreenPrint("\nExiting...");
    PageDelay(1000);
    page.PagePop();
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
    ClearDisplay();
    Init_StatusBar();
}

/**
  * @brief  �����¼�
  * @param  ��
  * @retval ��
  */
static void Event(int event, void* param)
{
    if(param == &btBACK)
    {
        if(event == EVENT_ButtonPress)
        {
        }
        else if(event == EVENT_ButtonLongPressed)
        {
            luaScript.end();
        }
    }
}

/**
  * @brief  ҳ��ע��
  * @param  pageID:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_LuaScript(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
