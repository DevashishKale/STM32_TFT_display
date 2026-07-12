#include "uart1_line_receiver.h"

#include "main.h"
#include <string.h>

extern UART_HandleTypeDef huart1;

#define UART1_LINE_MAX 96

static char uartLine[UART1_LINE_MAX];
static char uartReadyLine[UART1_LINE_MAX];
static uint8_t uartLineIndex;
static uint8_t uartRxByte;
static volatile bool uartLineReady;
static uint32_t uartRxByteCount;
static uint32_t uartRxLineCount;
static uint32_t uartRxOverflowCount;
static uint32_t uartRxErrorCount;
static uint8_t uartLastByte;
static HAL_StatusTypeDef uartStartStatus;

void Uart1LineReceiver_Init(void)
{
  uartLineIndex = 0;
  uartRxByte = 0;
  uartLineReady = false;
  uartRxByteCount = 0;
  uartRxLineCount = 0;
  uartRxOverflowCount = 0;
  uartRxErrorCount = 0;
  uartLastByte = 0;
  uartStartStatus = HAL_OK;
  memset(uartLine, 0, sizeof(uartLine));
  memset(uartReadyLine, 0, sizeof(uartReadyLine));

  uartStartStatus = HAL_UART_Receive_IT(&huart1, &uartRxByte, 1);
}

bool Uart1LineReceiver_ReadLine(char *line, uint8_t lineSize)
{
  if (!uartLineReady)
  {
    return false;
  }

  __disable_irq();
  strncpy(line, uartReadyLine, lineSize - 1);
  line[lineSize - 1] = '\0';
  uartLineReady = false;
  __enable_irq();

  return true;
}

uint32_t Uart1LineReceiver_GetByteCount(void)
{
  return uartRxByteCount;
}

uint32_t Uart1LineReceiver_GetLineCount(void)
{
  return uartRxLineCount;
}

uint32_t Uart1LineReceiver_GetOverflowCount(void)
{
  return uartRxOverflowCount;
}

uint32_t Uart1LineReceiver_GetErrorCount(void)
{
  return uartRxErrorCount;
}

uint8_t Uart1LineReceiver_GetLastByte(void)
{
  return uartLastByte;
}

HAL_StatusTypeDef Uart1LineReceiver_GetStartStatus(void)
{
  return uartStartStatus;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  uint8_t received = uartRxByte;

  if (huart->Instance != USART1)
  {
    return;
  }

  uartRxByteCount++;
  uartLastByte = received;

  if (received == '\r')
  {
    HAL_UART_Receive_IT(&huart1, &uartRxByte, 1);
    return;
  }

  if (received == '\n')
  {
    uartLine[uartLineIndex] = '\0';
    strncpy(uartReadyLine, uartLine, sizeof(uartReadyLine) - 1);
    uartReadyLine[sizeof(uartReadyLine) - 1] = '\0';
    uartLineIndex = 0;
    uartRxLineCount++;
    uartLineReady = true;
    HAL_UART_Receive_IT(&huart1, &uartRxByte, 1);
    return;
  }

  if (uartLineIndex < (UART1_LINE_MAX - 1))
  {
    uartLine[uartLineIndex] = (char)received;
    uartLineIndex++;
  }
  else
  {
    uartRxOverflowCount++;
    uartLineIndex = 0;
  }

  HAL_UART_Receive_IT(&huart1, &uartRxByte, 1);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance != USART1)
  {
    return;
  }

  uartRxErrorCount++;
  HAL_UART_Receive_IT(&huart1, &uartRxByte, 1);
}
