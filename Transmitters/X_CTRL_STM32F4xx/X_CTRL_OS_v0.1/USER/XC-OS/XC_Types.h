#ifndef __XC_TYPES_H
#define __XC_TYPES_H

#include <stdint.h>

namespace XC_Types
{

/*���ֽڶ���*/
#pragma pack (1)

typedef enum
{
    TYPE_Info,
    TYPE_Media,
    TYPE_Code
} TYPE_Property_t;

/*�ļ�ͷ��Ϣ*/
typedef struct
{
    TYPE_Property_t Type;
    uint16_t Version;
    char Author[20];
    char Date[20];
    char Time[20];
} BasicInfo_t;

/*�ļ���Ϣ*/
typedef struct
{
    TYPE_Property_t Type;
    char Name[20];
    uint32_t Size;
} File_t;

/*ý���ļ���Ϣ*/
typedef struct
{
    uint8_t PictureNum;
    File_t Pictrue[20];
    uint8_t VideoNum;
    File_t Video[20];
} MediaInfo_t;

/*�ű��ļ���Ϣ*/
typedef struct
{
    uint8_t ScriptNum;
    File_t Script[20];
} ScriptInfo_t;

/*��װ����Ϣ*/
typedef struct
{
    BasicInfo_t Basic;
    MediaInfo_t Media;
    ScriptInfo_t Script;
    uint64_t CheckSum;
} XC_Package_t;

/*ȡ�����ֽڶ���*/
#pragma pack ()

}

#endif
