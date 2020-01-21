# A Cross-platform migration Project @FASTSHIFT
# Arduino for STM32F4xx

# https://github.com/FASTSHIFT/Arduino-For-Keil

ע��: 
    1.��׼�����Ĭ�Ͼ���Ƶ����25MHz�������ؾ���Ϊ8MHz,��Ҫ��system_stm32f4xx.c -> ��254�� -> PLL_M���� 25 ��Ϊ 8��
    2.����Ŀģ������������̣�
        (1)GCC:ʹ��Keil����GCC(arm-none-eabi)���������б��룬ǰ�������ĵ��Ա��밲װ�������������"xxx.ld"�ļ�����ʹ��CubeMX���ɣ���Ҫ��Linker�˵�������ѡ��
        (2)STD v1.3.0:ʹ��pack��װ��Keil�ڵ�v1.3.0�ı�׼�⣬��������ù��̱ȽϷ��㣬����STM32F4�ĺܶ����ͺŲ�֧�֡�
        (3)STD v1.8.0:ʹ�ù����Դ���v1.8.0�ı�׼�⣬�����ͺ�֧�ֽϺã����̴򿪺����ʾ��װpack����װ��Ϳ�������ʹ��оƬѡ�͡�ȱ����Ҫ�ֶ����ù�����ɾ�ļ���

//*********************************UPGRADE LOGS************************************//

//Finish in 2018.8.1    V_1.0 ����STM32F4xx��׼�����(1.3.0��)����ֲ��ȫ����ArduinoAPI
//Upgrade   2018.8.14   V_1.1 ����USART��ش��룬�޸�Tone��ֻռ��һ����ʱ��(��ʹ��toneSetTimer()�����л�),time_exti����Ϊtimer
//Upgrade   2018.8.17   V_1.2 ��Arduino.h����˸���������GPIO�Ĵ��������������������������ļ�����
//Upgrade   2018.8.17   V_1.3 ���WCharacter.h��֧��Arduino�й�Characters�ĺ��������mcu_type.h��ʹArduino.h���Կ�MCUʹ��
//Upgrade   2018.11.28  V_1.4 ������ע��
//Upgrade   2019.2.17   V_1.5 ����Ч���Լ�������,SPI���ѿ�����������
//Upgrade   2019.3.22   V_1.6 ΪSysTick��������ж����ȼ�����ֹ�������жϴ��(�ǳ���Ҫ!)
//Upgrade   2019.5.9    V_1.7 ����:ADC�⡢PWM�⡢��ʱ���⡢Servo�⡢PIN_MAP��������ӳ���Tone���ѿ�����������
//Upgrade   2019.6.3    V_1.8 pwm.c�⽫Ƶ���趨ֵ��16λ��Ϊ32λ������������Ϸ���Χ�жϣ�ARDUINO�궨���Arduino.hת��ȫ�ֺ궨��
//Upgrade   2019.7.12   V_1.9 Arduino.h�����yield()��digitalPinToInterrupt(Pin)���壬����analogRead_DMA()�Ϸ������жϷ�ʽ
//Upgrade   2019.8.22   V_2.0 ͬ��_Keilduino(STM32F10x) V_7.0����
//Upgrade   2019.8.24   V_2.1 digitalRead_Fast�޸�ΪdigitalRead_FAST���޸� gpio.c �� PIN_MAP �� PC3��PC4��PC5 ADCͨ���Ĵ�����
//Upgrade   2019.9.28   V_2.2 ����SPI��
//Upgrade   2019.10.9   V_2.3 �����STM32F4xx V1.8.0��׼�⣬��֧���µ�F4�ͺţ�������Ҫ�ֶ����FLASH��д�㷨�ļ������߰�װ�µ�PACK
//Upgrade   2019.11.4   V_2.4 ����Print��(��ֲGeorges Menie��printf-stdarg.c)��printf�����г������ƣ����sprintf��Ա
//Upgrade   2019.12.1   V_2.5 ����timer�⣬ʹ�ö����Ŀ��������㺯����������math.h��
//Upgrade   2019.12.4   V_2.6 ���̿���޸ģ������ļ�������ļ����룬�޸���digitalWrite_LOW����ճ�����µ�BUG
//Upgrade   2019.12.6   V_2.7 ʹCore���һЩ�����������ӹ淶��������׼�⣬EXTI�ж���ڸ�����EXTIx_IRQHANDLERд���������GNU֧��
//Upgrade   2019.12.31  V_2.8 ����SPI���setClockDivider