#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "ComPrivate.h"
#include "IMU_Private.h"

/*ʵ������ǰҳ�������*/
static MillisTaskManager mtm_SetGravity(2);


/**
  * @brief  ����MPU��������
  * @param  ��
  * @retval ��
  */
static void Task_UpdateMPUPos()
{
}

/**
  * @brief  ����MPU״̬����
  * @param  ��
  * @retval ��
  */
static void Task_UpdateMPU_State()
{
    screen.setCursor(5, StatusBar_Height + TEXT_HEIGHT_1);
    screen.setTextSize(1);
    if(State_MPU)
    {
        screen.setTextColor(IsCalibrateStart ? screen.Yellow : screen.Green, screen.Black);
        screen.print(IsCalibrateStart ? "CAB" : "ON ");
    }
    else
    {
        screen.setTextColor(screen.Red, screen.Black);
        screen.print("OFF");
    }

    screen.setTextColor(screen.White, screen.Black);
    screen.setCursor(5, StatusBar_Height + 6 * TEXT_HEIGHT_1);
    screen.printf("Pitch:% 7.2f", IMU_Axis.Pitch.Angle);
    screen.setCursor(5, StatusBar_Height + 7 * TEXT_HEIGHT_1);
    screen.printf("Roll: % 7.2f", IMU_Axis.Roll.Angle);
    screen.setCursor(5, StatusBar_Height + 8 * TEXT_HEIGHT_1);
    screen.printf("Yaw:  % 7.2f", IMU_Axis.Yaw.Angle);
}

/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    mtm_SetGravity.TaskRegister(0, Task_UpdateMPUPos, 10);
    mtm_SetGravity.TaskRegister(1, Task_UpdateMPU_State, 50);
}

/**
  * @brief  ҳ��ѭ���¼�
  * @param  ��
  * @retval ��
  */
static void Loop()
{
    mtm_SetGravity.Running(millis());
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
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
        if(param == &btUP)
        {
            
        }
        if(param == &btDOWN)
        {
            
        }
    }
    else if(event == EVENT_ButtonLongPressed)
    {
        if(IsCalibrateStart)
            return; 
        
        if(param == &btBACK)
        {
            page.PagePop();
        }
        if(param == &btOK)
        {
            State_MPU = !State_MPU;
        }
    }
    else if(event == EVENT_ButtonDoubleClick)
    {
        if(param == &btOK)
        {
            IMU_CalibrateStart();
        }
    }
}

/**
  * @brief  ������Ӧҳ��ע��
  * @param  pageID:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_SetGravity(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
