#include "FileGroup.h"

/*�͹���ʹ�ܿ���*/
bool State_PowerSaving = true;

/**
  * @brief  ϵͳʱ������
  * @param  ��
  * @retval ��
  */
void SysClock_Config()
{
    Init_Value();
    if(State_PowerSaving);
        //SysClock_Init(F_CPU_72MHz);
    else
        SysClock_Init(F_CPU_128MHz);

    Delay_Init();
}
