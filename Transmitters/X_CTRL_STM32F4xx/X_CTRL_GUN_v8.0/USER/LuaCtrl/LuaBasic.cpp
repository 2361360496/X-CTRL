#include "FileGroup.h"
#include "LuaCallCpp.h"
#include "LuaGroup.h"
#include "GUI_Private.h"

/*Lua�����*/
lua_State *L = NULL;

/*Lua����״̬*/
bool State_LuaScript = false;

/*Lua���б�־λ*/
static bool Is_LuaRunning = false;
static bool Is_LuaExit = false;
static bool Is_LuaOpened = false;

LuaOutput_Callback_t LuaOutput_Callback = 0;

void LuaReg_Output(LuaOutput_Callback_t func)
{
    LuaOutput_Callback = func;
}

/**
  * @brief  Lua�ַ�������ӿڣ���Lua����������
  * @param  ��
  * @retval ��
  */
void Lua_Output(const char *s)
{
    if(LuaOutput_Callback)
        LuaOutput_Callback(s);
    else
        Lua_SERIAL.print(s);
}

/**
  * @brief  ����Lua�����
  * @param  ��
  * @retval true:�����ɹ�, false:�ѱ�������
  */
bool Lua_Open()
{
    if(Is_LuaOpened)
        return true;

    L = luaL_newstate();
    luaL_openlibs(L);
    
    LuaReg_Com();
    LuaReg_GPIO();
    LuaReg_GUI();
    LuaReg_ModuleCtrl();
    LuaReg_MouseKeyboard();
    LuaReg_Time();
    Is_LuaOpened = true;
    
    return false;
}

/**
  * @brief  ִ��Lua�ű�
  * @param  *s:�ű��ַ���
  * @retval �Ƿ����
  */
bool Lua_DoString(const char *s)
{
    Is_LuaRunning = true;
    bool error = luaL_dostring(L, s);
    Is_LuaRunning = false;
    if(error)
    {
        Lua_Output(lua_tostring(L, -1));
    }
    return !error;
}

/**
  * @brief  �Զ��������
  * @param  cmd:����
  * @retval ��
  */
static bool CMD_Handler(String cmd)
{
    bool ret = true;

    if(cmd == "exit")
        Is_LuaExit = true;
    else if(cmd == "reboot")
        NVIC_SystemReset();
    else
        ret = false;

    return ret;
}

/**
  * @brief  Luaǿ���˳��ص�
  * @param  lua_State *L, lua_Debug *ar
  * @retval ��
  */
static void Lua_BreakCallback(lua_State *L, lua_Debug *ar)
{
    lua_sethook(L, NULL, 0, 0);
    if(Is_LuaRunning)
        luaL_error(L, "lua script break");
}

/**
  * @brief  Luaǿ���˳�
  * @param  ��
  * @retval ��
  */
void Lua_ForceToBreak()
{
    if(!Is_LuaRunning)
        return;
    
    int mask = LUA_MASKCALL | LUA_MASKRET | LUA_MASKLINE | LUA_MASKCOUNT;
    lua_sethook(L, Lua_BreakCallback, mask, 1);
}

/**
  * @brief  Lua�����жϻص�
  * @param  ��
  * @retval ��
  */
static void Lua_USART_Callback()
{
    if(Is_LuaRunning)
    {
        char rcev = Lua_SERIAL.read();
        if(rcev == 'C')
        {
            Lua_ForceToBreak();
        }
        else if(rcev == 'R')
        {
            NVIC_SystemReset();
        }

        Lua_SERIAL.flush();
    }
}

/**
  * @brief  Lua�ű���������ʼ��
  * @param  ��
  * @retval ��
  */
void Init_LuaScript()
{
    screen.setCursor(TextMid(0.5f, 8), StatusBar_POS - TEXT_HEIGHT_1 - 2);
    screen.setTextSize(1);
    screen.print("Lua MODE");

    Lua_SERIAL.begin(115200);
    Lua_SERIAL.setTimeout(100);
    Lua_SERIAL.attachInterrupt(Lua_USART_Callback);
    Lua_SERIAL.println("\r\nInit lua script...");

    Is_LuaExit = false;

    Lua_Open();
    
    luaL_dostring(L, "print(\"> Version:\",_VERSION)");

    while (!Is_LuaExit)
    {
        if(Lua_SERIAL.available())
        {
            String UserInput = Lua_SERIAL.readString();

            if(UserInput.startsWith("CMD:"))
            {
                String Cmd = UserInput.substring(4);
                if(!CMD_Handler(Cmd))
                {
                    Lua_SERIAL.println();
                    Lua_SERIAL.print(Cmd);
                    Lua_SERIAL.println(" is error cmd!");
                    Lua_SERIAL.flush();
                }
            }
            else
            {
                const char *LuaCode_buffer = UserInput.c_str();

                Lua_SERIAL.println("\r\nCode:\r\n");
                Lua_SERIAL.println(UserInput);
                Lua_SERIAL.printf("\r\n(Size: %d )\r\n", UserInput.length());
                Lua_SERIAL.print("\r\n> ");

                Lua_DoString(LuaCode_buffer);
                Lua_SERIAL.flush();
            }
        }
    }

    ClearStatusBar();
    Lua_SERIAL.println("lua script exit");
}
