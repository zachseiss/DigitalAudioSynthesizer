/*
 * voice.c
 *
 *  Created on: May 19, 2025
 *      Author: Zach S
 */


#include "voice.h"
#include "wavetable.h"

extern OscillatorConfiguration oscillator_configuration;
extern int16_t* p_wavetables;

// PUBLIC API
void voice_init_voices(Voice* voices)
{
	for (int i = MIDI_KEY_MIN; i < MIDI_KEY_MAX + 1; i++)
	{
		voices[i].is_active = 0;
		voices[i].velocity = 0;

		for (int j = 0; j < NUM_OSCILLATORS; j++)
		{
			Oscillator *p_osc = &(voices[i].oscillator[j]);

			if (oscillator_configuration.is_active[j])
			{
				oscillator_init_oscillator(p_osc);
				p_osc->is_active = 1;
				uint8_t detune = oscillator_configuration.detune[j];
				p_osc->frequency = 440.0f * powf(2.0f, (i - detune - 69.0f) / 12.0f);
				p_osc->phase_increment = WAVETABLE_STD_SIZE * voices[i].oscillator[j].frequency / SAMPLE_RATE;
				p_osc->p_wavetable = oscillator_configuration.p_wavetable[j];
			}
		}
	}
}
