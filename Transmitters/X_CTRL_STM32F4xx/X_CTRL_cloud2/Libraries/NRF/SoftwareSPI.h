
#ifndef __SOFTWARESPI_H__
#define __SOFTWARESPI_H__

#include "arduino.h"

typedef uint32_t SOFTWARESPI_PORT_TYPEDEF;

#define SoftwareSPI_SetPin(n)				digitalWrite_HIGH(n)
#define SoftwareSPI_ResetPin(n)			digitalWrite_LOW(n)
#define SoftwareSPI_GetPin(n)				digitalRead_Fast(n)

/* ��ʾһ����� SPI �ӿڵ�ʵ�� */
class SoftwareSPI
{
private:
  char Data;						/* д���ͬʱȡ�������� */
public:
  SOFTWARESPI_PORT_TYPEDEF MOSI;				/* MOSI ���ŵ�λ���� */
  SOFTWARESPI_PORT_TYPEDEF MISO;
  SOFTWARESPI_PORT_TYPEDEF SCK;
  SOFTWARESPI_PORT_TYPEDEF CSN;
	SoftwareSPI(SOFTWARESPI_PORT_TYPEDEF mosi, SOFTWARESPI_PORT_TYPEDEF miso, SOFTWARESPI_PORT_TYPEDEF sck, SOFTWARESPI_PORT_TYPEDEF csn);
  void ChipSetect();					/* ѡ�и� SPI �豸 */
  void ChipRelease();					/* �ͷŸ� SPI �豸 */
  void WriteByte(char dat);		/* ��� SPI �豸д��һ���ֽ� */
  char ReadByte();						/* ��ȡ��һ��д�ֽ�ͬʱȡ�����ֽ� */
  void Init();								/* ��ʼ���� SPI ʵ������ʼ�����Ų���������ΪĬ��״̬ */
};


#endif


