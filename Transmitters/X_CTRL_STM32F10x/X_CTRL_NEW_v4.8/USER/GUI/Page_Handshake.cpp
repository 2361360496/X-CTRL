#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

/*ѡ����ʼ����*/
#define ItemStartY (StatusBar_POS+8)
#define ItemStartX 14

/*��ѡ�е�ѡ��*/
static int16_t ItemSelect = 0;

/*ѡ����±�־λ*/
static bool ItemSelectUpdating = true;

/*��ѡѡ��������*/
static uint8_t ItemSelect_MAX = 0;

/*ǿ���������ֹ���*/
static bool ExitHandshake = false;

/*�Ƿ��а����¼�����*/
static bool HaveButtonEvent = false;

/**
  * @brief  ����ѡ���ַ���
  * @param  ��
  * @retval ��
  */
static void UpdateItemStr()
{
    for(uint8_t i = 0; i < 4; i++)
    {
        if(!HandshakePack_Slave[i].HeadCode)continue;

        if(i == ItemSelect)
        {
            screen.setTextColor(screen.Yellow, screen.Black);
        }
        else
        {
            screen.setTextColor(screen.White, screen.Black);
        }
        
        /*��ʾ�ӻ�ID���ı�����*/
        screen.setCursor(ItemStartX, ItemStartY + i * 10);
        screen.printf("ID:0x%x", HandshakePack_Slave[i].ID);
        screen.printf(" %s", HandshakePack_Slave[i].Description);
    }
}

/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    ClearPage();
    State_RF = OFF;//ң�عر�
    ExitHandshake = false;
    ItemSelectUpdating = true;
    HaveButtonEvent = false;
    ItemSelect_MAX = 0;
    ItemSelect = 0;
    
    /*����׼������*/
    HandshakeRun(HandshakeState_Prepare);

    /*ʵ�����������ؼ�*/
    LightGUI::ProgressBar<SCREEN_CLASS> SearchProgress(&screen, 0, screen.height() - 6, screen.width(), 6, 0);
	SearchProgress.Color_FM = screen.Black;
	
    screen.setCursor(ItemStartX, ItemStartY);
    screen.setTextColor(screen.White, screen.Black);
    screen.print("Searching...");
    XFS_Speak("��������");

//    /*���������ϻ��붯��*/
//    for(uint8_t y = screen.height(); y > screen.height() - 10; y--)
//    {
//        SearchScroll.setPosition(SearchScroll.X, y);
//        PageDelay(10);
//    }

    /*��ʱ����5000ms*/
    uint32_t time = millis();
    uint8_t ItemSelect_MAX_Last = 0;
    while(millis() - time < 5000)
    {
        /*��ȡ�ӻ��б�����*/
        ItemSelect_MAX = HandshakeRun(HandshakeState_Search);
        if(ItemSelect_MAX != ItemSelect_MAX_Last)
        {
            screen.setCursor(ItemStartX, ItemStartY + 10);
            screen.setTextColor(screen.White, screen.Black);
            screen.printf("Find %d Slave...", ItemSelect_MAX);
            ItemSelect_MAX_Last = ItemSelect_MAX;
        }

        /*��������*/
//        SearchScroll.setScroll((millis() % 400) / 400.0);
		if(millis() / 400 % 2)
		{
			SearchProgress.Color_BG = screen.Black;
			SearchProgress.Color_PB = screen.White;
		}
		else
		{
			SearchProgress.Color_PB = screen.Black;
			SearchProgress.Color_BG = screen.White;
		}
        SearchProgress.setProgress((millis() % 400) / 400.0);
        PageDelay(1);

        /*�Ƿ�ǿ���������ֹ���*/
        if(HaveButtonEvent) break;
    }

    /*��ʾ�������*/
    screen.setCursor(ItemStartX, ItemStartY + 20);
    if(ItemSelect_MAX > 0)
    {
        screen.setTextColor(screen.Green, screen.Black);
        screen.print("Search Done!");
        XFS_Speak("�������,�ҵ� %d ���豸", ItemSelect_MAX);
    }
    else
    {
        screen.setTextColor(screen.Red, screen.Black);
        screen.print("Not Found!");
        XFS_Speak("δ�ҵ��κ��豸");
        ExitHandshake = true;
    }
    
    /*���������»�������*/
    for(uint8_t y = SearchProgress.Y; y <= screen.height(); y++)
    {
        SearchProgress.setPosition(SearchProgress.X, y);
        PageDelay(10);
    }

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
        page.PageChangeTo(PAGE_MainMenu);
    
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
#ifdef TIM_Handshake
        TIM_Cmd(TIM_Handshake, DISABLE);
#endif
        return;
    }

    /*�������Ӵӻ�*/
    screen.setTextColor(screen.White, screen.Black);
    screen.setCursor(ItemStartX, ItemStartY);
    screen.print(HandshakePack_Slave[ItemSelect].Description);
    screen.setCursor(ItemStartX, ItemStartY + 10);
    screen.print("Connecting...");
    XFS_Speak("���ڳ������ӵ�%s", HandshakePack_Slave[ItemSelect].Description);
    /*��ʱ����*/
    uint32_t timeout = millis();
    bool IsTimeout = false;
    /*�ȴ��ӻ���Ӧ�����ź�*/
    while(HandshakeRun(HandshakeState_ReqConnect, ItemSelect, CMD_AttachConnect) != CMD_AgreeConnect)
    {
        /*2500ms��ʱ*/
        if(millis() - timeout > 2500)
        {
            screen.setTextColor(screen.Red, screen.Black);
            screen.setCursor(ItemStartX, ItemStartY + 10);
            screen.printf("Timeout");
            XFS_Speak("���ӳ�ʱ");
            IsTimeout = true;
            break;
        }
        PageDelay(1);
    }
    
    /*������β���ã���ת��Լ���õ�����Ƶ���Լ���ַ*/
    HandshakeRun(HandshakeState_Connected);

    /*��Ӧ�ӻ�����*/
    CTRL.Info.CtrlObject = HandshakePack_Slave[ItemSelect].CtrlType;

    /*���δ��ʱ��ʾ���ֳɹ�*/
    if(!IsTimeout)
    {
        screen.setTextColor(screen.Green, screen.Black);
        screen.setCursor(ItemStartX, ItemStartY + 10);
        screen.printf("Success");
        XFS_Speak("���ӳɹ�");
    }

    ClearPage();
}

/**
  * @brief  ���������¼�
  * @param  ��
  * @retval ��
  */
static void ButtonPressEvent()
{
    if(btOK || btEcd)
    {
        page.PageChangeTo(PAGE_CtrlInfo);
    }
    if(btBACK)
    {
        page.PageChangeTo(PAGE_MainMenu);
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

    HaveButtonEvent = true;
}

/**
  * @brief  ��ת�������¼�
  * @param  ��
  * @retval ��
  */
static void EncoderEvent()
{
    if(ecd > 0)
    {
        ItemSelect = (ItemSelect + 1) % ItemSelect_MAX;
        ItemSelectUpdating = true;
    }
    if(ecd < 0)
    {
        ItemSelect = (ItemSelect + ItemSelect_MAX - 1) % ItemSelect_MAX;
        ItemSelectUpdating = true;
    }
}

/**
  * @brief  ����ҳ��ע��
  * @param  ThisPage:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_Handshake(uint8_t ThisPage)
{
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
//    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_EncoderRotate, EncoderEvent);
}
