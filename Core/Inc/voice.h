/*
 * voice.h
 *
 *  Created on: May 19, 2025
 *      Author: zach
 */

#ifndef INC_VOICE_H_
#define INC_VOICE_H_

#include "synth.h"
#include "oscillator_configuration.h"


#define MIDI_KEY_MIN 21
#define MIDI_KEY_MAX 127

// TYPEDEFS
typedef struct
{
	uint8_t is_active;
	float velocity;
	Oscillator oscillator[NUM_OSCILLATORS];
}Voice;


// PUBLIC API FUNCTION DECLARATIONS
void voice_init_voices(Voice*);


#endif /* INC_VOICE_H_ */
