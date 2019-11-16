#include "FileGroup.h"
#include "GUI_Private.h"
#include "Fonts/FreeMono12pt7b.h"
#include "cm_backtrace.h"
#include <stdarg.h>

/*ʵ������ʾ������*/
TFT_SSD1283A screen(TFT_CS, TFT_RST, TFT_DC/*, TFT_SDA, TFT_SCK*/);

/*ʵ����GUIҳ�������*/
PageManager page(PAGE_MAX, EVENT_MAX);

void InfoPrintf(const char *__restrict __format, ...)
{
    char printf_buff[1024];

    va_list args;
    va_start(args, __format);
    int ret_status = vsnprintf(printf_buff, sizeof(printf_buff), __format, args);
    va_end(args);
    Serial.print(printf_buff);
}

void assert_failed(uint8_t* file, uint32_t line)
{
    Serial.printf("Assert Failed File:%s Line:%d\r\n", (char*)file, line);
    while(1);
}

/**
  * @brief  ��ʾ����ʼ��
  * @param  ��
  * @retval ��
  */
void Init_Display()
{
    Serial.begin(115200);
    cm_backtrace_init("Pro", _X_CTRL_VERSION , _X_CTRL_VERSION);
    
    pinMode(TFT_LED, OUTPUT);
    digitalWrite(TFT_LED, LOW);

    screen.begin();
    screen.setRotation(1);
    ClearDisplay();
    screen.setTextColor(screen.White, screen.Black);
    screen.setTextWrap(true);
    screen.setTextSize(1);

    delay(200);
    digitalWrite(TFT_LED, HIGH);
}

/**
  * @brief  ҳ��ע��
  * @param  ��
  * @retval ��
  */
static void Init_Pages()
{
    /*���SD��������ȷ��ע�ᶯ̬���˵�������ʹ��Ĭ�ϲ˵�*/
    if(State_SD_Enable && State_BV_Enable)
        PageRegister_MainMenuDymanic(PAGE_MainMenu);
    else
        PageRegister_MainMenu(PAGE_MainMenu);

    /*����ҳ��ע��*/
    PageRegister_CtrlInfo(PAGE_CtrlInfo);
    PageRegister_Options(PAGE_Options);
    PageRegister_SetJoystick(PAGE_SetJoystick);
    PageRegister_SetBluetooth(PAGE_SetBluetooth);
    PageRegister_SetGravity(PAGE_SetGravity);
    PageRegister_About(PAGE_About);
    PageRegister_Handshake(PAGE_Handshake);
    PageRegister_GameHopper(PAGE_GameHopper);
    PageRegister_GameDoom(PAGE_GameDoom);
    PageRegister_FreqGraph(PAGE_FreqGraph);
    PageRegister_FileExplorer(PAGE_FileExplorer);
    PageRegister_BvPlayer(PAGE_BvPlayer);
    PageRegister_WavPlayer(PAGE_WavPlayer);
    PageRegister_LuaScript(PAGE_LuaScript);

    /*��ת�����˵�*/
    page.PageChangeTo(PAGE_MainMenu);
}

/**
  * @brief  GUI��ʼ��
  * @param  ��
  * @retval ��
  */
void Init_GUI(uint8_t step)
{
    if(step == 0)
    {
        Draw_RGBBitmap_Logo(0.5, 0.5, 30);  //����LOGO����
        //Init_HMI();                         //��ʼ��HMI
    }
    else
    {
        ClearDisplay();                     //����
        Init_StatusBar();                   //״̬����ʼ��
        Init_Pages();                       //ҳ���ʼ��
    }
}

/**
  * @brief  �ڸ�ҳ����ʱ�������ֶ�״̬����ˢ��
  * @param  ms:��ʱ������
  * @retval ��
  */
void PageDelay(uint32_t ms)
{
    uint32_t time = millis();
    while(millis() - time < ms)
        Thread_StatusBar();
}

/**
  * @brief  �����ʱ
  * @param  ms:����
  * @retval ��
  */
static void SoftDealy(uint32_t ms)
{
    volatile uint32_t i = F_CPU / 1000 * ms / 5;
    while(i--);
}

/**
  * @brief  GUI�߳�
  * @param  ��
  * @retval ��
  */
void Thread_GUI()
{
    Thread_StatusBar();
    page.Running();
}

/**
  * @brief  ��ʾ������������
  * @param  ��
  * @retval ��
  */
static void ShowCrashReports()
{
    screen.fillScreen(screen.Blue);
    screen.setTextColor(screen.White);
    screen.setFont(&FreeMono12pt7b);
    screen.setTextSize(2);
    screen.setCursor(0, 40);
    screen.println(":(");

    screen.setFont();
    screen.setTextSize(1);

    screen.setCursor(0, ScreenMid_H - TEXT_HEIGHT_1);
    screen.println("FATAL ERROR!");
    screen.print("Syetem will reboot...");

    screen.setCursor(0, screen.height() - TEXT_HEIGHT_1 * 6 - 2);
    screen.println("Error code:");
    screen.printf("MMFAR=0x%x\r\n", SCB->MMFAR);
    screen.printf("BFAR=0x%x\r\n", SCB->BFAR);
    screen.printf("CFSR=0x%x\r\n", SCB->CFSR);
    screen.printf("HFSR=0x%x\r\n", SCB->HFSR);
    screen.printf("DFSR=0x%x\r\n", SCB->DFSR);
}

extern "C" {
    void HardFault_Handler()
    {
        ShowCrashReports();
        SoftDealy(5000);
        NVIC_SystemReset();
    }
}
