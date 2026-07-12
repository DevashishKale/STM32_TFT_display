#include "app.h"

#include "display.h"
#include "display_bringup.h"
#include "ILI9341_Driver.h"
#include "stopwatch_display.h"
#include "weather_display.h"

void App_Init(AppMode mode)
{
  switch (mode)
  {
    case APP_MODE_PROJECT:
      StopwatchDisplay_Init();
      break;

    case APP_MODE_WEATHER:
      WeatherDisplay_Init();
      break;

    case APP_MODE_DISPLAY_EXPERIMENT:
      ILI9341_Init();
      break;

    case APP_MODE_RESET_HEARTBEAT_TEST:
    case APP_MODE_BITBANG_SPI_COLOR_TEST:
    case APP_MODE_HARDWARE_SPI_COLOR_TEST:
    default:
      break;
  }
}

void App_Run(AppMode mode)
{
  switch (mode)
  {
    case APP_MODE_PROJECT:
      StopwatchDisplay_Run();
      break;

    case APP_MODE_WEATHER:
      WeatherDisplay_Run();
      break;

    case APP_MODE_RESET_HEARTBEAT_TEST:
      DisplayBringup_ResetHeartbeatTest();
      break;

    case APP_MODE_DISPLAY_EXPERIMENT:
      DisplayBringup_DisplayExperiment();
      break;

    case APP_MODE_BITBANG_SPI_COLOR_TEST:
      DisplayBringup_BitBangSpiColorTest();
      break;

    case APP_MODE_HARDWARE_SPI_COLOR_TEST:
      DisplayBringup_HardwareSpiColorTest();
      break;

    default:
      break;
  }
}
