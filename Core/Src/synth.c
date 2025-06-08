/*
 * synth.c
 *
 *  Created on: May 9, 2025
 *      Author: Zach S
 */

#include <math.h>
#include "synth.h"
#include "wavetable.h"
#include "voice.h"
#include "oscillator_configuration.h"

#include <string.h>
#include <stdio.h>

// Global Variables
SynthParams synth_params;
OscillatorConfiguration oscillator_configuration;
int16_t* p_wavetables[NUM_WAVETABLES];
int16_t sine_wavetable[WAVETABLE_STD_SIZE];
int16_t square_wavetable[WAVETABLE_STD_SIZE];
int16_t saw_wavetable[WAVETABLE_STD_SIZE];
int16_t triangle_wavetable[WAVETABLE_STD_SIZE];

// Externals
extern UART_HandleTypeDef huart1;

// PRIVATE CONSTANTS

float* const synth_param_ptrs[] =
{
		&synth_params.detune,
};


#define NUM_SYNTH_PARAMS sizeof(synth_param_ptrs) / sizeof(synth_param_ptrs[0])


// PRIVATE VARIABLES
static char msg[50];

float voice_gain[] =
{
		0.0f,
		1/1.0f,
		1/2.0f,
		1/3.0f,
		1/4.0f,
		1/5.0f,
		1/6.0f,
		1/7.0f,
		1/8.0f,
		1/9.0f,
		1/10.0f,
		1/11.0f,
		1/12.0f,
		1/13.0f,
		1/14.0f,
		1/15.0f
};

uint8_t num_active_voices;

Voice voices[MIDI_KEY_MAX];
uint8_t active_voice_index[POLYPHONY_MAX];


// PRIVATE FUNCTION PROTOTYPES
static void fill_audio_buffer(uint32_t*, Voice*, uint8_t);
static void init_msg(void);
static void init_int8_array(int8_t[], size_t, int8_t);
static void init_uint8_array(uint8_t[], size_t, uint8_t);



// PUBLIC API FUNCTION DEFINITIONS
void synth_init_synth(void)
{
	synth_set_parameter(DETUNE, 0.0f);

	init_uint8_array(active_voice_index, POLYPHONY_MAX, 0);

	wavetable_init_wavetables();     // initializing before voices caused a memory overwrite error that I still haven't figured out yet

	init_oscillator_configuration();  // must be initialized before voices in current version

	osc_conf_to_string(&oscillator_configuration);

	voice_init_voices(voices);

	for (int i = MIDI_KEY_MIN; i < MIDI_KEY_MAX; i++)
	{
		sprintf(msg, "VOICE NUMBER %d\r\n\n", i);
		HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
		init_msg();

		for (int j = 0; j < NUM_OSCILLATORS; j++)
		{
			sprintf(msg, "\tosc: %d", j);
			HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
			oscillator_to_string(&voices[i].oscillator[j]);
			init_msg();
		}
	}
	sprintf(msg, "\r\n");
	HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
	init_msg();

	for (int i = 0; i < WAVETABLE_STD_SIZE; i++)
	{
		sprintf(msg, "sine_wavetable[%d]:     %d\n\r", i, voices[60].oscillator[0].p_wavetable[i]);
		HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
		init_msg();
	}
}

void synth_note_on(uint8_t note, float velocity)
{
	if (num_active_voices < POLYPHONY_MAX)
	{
		voices[note].velocity = velocity;
		active_voice_index[num_active_voices++] = note;
	}
}

void synth_note_off(uint8_t note)
{
	for (size_t i = 0; i < num_active_voices; i++)
	{
		if (active_voice_index[i] == note)
			active_voice_index[i] = active_voice_index[--num_active_voices];
	}
}

void synth_set_parameter(uint8_t param_id, float val)
{
	if (param_id < NUM_SYNTH_PARAMS && synth_param_ptrs[param_id] != NULL)
	{
		*synth_param_ptrs[param_id] = val;
	}
}

float synth_get_parameter(uint8_t param_id)
{
	return *synth_param_ptrs[param_id];
}

// PRIVATE FUNCTIONS

static void fill_audio_buffer(uint32_t *buf, Voice* voices, uint8_t is_half)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
	int16_t final_sample = 0;


	uint16_t start = is_half ? 0 : HALF_BUFFER;
	uint16_t end = is_half ? HALF_BUFFER : AUDIO_BUFFER_SIZE;

	for (size_t i = start; i < end; i += 2)
	{
		int32_t sample = 0;

		for (size_t j = 0; j < num_active_voices; j++)  // iterate through all voices
		{
			uint8_t active_osc = 0;
			uint8_t voice_idx = active_voice_index[j];
			int32_t inner_sample = 0;
			for (size_t k = 0; k < NUM_OSCILLATORS; k++)
			{
				active_osc += oscillator_configuration.is_active[k];
				inner_sample += oscillator_process(&voices[voice_idx].oscillator[k]) * (float)voices[voice_idx].velocity;
			}
			sample += (int32_t)(inner_sample * voice_gain[active_osc]);
		}

		if (num_active_voices > 0)
			final_sample = (int16_t)(sample / (float)num_active_voices);
		else
			final_sample = 0;

		// fill buffer
		buf[i] = final_sample;         // Left stereo sample
		buf[(i + 1)] = final_sample;   // Right stereo sample
	}
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
}

static void init_int8_array(int8_t array[], size_t length, int8_t fill_value)
{
	for (size_t i = 0; i < length; i++)
	{
		array[i] = fill_value;
	}
}

static void init_uint8_array(uint8_t array[], size_t length, uint8_t fill_value)
{
	for (size_t i = 0; i < length; i++)
	{
		array[i] = fill_value;
	}
}

static void init_msg(void)
{
	memset(msg, '\0', sizeof(msg));
}

// HAL CALLBACKS
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
	// Called when DMA has sent the last element of the buffer
	// at this point we want to re-fill the second half of the audio buffer
	if (hi2s == &hi2s3)
	{
		fill_audio_buffer(i2s_tx_buffer, voices, 0);
	}
}

void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
	// Called when DMA has sent the last element of the first half of the audio buffer
	// at this point we want to re-fill the first half of the audio buffer
	if (hi2s == &hi2s3)
	{
		fill_audio_buffer(i2s_tx_buffer, voices, 1);
	}
}

