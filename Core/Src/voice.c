/*
 * voice.c
 *
 *  Created on: May 19, 2025
 *      Author: Zach S
 */


#include "voice.h"

extern Voice voices;

// PUBLIC API
void voice_init_voices(Voice* voices, int16_t wavetables[][WAVETABLE_STD_SIZE])
{
	uint8_t num_oscillators = (uint8_t)synth_get_parameter(NUM_OSCILLATORS);

	for (int i = MIDI_KEY_MIN; i < MIDI_KEY_MAX + 1; i++)
	{
		voices[i].is_active = 0;
		voices[i].velocity = 0;

		for (int j = 0; j < num_oscillators; j++)
		{
			oscillator_init_oscillator(&(voices[i].oscillator[j]), wavetables[WAVEFORM_TRIANGLE]);

			voices[i].oscillator[j].frequency = 440.0f * powf(2.0f, (i - 69.0f) / 12.0f);
			voices[i].oscillator[j].phase_increment = WAVETABLE_STD_SIZE * voices[i].oscillator[j].frequency / SAMPLE_RATE;
		}
	}
}
