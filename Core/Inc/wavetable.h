/*
 * wavetable.h
 *
 *  Created on: May 19, 2025
 *      Author: zach
 */

#ifndef INC_WAVETABLE_H_
#define INC_WAVETABLE_H_

#include "stdint.h"

#define FIXED_POINT_MASK (0x7FFF)
#define NUM_WAVETABLES 4
#define WAVETABLE_REDUCED_SIZE 128
#define WAVETABLE_STD_SIZE 256
#define SHIFT 15  //  shift for phase and phase_increment to format uint32_t as Q17.15 fixed point

typedef enum
{
	WAVEFORM_SINE,
	WAVEFORM_SQUARE,
	WAVEFORM_SAW,
	WAVEFORM_TRIANGLE
}WaveformType;

// PUBLIC API FUNCTION DECLARATIONS
void wavetable_init_wavetables(void);



#endif /* INC_WAVETABLE_H_ */
