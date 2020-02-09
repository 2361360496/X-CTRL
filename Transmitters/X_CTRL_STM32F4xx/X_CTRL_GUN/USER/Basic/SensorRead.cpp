#include "FileGroup.h"
#include "DigitalFilter.h"
#include "ComPrivate.h"
#include "LuaGroup.h"
#include "math.h"

/*������*/
CTRL_TypeDef CTRL;

static void JoystickInit()
{
    CTRL.JS_L.X.Min = 0;
    CTRL.JS_L.X.Mid = ADC_MaxValue / 2;
    CTRL.JS_L.X.Max = ADC_MaxValue;
    CTRL.JS_L.X.Curve.Start = CTRL.JS_L.X.Curve.End = 5.0f;

    CTRL.JS_L.Y.Min = 1703;
    CTRL.JS_L.Y.Mid = 2020;
    CTRL.JS_L.Y.Max = 2929;
    CTRL.JS_L.Y.Curve.Start = CTRL.JS_L.Y.Curve.End = 5.0f;

    CTRL.JS_R.X.Min = 324;
    CTRL.JS_R.X.Mid = 2017;
    CTRL.JS_R.X.Max = 3917;
    CTRL.JS_R.X.Curve.Start = CTRL.JS_R.X.Curve.End = 5.0f;

    CTRL.JS_R.Y.Min = 0;
    CTRL.JS_R.Y.Mid = ADC_MaxValue / 2;
    CTRL.JS_R.Y.Max = ADC_MaxValue;
    CTRL.JS_R.Y.Curve.Start = CTRL.JS_R.Y.Curve.End = 5.0f;
}

/**
  * @brief  ��һ��ֵ�ı仯���������ӳ�䵽��һ������
  * @param  x:��ӳ���ֵ
  * @param  in_min:��ӳ���ֵ����Сֵ
  * @param  in_min:��ӳ���ֵ�����ֵ
  * @param  out_min:��ӳ���ֵ����Сֵ
  * @param  out_min:��ӳ���ֵ�����ֵ
  * @param  startK:���б��
  * @param  endK:�յ�б��
  * @retval ӳ��ֵ���
  */
double NonlinearMap(double value, double in_min, double in_max, double out_min, double out_max, double startK, double endK)
{
    if(ABS(startK - endK) < 0.00001f)
        return __Map(value, in_min, in_max, out_min, out_max);

    double stY = exp(startK);
    double edY = exp(endK);
    float x = __Map(value, in_min, in_max, startK, endK);

    return __Map(exp(x), stY, edY, out_min, out_max);
}

/**
  * @brief  ҡ��ֵӳ��
  * @param  *js: ҡ�˶����ַ
  * @param  adc_x: X��ADCֵ
  * @param  adc_y: Y��ADCֵ
  * @retval ��
  */
static void JoystickMap(Joystick_TypeDef* js, int16_t adc_x, int16_t adc_y)
{
    __LimitValue(adc_x, js->X.Min, js->X.Max);
    __LimitValue(adc_y, js->Y.Min, js->Y.Max);

    js->X.Val = constrain(
                    ((adc_x - js->X.Mid) >= 0) ?
                    NonlinearMap(adc_x, js->X.Mid, js->X.Max, 0, RCX_ChannelData_Max, js->X.Curve.Start, js->X.Curve.End) :
                    NonlinearMap(adc_x, js->X.Mid, js->X.Min, 0, -RCX_ChannelData_Max, js->X.Curve.Start, js->X.Curve.End),
                    -RCX_ChannelData_Max,
                    RCX_ChannelData_Max
                );
    js->Y.Val = constrain(
                    ((adc_y - js->Y.Mid) >= 0) ?
                    NonlinearMap(adc_y, js->Y.Mid, js->Y.Max, 0, RCX_ChannelData_Max, js->Y.Curve.Start, js->Y.Curve.End) :
                    NonlinearMap(adc_y, js->Y.Mid, js->Y.Min, 0, -RCX_ChannelData_Max, js->Y.Curve.Start, js->Y.Curve.End),
                    -RCX_ChannelData_Max,
                    RCX_ChannelData_Max
                );
}

/**
  * @brief  ҡ�˶�ȡ
  * @param  ��
  * @retval ��
  */
static void JoystickUpdate()
{
    JoystickMap(&CTRL.JS_L, JSL_X_ADC(), JSL_Y_ADC());
    JoystickMap(&CTRL.JS_R, JSR_X_ADC(), JSR_Y_ADC());

    if(IS_EnableCtrl())//�Ƿ�����������
    {
        CTRL.KnobLimit.L = DR_TH_Value;//map(ADL_ADC(), 0, ADC_MaxValue, 0, RCX_ChannelData_Max);//���޷���ť��ȡ
        CTRL.KnobLimit.R = DR_ST_Value;//map(ADR_ADC(), 0, ADC_MaxValue, 0, RCX_ChannelData_Max);//���޷���ť��ȡ
    }
    else
    {
        /*�޷���0*/
        CTRL.KnobLimit.L = 0;
        CTRL.KnobLimit.R = 0;
    }
}

/*��ص�ѹ��ֵ�˲���*/
static FilterAverage<float> dfBattVoltage(13);

/*��ص����ͻ��˲���*/
static FilterHysteresis<float> dfBattUsage(3.0f);

/**
  * @brief  ��ص�ѹ��ȡ
  * @param  ��
  * @retval ��ص�ѹ
  */
static float BattVoltageUpdate()
{
    CTRL.Battery.Voltage = dfBattVoltage.getNext((BattSensor_ADC() / float(ADC_MaxValue)) * 3.3f * 2.0f);
    __LimitValue(CTRL.Battery.Voltage, BattEmptyVoltage, BattFullVoltage);
    return (CTRL.Battery.Voltage);
}

/**
  * @brief  ��ص�����ȡ
  * @param  ��
  * @retval ��
  */
static void BattUsageUpdate()
{
    /*���˲���ȡ���˲���ĵ�ѹֵȻ��ӳ��Ϊ0~100%�ĵ���*/
    CTRL.Battery.Usage = dfBattUsage.getNext(__Map(BattVoltageUpdate(), BattEmptyVoltage, BattFullVoltage, 0.0f, 100.0f));
}

/*�޲���ʱ�䳬ʱ*/
const uint32_t IdleWarnTimeoutMs = IdleWarnTimeout * 1000;

/**
  * @brief  ��ʱ���޲������
  * @param  ��
  * @retval ��
  */
static void IdleWarnMonitor()
{
    if(!CTRL.State.IdleWarn)
        return;

    static uint32_t lastOperateTime = 0;

    /*ҡ���Ƿ񱻲���*/
    if(ABS(CTRL.JS_L.X.Val) + ABS(CTRL.JS_L.Y.Val) + ABS(CTRL.JS_R.X.Val) + ABS(CTRL.JS_R.Y.Val) > 400)
    {
        lastOperateTime = millis();
    }

    /*�����Ƿ񱻲���*/
    if(btUP || btDOWN || btOK || btBACK)
    {
        lastOperateTime = millis();
    }

    if(millis() - lastOperateTime > IdleWarnTimeoutMs)
    {
        BuzzMusic(MC_Type::MC_NoOperationWarning);
    }
}

/**
  * @brief  ��������ʼ��
  * @param  ��
  * @retval ��
  */
void Init_Sensors()
{
    DEBUG_FUNC();
    pinMode(BattCharge_Pin, INPUT_PULLUP);

    /*ADC���ų�ʼ��*/
    pinMode(JSL_Y_Pin, INPUT_ANALOG_DMA);
    pinMode(JSR_X_Pin, INPUT_ANALOG_DMA);
    pinMode(BattSensor_Pin, INPUT_ANALOG_DMA);
    ADC_DMA_Register(ADC_Channel_TempSensor);
    ADC_DMA_Init();

    Init_BottonEvent();     //��ʼ�������¼�
    Init_EncoderEvent();    //��ʼ����ת�������¼�
    JoystickInit();
}

/**
  * @brief  ��ص�����ȡ
  * @param  ��
  * @retval ��
  */
void Task_SensorUpdate()
{
    JoystickUpdate();       //��ȡҡ��
    ButtonEventMonitor();   //�����¼�����
#ifdef USE_Encoder
    EncoderMonitor();       //��ת�������¼�����
#endif
    BattUsageUpdate();       //���µ��״̬
    __IntervalExecute(IdleWarnMonitor(), 5000);//��ʱ���޲������
}
