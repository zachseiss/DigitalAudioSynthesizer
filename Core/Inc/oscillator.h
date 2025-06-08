/*
 * oscillator.h
 *
 *  Created on: May 11, 2025
 *      Author: zach
 */

#ifndef INC_OSCILLATOR_H_
#define INC_OSCILLATOR_H_
#define MAX_UNISON_VOICES 8

#include "wavetable.h"


typedef struct
{
	uint8_t is_active;
	float frequency;
	uint32_t phase;
	uint32_t phase_increment;
	uint8_t detune;  // number of semi-tones de-tuned from frequency e.g. -12 is one octave down from frequency
	int16_t *p_wavetable;
}Oscillator;

// PUBLIC API FUNCTION DECLARATIONS
void oscillator_init_oscillator(Oscillator* osc);

void oscillator_set_waveform(Oscillator* osc, WaveformType type);  // should be defined in oscillator.c ???

void oscillator_set_detune(Oscillator* osc, uint8_t detune_semitones);  // should be defined in oscillator.c ???

void oscillator_to_string(Oscillator* osc);

int16_t oscillator_process(Oscillator* osc);

#endif /* INC_OSCILLATOR_H_ */
