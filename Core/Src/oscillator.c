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

// Externals
extern int16_t* p_wavetables[];
extern UART_HandleTypeDef huart1;


// PRIVATE VARIABLES
uint8_t num_osc_initialized = 0;  // how many oscillators have been initialized currently

// PUBLIC API FUNCTION DEFINITIONS
int16_t oscillator_process(Oscillator *osc)
{
	uint16_t idx_u16 = (uint16_t)osc->phase;

	// linear interpolation
	float frac = osc->phase - idx_u16;
	int16_t a = osc->p_wavetable[idx_u16];
	int16_t b = osc->p_wavetable[idx_u16 + 1];
	int16_t sample = (1.0f - frac) * a + frac * b;

	osc->phase += osc->phase_increment;

	if (osc->phase >= WAVETABLE_STD_SIZE - 1) osc->phase -= WAVETABLE_STD_SIZE;

	return sample;
}

void oscillator_init_oscillator(Oscillator* osc)
{
	osc->frequency = 0.0f;
	osc->phase = 0.0f;
	osc->phase_increment = 0.0f;
	osc->detune = 0.0f;
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

