#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

/*����ʹ�ܿ���*/
bool State_Bluetooth = ON;

static bool State_Bluetooth_Last;

/*�������Ӷ���*/
uint8_t Bluetooth_ConnectObject = BC_Type::BC_XFS;

/*ʵ����HC05��������*/
Bluetooth_HC05 hc05(&Bluetooth_SERIAL, BT_State_Pin, BT_EN_Pin, BT_PowerCtrl_Pin);

/*������ѡ������*/
static const uint32_t UseBaudRate[] = {1200, 2400, 4800, 9600, 14400, 19200, 38400, 43000, 57600, 76800, 115200};

/*������ѡ�����ʸ���*/
#define UseBaudRate_Size __Sizeof(UseBaudRate)

/*����������ѡ����*/
static uint8_t BaudRateSelect = 0;

/*����*/
static String Password;

/*���ӽ�ɫ*/
static bool Role;

/*ѡ��������*/
#define ItemSelect_MAX 5

/*ѡ���ַ�������*/
static String ItemStr[ItemSelect_MAX];

/*��ǰѡ�е�ѡ��*/
static int16_t ItemSelect = 0;

/*ѡ����±�־λ*/
static bool ItemSelectUpdating = false;

static bool SaveChanges = false;

/*ѡ����ʾ��ʼ����*/
#define ItemStartY (StatusBar_POS+8)
#define ItemStartX 14


/**
  * @brief  ����ѡ���ַ���
  * @param  ��
  * @retval ��
  */
static void UpdateItemStr()
{
    for(uint8_t i = 0; i < ItemSelect_MAX; i++)
    {
        if(i == ItemSelect)
            screen.setTextColor(screen.Yellow, screen.Black);
        else
            screen.setTextColor(screen.White, screen.Black);

        screen.setCursor(ItemStartX, ItemStartY + i * (TEXT_HEIGHT_1 + 2));
        screen.print(ItemStr[i]);
    }
}

/**
  * @brief  ATָ����ʱ�ص�
  * @param  ��
  * @retval ��
  */
static void BluetoothDelayCallback()
{
    Thread_StatusBar();
}

/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    SaveChanges = false;
    State_Bluetooth_Last = State_Bluetooth;
    State_Bluetooth = OFF;

    screen.setTextColor(screen.White, screen.Black);
    screen.setCursor(20, StatusBar_POS + 10);
    screen.print("Enter AT Mode...");

    hc05.SetDelayCallback(BluetoothDelayCallback);

    if(hc05.AT_Enter())//����ATģʽ
    {
        ItemSelect = 0;
        BaudRateSelect = 0;
        ItemSelectUpdating = false;
        screen.setCursor(20, StatusBar_POS + 20);
        screen.setTextColor(screen.Green, screen.Black);
        screen.print("SUCCESS");

        uint32_t baudRate = 0;
        String name, sbaudRate, password;

        if(!hc05.GetName(&name))
            name = "*";

        if(!hc05.GetBaudRate(&baudRate))
            sbaudRate = "*";
        else
            sbaudRate = String(baudRate);

        if(!hc05.GetPassword(&password))
            password = "*";

        ItemStr[0] = "1.Name:" + name;
        ItemStr[1] = "2.BaudRate:" + sbaudRate;
        ItemStr[2] = "3.Password:" + password;
        ItemStr[3] = "4.Role:" + String(hc05.GetRole() ? "Master" : "Slaver");
        ItemStr[4] = "5.Save & Exit";
        Role = hc05.GetRole();
        ClearPage();
        UpdateItemStr();
    }
    else
    {
        screen.setTextColor(screen.Red, screen.Black);
        screen.print("FAILED");
        PageDelay(1000);
        page.PagePop();
    }
}

/**
  * @brief  ҳ��ѭ���¼�
  * @param  ��
  * @retval ��
  */
static void Loop()
{
    if(ItemSelectUpdating)
    {
        UpdateItemStr();
        ItemSelectUpdating = false;
    }
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
    ClearPage();
    screen.setTextColor(screen.White, screen.Black);
    screen.setCursor(20, StatusBar_POS + 10);
    screen.print("Exit AT Mode...");
    if(SaveChanges)
    {
        hc05.SetName(String(Master_Description));
        hc05.SetBaudRate(UseBaudRate[BaudRateSelect]);
        hc05.SetPassword(Password);
        hc05.SetRole(Role);
    }
    hc05.AT_Exit();

    State_Bluetooth = State_Bluetooth_Last;
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
        if(btOK || btEcd)
        {
            switch(ItemSelect)
            {
            case 1:
                BaudRateSelect = (BaudRateSelect + 1) % UseBaudRate_Size;
                ItemStr[1] = "2.BaudRate:" + String(UseBaudRate[BaudRateSelect]) + "  ";
                ItemSelectUpdating = true;
                break;
            case 2:
                static uint8_t i = 0;
                i++;
                i %= 10;
                Password = String(i * 1000 + i * 100 + i * 10 + i);
                ItemStr[2] = "3.Password:" + Password;
                ItemSelectUpdating = true;
                break;
            case 3:
                Role = ! Role;
                ItemStr[3] = "4.Role:" + String(Role ? "Master" : "Slaver");
                ItemSelectUpdating = true;
                break;
            case 4:
                SaveChanges = true;
                page.PagePop();
                break;
            }
        }
        if(btBACK)
        {
            page.PagePop();
        }

        if(btUP)
        {
            ItemSelect = (ItemSelect + 1) % ItemSelect_MAX;
            ItemSelectUpdating = true;
        }
        if(btDOWN)
        {
            ItemSelect = (ItemSelect + ItemSelect_MAX - 1) % ItemSelect_MAX;
            ItemSelectUpdating = true;
        }
    }
}

/**
  * @brief  ��������ҳ��ע��
  * @param  ThisPage:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_SetBluetooth(uint8_t ThisPage)
{
    page.PageRegister(ThisPage, Setup, Loop, Exit, Event);
}
