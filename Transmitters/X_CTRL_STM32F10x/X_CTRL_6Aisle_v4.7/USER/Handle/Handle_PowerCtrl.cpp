#include "FileGroup.h"

/*�͹���ʹ�ܿ���*/
bool State_PowerSaving = false;

/**
  * @brief  ϵͳʱ������
  * @param  ��
  * @retval ��
  */
void SysClock_Config()
{
    __LoopExecute(Init_Value(),10);
    if(State_PowerSaving);
        //SysClock_Init(F_CPU_72MHz);
    else
        SysClock_Init(F_CPU_128MHz);

    Delay_Init();
}
