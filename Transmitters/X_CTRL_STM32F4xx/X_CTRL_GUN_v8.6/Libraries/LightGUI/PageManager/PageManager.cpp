#include "PageManager.h"

#define IS_PAGE(page)   (page<MaxPage)
#define IS_EVENT(event) (event<MaxEvent)

/**
  * @brief  ��ʼ��ҳ�������
  * @param  pageMax: ҳ���������
  * @param  eventMax: �¼��������
  * @retval ��
  */
PageManager::PageManager(uint8_t pageMax, uint8_t eventMax)
{
    MaxPage = pageMax;
    MaxEvent = eventMax;
    NowPage = 0;
    LastPage = 0xFF;

    /* �����ڴ棬����б� */
    PageList = new PageList_TypeDef[MaxPage];
    for(uint8_t page = 0; page < MaxPage; page++)
    {
        PageList[page].EventList = new CallbackFunction_t[MaxEvent];
        PageClear(page);
    }
}

/**
  * @brief  ���һ��ҳ��
  * @param  pageID: ҳ����
  * @retval true:�ɹ� false:ʧ��
  */
bool PageManager::PageClear(uint8_t pageID)
{
    if(!IS_PAGE(pageID))
        return false;

    PageList[pageID].SetupCallback = 0;
    PageList[pageID].LoopCallback = 0;
    PageList[pageID].ExitCallback = 0;
    for(uint8_t e = 0; e < MaxEvent; e++)
    {
        PageList[pageID].EventList[e] = 0;
    }
    return true;
}

/**
  * @brief  ע��һ������ҳ�棬����һ����ʼ��������ѭ���������˳�����
  * @param  pageID: ҳ����
  * @param  setupCallback: ��ʼ�������ص�
  * @param  loopCallback: ѭ�������ص�
  * @param  exitCallback: �˳������ص�
  * @retval true:�ɹ� false:ʧ��
  */
bool PageManager::PageRegister_Basic(uint8_t pageID,
                                     CallbackFunction_t setupCallback,
                                     CallbackFunction_t loopCallback,
                                     CallbackFunction_t exitCallback)
{
    if(!IS_PAGE(pageID))
        return false;

    PageList[pageID].SetupCallback = setupCallback;
    PageList[pageID].LoopCallback = loopCallback;
    PageList[pageID].ExitCallback = exitCallback;
    return true;
}

/**
  * @brief  ��һ��ҳ��ע��һ���¼�
  * @param  pageID: ҳ����
  * @param  eventID: �¼����
  * @param  eventCallback: �¼������ص�
  * @retval true:�ɹ� false:ʧ��
  */
bool PageManager::PageRegister_Event(uint8_t pageID, uint8_t eventID, CallbackFunction_t eventCallback)
{
    if(!IS_PAGE(pageID))
        return false;

    if(!IS_EVENT(eventID))
        return false;

    PageList[pageID].EventList[eventID] = eventCallback;
    return true;
}

/**
  * @brief  ҳ���¼�����
  * @param  eventID: �¼����
  * @retval ��
  */
void PageManager::PageEventTransmit(uint8_t eventID)
{
    if(PageList[NowPage].EventList[eventID])
        PageList[NowPage].EventList[eventID]();
}

/**
  * @brief  �л���ָ��ҳ��
  * @param  pageID: ҳ����
  * @retval ��
  */
void PageManager::PageChangeTo(uint8_t pageID)
{
    if(!IsPageBusy)
    {
        if(IS_PAGE(pageID))
            NowPage = pageID;

        IsPageBusy = true;
    }
}

/**
  * @brief  ҳ�������״̬��
  * @param  ��
  * @retval ��
  */
void PageManager::Running()
{
    if(NowPage != LastPage)
    {
        IsPageBusy = true;
        NextPage = NowPage;

        if(PageList[LastPage].ExitCallback && IS_PAGE(LastPage))
            PageList[LastPage].ExitCallback();

        if(PageList[NowPage].SetupCallback && IS_PAGE(NowPage))
            PageList[NowPage].SetupCallback();

        LastPage = NowPage;
    }
    else
    {
        IsPageBusy = false;
        if(PageList[NowPage].LoopCallback && IS_PAGE(NowPage))
            PageList[NowPage].LoopCallback();
    }
}
