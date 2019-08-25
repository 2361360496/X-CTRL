#include "SystemUseage.h"
#include "CommonMacro.h"

#define MTMGRP_MAX 10

MillisTaskManager* mtmGroup[MTMGRP_MAX] = {0};

float CPU_Usage = 0.0f;

void Task_ClacCPU_Usage()
{
    CPU_Usage = (mtmGroup[0] -> GetCPU_Useage()) * 100.0f;
}

bool Add_MTM_ToGroup(MillisTaskManager *mtm)
{
    /*�������Ƿ����ظ�������*/
    for(uint8_t i = 0; i < MTMGRP_MAX; i++)
    {
        if(mtmGroup[i] == mtm)
        {
            return true;
        }
    }
    /*�ڵ��������ҵ���λ����*/
    for(uint8_t i = 0; i < MTMGRP_MAX; i++)
    {
        if(mtmGroup[i] == 0)
        {
            mtmGroup[i] = mtm;
            return true;
        }
    }
    return false;
}
