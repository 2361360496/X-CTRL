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
    return (
        (ADC_Value - mid) >= 0) ? 
       __Map(ADC_Value, mid, max, 0, CtrlOutput_MaxValue) : 
       __Map(ADC_Value, min, mid, -CtrlOutput_MaxValue, 0
    );
}

/**
  * @brief  ҡ�˶�ȡ
  * @param  ��
  * @retval ��
  */
static void Read_Joystick()
{
    CTRL.Left.X = JS_L.X = JoystickMap(JSL_X_ADC(), JS_L.Xmin, JS_L.Xmid, JS_L.Xmax);   //��ҡ��Y��ӳ��
    CTRL.Left.Y = JS_L.Y = JoystickMap(JSL_Y_ADC(), JS_L.Ymin, JS_L.Ymid, JS_L.Ymax);   //��ҡ��X��ӳ��
    CTRL.Right.X = JS_R.X = JoystickMap(JSR_X_ADC(), JS_R.Xmin, JS_R.Xmid, JS_R.Xmax);  //��ҡ��Y��ӳ��
    CTRL.Right.Y = JS_R.Y = JoystickMap(JSR_Y_ADC(), JS_R.Ymin, JS_R.Ymid, JS_R.Ymax);  //��ҡ��X��ӳ��

    /*��ȡ���ο���*/
    if(digitalRead(SPDT_Switch_Pin))
    {
        CTRL.KnobLimit.L = CtrlOutput_MaxValue;
    }
    else
    {
        CTRL.KnobLimit.L = 0;
    }
    
    /*��ȡ���ο���*/
    CTRL.KnobLimit.R = __Map(SPTT_ADC(), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);
}

/*��ص�ѹ����ص���*/
float BattVoltage,BattUsage;
/*��ص�ѹ��ֵ�˲���*/
static DigitalFilter<float> dfBattUsage(13);

/**
  * @brief  ��ص�ѹ��ȡ
  * @param  ��
  * @retval ��ص�ѹ
  */
static float Read_BattVoltage()
{
    BattVoltage = (BattSensor_ADC() / float(ADC_MaxValue)) * 3.3 * 2.0;
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
    BattUsage = dfBattUsage.getSmooth(__Map(Read_BattVoltage(), BattEmptyVoltage, BattFullVoltage, 0.0, 100.0));
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
    //EncoderMonitor();       //��ת�������¼�����
    Read_BattUsage();       //���µ��״̬
}
