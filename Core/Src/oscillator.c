/*
 * oscillator.c
 *
 *  Created on: May 11, 2025
 *      Author: Zach S
 */

#include <stddef.h>
#include <math.h>
#include "oscillator.h"
#include "synth.h"


// PRIVATE FUNCTION PROTOTYPES


// PRIVATE VARIABLES
float sine_wavetable[WAVETABLE_STD_SIZE];
float square_wavetable[WAVETABLE_STD_SIZE];
float saw_wavetable[WAVETABLE_STD_SIZE];
float triangle_wavetable[WAVETABLE_STD_SIZE];


// PUBLIC API FUNCTION DEFINITIONS
int16_t oscillator_process(Oscillator *osc)
{
	int32_t sample = 0;
	int16_t final_sample = 0;
	uint8_t num_unison = synth_get_parameter(UNISON);
	for (size_t i = 0; i < num_unison; i++)
	{
		uint16_t idx_u16 = (uint16_t)osc->phase[i];

		// linear interpolation
		float frac = osc->phase[i] - idx_u16;
		int16_t a = osc->wave_table[idx_u16];
		int16_t b = osc->wave_table[(idx_u16 + 1) % WAVETABLE_STD_SIZE];
		int16_t sample_i = (1.0f - frac) * a + frac * b;
		sample = sample + sample_i;

		osc->phase[i] += osc->phase_increment[i];

		if (osc->phase[i] >= WAVETABLE_STD_SIZE) osc->phase[i] -= WAVETABLE_STD_SIZE;
	}
	final_sample = (int16_t)(sample / (float)num_unison);

	return final_sample;
}

void oscillator_init_oscillator(Oscillator* osc, int16_t *wavetable)
{
	uint8_t num_unison_voices = (uint8_t)synth_get_parameter(UNISON);

	for (size_t i = 0; i < num_unison_voices; i++)
	{
		osc->frequency[i] = 0.0f;
		osc->phase[i] = 0.0f;
		osc->phase_increment[i] = 0.0f;
	}
	osc->wave_table = wavetable;
}


// PRIVATE FUNCTION DEFINITIONSs

