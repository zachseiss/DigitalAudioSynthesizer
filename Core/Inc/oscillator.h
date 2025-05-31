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
	float frequency;
	float phase;
	float phase_increment;
	int16_t *wave_table;
}Oscillator;

// PUBLIC API FUNCTION DECLARATIONS
void oscillator_init_oscillator(Oscillator* osc, int16_t* wavetable);

void oscillator_set_waveform(Oscillator* osc, WaveformType type);

int16_t oscillator_process(Oscillator* osc);

#endif /* INC_OSCILLATOR_H_ */
