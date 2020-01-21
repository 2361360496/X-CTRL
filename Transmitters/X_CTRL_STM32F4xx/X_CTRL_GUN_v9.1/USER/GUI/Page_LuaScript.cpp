#include "FileGroup.h"
#include "FileSystem.h"
#include "GUI_Private.h"
#include "LuaGroup.h"

/*Lua�ļ�*/
static File LuaFile;

/*Lua�ļ�·��*/
String LuaFilePath;

/*Lua�����־λ*/
static bool IsLuaError = false;

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
    screen.setCursor(0, TEXT_HEIGHT_1);
    screen.print("Loading ");
    screen.print(LuaFilePath);
    screen.println("...");

    LuaFile = SD.open(LuaFilePath);
    if(!LuaFile)
    {
        return;
    }
    LuaFile.setTimeout(20);

    IsLuaError = false;
    luaScript.registerStrOutput(ScreenPrint);
    Lua_Begin();

//    luaL_dostring(L, "print(\"> Version:\",_VERSION)");

    if(LuaFile.available())
    {
        String UserInput = LuaFile.readString();
        const char *LuaCode_buffer = UserInput.c_str();

        screen.println("\r\n" + UserInput);
        PageDelay(1000);

        IsLuaError = luaScript.doString(LuaCode_buffer);
    }
    else
    {
        IsLuaError = true;
    }
}

/**
  * @brief  ҳ��ѭ���¼�
  * @param  ��
  * @retval ��
  */
static void Loop()
{
    if(!LuaFile || !IsLuaError)
    {
        page.PagePop();
    }
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
    LuaFile.close();
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
            if(IsLuaError)
            {
                page.PagePop();
            }
        }
        if(event == EVENT_ButtonLongPressRepeat)
        {
            luaScript.end();
        }
    }
}

/**
  * @brief  ҳ��ע��
  * @param  ThisPage:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_LuaScript(uint8_t ThisPage)
{
    page.PageRegister(ThisPage, Setup, Loop, Exit, Event);
}
