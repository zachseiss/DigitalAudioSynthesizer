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
#define FREQUENCY_CORRECTION 1.01
#define SAMPLE_RATE 48000.0f
#define TWO_PI 6.283185f
#define ATTACK_RATE 0.005f

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2S_HandleTypeDef hi2s3;
DMA_HandleTypeDef hdma_spi3_tx;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint32_t i2s_tx_buffer[AUDIO_BUFFER_SIZE];
int16_t sine_table[TABLE_SIZE];
float lfo_table[LFO_TABLE_SIZE];

// LFO state
float lfo_phase = 0.0f;
float frequency = 0.0f;
float amplitude_current = 0.0f;
float amplitude_target = 0.0f;
float lfo_frequency = 0.0f;
uint8_t lfo_active = 0;
uint8_t drum_active = 0;
uint8_t square_wave_active = 0;
float lfo_depth = 0.0f;
float pitch_change = 0.0f;
float pitch_decay = 1.0f;

// UART receive
uint8_t rx_byte;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2S3_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void fill_audio_buffer(uint32_t*, int16_t*, uint8_t);
void init_sine_table(int16_t*, size_t);
void init_lfo_table(float*, size_t);
float get_lfo_value();
void process_midi_bytes(void);
void start_midi_reception(void);
float get_current_amplitude(void);

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
  /* USER CODE BEGIN 2 */
  HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*)i2s_tx_buffer, AUDIO_BUFFER_SIZE);
  start_midi_reception();

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
		process_midi_bytes();
		start_midi_reception();
	}
}

void process_midi_bytes(void)
{
    static uint8_t running_status = 0;
    static uint8_t data_byte1 = 0;
    static uint8_t waiting_for_second_data_byte = 0;

    if (rx_byte >= 0x80)  // rx_byte is a status byte
    {
        if (rx_byte == 0xF8) return; // Ignore MIDI clock

        running_status = rx_byte;
        waiting_for_second_data_byte = 0;
        return;
    }

    // Now rx_byte is a data byte (0x00..0x7F)

    if (running_status == 0)
    {
        // Got a data byte but no known running status. Ignore it.
        return;
    }


    if (!waiting_for_second_data_byte)
    {
        data_byte1 = rx_byte;
        waiting_for_second_data_byte = 1;
    }
    else
    {
        uint8_t data_byte2 = rx_byte;
        waiting_for_second_data_byte = 0;

        if ((running_status & 0xF0) == 0x90) // Note on (channel 0-15)
        {
        	switch (running_status)
        	{
        		case 0x90:
        			drum_active = 0;

					if (data_byte2 == 0)
					{
		//            	return;
						amplitude_target = 0.0f; // Treat velocity 0 as Note Off
					}
					else
					{
						frequency = 440.0f * powf(2.0f, (data_byte1 - 69.0f) / 12.0f);
						amplitude_target = data_byte2 / 127.0f;
					}
					break;
        		case 0x99:
        			frequency = 150.0f;
					amplitude_target = data_byte2 / 127.0f;
					lfo_active = 0;
					drum_active = 1;
					if (data_byte2 ^ 0x00) pitch_decay = 1.0f;
					break;
        	}
        }
        else if ((running_status & 0xF0) == 0xE0) // Pitch bend
        {
            uint16_t pitch_value = (data_byte2 << 7) | data_byte1;
            int32_t centered = (int32_t)pitch_value - 8192;
            pitch_change = (float)centered / 8192.0f;
        }
        else if ((running_status & 0xF0) == 0xB0) // Control change
        {
            if (data_byte1 == 0x15)
            {
                lfo_frequency = (float)data_byte2;
            }
            else if (data_byte1 == 0x16)
            {
                lfo_depth = (float)data_byte2;
            }
            else if (data_byte1 == 0x17)
            {
            	square_wave_active = data_byte2 == 1 ? 1 : 0;
            }
        }
    }
}


void start_midi_reception(void)
{
	HAL_UART_Receive_IT(&huart2, (uint8_t*)&rx_byte, 1);
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
		float sample_f = sinf(phase);
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
	float pitch_multiplier = powf(2.0f, pitch_change*0.1666f);
	amplitude_current = get_current_amplitude();

	// calculate phase increment
	float base_phase_increment = frequency * pitch_multiplier * FREQUENCY_CORRECTION * TABLE_SIZE / (SAMPLE_RATE);
	float phase_increment = base_phase_increment;

	if (drum_active)
	{
		phase_increment *= pitch_decay;  //haven't compiled this yet!
		pitch_decay *= 0.999f;

		if (pitch_decay < 0.01f)
		{
			drum_active = 0;
			pitch_decay = 1.0f;
			frequency = 0;
		}
	}

	if (lfo_active)  // will never be active if drum is active
	{
		float modulated_frequency = (frequency * pitch_multiplier) + (get_lfo_value() * lfo_depth);
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
		int16_t sample = amplitude_current * ((1.0f - frac) * a + frac * b);

		if (square_wave_active)
		{
			sample = sample >= 0 ? 27000.0f : -27000.0f;
		}

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

	float lfo_phase_increment = TWO_PI * lfo_frequency * LFO_TABLE_SIZE / SAMPLE_RATE;

	lfo_phase += lfo_phase_increment;

	if (lfo_phase >= LFO_TABLE_SIZE) lfo_phase -= LFO_TABLE_SIZE;

	return value;
}

float get_current_amplitude(void)
{
	// Ramp amplitude smoothly toward the target
	if (amplitude_current < amplitude_target)
	{
	    amplitude_current += ATTACK_RATE;
	    if (amplitude_current > amplitude_target)
	        amplitude_current = amplitude_target;
	}
	else if (amplitude_current >= amplitude_target)
	{
	    amplitude_current *= 0.99995;
	    if (amplitude_current < amplitude_target)
	        amplitude_current = amplitude_target;
	}

	return amplitude_current;
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
