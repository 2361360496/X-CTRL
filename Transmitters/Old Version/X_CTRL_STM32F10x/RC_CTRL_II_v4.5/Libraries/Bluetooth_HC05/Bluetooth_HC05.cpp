#include "Bluetooth_HC05.h"

Bluetooth_HC05::Bluetooth_HC05(HardwareSerial *serialx, int16_t state_Pin, int16_t en_Pin, int16_t powerCtrl_Pin) 
{
    State_Pin = state_Pin;
    EN_Pin = en_Pin;
    PowerCtrl_Pin = powerCtrl_Pin;
    Serialx = serialx;
}

void Bluetooth_HC05::Init(uint32_t baudRate)
{
    BaudRate = baudRate;
    Serialx->begin(BaudRate);
    Serialx->setTimeout(20);
    if(PowerCtrl_Pin != -1)pinMode(PowerCtrl_Pin, OUTPUT);
    if(State_Pin != -1)pinMode(State_Pin, INPUT);
    if(EN_Pin != -1)pinMode(EN_Pin, OUTPUT);
}

bool Bluetooth_HC05::Power(bool state)
{
    if(PowerCtrl_Pin == -1)
        return false;
    
    state ? digitalWrite(PowerCtrl_Pin, HIGH) : digitalWrite(PowerCtrl_Pin, LOW);
    return true;
}

bool Bluetooth_HC05::AT_Enter()
{
    Serialx->begin(38400);
    if(Power(false))
    {
        if(EN_Pin != -1)
            digitalWrite(EN_Pin, HIGH);
        delay(1000);
        Power(true);
        delay(1000);
    }
    else
        delay(1000);

    return (AT_Cmd("AT") != "ERROR" ? true : false);
}

void Bluetooth_HC05::AT_Exit()
{
    if(Power(false))
    {
        if(EN_Pin != -1)
            digitalWrite(EN_Pin, LOW);
        delay(1500);
        Power(true);
    }
    Serialx->begin(BaudRate);
}

String Bluetooth_HC05::AT_Cmd(String Cmd, uint32_t TimeOut)
{
    uint32_t Exit_TimePoint = millis() + TimeOut;
    String AT_Return;
    Cmd.trim();

AT_Retry:
    Serialx->println(Cmd);

    while (millis() < Exit_TimePoint)
    {
        if (Serialx->available())
        {
            String AT_Return = Serialx->readString();
			if (AT_Return.startsWith("ERROR"))
			{
				delay(200);
				goto AT_Retry;
			}
            else
			{
				AT_Return.trim();
				return AT_Return;
			}
        }
    }
	
	Serialx->flush();
    return "ERROR";
}

bool Bluetooth_HC05::SetRole(bool role)
{
    if(AT_Cmd("AT+ROLE=" + String(role)) != "ERROR") //设置主从模式：0从机，1主机
        return true;
    else 
        return false;
}

bool Bluetooth_HC05::SetName(String name)
{
	if(AT_Cmd("AT+NAME=" + name) != "ERROR")//命名模块名
        return true;
	else 
        return false;
}

bool Bluetooth_HC05::SetPassword(String passord)
{
    AT_Cmd("AT+CMODE=1");//蓝牙连接模式为任意地址连接模式
    delay(200);
    AT_Cmd("AT+PSWD=" + passord); //设置配对密码
    delay(200);
    AT_Cmd("AT+RMAAD");//清空配对列表
	
	return true;
}

bool Bluetooth_HC05::SetBaudRate(uint32_t baudRate)
{
    if(AT_Cmd("AT+UART=" + String(baudRate) + ",0,0") != "ERROR") //设置波特率，停止位1，校验位无
        return true;
    else
        return false;
}

bool Bluetooth_HC05::GetState()
{
	if(State_Pin != -1)
		return digitalRead(State_Pin);
	else
		return true;
}

String Bluetooth_HC05::GetPassword()
{
	String passwrod = AT_Cmd("AT+PSWD?");
	
	if (passwrod.startsWith("+PSWD:"))
	{
		passwrod = passwrod.substring(6);
		passwrod.replace("\r\nOK","\0");
		return passwrod;
	}
	else
		return "NO PASSWORD";
}

uint32_t Bluetooth_HC05::GetBaudRate()
{
	String baudRate = AT_Cmd("AT+UART?");
	
	if(baudRate.startsWith("+UART:"))
		return baudRate.substring(6).toInt();
	else
		return 0;
}

String Bluetooth_HC05::GetName()
{
	String name = AT_Cmd("AT+NAME?");
	
	if (name.startsWith("+NAME:"))
	{
		name = name.substring(6);
		name.replace("\r\nOK","\0");
		return name;
	}
	else
		return "NO NAME";
}

int8_t Bluetooth_HC05::GetRole()
{
	String role = AT_Cmd("AT+ROLE?");
	
	if(role.startsWith("+ROLE:"))
		return role.substring(6).toInt();
	else
		return -1;
}
