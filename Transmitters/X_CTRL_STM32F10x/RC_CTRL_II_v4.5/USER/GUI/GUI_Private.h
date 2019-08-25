#ifndef __GUI_PRIVATE_H
#define __GUI_PRIVATE_H

#include "stdint.h"
#include "LightGUI.h"
#include "PageManager.h"
#include "Bitmaps/Bitmaps.h"
#include "Adafruit_SSD1306.h"

#define SCREEN_CLASS Adafruit_SSD1306
#define StatusBar_POS 12
#define clear() fillScreen(screen.Black)
#define ClearPage() screen.fillRect(0,StatusBar_POS+1,screen.width(),screen.height()-(StatusBar_POS+1),screen.Black)

extern Adafruit_SSD1306 screen;
extern PageManager page;

typedef enum{
    PAGE_MainMenu,
    PAGE_CtrlInfo,
    PAGE_Options,
    PAGE_SetJoystick,
    PAGE_SetBluetooth,
	PAGE_SetGravity,
    PAGE_About,
    PAGE_Handshake,
	PAGE_MAX
}PageNum_Type;

typedef enum{
    EVENT_ButtonPress,
    EVENT_ButtonLongPress,
    EVENT_EncoderRotate,
    EVENT_MAX
}Event_Type;

void PageRegister_MainMenu(uint8_t ThisPage);
void PageRegister_CtrlInfo(uint8_t ThisPage);
void PageRegister_Options(uint8_t ThisPage);
void PageRegister_SetJoystick(uint8_t ThisPage);
void PageRegister_SetBluetooth(uint8_t ThisPage);
void PageRegister_SetGravity(uint8_t ThisPage);
void PageRegister_About(uint8_t ThisPage);
void PageRegister_Handshake(uint8_t ThisPage);

void Draw_Bitmap_BasicPage(float pos_x1, float pos_x2, float pos_y);
void Draw_Bitmap_CtrlPage(float pos_x1, float pos_x2, float pos_x3, float pos_y);

void Init_StatusBar();
void Thread_StatusBar();
void PageDelay(uint32_t ms);
void StatusBarDelay(uint32_t ms);

#endif
