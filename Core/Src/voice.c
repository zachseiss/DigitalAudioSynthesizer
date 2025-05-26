/*
 * voice.c
 *
 *  Created on: May 19, 2025
 *      Author: zach
 */


#include "voice.h"

extern Voice voices;

// PUBLIC API
void voice_init_voices(Voice* voices, int16_t wavetables[][WAVETABLE_STD_SIZE])
{
	uint8_t num_oscillators = synth_get_parameter(NUM_OSCILLATORS);

	for (int i = MIDI_KEY_MIN; i < MIDI_KEY_MAX + 1; i++)
	{

		for (int j = 0; j < num_oscillators; j++)
		{
			// later this needs to be modified to account for multiple unison voices, detune, etc
			oscillator_init_oscillator(&(voices[i].oscillator[j]), wavetables[WAVEFORM_SINE]);

			uint8_t num_unison_voices = (uint8_t)synth_get_parameter(UNISON);
			for (int k = 0; k < num_unison_voices; k++)
			{
				float freq_k = 440.0f * powf(2.0f, (i - 69.0f) / 12.0f);
				voices[i].oscillator[j].frequency[k] = freq_k;
				voices[i].oscillator[j].phase_increment[k] = (float)WAVETABLE_STD_SIZE * freq_k / SAMPLE_RATE;
			}
			voices[i].is_active = 0;
		}
	}
}
