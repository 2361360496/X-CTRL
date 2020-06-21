#include "Filegroup.h"
#include "DisplayPrivate.h"
#include "doom_nano.h"

/********** ���� ************/
/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    doom_setup();
}

/**
  * @brief  ҳ��ѭ���¼�
  * @param  ��
  * @retval ��
  */
static void Loop()
{
    doom_loop();
    if(btBACK)
        page.PagePop();
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
}

/**
  * @brief  ҳ���¼�
  * @param  ��
  * @retval ��
  */
static void Event(int event, void* param)
{
}

/**
  * @brief  ҳ��ע��
  * @param  pageID:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_GameDoom(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
