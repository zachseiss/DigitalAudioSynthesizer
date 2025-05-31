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


// PUBLIC API FUNCTION DEFINITIONS
int16_t oscillator_process(Oscillator *osc)
{
	uint16_t idx_u16 = (uint16_t)osc->phase;

	// linear interpolation
	float frac = osc->phase - idx_u16;
	int16_t a = osc->wave_table[idx_u16];
	int16_t b = osc->wave_table[idx_u16 + 1];
	int16_t sample = (1.0f - frac) * a + frac * b;

	osc->phase += osc->phase_increment;

	if (osc->phase >= WAVETABLE_STD_SIZE - 1) osc->phase -= WAVETABLE_STD_SIZE;

	return sample;
}

void oscillator_init_oscillator(Oscillator* osc, int16_t *wavetable)
{
	osc->frequency = 0.0f;
	osc->phase = 0.0f;
	osc->phase_increment = 0.0f;
	osc->wave_table = wavetable;
}


// PRIVATE FUNCTION DEFINITIONSs

