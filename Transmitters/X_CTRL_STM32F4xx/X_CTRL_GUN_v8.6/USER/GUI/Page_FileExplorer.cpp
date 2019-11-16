#include "FileGroup.h"
#include "FileSystem.h"
#include "GUI_Private.h"

/*ʵ������ǰҳ�������*/
static MillisTaskManager mtm_FileExplorer(2);

/*�ļ�������*/
int16_t FileCnt = 0;
/*�ļ�Ŀ¼*/
dir_t FileDir;

/*֧�ִ򿪵��ļ��б�*/
typedef struct{
    String ExtName;
    uint8_t PageID;
    String& LoaderPath;
}FileSupport_TypeDef;

static FileSupport_TypeDef FileSptGrp[] = {
    {".BV", PAGE_BvPlayer, BvFilePath},
    {".WAV", PAGE_WavPlayer, WavFilePath},
    {".LUA", PAGE_LuaScript, LuaFilePath}
};

/*�˵�������*/
MenuManager *FileMenu;

/*Ŀ¼����*/
#define ItemStartY (StatusBar_POS+4)
#define ItemStartX (TEXT_WIDTH_1+8)
#define ItemEndX   (screen.width()-35)

/*��ǰѡ����ʾ��λ��*/
static int16_t ItemDispalyPos;

/*ʵ�����������ؼ�����*/
/*ScrollBar(T *obj, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t dir, uint16_t l);*/
static LightGUI::ScrollBar<SCREEN_CLASS> Scroll(
    &screen,
    screen.width() + 5,
    StatusBar_POS + 4,
    4,
    0,
    1,
    0
);
    
/*ʵ�������ؼ�����*/
static LightGUI::Cursor<SCREEN_CLASS> ItemCursor(&screen, ItemStartX, screen.height(), 8, 8);

/**
  * @brief  ���ָ����ѡ������
  * @param  pos:ָ����ѡ������
  * @retval ��
  */
static void ClearItemPos(uint16_t pos)
{
    screen.fillRect(
        ItemStartX,
        ItemStartY + pos * TEXT_HEIGHT_2,
        Scroll.X - ItemStartX,
        TEXT_HEIGHT_2,
        screen.Black
    );
}

/**
  * @brief  ���¹��λ��
  * @param  ��
  * @retval ��
  */
static void UpadateCursor()
{
    ItemDispalyPos = FileMenu->GetItemDispalyPos();
}

/**
  * @brief  ����ѡ���
  * @param  ��
  * @retval ��
  */
static void UpdateItemTop()
{
    FileMenu->UpdateItemStrBuffer();
    Scroll.setScroll(1.0f - ((float)FileMenu->ItemTop / (float)(FileMenu->ItemNum_Max - FileMenu->ItemNumDisplay_Max)));
    screen.setTextColor(screen.White, screen.Black);
    for(int16_t i = 0; i < FileMenu->ItemNumDisplay_Max; i++)
    {
        ClearItemPos(i);
        screen.setCursor(ItemStartX, ItemStartY + i * TEXT_HEIGHT_2);
        screen.printfX(FileMenu->ItemStrBuffer[i]);
        
        int16_t nowItemNum = (FileMenu->ItemTop + i) % FileMenu->ItemNum_Max;
        if(FileMenu->ItemList[nowItemNum].Type == FileMenu->TYPE_PageJump)
        {
            screen.setCursor(ItemEndX + 6, ItemStartY + i * TEXT_HEIGHT_2);
            screen.printfX(">");
        }
    }
}

/**
  * @brief  ����ָ����ѡ��
  * @param  pos:��
  * @retval ��
  */
static void UpdateItemPos(uint8_t pos)
{
    FileMenu->UpdateItemStrBuffer();
    screen.setTextColor(screen.White, screen.Black);
    ClearItemPos(pos);
    screen.setCursor(ItemStartX, ItemStartY + pos * TEXT_HEIGHT_2);
    screen.printfX(FileMenu->ItemStrBuffer[pos]);
}

/**
  * @brief  ѡ�ѡ���¼�
  * @param  ��
  * @retval ��
  */
static void When_ItemSelect()
{
    String filePath = FileMenu->ItemList[FileMenu->ItemSelect].LabelText;
    /*�����б�*/
    for(int i = 0; i < __Sizeof(FileSptGrp); i++)
    {
        /*Ѱ���Ƿ��ж�Ӧ����չ��*/
        if(filePath.endsWith(FileSptGrp[i].ExtName))
        {
            /*���ļ�·����ֵ����Ӧ�ļ�����·��*/
            FileSptGrp[i].LoaderPath = filePath;
        }
    }
}

/**
  * @brief  �˵���������
  * @param  ��
  * @retval ��
  */
static void Task_MenuUpdate()
{
    if(FileMenu->ItemSelectUpdating)
    {
        UpadateCursor();
        FileMenu->ItemSelectUpdating = false;
    }

    if(FileMenu->ItemTopUpdating)
    {
        UpdateItemTop();
        FileMenu->ItemTopUpdating = false;
    }

    if(FileMenu->ItemXUpdating)
    {
        UpdateItemPos(ItemDispalyPos);
        FileMenu->ItemXUpdating = false;
    }
}

/*��ǰ���λ��*/
static float CursorPosNow = ItemStartY;

/*��һ�εĹ��λ��*/
static float CursorPosLast = ItemStartY + FileMenu->ItemNumDisplay_Max * 10;

/**
  * @brief  ���λ�ø�������
  * @param  ��
  * @retval ��
  */
static void Task_DrawCursorUpdate()
{
    CursorPosNow = ItemStartY + ItemDispalyPos * TEXT_HEIGHT_2 + 5;

    if((CursorPosNow - CursorPosLast) != 0)
    {
        CursorPosLast += (CursorPosNow - CursorPosLast) * 0.4f;
    }

    ItemCursor.setPosition(ItemStartX - 8, CursorPosLast);
}

/**
  * @brief  �Զ�����ص�����
  * @param  ��
  * @retval ��
  */
static void DrawCursor_Callback()
{
    screen.setTextColor(screen.Yellow, screen.Black);
    screen.setCursor(ItemCursor.X, ItemCursor.Y);
    screen.print('>');
}

static uint8_t GetFilePage(String filename)
{
    /*�����б�*/
    for(int i = 0; i < __Sizeof(FileSptGrp); i++)
    {
        /*Ѱ���Ƿ��ж�Ӧ����չ��*/
        if(filename.endsWith(FileSptGrp[i].ExtName))
        {
            /*����ҳ��ID*/
            return FileSptGrp[i].PageID;
        }
    }
    return 0;
}

static void LoadFileDir()
{
    /*��ȡ��ǰĿ¼���ļ�����*/
    FileCnt = 0;
    root.rewind();
    while(root.readDir(FileDir) != 0)
        FileCnt ++;

    /*�����˵�����*/
    static MenuManager menu(
        FileCnt,
        (screen.height() - StatusBar_POS) / TEXT_HEIGHT_2 - 1,
        &page
    );
    
    /*��ȡ�²˵������ַ*/
    FileMenu = &menu;
    /*�ر��Զ����*/
    FileMenu->ItemAutoSquence = false;
    /*����������Ӧ*/
    Scroll.Height = FileMenu->ItemNumDisplay_Max * TEXT_HEIGHT_2 + 2;
    Scroll.Long = FileMenu->ItemNumDisplay_Max * TEXT_HEIGHT_2 * (FileMenu->ItemNumDisplay_Max / (float)FileMenu->ItemNum_Max);

    /*�˵�ѡ��ѡ���¼���*/
    FileMenu->CallbackFunc_ItemSelectEvent = When_ItemSelect;
    /*�˵�ѡ��ע��*/
    root.rewind();
    for(int i = 0; i < FileCnt; i++)
    {
        /*�����ļ���Ϣ*/
        root.readDir(FileDir);
        
        /*��ȡ�ļ���*/
        char name[20];
        root.dirName(FileDir, name);
        
        /*�ɴ򿪵��ļ���չ���ж�*/
        String fileName = String(name);
        uint8_t PAGE_ID = 0;
        if((PAGE_ID = GetFilePage(fileName)) != 0)
        {
            FileMenu->UpdateItem(i, fileName, FileMenu->TYPE_PageJump, PAGE_ID);
        }
        else
        {
            FileMenu->UpdateItem(i, fileName);
        }
    }
}

/********** ���� ************/
/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    if(!State_SD_Enable)
    {
        page.PageChangeTo(PAGE_Options);
        return;
    }
    TextSetDefault();
    TextMidPrint(0.5f, 0.5f, "LOADIND...");
    LoadFileDir();
    
    CursorPosNow = ItemStartY;
    CursorPosLast = screen.height();
    Scroll.Color_FM = Scroll.Color_SB = screen.White;
    
    if(page.LastPage == PAGE_Options)
    {
        ItemDispalyPos = FileMenu->ItemSelect = FileMenu->ItemTop = 0;
        FileMenu->ItemXUpdating = FileMenu->ItemSelectUpdating = false;
    }
    FileMenu->ItemTopUpdating = true;
    FileMenu->UpdateItemStrBuffer();
    
    ClearPage();
    
    for(uint8_t i = 0; i < FileMenu->ItemNumDisplay_Max; i++)
    {
        for(int16_t j = -20; j < ItemStartX - 2; j++)
        {
            screen.setOpacityX(__Map(j, -20, ItemStartX - 3, 0, 255));
            screen.setTextColor(screen.White, screen.Black);
            screen.setCursor(j, ItemStartY + i * TEXT_HEIGHT_2);
            screen.printfX(" " + FileMenu->ItemStrBuffer[i]);
            screen.setOpacityX(255);
            PageDelay(1);
        }
        BuzzTone(1000 + 100 * i, 5);
        PageDelay(10);
    }

    Scroll.setScroll(1.0);
    for(uint8_t i = screen.width() + 5; i > screen.width() - 7; i--)
    {
        Scroll.setPosition(i, Scroll.Y);
        PageDelay(10);
    }
    
    /*ע���Զ�����ص�����*/
    ItemCursor.DisplayCallbackFunction = DrawCursor_Callback;

    /*ע������*/
    mtm_FileExplorer.TaskRegister(0, Task_MenuUpdate, 50);
    mtm_FileExplorer.TaskRegister(1, Task_DrawCursorUpdate, 20);
}

/**
  * @brief  ҳ��ѭ���¼�
  * @param  ��
  * @retval ��
  */
static void Loop()
{
    mtm_FileExplorer.Running(millis());
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
    /*���������󻬶�����*/
    for(float i = 1.0f; i > 0.0f; i -= ((i * 0.01f) + 0.001f))
    {
        Scroll.Color_FM = Scroll.Color_SB = StrPrinter_ColorMap(screen.Black, screen.White, (uint8_t)(i * 15));
        Scroll.setPosition(screen.width()*i, Scroll.Y);
        PageDelay(2);
    }
    ClearPage();
}

/************ �¼����� **********/
/**
  * @brief  ���������¼�
  * @param  ��
  * @retval ��
  */
static void ButtonPressEvent()
{
    if(btUP)
    {
        FileMenu->SelectMove(+1);//�˵��������
    }
    if(btDOWN)
    {
        FileMenu->SelectMove(-1);//�˵��������
    }

    if(btOK || btEcd)
    {
        FileMenu->ItemSelectEvent();//����ѡ�ѡ���¼�
    }
    
    if(btBACK)
    {
        page.PageChangeTo(PAGE_Options);
    }
}

/**
  * @brief  ���������¼�
  * @param  ��
  * @retval ��
  */
static void ButtonLongPressEvent()
{
    if(btUP)
    {
        FileMenu->SelectMove(+1);//�˵��������
    }
    if(btDOWN)
    {
        FileMenu->SelectMove(-1);//�˵��������
    }
}

/**
  * @brief  ҳ��ע��
  * @param  ThisPage:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_FileExplorer(uint8_t ThisPage)
{
    /*����*/
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    
    /*�¼�*/
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
}
