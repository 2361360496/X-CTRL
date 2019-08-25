#ifndef __GUI_PRIVATE_H
#define __GUI_PRIVATE_H

#include "stdint.h"
#include "LightGUI.h"
#include "PageManager.h"
#include "Bitmaps.h"
#include "Adafruit_ST7789.h"

#define SCREEN_CLASS Adafruit_ST7789

#define StatusBar_POS 17
#define ClearDisplay()      screen.fillScreen(screen.Black)
#define ClearPage()         screen.fillRect(0,StatusBar_POS+1,screen.width(),screen.height()-(StatusBar_POS+1),screen.Black)
#define ClearStatusBar()    screen.fillRect(0,0,screen.width(),StatusBar_POS-1,screen.Black)

extern SCREEN_CLASS screen;
extern PageManager page;

typedef enum {
    PAGE_MainMenu,
    PAGE_CtrlInfo,
    PAGE_Options,
    PAGE_SetJoystick,
    PAGE_SetBluetooth,
    PAGE_SetGravity,
    PAGE_About,
    PAGE_Handshake,
	PAGE_Game,
    PAGE_MAX
} PageNum_Type;

typedef enum {
    EVENT_ButtonPress,
    EVENT_ButtonLongPress,
    EVENT_ButtonRelease,
    EVENT_EncoderRotate,
    EVENT_MAX
} Event_Type;

void PageRegister_MainMenu(uint8_t ThisPage);
void PageRegister_CtrlInfo(uint8_t ThisPage);
void PageRegister_Options(uint8_t ThisPage);
void PageRegister_SetJoystick(uint8_t ThisPage);
void PageRegister_SetBluetooth(uint8_t ThisPage);
void PageRegister_SetGravity(uint8_t ThisPage);
void PageRegister_About(uint8_t ThisPage);
void PageRegister_Handshake(uint8_t ThisPage);
void PageRegister_Game(uint8_t ThisPage);

void Draw_RGBBitmap_Logo(float x, float y, uint16_t time, bool dir = 0);

void Init_StatusBar();
void Thread_StatusBar();
void PageDelay(uint32_t ms);
void StatusBarDelay(uint32_t ms);

#endif
