#include "FileGroup.h"
#include "EEPROM_File.h"

static EEPROM_File eeprom_file;

/*��ʼ�����ַ*/
#define STARTADDR 0

/*���ע���������*/
#define RegDataList_MAX 30

/*FLASH��ȡ��ʱ����*/
#define FlashReadTimeOut 2000

using namespace EEPROM_Chs;

#pragma pack(1)
/*
���ݽṹ:
|                     ֡ͷ                       |                 �û�����
| �û������ܳ� | �û������ܸ��� | �û�����У��� | ����1��С |  ����1  | ����2��С |  ����2  |...
|     2Byte    |     2Byte      |     4Byte      |   2Byte   |  xByte  |   2Byte   |  xByte  |...
*/

/*֡ͷ���ݽṹ*/
typedef struct {
    uint16_t DataLength;
    uint16_t DataCntSum;
    uint32_t CheckSum;
} EEPROM_DataHead_t;

/*�������ݽṹ*/
typedef struct {
    uint8_t *pData;
    uint16_t Size;
} EEPROM_RegList_t;
#pragma pack()

/*�û������б�*/
EEPROM_RegList_t DataRegList[RegDataList_MAX] = {0};

/*ע�������ַƫ����*/
static uint16_t RegListOffset = 0;

static uint16_t EEPROM_Read16(uint16_t addr)
{
    uint8_t high = eeprom_file.read(addr * 2);
    uint8_t low = eeprom_file.read(addr * 2 + 1);
    
//    Serial.printf("R %d high=0x%x\r\n",addr * 2, high);
//    Serial.printf("R %d low=0x%x\r\n",addr * 2 + 1, low);
    
    return (high << 8 | low);
}

static void EEPROM_Update16(uint16_t addr, uint16_t data)
{
//    Serial.printf("W %d = 0x%x\r\n",addr * 2, data);
    eeprom_file.update(addr * 2 + 1, lowByte(data));
    eeprom_file.update(addr * 2, highByte(data));
}

/**
  * @brief  ��ȡ��һ����ַ������
  * @param  reset:����ָ�븴λ
  * @param  start:�趨��ʼ�����ַ
  * @retval ��һ����ַ���������
  */
static uint16_t EEPROM_ReadNext(bool reset = false, uint16_t start = STARTADDR)
{
    static uint16_t OffsetAddr = STARTADDR;
    if(reset)
        OffsetAddr = start;

    uint16_t ret = EEPROM_Read16(OffsetAddr);
    OffsetAddr++;
    return ret;
}

/**
  * @brief  ����һ����ַд����
  * @param  data:��д������
  * @param  reset:����ָ�븴λ
  * @param  start:�趨��ʼ�����ַ
  * @retval ��
  */
static void EEPROM_WriteNext(uint16_t data, bool reset = false, uint16_t start = STARTADDR)
{
    static uint16_t OffsetAddr = STARTADDR;
    if(reset)
        OffsetAddr = start;

    EEPROM_Update16(OffsetAddr, data);
    OffsetAddr++;
}

/**
  * @brief  ע���������ͱ���
  * @param  *pdata:ָ����������ݵ�ָ��
  * @param  size:���ݴ�С
  * @retval true�ɹ� falseʧ��
  */
bool EEPROM_Register(void *pdata, uint16_t size)
{
    if(RegListOffset == RegDataList_MAX)
        return false;
    
    DataRegList[RegListOffset].pData = (uint8_t*)pdata;
    DataRegList[RegListOffset].Size = size;
    RegListOffset++;
    return true;
}

/**
  * @brief  ���ݴ�ȡ����
  * @param  chs:ѡ�����
  * @retval true�ɹ� falseʧ��
  */
bool EEPROM_Handle(EEPROM_Chs_t chs)
{
    bool retval = false;
    EEPROM_DataHead_t head = {0};
    
    eeprom_file.begin("/"_X_CTRL_NAME"/system", "save.bin", 2048);
    if(chs == ReadData)
    {
        /*��ȡ֡ͷ��Ϣ*/
        ((uint16_t*)(&head))[0] = EEPROM_ReadNext(true);//DataLength����ָ��ָ���׵�ַ
        ((uint16_t*)(&head))[1] = EEPROM_ReadNext();//DataCntSum
        ((uint16_t*)(&head))[2] = EEPROM_ReadNext();//CheckSum(HIGH 16bit)
        ((uint16_t*)(&head))[3] = EEPROM_ReadNext();//CheckSum(LOW  16bit)

        /*��ȡ�û�����У���*/
        uint32_t UserDataSum = 0;
        uint32_t StartReadTime = millis();

        for(uint16_t i = 0; i < head.DataLength; i++)
        {
            if(millis() - StartReadTime > FlashReadTimeOut)//��ȡ�Ƿ�ʱ
                goto failed;
            
            UserDataSum += EEPROM_ReadNext();
        }

        /*У���û������Ƿ���ע���б�ƥ��*/
        if(UserDataSum != head.CheckSum || head.DataCntSum != RegListOffset)
            goto failed;

        /*��֡ͷ��Ϣĩ��ַ�Ļ�������ǰ�ƶ�һ����ַ������һ�ζ�ȡ�׵�ַָ���û������׵�ַ*/
        EEPROM_ReadNext(true, STARTADDR + sizeof(head) / 2 - 1);

        /*����ע���б�*/
        for(uint16_t cnt = 0; cnt < head.DataCntSum; cnt++)
        {
            uint16_t UserSize = EEPROM_ReadNext();

            /*/У���Ƿ����û�ע���б�ƥ��*/
            if(UserSize != DataRegList[cnt].Size)
                goto failed;

            /*����ȡ������д��ע���б���pDataָ��ָ�������*/
            for(uint16_t offset = 0; offset < UserSize; offset++)
                (DataRegList[cnt].pData)[offset] = EEPROM_ReadNext();
        }
        retval = true;
    }
    else if(chs == SaveData)
    {
        /*����֡ͷ��Ϣ*/

        /*�û������ܸ���*/
        head.DataCntSum = RegListOffset;

        /*����ע���б�*/
        for(uint16_t cnt = 0; cnt < RegListOffset; cnt++)
        {
            /*�û������ܳ�*/
            head.DataLength += sizeof(DataRegList[cnt].Size) / 2 + DataRegList[cnt].Size;
            /*�û�����У���*/
            head.CheckSum += DataRegList[cnt].Size;
            for(uint16_t offset = 0; offset < DataRegList[cnt].Size; offset++)
                head.CheckSum += (DataRegList[cnt].pData)[offset];
        }

        /*д��֡ͷ��Ϣ*/
        EEPROM_WriteNext(((uint16_t*)(&head))[0], true);//DataLength����ָ��ָ���׵�ַ
        EEPROM_WriteNext(((uint16_t*)(&head))[1]);//DataCntSum
        EEPROM_WriteNext(((uint16_t*)(&head))[2]);//CheckSum(HIGH 16bit)
        EEPROM_WriteNext(((uint16_t*)(&head))[3]);//CheckSum(LOW  16bit)

        /*����ע���б�д���û�����*/
        for(uint16_t cnt = 0; cnt < RegListOffset; cnt++)
        {
            EEPROM_WriteNext(DataRegList[cnt].Size);
            for(uint16_t offset = 0; offset < DataRegList[cnt].Size; offset++)
                EEPROM_WriteNext((DataRegList[cnt].pData)[offset]);
        }
        eeprom_file.end();
        
        /*�ж��Ƿ�д��ɹ�*/
        retval = EEPROM_Handle(ReadData);
    }
failed:
    eeprom_file.end();
    return retval;
}
