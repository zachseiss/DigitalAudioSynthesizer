/*
 * synth.c
 *
 *  Created on: May 9, 2025
 *      Author: Zach S
 */

#include <math.h>
#include "synth.h"

// global synth parameters
SynthParams synth_params;

// PRIVATE CONSTANTS

float* const synth_param_ptrs[] =
{
		&synth_params.frequency,
		&synth_params.amplitude_target,
		&synth_params.attack,
		&synth_params.decay,
		&synth_params.lfo_frequency,
		&synth_params.lfo_depth,
		&synth_params.lfo_phase,
		&synth_params.pitch_bend,
		&synth_params.pitch_decay,
		&synth_params.pitch_decay_delta,
		&synth_params.pitch_decay_lower_limit,
		&synth_params.lfo_active,
		&synth_params.drum_active
};

#define NUM_PARAMS (sizeof(synth_param_ptrs) / sizeof(synth_param_ptrs[0]))


// PRIVATE VARIABLES
int16_t sine_table[WAVETABLE_STD_SIZE];
float lfo_table[WAVETABLE_REDUCED_SIZE];

// PRIVATE FUNCTION PROTOTYPES
static void get_current_amplitude(float*);
static float get_lfo_value(void);
static void init_sine_table(int16_t*, size_t);
static void init_lfo_table(float*, size_t);
static void fill_audio_buffer(uint32_t*, int16_t*,uint8_t);


// PUBLIC API FUNCTION DEFINITIONS
void synth_init(void)
{
	init_sine_table(sine_table, WAVETABLE_STD_SIZE);
	init_lfo_table(lfo_table, WAVETABLE_REDUCED_SIZE);
	synth_set_parameter(FREQUENCY, 0.0f);
	synth_set_parameter(AMPLITUDE_TARGET, 0.0f);
	synth_set_parameter(ATTACK, 0.005);
	synth_set_parameter(DECAY, 0.99995);
	synth_set_parameter(LFO_FREQUENCY, 0.0f);
	synth_set_parameter(LFO_DEPTH, 0.0f);
	synth_set_parameter(LFO_PHASE, 0.0f);
	synth_set_parameter(PITCH_BEND, 0.0f);
	synth_set_parameter(PITCH_DECAY, 1.0f);
	synth_set_parameter(PITCH_DECAY_DELTA, 0.999f);
	synth_set_parameter(PITCH_DECAY_LOWER_LIMIT, 0.01f);
	synth_set_parameter(LFO_ACTIVE, 0.0f);
	synth_set_parameter(DRUM_ACTIVE, 0.0f);
}

void synth_set_parameter(uint8_t param_id, float val)
{
	if (param_id < NUM_PARAMS && synth_param_ptrs[param_id] != NULL)
	{
		*synth_param_ptrs[param_id] = val;
	}
}

float synth_get_parameter(uint8_t param_id)
{
	return *synth_param_ptrs[param_id];
}


// PRIVATE FUNCTIONS

static void fill_audio_buffer(uint32_t *buf, int16_t *wavetable, uint8_t is_half)
{
	static float idx_f = 0.0f;
	float pitch_bend_multiplier = powf(2.0f, synth_params.pitch_bend * 0.1666f);

	static float amplitude_current = 0;  // set to 0 just to avoid 'uninitialized' warning
	get_current_amplitude(&amplitude_current);

	// calculate phase increment
	float base_phase_increment = synth_params.frequency * pitch_bend_multiplier * FREQUENCY_CORRECTION * WAVETABLE_STD_SIZE / SAMPLE_RATE;
	float phase_increment = base_phase_increment;

	if (synth_params.drum_active > 0.5)  // remember drum_active is a fake boolean
	{
		phase_increment *= synth_params.pitch_decay;
		synth_params.pitch_decay *= synth_params.pitch_decay_delta;

		if (synth_params.pitch_decay < synth_params.pitch_decay_lower_limit)
		{
			synth_params.drum_active = 0.0f;
			synth_params.pitch_decay = 1.0f;
			synth_params.frequency = 0.0f;
		}
	}

	if (synth_params.lfo_active > 0.5)  // will never be active if drum is active  (also remember fake boolean)
	{
		float modulated_frequency = synth_params.frequency + (get_lfo_value() * synth_params.lfo_depth);
		phase_increment = 0.5f * (base_phase_increment + modulated_frequency * WAVETABLE_STD_SIZE / SAMPLE_RATE);  // multiply by 1/2 because summing two waves
	}

	uint16_t start = is_half ? 0 : HALF_BUFFER;
	uint16_t end = is_half ? HALF_BUFFER : AUDIO_BUFFER_SIZE;

	for (size_t i = start; i < end; i += 2)
	{

		uint16_t idx_u16 = (uint16_t)idx_f;

		// linear interpolation
		float frac = idx_f - idx_u16;
		int16_t a = wavetable[idx_u16];
		int16_t b = wavetable[(idx_u16 + 1) % WAVETABLE_STD_SIZE];
		int16_t sample = amplitude_current * ((1.0f - frac) * a + frac * b);

		// fill buffer
		buf[i] = (uint16_t)sample;                             // Left stereo sample
		buf[(i + 1) % AUDIO_BUFFER_SIZE] = (uint16_t)sample;   // Right stereo sample
		idx_f = idx_f + phase_increment;

		if (idx_f >= WAVETABLE_STD_SIZE) idx_f -= WAVETABLE_STD_SIZE;
	}
}

static void get_current_amplitude(float *p_amplitude_current)
{
	// Ramp amplitude smoothly toward the target
	if (*p_amplitude_current < synth_params.amplitude_target)
	{
	    *p_amplitude_current += synth_params.attack;
	    if (*p_amplitude_current > synth_params.amplitude_target)
	        *p_amplitude_current = synth_params.amplitude_target;
	}
	else if (*p_amplitude_current >= synth_params.amplitude_target)
	{
	    *p_amplitude_current *= synth_params.decay;

	    if (*p_amplitude_current < synth_params.amplitude_target)
	        *p_amplitude_current = synth_params.amplitude_target;
	}
}


static float get_lfo_value()
{
	uint8_t index = ((uint8_t)synth_params.lfo_phase) % WAVETABLE_REDUCED_SIZE;
	float value = lfo_table[index];

	float lfo_phase_increment = TWO_PI * synth_params.lfo_frequency * WAVETABLE_REDUCED_SIZE / SAMPLE_RATE;

	synth_params.lfo_phase += lfo_phase_increment;

	if (synth_params.lfo_phase >= WAVETABLE_REDUCED_SIZE) synth_params.lfo_phase -= WAVETABLE_REDUCED_SIZE;

	return value;
}

static void init_sine_table(int16_t *wavetable, size_t length)
{
	const float PHASE_INCREMENT = TWO_PI / WAVETABLE_STD_SIZE;
	float phase = 0.0f;

	for (size_t i = 0; i < length; i += 1)
	{
		float sample_f = sinf(phase);
		int16_t sample_i16 = (int16_t)(sample_f * 32767.0f);
		wavetable[i] = sample_i16;
		phase += PHASE_INCREMENT;
	}
}

static void init_lfo_table(float *wavetable, size_t length)
{
	for (size_t i = 0; i < WAVETABLE_REDUCED_SIZE; i++)
	{
		wavetable[i] = sinf(TWO_PI * i / WAVETABLE_REDUCED_SIZE);
	}
}

// HAL CALLBACKS
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
	// Called when DMA has sent the last element of the buffer
	// at this point we want to re-fill the second half of the audio buffer
	if (hi2s == &hi2s3)
	{
		fill_audio_buffer(i2s_tx_buffer, sine_table, 0);
	}
}

void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
	// Called when DMA has sent the last element of the first half of the audio buffer
	// at this point we want to re-fill the first half of the audio buffer
	if (hi2s == &hi2s3)
	{
		fill_audio_buffer(i2s_tx_buffer, sine_table, 1);
	}
}

