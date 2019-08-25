#include "FileGroup.h"
#include "LuaGroup.h"
#include "ComPrivate.h"

bool LuaOccupy = false;

static int Lua_SetRFState(lua_State *L)
{
    int nValue = lua_gettop(L);
    if(nValue != 1)
    {
        lua_pushstring(L, "Error! Example: setRF(1) -> Turn on RF");
        lua_error(L);
    }
    if (!lua_isinteger(L, 1))
    {
        lua_pushstring(L, "Error! Use integer set RF state");
        lua_error(L);
    }
    State_RF = lua_tointeger(L, 1);
    State_RF ? LuaSerial.println("RF Enalbe") : LuaSerial.println("RF Disable");
    return 0;
}


static uint8_t ItemSelect_MAX = 0;
static int Lua_Handshake(lua_State *L)
{
    State_RF = OFF;//ң�عر�
    ItemSelect_MAX = 0;
    
    int nValue = lua_gettop(L);
    if(nValue != 1 || !lua_isnumber(L, 1))
    {
        lua_pushstring(L, "Error! Example: handshake(2000) -> run handshake process 2000ms");
        lua_error(L);
    }

    LuaSerial.println("Handshake preparing...");
    /*����׼������*/
    HandshakeRun(HandshakeState_Prepare);

    uint32_t StopTime = millis() + lua_tonumber(L, 1);
    uint8_t ItemSelect_MAX_Last = 0;
    while(millis() < StopTime)
    {
        /*��ȡ�ӻ��б�����*/
        ItemSelect_MAX = HandshakeRun(HandshakeState_Search);

        if(ItemSelect_MAX != ItemSelect_MAX_Last)
        {
            LuaSerial.printf(
                "Find-> ID:0x%x %s\r\n",
                HandshakePack_Slave[ItemSelect_MAX_Last].ID,
                HandshakePack_Slave[ItemSelect_MAX_Last].Description
            );

            ItemSelect_MAX_Last = ItemSelect_MAX;
        }
    }

    /*��ʾ�������*/
    if(ItemSelect_MAX > 0)
    {
        LuaSerial.println("Search Done!");
    }
    else
    {
        LuaSerial.println("Not Found!");
    }

    lua_pushnumber(L, ItemSelect_MAX);

    return 1;
}

static int Lua_ConnectSlave(lua_State *L)
{
    if(ItemSelect_MAX == 0)
    {
        lua_pushstring(L, "No slave has found! please retry handshake");
        lua_error(L);
    }

    int nValue = lua_gettop(L);
    if(nValue != 1)
    {
        lua_pushstring(L, "Error! Example: connect(1) -> connect to slave(1)");
        lua_error(L);
    }
    if (!lua_isinteger(L, 1))
    {
        lua_pushstring(L, "Error! Use integer set slave");
        lua_error(L);
    }

    uint8_t ItemSelect = lua_tonumber(L, 1);
    if(ItemSelect >= ItemSelect_MAX)
    {
        char str[20];
        sprintf(str, "slave num max is %d!", ItemSelect_MAX - 1);
        lua_pushstring(L, str);
        lua_error(L);
    }


    /*�������Ӵӻ�*/
    LuaSerial.printf("connecting to %s...\r\n", HandshakePack_Slave[ItemSelect].Description);

    /*��ʱ����*/
    uint32_t timeout = millis();
    bool IsTimeout = false;
    /*�ȴ��ӻ���Ӧ�����ź�*/
    while(HandshakeRun(HandshakeState_ReqConnect, ItemSelect, CMD_AttachConnect) != CMD_AgreeConnect)
    {
        /*2500ms��ʱ*/
        if(millis() - timeout > 2500)
        {
            LuaSerial.println("timeout");
            IsTimeout = true;
            break;
        }
    }

    /*������β���ã���ת��Լ���õ�����Ƶ���Լ���ַ*/
    HandshakeRun(HandshakeState_Connected);

    /*��Ӧ�ӻ�����*/
    CTRL.Info.CtrlObject = HandshakePack_Slave[ItemSelect].CtrlType;

    /*���δ��ʱ��ʾ���ֳɹ�*/
    if(!IsTimeout)
    {
        LuaSerial.println("connecting successful");
    }

    return 0;
}

static int Lua_SetChannle(lua_State *L)
{
    int nValue = lua_gettop(L);
    if(nValue != 2)
    {
        lua_pushstring(L, "Error! Example: setCh(0,1000) -> set channle0 to 1000");
        lua_error(L);
    }

    if (!(lua_isnumber(L, 1) && lua_isnumber(L, 2)))
    {
        lua_pushstring(L, "Error! Use number set channle num and value");
        lua_error(L);
    }

    LuaOccupy = true;
    uint8_t channle = lua_tointeger(L, 1);
    float value = (float)lua_tonumber(L, 2);

    switch(channle)
    {
    case 0:
        CTRL.Left.X = constrain(value, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        break;
    case 1:
        CTRL.Left.Y = constrain(value, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        break;
    case 2:
        CTRL.Right.X = constrain(value, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        break;
    case 3:
        CTRL.Right.Y = constrain(value, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        break;
    default:
        LuaSerial.println("Error Channle num");
        break;
    }

    return 0;
}

static int Lua_GetChannle(lua_State *L)
{
    int nValue = lua_gettop(L);
    if(nValue != 1)
    {
        lua_pushstring(L, "Error! Example: getCh(0) -> get channle0 value");
        lua_error(L);
    }

    if (!lua_isnumber(L, 1))
    {
        lua_pushstring(L, "Error! Use number set channle num");
        lua_error(L);
    }

    uint8_t channle = lua_tointeger(L, 1);
    float value = 0.0;

    switch(channle)
    {
    case 0:
        value = JS_L.X;
        break;
    case 1:
        value = JS_L.Y;
        break;
    case 2:
        value = JS_R.X;
        break;
    case 3:
        value = JS_R.Y;
        break;
    default:
        LuaSerial.println("Error channle num");
        break;
    }

    lua_pushnumber(L, value);

    return 1;
}

static int Lua_SetButton(lua_State *L)
{
    int nValue = lua_gettop(L);
    if(nValue != 2)
    {
        lua_pushstring(L, "Error! Example: setBt(0,1) -> press button 0");
        lua_error(L);
    }

    if (!lua_isnumber(L, 1))
    {
        lua_pushstring(L, "Error! Use number set button num");
        lua_error(L);
    }
	
	if (!lua_isnumber(L, 2))
    {
        lua_pushstring(L, "Error! Use number set button value");
        lua_error(L);
    }

    uint8_t bt = constrain(lua_tointeger(L, 1), 0, 7);
	bool val = lua_tointeger(L, 2);
	
	if(val)
	{
		CTRL.Key |= 1 << bt;
	}
	else
	{
		CTRL.Key &= ~(1 << bt);
	}

    return 1;
}

void LuaReg_Com()
{
    lua_register(L, "setRF", Lua_SetRFState);
    lua_register(L, "handshake", Lua_Handshake);
    lua_register(L, "connect", Lua_ConnectSlave);
    lua_register(L, "setCh", Lua_SetChannle);
	lua_register(L, "setBt", Lua_SetButton);
    lua_register(L, "getCh", Lua_GetChannle);
}
