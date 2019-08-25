#include "FileGroup.h"
#include "DigitalFilter.h"
#include "ComPrivate.h"
#include "LuaGroup.h"
#include "math.h"

/*��ҡ��*/
Joystick_t JS_L;

/*��ҡ��*/
Joystick_t JS_R;

/*��������*/
double JS_L_SlopeStart = 5.0f;
double JS_L_SlopeEnd  = 5.0f;

double JS_R_SlopeStart = 5.0f;
double JS_R_SlopeEnd  = 5.0f;

float CPU_Temperature = 0.0f;

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
  * @param  &js: ҡ�˶�������
  * @param  adc_x: X��ADCֵ
  * @param  adc_y: Y��ADCֵ
  * @retval ��
  */
static void JoystickMap(Joystick_t &js, int16_t adc_x, int16_t adc_y, double slopeStart, double slopeEnd)
{
    __LimitValue(adc_x, js.Xmin, js.Xmax);
    __LimitValue(adc_y, js.Ymin, js.Ymax);

    js.X = constrain(
               ((adc_x - js.Xmid) >= 0) ?
               NonlinearMap(adc_x, js.Xmid, js.Xmax, 0, CtrlOutput_MaxValue, slopeStart, slopeEnd) :
               NonlinearMap(adc_x, js.Xmid, js.Xmin, 0, -CtrlOutput_MaxValue, slopeStart, slopeEnd),
               -CtrlOutput_MaxValue,
               CtrlOutput_MaxValue
           );
    js.Y = constrain(
               ((adc_y - js.Ymid) >= 0) ?
               NonlinearMap(adc_y, js.Ymid, js.Ymax, 0, CtrlOutput_MaxValue, slopeStart, slopeEnd) :
               NonlinearMap(adc_y, js.Ymid, js.Ymin, 0, -CtrlOutput_MaxValue, slopeStart, slopeEnd),
               -CtrlOutput_MaxValue,
               CtrlOutput_MaxValue
           );
}

/**
  * @brief  ҡ�˶�ȡ
  * @param  ��
  * @retval ��
  */
static void Read_Joystick()
{
    JoystickMap(JS_L, JSL_X_ADC(), JSL_Y_ADC(), JS_L_SlopeStart, JS_L_SlopeEnd);
    JoystickMap(JS_R, JSR_X_ADC(), JSR_Y_ADC(), JS_R_SlopeStart, JS_R_SlopeEnd);
    if(LuaOccupy)
    {

    }
    else if(State_MPU)//�Ƿ�����������Ӧ
    {
        /*Page_SetGravity.cpp*/
        /*��MPU���������������ҡ��ֵ*/
        extern int16_t *from_MPU_LX, *from_MPU_LY, *from_MPU_RX, *from_MPU_RY;
        if(from_MPU_LX) CTRL.Left.X = *from_MPU_LX;
        if(from_MPU_LY) CTRL.Left.Y = *from_MPU_LY;
        if(from_MPU_RX) CTRL.Right.X = *from_MPU_RX;
        if(from_MPU_RY) CTRL.Right.Y = *from_MPU_RY;
    }
    else
    {
        CTRL.Left.X = JS_L.X;
        CTRL.Left.Y = JS_L.Y;
        CTRL.Right.X = JS_R.X;
        CTRL.Right.Y = JS_R.Y;
    }

    if(IS_EnableCtrl())//�Ƿ�����������
    {
        CTRL.KnobLimit.L = DR_TH_Value;//map(ADL_ADC(), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);//���޷���ť��ȡ
        CTRL.KnobLimit.R = DR_ST_Value;//map(ADR_ADC(), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);//���޷���ť��ȡ
    }
    else
    {
        /*�޷���0*/
        CTRL.KnobLimit.L = 0;
        CTRL.KnobLimit.R = 0;
    }
}

/*��ص�ѹ����ص���*/
float BattVoltage, BattUsage;

/*��ص�ѹ��ֵ�˲���*/
static FilterAverage<float> dfBattVoltage(13);

/*��ص����ͻ��˲���*/
static FilterHysteresis<float> dfBattUsage(3.0f);

/*CPU�¶Ⱦ�ֵ�˲���*/
static FilterAverage<int> dfCPUTemp(13);

/**
  * @brief  ��ص�ѹ��ȡ
  * @param  ��
  * @retval ��ص�ѹ
  */
static float Read_BattVoltage()
{
    BattVoltage = dfBattVoltage.getNext((BattSensor_ADC() / float(ADC_MaxValue)) * 3.3f * 2.0f);
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
    BattUsage = dfBattUsage.getNext(__Map(Read_BattVoltage(), BattEmptyVoltage, BattFullVoltage, 0.0f, 100.0f));
}

/**
  * @brief  CPU�¶ȶ�ȡ
  * @param  ��
  * @retval ��
  */
static void Read_CPU_Temperature()
{
    CPU_Temperature = (dfCPUTemp.getNext(Temp_ADC()) * 3300.0f / 4095.0f - 760.0f) / 2.5f + 25;
}

/*��ʱ���޲������ʹ��*/
bool State_NoOperationMonitor = true;

/*�޲���ʱ�䳬ʱ*/
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
#ifdef USE_Encoder
    EncoderMonitor();       //��ת�������¼�����
#endif
    Read_BattUsage();       //���µ��״̬
    Read_CPU_Temperature(); //����CPU�¶�
    __IntervalExecute(NoOperationMonitor(), 5000);//��ʱ���޲������
}
