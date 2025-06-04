/*
 * wavetable.c
 *
 *  Created on: May 19, 2025
 *      Author: Zach S
 */

#include "wavetable.h"
#include <stddef.h>
#include "synth.h"

#include <stdio.h>
#include <string.h>

#define INV_PI 1.0f / M_PI
#define PHASE_INCREMENT (2.0f * M_PI) / (float)WAVETABLE_STD_SIZE

// Externals
extern int16_t* p_wavetables[NUM_WAVETABLES];
extern int16_t sine_wavetable[WAVETABLE_STD_SIZE];
extern int16_t square_wavetable[WAVETABLE_STD_SIZE];
extern int16_t saw_wavetable[WAVETABLE_STD_SIZE];
extern int16_t triangle_wavetable[WAVETABLE_STD_SIZE];
extern UART_HandleTypeDef huart1[WAVETABLE_STD_SIZE];

// Variables
static char msg[50];

// PRIVATE FUNCTION PROTOTYPES
static void wavetable_init_sine_wavetable(int16_t*, size_t);
static void wavetable_init_square_wavetable(int16_t*, size_t);
static void wavetable_init_saw_wavetable(int16_t*, size_t);
static void wavetable_init_triangle_wavetable(int16_t*, size_t);
static void init_msg(void);


// PUBLIC API FUNCTION DEFINITIONS
void wavetable_init_wavetables(void)
{
	wavetable_init_sine_wavetable(sine_wavetable, WAVETABLE_STD_SIZE);
	wavetable_init_square_wavetable(square_wavetable, WAVETABLE_STD_SIZE);
	wavetable_init_saw_wavetable(saw_wavetable, WAVETABLE_STD_SIZE);
	wavetable_init_triangle_wavetable(triangle_wavetable, WAVETABLE_STD_SIZE);
}


// PRIVATE FUNCTIONS
static void wavetable_init_sine_wavetable(int16_t* wavetable, size_t length)
{
	float phase = 0.0f;

	for (size_t i = 0; i < length; i += 1)
	{
		float sample_f = sinf(phase);
		int16_t sample_i16 = (int16_t)(sample_f * 32767.0f);
		wavetable[i] = sample_i16;
		phase += PHASE_INCREMENT;
	}
	p_wavetables[WAVEFORM_SINE] = wavetable;

	for (size_t i = 0; i < length; i += 1)
	{
		/////////////////////////////
		init_msg();
		sprintf(msg, "wavtable[%d]:     %d\n\r", i, wavetable[i]);
		HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
		/////////////////////////////
	}

}

static void wavetable_init_square_wavetable(int16_t* wavetable, size_t length)
{
	float phase = 0.0f;

	for (size_t i = 0; i < length; i += 1)
	{
		float sample_f = sinf(phase) <= 0.0f ? -1 : 1;
		int16_t sample_i16 = (int16_t)(sample_f * 32767.0f);
		wavetable[i] = sample_i16;
		phase += PHASE_INCREMENT;
	}
	p_wavetables[WAVEFORM_SQUARE] = wavetable;
	for (size_t i = 0; i < length; i += 1)
	{
		/////////////////////////////
		init_msg();
		sprintf(msg, "wavtable[%d]:     %d\n\r", i, wavetable[i]);
		HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
		/////////////////////////////
	}
}

static void wavetable_init_saw_wavetable(int16_t* wavetable, size_t length)
{
	float phase = 0.0f;

	for (size_t i = 0; i < length; i += 1)
	{
		float sample_f = INV_PI * phase;
		int16_t sample_i16 = (int16_t)(sample_f * 32767.0f);
		wavetable[i] = sample_i16;
		phase += PHASE_INCREMENT;
	}
	p_wavetables[WAVEFORM_SAW] = wavetable;

	for (size_t i = 0; i < length; i += 1)
	{
		/////////////////////////////
		init_msg();
		sprintf(msg, "wavtable[%d]:     %d\n\r", i, wavetable[i]);
		HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
		/////////////////////////////
	}
}

static void wavetable_init_triangle_wavetable(int16_t* wavetable, size_t length)
{
	float phase = 0.0f;

	for (size_t i = 0; i < length; i += 1)
	{
		float sample_f = phase <= M_PI ? INV_PI * phase : -INV_PI * phase + 2;
		int16_t sample_i16 = (int16_t)(sample_f * 32767.0f);
		wavetable[i] = sample_i16;
		phase += PHASE_INCREMENT;
	}
	p_wavetables[WAVEFORM_TRIANGLE] = wavetable;
	for (size_t i = 0; i < length; i += 1)
	{
		/////////////////////////////
		init_msg();
		sprintf(msg, "wavtable[%d]:     %d\n\r", i, wavetable[i]);
		HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
		/////////////////////////////
	}
}

static void init_msg(void)
{
	memset(msg, '\0', sizeof(msg));
}
