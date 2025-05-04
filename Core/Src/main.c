/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "math.h"
#include "stdint.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define AUDIO_BUFFER_SIZE 32
#define HALF_BUFFER 16
#define TABLE_SIZE 1024
#define LFO_TABLE_SIZE 128
#define LFO_FREQUENCY 4.0f
#define FREQUENCY 200.0f
#define FREQUENCY_CORRECTION 1.01
#define SAMPLE_RATE 48000.0f
#define AMPLITUDE 0.8f
#define TWO_PI 6.283185f

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2S_HandleTypeDef hi2s3;
DMA_HandleTypeDef hdma_spi3_tx;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint32_t i2s_tx_buffer[AUDIO_BUFFER_SIZE];
int16_t sine_table[TABLE_SIZE];
float lfo_table[LFO_TABLE_SIZE];

// LFO state
float lfo_phase = 0.0f;
float lfo_phase_increment = TWO_PI * LFO_FREQUENCY * LFO_TABLE_SIZE / SAMPLE_RATE;
uint8_t lfo_active = 1;
float lfo_depth = 200.0f;

// UART receive
char rx_byte;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2S3_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void fill_audio_buffer(uint32_t*, int16_t*, uint8_t);
void init_sine_table(int16_t*, size_t);
void init_lfo_table(float*, size_t);
float get_lfo_value();
void uart_forward_loop(void);

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
  init_sine_table(sine_table, TABLE_SIZE);
  init_lfo_table(lfo_table, LFO_TABLE_SIZE);

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2S3_Init();
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*)i2s_tx_buffer, AUDIO_BUFFER_SIZE);
  HAL_UART_Receive_IT(&huart2, (uint8_t*)&rx_byte, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2S3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2S3_Init(void)
{

  /* USER CODE BEGIN I2S3_Init 0 */

  /* USER CODE END I2S3_Init 0 */

  /* USER CODE BEGIN I2S3_Init 1 */

  /* USER CODE END I2S3_Init 1 */
  hi2s3.Instance = SPI3;
  hi2s3.Init.Mode = I2S_MODE_MASTER_TX;
  hi2s3.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s3.Init.DataFormat = I2S_DATAFORMAT_16B_EXTENDED;
  hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
  hi2s3.Init.AudioFreq = I2S_AUDIOFREQ_48K;
  hi2s3.Init.CPOL = I2S_CPOL_LOW;
  hi2s3.Init.ClockSource = I2S_CLOCK_PLL;
  hi2s3.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
  if (HAL_I2S_Init(&hi2s3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2S3_Init 2 */

  /* USER CODE END I2S3_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 31250;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  huart2.Init.BaudRate = 31250;
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
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
	// Called when DMA has sent the last element of the buffer
	// at this point we want to re-fill the second half of the audio buffer
	fill_audio_buffer(i2s_tx_buffer, sine_table, 0);
}

void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
	// Called when DMA has sent the last element of the first half of the audio buffer
	// at this point we want to re-fill the first half of the audio buffer
	fill_audio_buffer(i2s_tx_buffer, sine_table, 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart -> Instance == USART2)
	{
		HAL_UART_Transmit(&huart2, (uint8_t*)&rx_byte, 1, HAL_MAX_DELAY);
		HAL_UART_Receive_IT(&huart2, (uint8_t*)&rx_byte, 1);
	}
}

void init_sine_table(int16_t *table, size_t length)
{
	// this expression sets the frequency so that the generated sine wave
	// will fill the sine wave table with a single full cycle
	float frequency = SAMPLE_RATE / TABLE_SIZE;

	const float PHASE_INCREMENT = TWO_PI * frequency / SAMPLE_RATE;
	float phase = 0.0f;

	for (size_t i = 0; i < length; i += 1)
	{
		float sample_f = sinf(phase) * AMPLITUDE;
		int16_t sample_i16 = (int16_t)(sample_f * 32767.0f);
		table[i] = sample_i16;
		phase += PHASE_INCREMENT;
	}
}

void init_lfo_table(float *table, size_t length)
{
	for (size_t i = 0; i < LFO_TABLE_SIZE; i++)
	{
		table[i] = sinf(TWO_PI * i / LFO_TABLE_SIZE);
	}
}

void fill_audio_buffer(uint32_t *buf, int16_t *table, uint8_t is_half)
{
	/*
	 * is_half is a boolean set to true if the DMA is at the half way point, false otherwise
	 */

	static float idx_f = 0.0f;

	// calculate phase increment
	static float base_phase_increment = FREQUENCY * FREQUENCY_CORRECTION * TABLE_SIZE / (SAMPLE_RATE);
	float phase_increment = base_phase_increment;

	if (lfo_active)
	{
		float modulated_frequency = FREQUENCY + (get_lfo_value() * lfo_depth);
		phase_increment = 0.5f * (base_phase_increment + modulated_frequency * TABLE_SIZE / SAMPLE_RATE);
	}

	uint16_t start = is_half ? 0 : HALF_BUFFER;
	uint16_t end = is_half ? HALF_BUFFER : AUDIO_BUFFER_SIZE;

	for (size_t i = start; i < end; i += 2)
	{

		uint16_t idx_u16 = (uint16_t)idx_f;

		// linear interpolation
		float frac = idx_f - idx_u16;
		int16_t a = table[idx_u16];
		int16_t b = table[(idx_u16 + 1) % TABLE_SIZE];
		int16_t sample = (1.0f - frac) * a + frac * b;

		buf[i] = (uint16_t)sample;                             // Left stereo sample
		buf[(i + 1) % AUDIO_BUFFER_SIZE] = (uint16_t)sample;   // Right stereo sample
		idx_f = idx_f + phase_increment;

		if (idx_f >= TABLE_SIZE) idx_f -= TABLE_SIZE;
	}

}

float get_lfo_value()
{
	static float lfo_phase = 0;

	uint8_t index = ((uint8_t)lfo_phase) % LFO_TABLE_SIZE;
	float value = lfo_table[index];

	lfo_phase += lfo_phase_increment;

	if (lfo_phase >= LFO_TABLE_SIZE) lfo_phase -= LFO_TABLE_SIZE;

	return value;
}

//void uart_forward_loop(void) {
//    while (1) {
//        if (USART2->SR & USART_SR_RXNE) {
//            uint8_t rx_byte = USART2->DR;
//
//            while (!(USART1->SR & USART_SR_TXE));
//            USART1->DR = rx_byte;
//        }
//    }
//}

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

#ifdef  USE_FULL_ASSERT
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
