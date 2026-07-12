#include "weather_display.h"

#include "ILI9341_Driver.h"
#include "main.h"
#include "uart1_line_receiver.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WEATHER_LINE_MAX 96
#define WEATHER_DEBUG_REFRESH_MS 2000

typedef struct
{
  char time[24];
  char temperature[12];
  char humidity[8];
  char windSpeed[12];
  char weatherCode[8];
} WeatherData;

static uint32_t weatherLastDebugDrawMs;
static uint32_t weatherParseErrorCount;
static bool weatherDebugFrameDrawn;
static bool weatherDataFrameDrawn;
static char weatherLastTimeText[40];
static char weatherLastDateText[40];
static char weatherLastTempText[40];
static char weatherLastHumidityText[40];
static char weatherLastWindText[40];
static char weatherLastSummaryText[40];

static bool WeatherDisplay_ParseLine(char *line, WeatherData *data);
static void WeatherDisplay_DrawFrame(void);
static void WeatherDisplay_DrawDebug(void);
static void WeatherDisplay_DrawData(const WeatherData *data);
static void WeatherDisplay_DrawWeatherShell(void);
static void WeatherDisplay_DrawChangedField(char *lastText, const char *newText, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color, uint8_t size);
static const char *WeatherDisplay_CodeText(int code);

void WeatherDisplay_Init(void)
{
  weatherLastDebugDrawMs = 0;
  weatherParseErrorCount = 0;
  weatherDebugFrameDrawn = false;
  weatherDataFrameDrawn = false;
  memset(weatherLastTimeText, 0, sizeof(weatherLastTimeText));
  memset(weatherLastDateText, 0, sizeof(weatherLastDateText));
  memset(weatherLastTempText, 0, sizeof(weatherLastTempText));
  memset(weatherLastHumidityText, 0, sizeof(weatherLastHumidityText));
  memset(weatherLastWindText, 0, sizeof(weatherLastWindText));
  memset(weatherLastSummaryText, 0, sizeof(weatherLastSummaryText));

  Uart1LineReceiver_Init();
  ILI9341_Init();
  WeatherDisplay_DrawFrame();
}

void WeatherDisplay_Run(void)
{
  char line[WEATHER_LINE_MAX];
  WeatherData data;

  if (!Uart1LineReceiver_ReadLine(line, sizeof(line)))
  {
    if (!weatherDataFrameDrawn)
    {
      WeatherDisplay_DrawDebug();
    }
    return;
  }

  if (WeatherDisplay_ParseLine(line, &data))
  {
    weatherDebugFrameDrawn = false;
    WeatherDisplay_DrawData(&data);
  }
  else
  {
    weatherParseErrorCount++;
  }
}

static bool WeatherDisplay_ParseLine(char *line, WeatherData *data)
{
  char *token;

  memset(data, 0, sizeof(*data));

  token = strtok(line, ",");
  if (token == NULL)
  {
    return false;
  }
  strncpy(data->time, token, sizeof(data->time) - 1);

  token = strtok(NULL, ",");
  if (token == NULL)
  {
    return false;
  }
  strncpy(data->temperature, token, sizeof(data->temperature) - 1);

  token = strtok(NULL, ",");
  if (token == NULL)
  {
    return false;
  }
  strncpy(data->humidity, token, sizeof(data->humidity) - 1);

  token = strtok(NULL, ",");
  if (token == NULL)
  {
    return false;
  }
  strncpy(data->windSpeed, token, sizeof(data->windSpeed) - 1);

  token = strtok(NULL, ",");
  if (token == NULL)
  {
    return false;
  }
  strncpy(data->weatherCode, token, sizeof(data->weatherCode) - 1);

  return true;
}

static void WeatherDisplay_DrawFrame(void)
{
  ILI9341_Set_Rotation(3);
  ILI9341_Fill_Screen(BLACK);
  ILI9341_Draw_String(12, 10, CYAN, BLACK, "Chennai Weather", 2);
  ILI9341_Draw_Empty_Rectangle(DARKGREY, 8, 36, 312, 232);
  ILI9341_Draw_String(20, 70, WHITE, BLACK, "Waiting for ESP8266...", 2);
  ILI9341_Draw_String(20, 102, DARKGREY, BLACK, "USART1 RX: PA10, 115200", 1);
}

static void WeatherDisplay_DrawDebug(void)
{
  char text[42];
  uint32_t now = HAL_GetTick();
  GPIO_PinState rxPinState = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10);

  if (weatherDebugFrameDrawn && ((now - weatherLastDebugDrawMs) < WEATHER_DEBUG_REFRESH_MS))
  {
    return;
  }

  weatherLastDebugDrawMs = now;

  ILI9341_Set_Rotation(3);
  if (!weatherDebugFrameDrawn)
  {
    ILI9341_Fill_Screen(BLACK);
    ILI9341_Draw_String(12, 10, CYAN, BLACK, "Weather UART Debug", 2);
    ILI9341_Draw_Empty_Rectangle(DARKGREY, 8, 36, 312, 232);
    weatherDebugFrameDrawn = true;
  }
  else
  {
    ILI9341_Draw_Filled_Rectangle(BLACK, 16, 48, 304, 222);
  }

  snprintf(text, sizeof(text), "RX bytes: %lu", Uart1LineReceiver_GetByteCount());
  ILI9341_Draw_String(20, 50, WHITE, BLACK, text, 1);
  snprintf(text, sizeof(text), "Lines: %lu  Parse err: %lu", Uart1LineReceiver_GetLineCount(), weatherParseErrorCount);
  ILI9341_Draw_String(20, 72, WHITE, BLACK, text, 1);
  snprintf(text, sizeof(text), "Overflow: %lu  UART err: %lu", Uart1LineReceiver_GetOverflowCount(), Uart1LineReceiver_GetErrorCount());
  ILI9341_Draw_String(20, 94, WHITE, BLACK, text, 1);
  snprintf(text, sizeof(text), "IT start: %d  PA10: %s", (int)Uart1LineReceiver_GetStartStatus(), rxPinState == GPIO_PIN_SET ? "HIGH" : "LOW");
  ILI9341_Draw_String(20, 116, WHITE, BLACK, text, 1);
  snprintf(text, sizeof(text), "Last byte: 0x%02X", Uart1LineReceiver_GetLastByte());
  ILI9341_Draw_String(20, 138, YELLOW, BLACK, text, 2);
}

static void WeatherDisplay_DrawData(const WeatherData *data)
{
  char date[11];
  char time[6];
  char timeText[40];
  char dateText[40];
  char tempText[40];
  char humidityText[40];
  char windText[40];
  int code = atoi(data->weatherCode);

  WeatherDisplay_DrawWeatherShell();

  memset(date, 0, sizeof(date));
  memset(time, 0, sizeof(time));
  strncpy(date, data->time, 10);
  if (strlen(data->time) >= 16 && data->time[10] == 'T')
  {
    strncpy(time, &data->time[11], 5);
  }
  else
  {
    strncpy(time, data->time, sizeof(time) - 1);
  }

  snprintf(timeText, sizeof(timeText), "%s", time);
  snprintf(dateText, sizeof(dateText), "Date: %s", date);
  snprintf(tempText, sizeof(tempText), "Temp: %s C", data->temperature);
  snprintf(humidityText, sizeof(humidityText), "Humidity: %s %%", data->humidity);
  snprintf(windText, sizeof(windText), "Wind: %s km/h", data->windSpeed);

  WeatherDisplay_DrawChangedField(weatherLastTimeText, timeText, 20, 50, 120, 22, YELLOW, 2);
  WeatherDisplay_DrawChangedField(weatherLastDateText, dateText, 20, 78, 190, 14, LIGHTGREY, 1);
  WeatherDisplay_DrawChangedField(weatherLastSummaryText, WeatherDisplay_CodeText(code), 20, 104, 220, 22, GREEN, 2);
  WeatherDisplay_DrawChangedField(weatherLastTempText, tempText, 20, 138, 260, 22, WHITE, 2);
  WeatherDisplay_DrawChangedField(weatherLastHumidityText, humidityText, 20, 172, 260, 14, WHITE, 1);
  WeatherDisplay_DrawChangedField(weatherLastWindText, windText, 20, 196, 260, 14, WHITE, 1);
}

static void WeatherDisplay_DrawWeatherShell(void)
{
  if (weatherDataFrameDrawn)
  {
    return;
  }

  ILI9341_Set_Rotation(3);
  ILI9341_Draw_Filled_Rectangle(BLACK, 0, 0, 319, 239);
  ILI9341_Draw_String(12, 10, CYAN, BLACK, "Chennai Weather", 2);
  ILI9341_Draw_Empty_Rectangle(DARKGREY, 8, 36, 312, 232);
  weatherDataFrameDrawn = true;
}

static void WeatherDisplay_DrawChangedField(char *lastText, const char *newText, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color, uint8_t size)
{
  if (strcmp(lastText, newText) == 0)
  {
    return;
  }

  ILI9341_Draw_Filled_Rectangle(BLACK, x, y, x + width, y + height);
  ILI9341_Draw_String(x, y, color, BLACK, (char *)newText, size);
  strncpy(lastText, newText, 39);
  lastText[39] = '\0';
}

static const char *WeatherDisplay_CodeText(int code)
{
  switch (code)
  {
    case 0:
      return "Clear";
    case 1:
    case 2:
    case 3:
      return "Partly cloudy";
    case 45:
    case 48:
      return "Fog";
    case 51:
    case 53:
    case 55:
      return "Drizzle";
    case 61:
    case 63:
    case 65:
      return "Rain";
    case 80:
    case 81:
    case 82:
      return "Rain showers";
    case 95:
    case 96:
    case 99:
      return "Thunderstorm";
    default:
      return "Unknown";
  }
}
