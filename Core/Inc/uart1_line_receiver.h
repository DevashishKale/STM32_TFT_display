#ifndef UART1_LINE_RECEIVER_H
#define UART1_LINE_RECEIVER_H

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <stdint.h>

void Uart1LineReceiver_Init(void);
bool Uart1LineReceiver_ReadLine(char *line, uint8_t lineSize);
uint32_t Uart1LineReceiver_GetByteCount(void);
uint32_t Uart1LineReceiver_GetLineCount(void);
uint32_t Uart1LineReceiver_GetOverflowCount(void);
uint32_t Uart1LineReceiver_GetErrorCount(void);
uint8_t Uart1LineReceiver_GetLastByte(void);
HAL_StatusTypeDef Uart1LineReceiver_GetStartStatus(void);

#endif /* UART1_LINE_RECEIVER_H */
