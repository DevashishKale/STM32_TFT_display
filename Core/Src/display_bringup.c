#include "display_bringup.h"

#include "display.h"
#include "ILI9341_Driver.h"
#include "main.h"

extern SPI_HandleTypeDef hspi1;

static void TFT_BB_Delay(void);
static void TFT_BB_Write8(uint8_t data);
static void TFT_BB_Command(uint8_t command);
static void TFT_BB_Data(uint8_t data);
static void TFT_BB_SetAddress(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
static void TFT_BB_Fill(uint16_t color);
static void TFT_HW_Write8(uint8_t data);
static void TFT_HW_Command(uint8_t command);
static void TFT_HW_Data(uint8_t data);
static void TFT_HW_SetAddress(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
static void TFT_HW_Fill(uint16_t color);

void DisplayBringup_ResetHeartbeatTest(void)
{
  GPIO_InitTypeDef BringupGpio = {0};

  /* PA5 is LD2 on the Nucleo board. In this test only, use it as a visible heartbeat. */
  BringupGpio.Pin = GPIO_PIN_5;
  BringupGpio.Mode = GPIO_MODE_OUTPUT_PP;
  BringupGpio.Pull = GPIO_NOPULL;
  BringupGpio.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &BringupGpio);

  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);

  while (1)
  {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, GPIO_PIN_RESET);
    HAL_Delay(250);

    HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, GPIO_PIN_SET);
    HAL_Delay(750);
  }
}

void DisplayBringup_DisplayExperiment(void)
{
  while (1)
  {
    Display_BMW_Picture();

    /*
     * Useful calls for quick display checks:
     * Display_Text();
     * Display_Menu();
     * Display_Back_Icon(200, 200);
     */
  }
}

void DisplayBringup_BitBangSpiColorTest(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);

  HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, GPIO_PIN_RESET);
  HAL_Delay(50);
  HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, GPIO_PIN_SET);
  HAL_Delay(120);

  TFT_BB_Command(0x01);
  HAL_Delay(120);
  TFT_BB_Command(0x11);
  HAL_Delay(120);
  TFT_BB_Command(0x3A);
  TFT_BB_Data(0x55);
  TFT_BB_Command(0x36);
  TFT_BB_Data(0x48);
  TFT_BB_Command(0x29);
  HAL_Delay(20);

  while (1)
  {
    TFT_BB_Fill(RED);
    HAL_Delay(1000);
    TFT_BB_Fill(GREEN);
    HAL_Delay(1000);
    TFT_BB_Fill(BLUE);
    HAL_Delay(1000);
    TFT_BB_Fill(BLACK);
    HAL_Delay(1000);
  }
}

void DisplayBringup_HardwareSpiColorTest(void)
{
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);

  HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, GPIO_PIN_RESET);
  HAL_Delay(50);
  HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, GPIO_PIN_SET);
  HAL_Delay(120);

  TFT_HW_Command(0x01);
  HAL_Delay(120);
  TFT_HW_Command(0x11);
  HAL_Delay(120);
  TFT_HW_Command(0x3A);
  TFT_HW_Data(0x55);
  TFT_HW_Command(0x36);
  TFT_HW_Data(0x48);
  TFT_HW_Command(0x29);
  HAL_Delay(20);

  while (1)
  {
    TFT_HW_Fill(RED);
    HAL_Delay(1000);
    TFT_HW_Fill(GREEN);
    HAL_Delay(1000);
    TFT_HW_Fill(BLUE);
    HAL_Delay(1000);
    TFT_HW_Fill(BLACK);
    HAL_Delay(1000);
  }
}

static void TFT_BB_Delay(void)
{
  for (volatile uint32_t i = 0; i < 40; i++)
  {
  }
}

static void TFT_BB_Write8(uint8_t data)
{
  for (uint8_t mask = 0x80; mask != 0; mask >>= 1)
  {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, (data & mask) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    TFT_BB_Delay();
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    TFT_BB_Delay();
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    TFT_BB_Delay();
  }
}

static void TFT_BB_Command(uint8_t command)
{
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);
  TFT_BB_Write8(command);
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}

static void TFT_BB_Data(uint8_t data)
{
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
  TFT_BB_Write8(data);
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}

static void TFT_BB_SetAddress(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  TFT_BB_Command(0x2A);
  TFT_BB_Data(x1 >> 8);
  TFT_BB_Data(x1);
  TFT_BB_Data(x2 >> 8);
  TFT_BB_Data(x2);

  TFT_BB_Command(0x2B);
  TFT_BB_Data(y1 >> 8);
  TFT_BB_Data(y1);
  TFT_BB_Data(y2 >> 8);
  TFT_BB_Data(y2);
}

static void TFT_BB_Fill(uint16_t color)
{
  TFT_BB_SetAddress(0, 0, 239, 319);
  TFT_BB_Command(0x2C);

  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
  for (uint32_t i = 0; i < 76800; i++)
  {
    TFT_BB_Write8(color >> 8);
    TFT_BB_Write8(color);
  }
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}

static void TFT_HW_Write8(uint8_t data)
{
  HAL_SPI_Transmit(&hspi1, &data, 1, 100);
}

static void TFT_HW_Command(uint8_t command)
{
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);
  TFT_HW_Write8(command);
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}

static void TFT_HW_Data(uint8_t data)
{
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
  TFT_HW_Write8(data);
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}

static void TFT_HW_SetAddress(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  TFT_HW_Command(0x2A);
  TFT_HW_Data(x1 >> 8);
  TFT_HW_Data(x1);
  TFT_HW_Data(x2 >> 8);
  TFT_HW_Data(x2);

  TFT_HW_Command(0x2B);
  TFT_HW_Data(y1 >> 8);
  TFT_HW_Data(y1);
  TFT_HW_Data(y2 >> 8);
  TFT_HW_Data(y2);
}

static void TFT_HW_Fill(uint16_t color)
{
  TFT_HW_SetAddress(0, 0, 239, 319);
  TFT_HW_Command(0x2C);

  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
  for (uint32_t i = 0; i < 76800; i++)
  {
    TFT_HW_Write8(color >> 8);
    TFT_HW_Write8(color);
  }
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}
