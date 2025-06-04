/*
 * oscillator_configuration.h
 *
 *  Created on: Jun 1, 2025
 *      Author: zach
 */

#ifndef INC_OSCILLATOR_CONFIGURATION_H_
#define INC_OSCILLATOR_CONFIGURATION_H_

#include "stdint.h"

#define NUM_OSCILLATORS 3


typedef struct
{
	uint8_t is_active[NUM_OSCILLATORS];  // boolean array containing 1 if the i-th oscillator is "turned on" else 0
	uint8_t detune[NUM_OSCILLATORS];
	int16_t* p_wavetable[NUM_OSCILLATORS]; // array containing pointers to the wave-table for the i-th oscillator

}OscillatorConfiguration;

// PUBLIC API
void init_oscillator_configuration(void);

void set_oscillator_wavetable(uint8_t osc_index, uint8_t p_wavetable_index);

void set_oscillator_activate(int16_t*, uint8_t);

void set_oscillator_deactivate(uint8_t);

void osc_conf_to_string(OscillatorConfiguration*);


#endif /* INC_OSCILLATOR_CONFIGURATION_H_ */
