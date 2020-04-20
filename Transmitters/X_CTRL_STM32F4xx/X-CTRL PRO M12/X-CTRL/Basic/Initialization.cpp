#include "Basic/FileGroup.h"
#include "GUI/DisplayPrivate.h"
#include "Communication/ComPrivate.h"

/*ң�����ṹ��*/
CTRL_TypeDef CTRL;

/**
  * @brief  ������ʼ��
  * @param  ��
  * @retval true�ɹ� falseʧ��
  */
bool Value_Init()
{
    DEBUG_FUNC_LOG();
    uint8_t eep_ret = EEPROM_Init();
    
    EEPROM_REG_VALUE(CTRL);
    
    if(eep_ret != 0)
    {
        Serial.printf("EEPROM error! (0x%x)\r\n", eep_ret);
        return false;
    }

    if(!EEPROM_ReadAll())
    {
        CTRL.State.IMU = true;
        CTRL.State.Sound = true;
        CTRL.State.Vibrate = true;
        CTRL.State.Handshake = true;
        CTRL.State.PassBack = true;
        CTRL.State.FHSS = true;
        Serial.println("EEPROM data error! set data to default value");
        EEPROM_SaveAll();
        return false;
    }
    
    CTRL.State.IMU = false;
    CTRL.State.Sound = true;
    CTRL.State.Vibrate = true;
    CTRL.State.Handshake = true;
    CTRL.State.PassBack = true;
    CTRL.State.FHSS = false;
    
    Serial.println("EEPROM init success");
    
    return true;
}

/**
  * @brief  ң������ʼ��
  * @param  ��
  * @retval ��
  */
void X_CTRL_Init()
{
    Serial.begin(115200);
    DEBUG_FUNC_LOG();
    
    I2C_Scan();
    
    Value_Init();
    Display_Init();
    Button_Init();
    Buzz_Init();
    IMU_Init();
    Power_Init();
    Joystick_Init();
    Bluetooth_Init();
    Motor_Init();
    BigMotor_Init();

    if(Com_Init())
    {
        Serial.println("Communication init success");
        Buzz_PlayMusic(MC_Type::MC_StartUp);
    }
    else
    {
        Serial.println("Communication error!");
        Buzz_PlayMusic(MC_Type::MC_Error);
    }
    
    Motor_Vibrate(1, 100);
}
