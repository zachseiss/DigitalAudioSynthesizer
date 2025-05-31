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

// global synth parameters
SynthParams synth_params;

// PRIVATE CONSTANTS

float* const synth_param_ptrs[] =
{
		&synth_params.detune,
		&synth_params.num_oscillators
};


#define NUM_SYNTH_PARAMS sizeof(synth_param_ptrs) / sizeof(synth_param_ptrs[0])


// PRIVATE VARIABLES
int16_t wavetables[NUM_WAVETABLES][WAVETABLE_STD_SIZE];
float voice_gain[] =
{
		0.0f,
		1/1.0f,
		1/2.0f,
		1/3.0f,
		1/4.0f,
		1/5.0f,
		1/6.0f,
		1/2.8f,
		1/3.1f,
		1/3.4f,
		1/3.7f,
		1/4.0f,
		1/4.3f,
		1/4.6f,
		1/4.9f,
		1/5.2f
};


Voice voices[MIDI_KEY_MAX];

// PRIVATE FUNCTION PROTOTYPES
static void fill_audio_buffer(uint32_t*, Voice*, uint8_t);


// PUBLIC API FUNCTION DEFINITIONS
void synth_init_synth(void)
{
	synth_set_parameter(DETUNE, 0.0f);
	synth_set_parameter(NUM_OSCILLATORS, 1.0f);
	wavetable_init_wavetables(wavetables);
	voice_init_voices(voices, wavetables);
}

void synth_note_on(uint8_t note, float velocity)
{
	voices[note].is_active = 1;
	voices[note].velocity = velocity;
}

void synth_note_off(uint8_t note)
{
	voices[note].is_active = 0;
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
	int16_t final_sample = 0;


	uint16_t start = is_half ? 0 : HALF_BUFFER;
	uint16_t end = is_half ? HALF_BUFFER : AUDIO_BUFFER_SIZE;

	for (size_t i = start; i < end; i += 2)
	{
		uint8_t num_voices = 0;
		int32_t sample = 0;

		for (size_t j = MIDI_KEY_MIN; j < MIDI_KEY_MAX; j++)  // iterate through all voices
		{
			if (voices[j].is_active)
			{
				sample += oscillator_process(voices[j].oscillator) * (float)voices[j].velocity;
				num_voices += 1;
			}
		}

		if (num_voices > 0)
			final_sample = (int16_t)(sample * voice_gain[num_voices]);
		else
			final_sample = 0;

		// fill buffer
		buf[i] = final_sample;         // Left stereo sample
		buf[(i + 1)] = final_sample;   // Right stereo sample
	}
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

