#include "FileGroup.h"
#include "ComPrivate.h"
#include "GUI_Private.h"

// the nRF24L01+ can tune to 128 channels with 1 MHz spacing from 2.400 GHz to 2.527 GHz.
#define CHANNELS 128
#define TEXT_BASE_Y (StatusBar_POS + TEXT_HEIGHT_1)
#define BASE_Y (TEXT_HEIGHT_1+10)
#define MAX_HEIGHT (screen.height() - StatusBar_POS - BASE_Y - 5)
static uint16_t signalStrength[CHANNELS]; // smooths signal strength with numerical range 0 - 0x7FFF
static uint8_t NRF_BKUP_EN_AA = 0;
static uint8_t NRF_BKUP_RF_SETUP = 0;
static uint8_t NRF_BKUP_RF_CH = 0;
static uint8_t NRF_BKUP_CONFIG = 0;
static bool HoldGraph = false;
static float RecvSensitivity = 0.4f;

/********** ���� ************/
/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    State_RF = false;
    HoldGraph = false;
    ClearPage();

    screen.setTextColor(screen.White, screen.Black);
    screen.setTextSize(1);
    screen.setCursor(TextMid(0.5f, 7), TEXT_BASE_Y);
    screen.print("2.4 GHz");
    screen.setCursor(TextMid(0.5f, 12), TEXT_BASE_Y + TEXT_HEIGHT_1);
    screen.print("Band scanner");
    screen.setCursor(TextMid(0.5f, 20), TEXT_BASE_Y + TEXT_HEIGHT_1 * 3);
    screen.print("By ceptimus. Nov '16");
    PageDelay(1000);
    ClearPage();

    nrf.SetCE(false);

    /*backup nrf reg*/
    NRF_BKUP_EN_AA = nrf.SPI_Read(nrf.EN_AA);
    NRF_BKUP_RF_SETUP = nrf.SPI_Read(nrf.RF_SETUP);
    NRF_BKUP_RF_CH = nrf.SPI_Read(nrf.RF_CH);
    NRF_BKUP_CONFIG = nrf.SPI_Read(nrf.CONFIG);

    /*write nrf reg*/
    nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.EN_AA, 0x00); // switch off Shockburst mode
    nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.RF_SETUP, 0x0F);// write default value to setup register

    nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.STATUS, 0x70);        // reset the flag(s)
    nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.CONFIG, 0x0F);        // switch to RX mode
    nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.STATUS, nrf.RX_DR | nrf.TX_DS | nrf.MAX_RT);    //reset the flag(s)
    nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.CONFIG,
                   (1 << nrf.EN_CRC)   // switch to RX mode
                   | (1 << nrf.CRCO)
                   | (1 << nrf.PWR_UP)
                   | (1 << nrf.PRIM_RX));
    delayMicroseconds(130);
    nrf.SetCE(true);

    screen.drawFastHLine(0, screen.height() - BASE_Y + 1, CHANNELS, screen.White);

    for (int x = 0; x < CHANNELS; x++)
    {
        if (!(x % 10))
        {
            screen.drawFastVLine(x, screen.height() - BASE_Y + 1, 3, screen.White); // graduation tick every 10 MHz
        }
        if (x == 10 || x == 60 || x == 110)
        {
            screen.drawFastVLine(x, screen.height() - BASE_Y + 1, 5, screen.White);; // scale markers at 2.41, 2.46, and 2.51 GHz
        }
    }
    screen.setCursor(10 - TEXT_WIDTH_1 * 1.5f, screen.height() - TEXT_HEIGHT_1 - 2);
    screen.print("2.41");
    screen.setCursor(60 - TEXT_WIDTH_1 * 1.5f, screen.height() - TEXT_HEIGHT_1 - 2);
    screen.print("2.46");
    screen.setCursor(110 - TEXT_WIDTH_1 * 1.5f, screen.height() - TEXT_HEIGHT_1 - 2);
    screen.print("2.51");
}

/**
  * @brief  ҳ��ѭ���¼�
  * @param  ��
  * @retval ��
  */
static void Loop()
{
    static uint8_t refresh;
    randomSeed(RNG_GetRandNum());
    for (uint8_t MHz = 0; MHz < CHANNELS; MHz++ )   // tune to frequency (2400 + MHz) so this loop covers 2.400 - 2.527 GHz (maximum range module can handle) when channels is set to 128.
    {
        nrf.SetFreqency(MHz);
        nrf.SetCE(true); // start receiving
        delayMicroseconds(random(130, 230)); // allow receiver time to tune and start receiving 130 uS seems to be the minimum time.  Random additional delay helps prevent strobing effects with frequency-hopping transmitters.
        nrf.SetCE(false); // stop receiving - one bit is now set if received power was > -64 dBm at that instant
        if (nrf.SPI_Read(nrf.CD))   // signal detected so increase signalStrength unless already maxed out
        {
            signalStrength[MHz] += (0x7FFF - signalStrength[MHz]) >> 5; // increase rapidly when previous value was low, with increase reducing exponentially as value approaches maximum
        }
        else     // no signal detected so reduce signalStrength unless already at minimum
        {
            signalStrength[MHz] -= signalStrength[MHz] >> 5; // decrease rapidly when previous value was high, with decrease reducing exponentially as value approaches zero
        }
        // Serial.print((signalStrength[MHz] + 0x0100) >> 9, HEX); // debugging without lcd display
        // Serial.print(" "); // debugging without lcd display


        __ValueMonitor(
            (int(RecvSensitivity * 10)),
            (
                screen.setTextColor(screen.White, screen.Black),
                screen.setCursor(TextMid(0.8f, 3), StatusBar_POS + TEXT_HEIGHT_1),
                screen.print(RecvSensitivity, 1)
            )
        );

        if(HoldGraph)
        {
            screen.setTextColor(screen.Red);
            screen.setCursor(TextMid(0.5f, 4), ScreenMid_H);
            screen.print("HOLD");
            return;
        }

        if (!--refresh)   // don't refresh whole display every scan (too slow)
        {
            refresh = 19; // speed up by only refreshing every n-th frequency loop - reset number should be relatively prime to CHANNELS
            float strength = ((signalStrength[MHz] + 0x0040) >> 7) * RecvSensitivity;
            __LimitValue(strength, 0.0f, (float)MAX_HEIGHT);

            screen.drawFastVLine(MHz, screen.height() - BASE_Y - MAX_HEIGHT, MAX_HEIGHT - strength, screen.Black);
            screen.drawFastVLine(MHz, screen.height() - BASE_Y - strength, strength, screen.Yellow);
        }
    }
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
    nrf.SetCE(false);
    nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.EN_AA, NRF_BKUP_EN_AA);
    nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.RF_SETUP, NRF_BKUP_RF_SETUP);
    nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.RF_CH, NRF_BKUP_RF_CH);
    nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.CONFIG, NRF_BKUP_CONFIG);
    nrf.SetCE(true);
}

/************ �¼����� **********/
/**
  * @brief  ���������¼�
  * @param  ��
  * @retval ��
  */
static void ButtonPressEvent()
{
    if(btOK)
        HoldGraph = !HoldGraph;

    if(btUP)
        RecvSensitivity += 0.1f;

    if(btDOWN)
        RecvSensitivity -= 0.1f;

    __LimitValue(RecvSensitivity, 0.1f, 1.0f);
}

/**
  * @brief  ���������¼�
  * @param  ��
  * @retval ��
  */
static void ButtonLongPressEvent()
{
    if(btBACK)
    {
        page.PageChangeTo(PAGE_Options);
    }
}

/**
  * @brief  ҳ��ע��
  * @param  ThisPage:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_FreqGraph(uint8_t ThisPage)
{
    /*����*/
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);

    /*�¼�*/
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
}
