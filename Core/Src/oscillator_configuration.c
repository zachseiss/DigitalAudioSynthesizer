/*
 * oscillator_configuration.c
 *
 *  Created on: Jun 1, 2025
 *      Author: Zach S
 *
 * This file deals with controlling how many oscillators each voice will get and what their parameter settings will be
 */

#include "oscillator_configuration.h"
#include "wavetable.h"
#include "synth.h"  // for debugging delete

#include <string.h>
#include <stdio.h>

extern int16_t* p_wavetables[];
extern UART_HandleTypeDef huart1;
extern OscillatorConfiguration oscillator_configuration;

static char msg[50];
static void init_msg(void);

// PUBLIC API
void init_oscillator_configuration(void)
{
	for (uint8_t i = 0; i < NUM_OSCILLATORS; i++)
	{
		if (i < 1)
		{
			oscillator_configuration.is_active[i] = 1;
		}
		else
		{
			oscillator_configuration.is_active[i] = 0;
		}

		oscillator_configuration.p_wavetable[i] = p_wavetables[WAVEFORM_SINE];

		if (i == 1)
		{
			oscillator_configuration.detune[i] = 12;
		}
		else
		{
			oscillator_configuration.detune[i] = 0;
		}
	}
}

void set_oscillator_activate(int16_t* p_wavetable, uint8_t osc_index)
{
	oscillator_configuration.p_wavetable[osc_index] = p_wavetable;
	oscillator_configuration.is_active[osc_index] = 1;
}

void set_oscillator_deactivate(uint8_t osc_index)
{
	oscillator_configuration.is_active[osc_index] = 0;
}

void osc_conf_to_string(OscillatorConfiguration *osc_conf)
{
	init_msg();
	sprintf(msg, "\t\tOSCILLATOR CONFIGURATION\r\n");
	HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
	init_msg();
	sprintf(msg, "is_active: ");
	HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
	init_msg();

	for (int i = 0; i < NUM_OSCILLATORS; i++)
	{
		sprintf(msg, " %d", osc_conf->is_active[i]);
		HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
		init_msg();
	}
	sprintf(msg, "\r\n");
	HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
	init_msg();

}

static void init_msg(void)
{
	memset(msg, '\0', sizeof(msg));
}
