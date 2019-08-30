#include "FileGroup.h"
#include "LuaCallCpp.h"
#include "LuaGroup.h"
#include "GUI_Private.h"

/*Lua�����*/
lua_State *L = NULL;

/*Lua����״̬*/
bool State_LuaScript = false;

/*Lua����ӿ�*/
HardwareSerial &LuaSerial = Lua_SERIAL;

/*Lua���б�־λ*/
static bool Is_LuaRunning = false;
static bool Is_LuaExit = false;
static bool Is_LuaError = false;

LuaOutput_Callback_t LuaOutput_Callback = 0;

void LuaOutputReg(LuaOutput_Callback_t func)
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
    LuaSerial << s;
    
    if(LuaOutput_Callback)
        LuaOutput_Callback(s);
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
        char rcev = LuaSerial.read();
        if(rcev == 'C')
        {
            Lua_ForceToBreak();
        }
        else if(rcev == 'R')
        {
            NVIC_SystemReset();
        }

        LuaSerial.flush();
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

    LuaSerial.begin(115200);
    LuaSerial.setTimeout(20);
    LuaSerial.attachInterrupt(Lua_USART_Callback);
    LuaSerial.println("\r\nInit lua script...");

    Is_LuaError = false;
    Is_LuaExit = false;

    L = luaL_newstate();
    luaL_openlibs(L);

    LuaReg_Com();
    LuaReg_GPIO();
    LuaReg_GUI();
    LuaReg_ModuleCtrl();
    LuaReg_MouseKeyboard();
    LuaReg_Time();
    
    luaL_dostring(L, "print(\"> Version:\",_VERSION)");

    while (!Is_LuaExit)
    {
        if(LuaSerial.available())
        {
            String UserInput = LuaSerial.readString();

            if(UserInput.startsWith("CMD:"))
            {
                String Cmd = UserInput.substring(4);
                if(!CMD_Handler(Cmd))
                {
                    LuaSerial.println();
                    LuaSerial.print(Cmd);
                    LuaSerial.println(" is error cmd!");
                    LuaSerial.flush();
                }
            }
            else
            {
                char *LuaCode_buffer = new char[UserInput.length() + 1];

                LuaSerial.println("\r\nCode:\r\n");
                LuaSerial.println(UserInput);
                LuaSerial.printf("\r\n(Size: %d )\r\n", UserInput.length());
                UserInput.toCharArray(LuaCode_buffer, UserInput.length() + 1);
                LuaSerial.print("\r\n> ");

                Is_LuaRunning = true;
                Is_LuaError = luaL_dostring(L, LuaCode_buffer);
                //luaL_loadbuffer(L, LuaCode_buffer, strlen(LuaCode_buffer), "line") || lua_pcall(L, 0, 0, 0);
                delete []LuaCode_buffer;
                Is_LuaRunning = false;
            }
        }
        if (Is_LuaError)
        {
            LuaSerial.println(lua_tostring(L, -1));
            Is_LuaError = false;
            Is_LuaRunning = false;
        }
    }

    lua_close(L);
    ClearStatusBar();
    LuaSerial.println("lua script exit");
}
