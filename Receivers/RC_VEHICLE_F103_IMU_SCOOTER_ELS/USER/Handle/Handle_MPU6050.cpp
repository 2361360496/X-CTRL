#include "FileGroup.h"

#ifdef USE_IMU
#include "MPU6050.h"
#include "filters.h"
#include "math.h"

/*ʵ����MPU6050����*/
static MPU6050 mpu;

int16_t MPU_AX, MPU_AY, MPU_AZ, MPU_GX, MPU_GY, MPU_GZ;

/**
  * @brief  MPU6050��ʼ��
  * @param  ��
  * @retval ��
  */
void Init_MPU6050()
{
    Wire.begin();

    __LoopExecute(mpu.initialize(), 100);

    mpu.setSleepEnabled(true);
}

/**
  * @brief  MPU6050��ȡ����
  * @param  ��
  * @retval ��
  */
void Task_MPU6050Read()
{
#define MPU6050_RAD(x) (radians(((double)x)*2000.0/32767.0))
    mpu.getMotion6(&MPU_AX, &MPU_AY, &MPU_AZ, &MPU_GX, &MPU_GY, &MPU_GZ);
}
#else
void Init_MPU6050(){}
void Task_MPU6050Read(){}
#endif
