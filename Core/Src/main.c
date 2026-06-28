/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f4xx_hal.h"
#include "ILI9341_Driver.h"
#include "display.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DISPLAY_BRINGUP_TEST 2

/*
 * DISPLAY_BRINGUP_TEST:
 * 0 = normal LCD demo
 * 1 = visible heartbeat/reset test
 * 2 = minimal LCD color cycle
 * 3 = bit-banged SPI LCD color cycle
 * 4 = hardware SPI LCD color cycle
 */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */
static void TFT_BitBang_Test(void);
static void TFT_HardwareSpi_Test(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
#if DISPLAY_BRINGUP_TEST == 1
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
#elif DISPLAY_BRINGUP_TEST == 2
  ILI9341_Init();
  // Display_Text();
  // Display_Back_Icon(200, 200);
  while (1)
  {
    // ILI9341_Fill_Screen(ORANGE);
    // HAL_Delay(1000);
    // ILI9341_Fill_Screen(WHITE);
    // HAL_Delay(1000);
    // ILI9341_Fill_Screen(GREEN);
    // HAL_Delay(1000);
    // ILI9341_Fill_Screen(BLACK);
    // HAL_Delay(1000);
    Display_BMW_Picture();
    
    // HAL_Delay(1000);
    // Display_Text();
    // HAL_Delay(1000);
    // Display_Menu();
    // HAL_Delay(1000);
  }
#elif DISPLAY_BRINGUP_TEST == 3
  TFT_BitBang_Test();
#elif DISPLAY_BRINGUP_TEST == 4
  TFT_HardwareSpi_Test();
#else
  /* =============Initialization=============== */
    ILI9341_Init();
    ILI9341_Set_Rotation(0);
    HAL_Delay(500);
    ILI9341_Fill_Screen(RED);
    HAL_Delay(500);
    ILI9341_Fill_Screen(GREEN);
    HAL_Delay(500);
    ILI9341_Fill_Screen(BLUE);
    HAL_Delay(500);
    ILI9341_Fill_Screen(BLACK);
    HAL_Delay(500);

#endif
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    /* ===============Draw menu================= */
#if DISPLAY_BRINGUP_TEST == 0
      Display_Menu();
#endif

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, DC_Pin|RESET_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CS_Pin */
  GPIO_InitStruct.Pin = CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : DC_Pin RESET_Pin */
  GPIO_InitStruct.Pin = DC_Pin|RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
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

static void TFT_BitBang_Test(void)
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

static void TFT_HardwareSpi_Test(void)
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

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
