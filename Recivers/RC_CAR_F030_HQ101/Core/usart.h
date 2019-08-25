#ifndef __USART_H
#define __USART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f0xx.h"
#include "Arduino.h"

#define SERIAL_RX_BUFFER_SIZE 128
#define SERIAL_NUM 3

#define IS_USARTx_SendDone(USARTx)	(USARTx->ISR & USART_ISR_TC)

typedef uint16_t rx_buffer_index_t;
extern volatile uint16_t _rx_buffer_head[SERIAL_NUM];
extern volatile uint16_t _rx_buffer_tail[SERIAL_NUM];
extern volatile uint8_t _rx_buffer[SERIAL_NUM][SERIAL_RX_BUFFER_SIZE];

void USARTx_Init(USART_TypeDef* USARTx, uint32_t BaudRate, uint8_t ChannelPriority);
void USARTx_putc(USART_TypeDef* USARTx, uint8_t c);
void USARTx_attachInterrupt(uint8_t USART_Num, void_func_point f);

#ifdef __cplusplus
}
#endif

#endif

