/*
 * wavetable.c
 *
 *  Created on: May 19, 2025
 *      Author: Zach S
 */

#include "wavetable.h"
#include <stddef.h>
#include "synth.h"

#define PI 3.14159f
#define INV_PI 1.0f / 3.14159f
#define PHASE_INCREMENT 2.0f * PI / WAVETABLE_STD_SIZE


// PRIVATE FUNCTION PROTOTYPES
static void wavetable_init_sine_wavetable(int16_t[][WAVETABLE_STD_SIZE], size_t);
static void wavetable_init_square_wavetable(int16_t[][WAVETABLE_STD_SIZE], size_t);
static void wavetable_init_saw_wavetable(int16_t[][WAVETABLE_STD_SIZE], size_t);
static void wavetable_init_triangle_wavetable(int16_t[][WAVETABLE_STD_SIZE], size_t);


// PUBLIC API FUNCTION DEFINITIONS
void wavetable_init_wavetables(int16_t wavetable_array[][WAVETABLE_STD_SIZE])
{
	wavetable_init_sine_wavetable(wavetable_array, WAVETABLE_STD_SIZE);
	wavetable_init_square_wavetable(wavetable_array, WAVETABLE_STD_SIZE);
	wavetable_init_saw_wavetable(wavetable_array, WAVETABLE_STD_SIZE);
	wavetable_init_triangle_wavetable(wavetable_array, WAVETABLE_STD_SIZE);
}


// PRIVATE FUNCTIONS
static void wavetable_init_sine_wavetable(int16_t wavetable[][WAVETABLE_STD_SIZE], size_t length)
{
	float phase = 0.0f;

	for (size_t i = 0; i < length; i += 1)
	{
		float sample_f = sinf(phase);
		int16_t sample_i16 = (int16_t)(sample_f * 32767.0f);
		wavetable[WAVEFORM_SINE][i] = sample_i16;
		phase += PHASE_INCREMENT;
	}
}

static void wavetable_init_square_wavetable(int16_t wavetable[][WAVETABLE_STD_SIZE], size_t length)
{
	float phase = 0.0f;

	for (size_t i = 0; i < length; i += 1)
	{
		float sample_f = sinf(phase) <= 0.0f ? -1 : 1;
		int16_t sample_i16 = (int16_t)(sample_f * 32767.0f);
		wavetable[WAVEFORM_SQUARE][i] = sample_i16;
		phase += PHASE_INCREMENT;
	}
}

static void wavetable_init_saw_wavetable(int16_t wavetable[][WAVETABLE_STD_SIZE], size_t length)
{
	float phase = 0.0f;

	for (size_t i = 0; i < length; i += 1)
	{
		float sample_f = INV_PI * phase;
		int16_t sample_i16 = (int16_t)(sample_f * 32767.0f);
		wavetable[WAVEFORM_SAW][i] = sample_i16;
		phase += PHASE_INCREMENT;
	}
}

static void wavetable_init_triangle_wavetable(int16_t wavetable[][WAVETABLE_STD_SIZE], size_t length)
{
	float phase = 0.0f;

	for (size_t i = 0; i < length; i += 1)
	{
		float sample_f = phase <= PI ? INV_PI * phase : -INV_PI * phase + 2;
		int16_t sample_i16 = (int16_t)(sample_f * 32767.0f);
		wavetable[WAVEFORM_TRIANGLE][i] = sample_i16;
		phase += PHASE_INCREMENT;
	}
}

//static void init_waveform_dispatcher(void)
//{
//	wave_table_initializers[WAVEFORM_SINE] = oscillator_init_sine_wavetable;
//	wave_table_initializers[WAVEFORM_SQUARE] = oscillator_init_square_wavetable;
//	wave_table_initializers[WAVEFORM_SAW] = oscillator_init_saw_wavetable;
//	wave_table_initializers[WAVEFORM_TRIANGLE] = oscillator_init_triangle_wavetable;
//}

