#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "ComPrivate.h"
#include "math.h"

using namespace RCX;

/*ѡ����ʼ����*/
#define ItemStartY (StatusBar_Height+8)
#define ItemStartX 14

#define TextHeight 16

/*��ѡ�е�ѡ��*/
static int16_t ItemSelect = 0;

/*ѡ����±�־λ*/
static bool ItemSelectUpdating = true;

/*��ѡѡ��������*/
static uint8_t ItemSelect_MAX = 0;

/*ǿ���������ֹ���*/
static volatile bool ExitHandshake = false;

/*�Ƿ��а����¼�����*/
static volatile bool HaveButtonEvent = false;

/**
  * @brief  ����ѡ���ַ���
  * @param  ��
  * @retval ��
  */
static void UpdateItemStr()
{
    for(uint8_t i = 0; i < 4; i++)
    {
        if(!Handshake::GetSlave(i)->BroadcastHead)continue;

        if(i == ItemSelect)
        {
            screen.setTextColor(screen.Yellow, screen.Black);
        }
        else
        {
            screen.setTextColor(screen.White, screen.Black);
        }

        /*��ʾ�ӻ�ID���ı�����*/
        screen.setCursor(ItemStartX, ItemStartY + i * TextHeight);
        screen.printfX("ID:0x%x", Handshake::GetSlave(i)->ID);
        screen.printfX(" %s", Handshake::GetSlave(i)->Description);
    }
}

/*ӳ��*/
static float LinearMap(float value, float oriMin, float oriMax, float nmin, float nmax)
{
    float k = 0;
    if (oriMax != oriMin)
    {
        k = (value - oriMin) / (oriMax - oriMin);
    }
    return nmin + k * (nmax - nmin);
}

static float PowMap(float value, float omin, float omax, float mmin, float mmax, float stK, float edK)
{
    if (stK == edK)
    {
        return LinearMap(value, omin, omax, mmin, mmax);
    }
    else
    {
        float stX = (stK);
        float edX = (edK);
        float stY = exp(stX);
        float edY = exp(edX);
        float x = LinearMap(value, omin, omax, stX, edX);
        return LinearMap(exp(x), stY, edY, mmin, mmax);
    }
}


/*ʵ�������ؼ�����*/
static LightGUI::Cursor<SCREEN_CLASS> ProgressCursor_1(&screen, 0, screen.height() - 20, 4, 4);
static LightGUI::Cursor<SCREEN_CLASS> ProgressCursor_2(&screen, 0, screen.height() - 20, 4, 4);
static LightGUI::Cursor<SCREEN_CLASS> ProgressCursor_3(&screen, 0, screen.height() - 20, 4, 4);
static LightGUI::Cursor<SCREEN_CLASS> ProgressCursor_4(&screen, 0, screen.height() - 20, 4, 4);
static LightGUI::Cursor<SCREEN_CLASS> ProgressCursor_5(&screen, 0, screen.height() - 20, 4, 4);

static float Animation_ProgressSlide(float from, float to, uint32_t ms, uint16_t T, uint16_t beginTime = 0)
{
    const uint16_t a1width = 700;
    const uint16_t a1begin = 500 - a1width / 2;
    const uint16_t a1end = 500 + a1width / 2;
    const float stK = 2.5f;
    const float edK = 1.2f;
    ms = ms % (T + beginTime);
    if (ms > beginTime)
    {
        ms -= beginTime;
    }
    else
    {
        return -99;
    }
    uint16_t pos = LinearMap(ms, 0, T, 0, 1000);
    float value = 0;
    if (pos < a1begin)
    {
        value = PowMap(pos, 0, a1begin, from, from + 0.3f * (to - from), stK, edK);
    }
    else if (pos < a1end)
    {
        value = LinearMap(pos - a1begin, 0, a1width, from + 0.3f * (to - from), from + 0.7f * (to - from));
    }
    else
    {
        value = PowMap(pos - a1end, 0, a1begin, from + 0.7f * (to - from), to, edK, stK);
    }
    return value;
}

static void SearchingAnimation(uint32_t ms)
{
    uint16_t bgtime = 200;
    float pos1 = Animation_ProgressSlide(0, screen.width(), ms, 2000, 2000);
    float pos2 = Animation_ProgressSlide(0, screen.width(), ms + bgtime * 1, 2000, 2000);
    float pos3 = Animation_ProgressSlide(0, screen.width(), ms + bgtime * 2, 2000, 2000);
    float pos4 = Animation_ProgressSlide(0, screen.width(), ms + bgtime * 3, 2000, 2000);
    float pos5 = Animation_ProgressSlide(0, screen.width(), ms + bgtime * 4, 2000, 2000);

    screen.fillRect(0, ProgressCursor_1.Y, 1, 4, screen.Black);

    ProgressCursor_1.setPosition(pos1, ProgressCursor_1.Y);
    ProgressCursor_2.setPosition(pos2, ProgressCursor_2.Y);
    ProgressCursor_3.setPosition(pos3, ProgressCursor_3.Y);
    ProgressCursor_4.setPosition(pos4, ProgressCursor_4.Y);
    ProgressCursor_5.setPosition(pos5, ProgressCursor_5.Y);
}

/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    ClearPage();
    Com_SetRFEnable(false);//ң�عر�
    ExitHandshake = false;
    ItemSelectUpdating = true;
    HaveButtonEvent = false;
    ItemSelect_MAX = 0;
    ItemSelect = 0;
    
    /*���û�����Ϣ*/
    Handshake::Pack_t* master = Handshake::GetMaster();
    master->EnableFunction.Passback = CTRL.State.PassBack;
    master->EnableFunction.FHSS = CTRL.State.FHSS;
    master->Speed = NRF_Cfg.Speed;

    /*���ֳ�ʼ��*/
    Handshake::Init(&nrfTRM, &nrfFHSS, _X_CTRL_NAME);
    
    /*����׼������*/
    Handshake::Process(Handshake::State_Prepare);

    /*ʵ�����������ؼ�*/
    LightGUI::ProgressBar<SCREEN_CLASS> SearchProgress(&screen, 0, screen.height() - 20, screen.width(), 10, 0);
    SearchProgress.Color_FM = screen.Black;

    screen.setCursor(ItemStartX, ItemStartY);
    screen.setTextColor(screen.White, screen.Black);
    screen.printfX("Searching...");
    XFS_Speak("��������");

    /*��ʱ����5000ms*/
    uint32_t time = millis();
    while(millis() - time < 12000)
    {
        /*��ȡ�ӻ��б�����*/
        ItemSelect_MAX = Handshake::Process(Handshake::State_Search);

        if(ItemSelect_MAX)
        {
            screen.setCursor(ItemStartX, ItemStartY + TextHeight);
            screen.setTextColor(screen.White, screen.Black);
            screen.printfX("Find %d Slave...", ItemSelect_MAX);
        }

        SearchingAnimation(millis() - time);

        PageDelay(1);

        /*�Ƿ�ǿ���������ֹ���*/
        if(HaveButtonEvent) break;
    }

    /*��ʾ�������*/
    screen.setCursor(ItemStartX, ItemStartY + 2 * TextHeight);
    if(ItemSelect_MAX > 0)
    {
        screen.setTextColor(screen.Green, screen.Black);
        screen.printfX("Search Done!");
        XFS_Speak("�������,�ҵ� %d ���豸", ItemSelect_MAX);
    }
    else
    {
        screen.setTextColor(screen.Red, screen.Black);
        screen.printfX("Not Found!");
        XFS_Speak("δ�ҵ��κ��豸");
        ExitHandshake = true;
    }
    PageDelay(200);

    ClearPage();
}

/**
  * @brief  ҳ��ѭ���¼�
  * @param  ��
  * @retval ��
  */
static void Loop()
{
    if(ExitHandshake)
        page.PagePop();

    if(ItemSelectUpdating)
    {
        UpdateItemStr();
        ItemSelectUpdating = false;
    }
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
    ClearPage();

    if(ExitHandshake)
    {
        nrf.SetRF_Enable(false);
#ifdef TIM_Handshake
        TIM_Cmd(TIM_Handshake, DISABLE);
#endif
        return;
    }

    /*�������Ӵӻ�*/
    screen.setTextColor(screen.White, screen.Black);
    screen.setCursor(ItemStartX, ItemStartY);
    screen.printfX(Handshake::GetSlave(ItemSelect)->Description);
    screen.setCursor(ItemStartX, ItemStartY + TextHeight);
    screen.printfX("Connecting...");
    XFS_Speak("���ڳ������ӵ�%s", Handshake::GetSlave(ItemSelect)->Description);
    /*��ʱ����*/
    uint32_t timeout = millis();
    bool IsTimeout = false;
    /*�ȴ��ӻ���Ӧ�����ź�*/
    while(Handshake::Process(Handshake::State_ReqConnect, ItemSelect, Handshake::CMD_AttachConnect) != Handshake::CMD_AgreeConnect)
    {
        /*2500ms��ʱ*/
        if(millis() - timeout > 2500)
        {
            screen.setTextColor(screen.Red, screen.Black);
            screen.setCursor(ItemStartX, ItemStartY + TextHeight);
            screen.printfX("Timeout");
            XFS_Speak("���ӳ�ʱ");
            IsTimeout = true;
            break;
        }
        PageDelay(1);
    }

    /*������β���ã���ת��Լ���õ�����Ƶ���Լ���ַ*/
    Handshake::Process(Handshake::State_Connected);

    /*���δ��ʱ��ʾ���ֳɹ�*/
    if(!IsTimeout)
    {
        screen.setTextColor(screen.Green, screen.Black);
        screen.setCursor(ItemStartX, ItemStartY + TextHeight);
        screen.printfX("Success");
        XFS_Speak("���ӳɹ�");
    }

    ClearPage();
    nrf.SetRF_Enable(false);
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
        HaveButtonEvent = true;

        if(btOK || btEcd)
        {
            page.PagePush(PAGE_CtrlInfo);
        }
        if(btBACK)
        {
            ExitHandshake = true;
        }

        if(btDOWN)
        {
            ItemSelect = (ItemSelect + 1) % ItemSelect_MAX;
            ItemSelectUpdating = true;
        }
        if(btUP)
        {
            ItemSelect = (ItemSelect + ItemSelect_MAX - 1) % ItemSelect_MAX;
            ItemSelectUpdating = true;
        }
    }
}

/**
  * @brief  ����ҳ��ע��
  * @param  pageID:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_Handshake(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
