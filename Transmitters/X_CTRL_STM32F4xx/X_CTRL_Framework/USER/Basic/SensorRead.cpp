#include "FileGroup.h"
#include "DigitalFilter.h"
#include "ComPrivate.h"

/*��ҡ��*/
Joystick_t JS_L;

/*��ҡ��*/
Joystick_t JS_R;

/**
  * @brief  ҡ��ֵӳ��
  * @param  ADC_Value: ADCֵ
  * @param  min: ҡ��ADC��Сֵ
  * @param  min: ҡ��ADC��ֵ
  * @param  min: ҡ��ADC���ֵ
  * @retval ��
  */
static int16_t JoystickMap(int16_t ADC_Value, int16_t min, int16_t mid, int16_t max)
{
    __LimitValue(ADC_Value, min, max);
    return (
           (ADC_Value - mid) >= 0) ?
           map(ADC_Value, mid, max, 0, CtrlOutput_MaxValue) :
           map(ADC_Value, min, mid, -CtrlOutput_MaxValue, 0
    );
}

/**
  * @brief  ҡ�˶�ȡ
  * @param  ��
  * @retval ��
  */
static void Read_Joystick()
{
    JS_L.X = constrain(JoystickMap(JSL_X_ADC(), JS_L.Xmin, JS_L.Xmid, JS_L.Xmax), -CtrlOutput_MaxValue, CtrlOutput_MaxValue);   //��ҡ��X��ӳ��
    JS_L.Y = constrain(JoystickMap(JSL_Y_ADC(), JS_L.Ymin, JS_L.Ymid, JS_L.Ymax), -CtrlOutput_MaxValue, CtrlOutput_MaxValue);   //��ҡ��Y��ӳ��
    JS_R.X = constrain(JoystickMap(JSR_X_ADC(), JS_R.Xmin, JS_R.Xmid, JS_R.Xmax), -CtrlOutput_MaxValue, CtrlOutput_MaxValue);  //��ҡ��X��ӳ��
    JS_R.Y = constrain(JoystickMap(JSR_Y_ADC(), JS_R.Ymin, JS_R.Ymid, JS_R.Ymax), -CtrlOutput_MaxValue, CtrlOutput_MaxValue);  //��ҡ��Y��ӳ��
}

/*��ص�ѹ����ص���*/
float BattVoltage, BattUsage;
/*��ص�ѹ��ֵ�˲���*/
static DigitalFilter<float> dfBattUsage(13);

/**
  * @brief  ��ص�ѹ��ȡ
  * @param  ��
  * @retval ��ص�ѹ
  */
static float Read_BattVoltage()
{
    BattVoltage = (BattSensor_ADC() / float(ADC_MaxValue)) * 3.3f * 2.0f;
    __LimitValue(BattVoltage, BattEmptyVoltage, BattFullVoltage);
    return (BattVoltage);
}

/**
  * @brief  ��ص�����ȡ
  * @param  ��
  * @retval ��
  */
static void Read_BattUsage()
{
    /*���˲���ȡ���˲���ĵ�ѹֵȻ��ӳ��Ϊ0~100%�ĵ���*/
    BattUsage = dfBattUsage.getSmooth(__Map(Read_BattVoltage(), BattEmptyVoltage, BattFullVoltage, 0.0f, 100.0f));
}

/**
  * @brief  ��ص�����ȡ
  * @param  ��
  * @retval ��
  */
void Task_SensorUpdate()
{
    Read_Joystick();        //��ȡҡ��
    ButtonEventMonitor();   //�����¼�����
    EncoderMonitor();       //��ת�������¼�����
    Read_BattUsage();       //���µ��״̬
}
