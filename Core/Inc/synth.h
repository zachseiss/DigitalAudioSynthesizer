/*
 * synth.h
 *
 *  Created on: May 9, 2025
 *      Author: Zach S
 */

#include <stdint.h>
#include <math.h>
#include "stm32f4xx_hal.h"
#include "oscillator.h"



#ifndef INC_SYNTH_H_
#define INC_SYNTH_H_

#define AUDIO_BUFFER_SIZE 16
#define HALF_BUFFER 8
#define FREQUENCY_CORRECTION 1.01
#define SAMPLE_RATE 43402.0f
#define TWO_PI 6.283185f

enum param_id
{
	DETUNE,
};

typedef struct
{
	float detune;
}SynthParams;

// Globals
extern I2S_HandleTypeDef hi2s3;
extern uint32_t i2s_tx_buffer[AUDIO_BUFFER_SIZE];



// Public API
void synth_init_synth(void);

void synth_set_parameter(uint8_t param_id, float val);

void oscillator_set_frequency(Oscillator* osc, float frequency);

float synth_get_parameter(uint8_t param_id);

void synth_note_on(uint8_t note, float velocity);

void synth_note_off(uint8_t note);

#endif /* INC_SYNTH_H_ */
