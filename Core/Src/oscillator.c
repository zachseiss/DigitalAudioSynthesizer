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
#include "wavetable.h"
#include "oscillator_configuration.h"

#include <string.h>
#include <stdio.h>

#define _2_POW_15 32768
#define FRAC_MAX ((uint32_t)(pow(2, SHIFT)))
#define MAX_PHASE ((WAVETABLE_STD_SIZE - 1) << SHIFT)

// Externals
extern int16_t* p_wavetables[];
extern UART_HandleTypeDef huart1;


// PRIVATE VARIABLES
uint8_t num_osc_initialized = 0;  // how many oscillators have been initialized currently

// PUBLIC API FUNCTION DEFINITIONS
int16_t oscillator_process(Oscillator *osc)
{
	// linear interpolation
	uint16_t idx_u16 = (uint16_t)(osc->phase >> SHIFT);
	uint16_t frac = (uint16_t)(osc->phase & FIXED_POINT_MASK);
	int16_t a = osc->p_wavetable[idx_u16];
	int16_t b = osc->p_wavetable[idx_u16 + 1];
	int16_t sample = ((FRAC_MAX - frac) * a + frac * b) >> SHIFT;

	osc->phase += osc->phase_increment;

	if (osc->phase >= MAX_PHASE) osc->phase -= MAX_PHASE;

	return sample;
}

void oscillator_init_oscillator(Oscillator* osc)
{
	osc->frequency = 0.0f;
	osc->phase = 0;
	osc->phase_increment = 0;
	osc->detune = 0;
	osc->p_wavetable = p_wavetables[num_osc_initialized];
	++num_osc_initialized;
}

void oscillator_to_string(Oscillator* osc)
{
	static char msg[60];

	sprintf(msg, "     freq: %d\n\r", (int)osc->frequency);
	HAL_UART_Transmit(&huart1, (uint8_t*)msg, sizeof(msg), HAL_MAX_DELAY);

}

// PRIVATE FUNCTION DEFINITIONSs

