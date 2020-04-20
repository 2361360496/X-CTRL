#include "Basic/FileGroup.h"
#include "Communication/ComPrivate.h"
#include "math.h"

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
static void Joystick_Map(Joystick_TypeDef* js, int16_t adc_x, int16_t adc_y)
{
    __LimitValue(adc_x, js->X.Min, js->X.Max);
    __LimitValue(adc_y, js->Y.Min, js->Y.Max);

    js->X.Val = constrain(
                    ((adc_x - js->X.Mid) >= 0) ?
                    NonlinearMap(adc_x, js->X.Mid, js->X.Max, 0, RCX_CHANNEL_DATA_MAX, js->X.Curve.Start, js->X.Curve.End) :
                    NonlinearMap(adc_x, js->X.Mid, js->X.Min, 0, -RCX_CHANNEL_DATA_MAX, js->X.Curve.Start, js->X.Curve.End),
                    -RCX_CHANNEL_DATA_MAX,
                    RCX_CHANNEL_DATA_MAX
                );
    js->Y.Val = constrain(
                    ((adc_y - js->Y.Mid) >= 0) ?
                    NonlinearMap(adc_y, js->Y.Mid, js->Y.Max, 0, RCX_CHANNEL_DATA_MAX, js->Y.Curve.Start, js->Y.Curve.End) :
                    NonlinearMap(adc_y, js->Y.Mid, js->Y.Min, 0, -RCX_CHANNEL_DATA_MAX, js->Y.Curve.Start, js->Y.Curve.End),
                    -RCX_CHANNEL_DATA_MAX,
                    RCX_CHANNEL_DATA_MAX
                );
}

void Joystick_Init()
{
    DEBUG_FUNC_LOG();
    
    pinMode(JSL_X_Pin, INPUT_ANALOG_DMA);
    pinMode(JSL_Y_Pin, INPUT_ANALOG_DMA);
    pinMode(JSR_X_Pin, INPUT_ANALOG_DMA);
    pinMode(JSR_Y_Pin, INPUT_ANALOG_DMA);
    ADC_DMA_Init();
    
    CTRL.JS_L.X.Min = 0;
    CTRL.JS_L.X.Mid = JS_ADC_MAX / 2;
    CTRL.JS_L.X.Max = JS_ADC_MAX;
    CTRL.JS_L.X.Curve.Start = CTRL.JS_L.X.Curve.End = 5.0f;

    CTRL.JS_L.Y.Min = 0;
    CTRL.JS_L.Y.Mid = JS_ADC_MAX / 2;
    CTRL.JS_L.Y.Max = JS_ADC_MAX;
    CTRL.JS_L.Y.Curve.Start = CTRL.JS_L.Y.Curve.End = 5.0f;

    CTRL.JS_R.X.Min = 0;
    CTRL.JS_R.X.Mid = JS_ADC_MAX / 2;
    CTRL.JS_R.X.Max = JS_ADC_MAX;
    CTRL.JS_R.X.Curve.Start = CTRL.JS_R.X.Curve.End = 5.0f;

    CTRL.JS_R.Y.Min = 0;
    CTRL.JS_R.Y.Mid = JS_ADC_MAX / 2;
    CTRL.JS_R.Y.Max = JS_ADC_MAX;
    CTRL.JS_R.Y.Curve.Start = CTRL.JS_R.Y.Curve.End = 5.0f;
    
    CTRL.KnobLimit.L = RCX_CHANNEL_DATA_MAX;
    CTRL.KnobLimit.R = RCX_CHANNEL_DATA_MAX;
}

/**
  * @brief  ҡ�˶�ȡ
  * @param  ��
  * @retval ��
  */
void Joystick_Update()
{
    Joystick_Map(&CTRL.JS_L, JSL_X_ADC(), JSL_Y_ADC());
    Joystick_Map(&CTRL.JS_R, JSR_X_ADC(), JSR_Y_ADC());
}
