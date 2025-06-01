/*
 * wavetable.h
 *
 *  Created on: May 19, 2025
 *      Author: zach
 */

#ifndef INC_WAVETABLE_H_
#define INC_WAVETABLE_H_

#include "stdint.h"


#define NUM_WAVETABLES 4
#define WAVETABLE_REDUCED_SIZE 128
#define WAVETABLE_STD_SIZE 256

typedef enum
{
	WAVEFORM_SINE,
	WAVEFORM_SQUARE,
	WAVEFORM_SAW,
	WAVEFORM_TRIANGLE
}WaveformType;


// PUBLIC API FUNCTION DECLARATIONS
void wavetable_init_wavetables(int16_t[][WAVETABLE_STD_SIZE]);



#endif /* INC_WAVETABLE_H_ */
