#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "Fonts/FreeMono12pt7b.h"
#include "cm_backtrace.h"
#include <stdarg.h>

/*ʵ������ʾ������*/
TFT_SSD1283A screen(TFT_CS, TFT_RST, TFT_DC/*, TFT_SDA, TFT_SCK*/);

/*ʵ����GUIҳ�������*/
PageManager page(PAGE_MAX);

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

    screen.begin();

    pinMode(TFT_LED, OUTPUT);
    digitalWrite(TFT_LED, LOW);

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
    if(SD_GetReady() && BvPlayer_GetReady())
        PageRegister_MainMenuDymanic(PAGE_MainMenu);
    else
        PageRegister_MainMenu(PAGE_MainMenu);

    /*����ҳ��ע��*/
    PAGE_REG(About);
    PAGE_REG(BvPlayer);
    PAGE_REG(ChannelCfg);
    PAGE_REG(CtrlInfo);
    PAGE_REG(FreqGraph);
    PAGE_REG(FileExplorer);
    PAGE_REG(GameHopper);
    PAGE_REG(GameDoom);
    PAGE_REG(Handshake);
    PAGE_REG(Jamming);
    PAGE_REG(LuaScript);
    PAGE_REG(Options);
    PAGE_REG(SetJoystick);
    PAGE_REG(SetBluetooth);
    PAGE_REG(SetGravity);
    PAGE_REG(WavPlayer);
    /*��ת�����˵�*/
    page.PagePush(PAGE_MainMenu);
}

/**
  * @brief  GUI��ʼ��
  * @param  ��
  * @retval ��
  */
void Init_GUI(uint8_t step)
{
    DEBUG_FUNC();
    if(step == 0)
    {
        Draw_RGBBitmap_Logo(0.5, 0.5, 30);  //����LOGO����
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
static void SoftDelay(uint32_t ms)
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
static void DisplayCrashReports()
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
    void vApplicationHardFaultHook()
    {
        DisplayCrashReports();
        SoftDelay(5000);
        NVIC_SystemReset();
    }

    __asm void HardFault_Handler()
    {
        extern vApplicationHardFaultHook
        extern cm_backtrace_fault

        mov r0, lr
        mov r1, sp
        bl cm_backtrace_fault
        bl vApplicationHardFaultHook
Fault_Loop
        b Fault_Loop
    }
}
