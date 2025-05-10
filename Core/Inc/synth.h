/*
 * synth.h
 *
 *  Created on: May 9, 2025
 *      Author: zach
 */

#include <stdint.h>
#include <math.h>
#include "stm32f4xx_hal.h"


#ifndef INC_SYNTH_H_
#define INC_SYNTH_H_

#define WAVETABLE_STD_SIZE 1024
#define WAVETABLE_REDUCED_SIZE 128
#define AUDIO_BUFFER_SIZE 32
#define HALF_BUFFER 16
#define FREQUENCY_CORRECTION 1.01
#define SAMPLE_RATE 48000.0f
#define TWO_PI 6.283185f
#define ATTACK_RATE 0.005f

enum param_id
{
	FREQUENCY,
	AMPLITUDE_TARGET,
	DECAY,
	LFO_FREQUENCY,
	LFO_DEPTH,
	LFO_PHASE,
	PITCH_BEND,
	PITCH_DECAY,
	PITCH_DECAY_DELTA,
	PITCH_DECAY_LOWER_LIMIT,
	LFO_ACTIVE,
	DRUM_ACTIVE
};

typedef struct
{
	float frequency;
	float amplitude_target;
	float decay;            // amplitude decay
	float lfo_frequency;
	float lfo_depth;
	float lfo_phase;
	float pitch_bend;
	float pitch_decay;
	float pitch_decay_delta;  // the change in pitch-decay with each iteration/sample
	float pitch_decay_lower_limit;  // when pitch-decay goes below lower limit, it will reset to 1.0f
	float lfo_active;    // fake bool
	float drum_active;   // fake bool
}SynthParams;

// Globals
extern I2S_HandleTypeDef hi2s3;
extern uint32_t i2s_tx_buffer[AUDIO_BUFFER_SIZE];


// Public API
void synth_init(void);

void synth_set_parameter(uint8_t param_id, float val);

void synth_note_on(uint8_t note, uint8_t velocity);

void synth_note_off(uint8_t note);

#endif /* INC_SYNTH_H_ */
