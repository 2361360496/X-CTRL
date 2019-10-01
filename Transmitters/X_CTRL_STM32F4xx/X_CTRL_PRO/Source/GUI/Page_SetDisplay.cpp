#include "FileGroup.h"
#include "DisplayPrivate.h"

static lv_obj_t * sliderBright;
static lv_obj_t * labelBright;

#define getBright() timer_get_compare(PIN_MAP[TFT_LED_Pin].TIMx, PIN_MAP[TFT_LED_Pin].TimerChannel)

static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        int value = lv_slider_get_value(obj);
        lv_label_set_text_format(labelBright, "%d%%", value / 10);
        analogWrite(TFT_LED_Pin, value);
    }
}

static void Creat_Slider(lv_obj_t** slider)
{
//    /*Create styles*/
//    static lv_style_t style_bg;
//    static lv_style_t style_indic;
//    static lv_style_t style_knob;
//    lv_style_copy(&style_bg, &lv_style_pretty);
//    style_bg.body.main_color =  LV_COLOR_BLACK;
//    style_bg.body.grad_color =  LV_COLOR_GRAY;
//    style_bg.body.radius = LV_RADIUS_CIRCLE;
//    style_bg.body.border.color = LV_COLOR_WHITE;

//    lv_style_copy(&style_indic, &lv_style_pretty_color);
//    style_indic.body.radius = LV_RADIUS_CIRCLE;
//    style_indic.body.shadow.width = 8;
//    style_indic.body.shadow.color = style_indic.body.main_color;
//    style_indic.body.padding.left = 3;
//    style_indic.body.padding.right = 3;
//    style_indic.body.padding.top = 3;
//    style_indic.body.padding.bottom = 3;

//    lv_style_copy(&style_knob, &lv_style_pretty);
//    style_knob.body.radius = LV_RADIUS_CIRCLE;
//    style_knob.body.opa = LV_OPA_70;
//    style_knob.body.padding.top = 10 ;
//    style_knob.body.padding.bottom = 10 ;

    /*Create a slider*/
    *slider = lv_slider_create(lv_scr_act(), NULL);

//    lv_slider_set_style(*slider, LV_SLIDER_STYLE_BG, &style_bg);
//    lv_slider_set_style(*slider, LV_SLIDER_STYLE_INDIC, &style_indic);
//    lv_slider_set_style(*slider, LV_SLIDER_STYLE_KNOB, &style_knob);
    lv_slider_set_range(*slider, 5, 1000);
    lv_slider_set_value(*slider, getBright(), LV_ANIM_ON);
    
    lv_obj_set_size(*slider, page_width - 30, 20);
    lv_obj_align(*slider, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(*slider, event_handler);
}

static void Creat_Label(lv_obj_t** label)
{
//    static lv_style_t style_label;
//    lv_style_copy(&style_label, &lv_style_plain);
//    style_label.text.color = LV_COLOR_BLACK;
    
    *label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text_format(*label, "%d%%", getBright() / 10);
//    lv_label_set_style(*label, LV_LABEL_STYLE_MAIN, &style_label);
    lv_obj_align(*label, sliderBright, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
}

/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    Creat_Slider(&sliderBright);
    Creat_Label(&labelBright);
}

/**
  * @brief  ҳ��ѭ���¼�
  * @param  ��
  * @retval ��
  */
static void Loop()
{
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
    lv_obj_del_safe(&sliderBright);
    lv_obj_del_safe(&labelBright);
}

/**
  * @brief  ҳ���¼�
  * @param  event:�¼����
  * @param  param:�¼�����
  * @retval ��
  */
static void Event(int event, void* param)
{
    lv_obj_t * btn = (lv_obj_t*)param;
    if(event == LV_EVENT_CLICKED)
    {
        if(btn == btnBack)
        {
            page.PageChangeTo(page.LastPage);
        }
    }
}

/**
  * @brief  ҳ��ע��
  * @param  pageID:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_SetDisplay(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
