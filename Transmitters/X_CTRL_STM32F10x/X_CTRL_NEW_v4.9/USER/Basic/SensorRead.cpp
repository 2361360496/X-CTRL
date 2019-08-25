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
    CTRL.Left.X = JS_L.X = JoystickMap(JSL_X_ADC(), JS_L.Xmin, JS_L.Xmid, JS_L.Xmax);   //��ҡ��Y��ӳ��
    CTRL.Left.Y = JS_L.Y = JoystickMap(JSL_Y_ADC(), JS_L.Ymin, JS_L.Ymid, JS_L.Ymax);   //��ҡ��X��ӳ��
    CTRL.Right.X = JS_R.X = JoystickMap(JSR_X_ADC(), JS_R.Xmin, JS_R.Xmid, JS_R.Xmax);  //��ҡ��Y��ӳ��
    CTRL.Right.Y = JS_R.Y = JoystickMap(JSR_Y_ADC(), JS_R.Ymin, JS_R.Ymid, JS_R.Ymax);  //��ҡ��X��ӳ��

    if(State_MPU)//�Ƿ�����������Ӧ
    {
        /*��MPU���������������ҡ��ֵ*/
        extern int16_t *from_MPU_LX, *from_MPU_LY, *from_MPU_RX, *from_MPU_RY;
        if(from_MPU_LX) CTRL.Left.X = *from_MPU_LX;
        if(from_MPU_LY) CTRL.Left.Y = *from_MPU_LY;
        if(from_MPU_RX) CTRL.Right.X = *from_MPU_RX;
        if(from_MPU_RY) CTRL.Right.Y = *from_MPU_RY;
    }

    if(digitalRead(SPDT_Switch_Pin))//�Ƿ�����������
    {
        CTRL.KnobLimit.L = map(ADL_ADC(), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);//���޷���ť��ȡ
        CTRL.KnobLimit.R = map(ADR_ADC(), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);//���޷���ť��ȡ
    }
    else
    {
        /*�����޷�*/
        CTRL.KnobLimit.L = 0;
        CTRL.KnobLimit.R = 0;
    }
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


/*��ʱ���޲������ʹ��*/
bool State_NoOperationMonitor = true;

/*�޲���ʱ������*/
#define NoOperationTime 60
const uint32_t NoOperationAlarmTimeoutMs = NoOperationTimeout * 1000;

/**
  * @brief  ��ʱ���޲������
  * @param  ��
  * @retval ��
  */
static void NoOperationMonitor()
{
    if(!State_NoOperationMonitor)
        return;
    
    static uint32_t lastOperateTime = 0;
    
    /*ҡ���Ƿ񱻲���*/
    if(ABS(JS_L.X) + ABS(JS_L.Y) + ABS(JS_R.X) + ABS(JS_R.Y) > 400)
    {
        lastOperateTime = millis();
    }
    
    /*�����Ƿ񱻲���*/
    if(btUP || btDOWN || btOK || btBACK)
    {
        lastOperateTime = millis();
    }
    
    if(millis() - lastOperateTime > NoOperationAlarmTimeoutMs)
    {
        BuzzMusic(MC_Type::MC_NoOperationWarning);
    }
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
    __IntervalExecute(NoOperationMonitor(), 5000);//��ʱ���޲������
}
