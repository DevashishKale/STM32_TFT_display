#include "stopwatch_display.h"

#include "characters.h"
#include "ILI9341_Driver.h"
#include "uart1_line_receiver.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define STOPWATCH_LINE_MAX 16
#define STOPWATCH_TIME_LEN 5
#define STOPWATCH_DIGIT_SOURCE_WIDTH 30
#define STOPWATCH_DIGIT_SOURCE_HEIGHT 48
#define STOPWATCH_DIGIT_SOURCE_BYTES_PER_ROW 4
#define STOPWATCH_DIGIT_SOURCE_BYTES 192
#define STOPWATCH_SCALE_NUMERATOR 2
#define STOPWATCH_SCALE_DENOMINATOR 1
#define STOPWATCH_DIGIT_WIDTH ((STOPWATCH_DIGIT_SOURCE_WIDTH * STOPWATCH_SCALE_NUMERATOR) / STOPWATCH_SCALE_DENOMINATOR)
#define STOPWATCH_DIGIT_HEIGHT ((STOPWATCH_DIGIT_SOURCE_HEIGHT * STOPWATCH_SCALE_NUMERATOR) / STOPWATCH_SCALE_DENOMINATOR)
#define STOPWATCH_COLON_WIDTH 12
#define STOPWATCH_DIGIT_GAP 3
#define STOPWATCH_COLON_PADDING 15
#define STOPWATCH_START_X 23
#define STOPWATCH_START_Y 72

static bool stopwatchFrameDrawn;
static char stopwatchLastTime[STOPWATCH_TIME_LEN + 1];

static bool StopwatchDisplay_ParseTime(const char *line, char *time);
static void StopwatchDisplay_DrawFrame(void);
static void StopwatchDisplay_DrawTime(const char *time);
static uint16_t StopwatchDisplay_CharX(uint8_t index);
static uint16_t StopwatchDisplay_CharWidth(char c);
static void StopwatchDisplay_ClearChar(uint16_t x, uint16_t y, char c);
static void StopwatchDisplay_DrawDigit(uint16_t x, uint16_t y, char digit, uint16_t color);
static void StopwatchDisplay_DrawColon(uint16_t x, uint16_t y, uint16_t color);

void StopwatchDisplay_Init(void)
{
  stopwatchFrameDrawn = false;
  memset(stopwatchLastTime, 0, sizeof(stopwatchLastTime));

  Uart1LineReceiver_Init();
  ILI9341_Init();
  StopwatchDisplay_DrawFrame();
}

void StopwatchDisplay_Run(void)
{
  char line[STOPWATCH_LINE_MAX];
  char time[STOPWATCH_TIME_LEN + 1];

  if (!Uart1LineReceiver_ReadLine(line, sizeof(line)))
  {
    return;
  }

  if (!StopwatchDisplay_ParseTime(line, time))
  {
    return;
  }

  StopwatchDisplay_DrawTime(time);
}

static bool StopwatchDisplay_ParseTime(const char *line, char *time)
{
  if (strlen(line) < STOPWATCH_TIME_LEN)
  {
    return false;
  }

  if (line[2] != ':')
  {
    return false;
  }

  for (uint8_t i = 0; i < STOPWATCH_TIME_LEN; i++)
  {
    if (i == 2)
    {
      continue;
    }

    if (line[i] < '0' || line[i] > '9')
    {
      return false;
    }
  }

  strncpy(time, line, STOPWATCH_TIME_LEN);
  time[STOPWATCH_TIME_LEN] = '\0';
  return true;
}

static void StopwatchDisplay_DrawFrame(void)
{
  ILI9341_Set_Rotation(3);
  ILI9341_Fill_Screen(BLACK);
  stopwatchFrameDrawn = true;
}

static void StopwatchDisplay_DrawTime(const char *time)
{
  if (!stopwatchFrameDrawn)
  {
    StopwatchDisplay_DrawFrame();
  }

  if (strcmp(stopwatchLastTime, time) == 0)
  {
    return;
  }

  for (uint8_t i = 0; i < STOPWATCH_TIME_LEN; i++)
  {
    if (stopwatchLastTime[i] == time[i])
    {
      continue;
    }

    StopwatchDisplay_ClearChar(StopwatchDisplay_CharX(i), STOPWATCH_START_Y, time[i]);

    if (time[i] == ':')
    {
      StopwatchDisplay_DrawColon(StopwatchDisplay_CharX(i), STOPWATCH_START_Y, YELLOW);
    }
    else
    {
      StopwatchDisplay_DrawDigit(StopwatchDisplay_CharX(i), STOPWATCH_START_Y, time[i], YELLOW);
    }
  }

  strncpy(stopwatchLastTime, time, sizeof(stopwatchLastTime) - 1);
  stopwatchLastTime[sizeof(stopwatchLastTime) - 1] = '\0';
}

static uint16_t StopwatchDisplay_CharX(uint8_t index)
{
  static const uint16_t positions[STOPWATCH_TIME_LEN] = {
    STOPWATCH_START_X,
    STOPWATCH_START_X + STOPWATCH_DIGIT_WIDTH + STOPWATCH_DIGIT_GAP,
    STOPWATCH_START_X + (2 * STOPWATCH_DIGIT_WIDTH) + STOPWATCH_DIGIT_GAP + STOPWATCH_COLON_PADDING,
    STOPWATCH_START_X + (2 * STOPWATCH_DIGIT_WIDTH) + STOPWATCH_DIGIT_GAP + (2 * STOPWATCH_COLON_PADDING) + STOPWATCH_COLON_WIDTH,
    STOPWATCH_START_X + (3 * STOPWATCH_DIGIT_WIDTH) + (2 * STOPWATCH_DIGIT_GAP) + (2 * STOPWATCH_COLON_PADDING) + STOPWATCH_COLON_WIDTH
  };

  return positions[index];
}

static uint16_t StopwatchDisplay_CharWidth(char c)
{
  return c == ':' ? STOPWATCH_COLON_WIDTH : STOPWATCH_DIGIT_WIDTH;
}

static void StopwatchDisplay_ClearChar(uint16_t x, uint16_t y, char c)
{
  ILI9341_Draw_Filled_Rectangle(BLACK, x, y, x + StopwatchDisplay_CharWidth(c), y + STOPWATCH_DIGIT_HEIGHT);
}

static void StopwatchDisplay_DrawDigit(uint16_t x, uint16_t y, char digit, uint16_t color)
{
  uint16_t sourceOffset;

  if (digit < '0' || digit > '9')
  {
    return;
  }

  sourceOffset = (uint16_t)(digit - '0') * STOPWATCH_DIGIT_SOURCE_BYTES;

  for (uint8_t row = 0; row < STOPWATCH_DIGIT_SOURCE_HEIGHT; row++)
  {
    for (uint8_t col = 0; col < STOPWATCH_DIGIT_SOURCE_WIDTH; col++)
    {
      uint8_t byte = Digit[sourceOffset + (row * STOPWATCH_DIGIT_SOURCE_BYTES_PER_ROW) + (col / 8)];

      if ((byte >> (7 - (col % 8))) & 0x01)
      {
        uint16_t x1 = x + ((uint16_t)col * STOPWATCH_SCALE_NUMERATOR) / STOPWATCH_SCALE_DENOMINATOR;
        uint16_t y1 = y + ((uint16_t)row * STOPWATCH_SCALE_NUMERATOR) / STOPWATCH_SCALE_DENOMINATOR;
        uint16_t x2 = x + (((uint16_t)col + 1) * STOPWATCH_SCALE_NUMERATOR) / STOPWATCH_SCALE_DENOMINATOR - 1;
        uint16_t y2 = y + (((uint16_t)row + 1) * STOPWATCH_SCALE_NUMERATOR) / STOPWATCH_SCALE_DENOMINATOR - 1;

        ILI9341_Draw_Filled_Rectangle(color,
                                      x1,
                                      y1,
                                      x2,
                                      y2);
      }
    }
  }
}

static void StopwatchDisplay_DrawColon(uint16_t x, uint16_t y, uint16_t color)
{
  uint16_t dot = STOPWATCH_COLON_WIDTH;

  ILI9341_Draw_Filled_Rectangle(color, x, y + 28, x + dot, y + 28 + dot);
  ILI9341_Draw_Filled_Rectangle(color, x, y + 62, x + dot, y + 62 + dot);
}
