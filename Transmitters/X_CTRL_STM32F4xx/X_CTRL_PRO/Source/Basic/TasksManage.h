#ifndef __TASKSMANAGE_H
#define __TASKSMANAGE_H

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"

/*Task Functions*/
void Task_Dispaly(void *pvParameters);
void Task_TransferData(void *pvParameters);
void Task_LuaScript(void *pvParameters);

void Task_MotorRunning(TimerHandle_t xTimer);
void Task_ReadBattInfo(TimerHandle_t xTimer);

/*TaskHandle*/
extern TaskHandle_t TaskHandle_LuaScript;

/*TimerHandle*/
extern TimerHandle_t TimerHandle_Motor;
extern TimerHandle_t TimerHandle_Charger;

#endif
