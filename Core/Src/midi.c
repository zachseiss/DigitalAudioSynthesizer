/*
 * midi.c
 *
 *  Created on: May 10, 2025
 *      Author: Zach Seiss
 */

#include "synth.h"
#include "midi.h"


// PRIVATE FUNCTION DEFINITIONS
static void process_midi_bytes(void)
{
    static uint8_t running_status = 0;
    static uint8_t data_byte1 = 0;
    static uint8_t waiting_for_second_data_byte = 0;

    if (rx_byte >= 0x80)  // rx_byte is a status byte
    {
        if (rx_byte == 0xF8) return; // Ignore MIDI clock

        running_status = rx_byte;
        waiting_for_second_data_byte = 0;
        return;
    }

    // Now rx_byte is a data byte (0x00..0x7F)

    if (running_status == 0)
    {
        // Got a data byte but no known running status. Ignore it.
        return;
    }


    if (!waiting_for_second_data_byte)
    {
        data_byte1 = rx_byte;
        waiting_for_second_data_byte = 1;
    }
    else
    {
        uint8_t data_byte2 = rx_byte;
        waiting_for_second_data_byte = 0;

        if ((running_status & 0xF0) == 0x90) // Note on (channel 0-15)
        {
        	switch (running_status)
        	{
        		case 0x90:
        			synth_set_parameter(DRUM_ACTIVE, 0.0f);

					if (data_byte2 == 0)
					{
						synth_set_parameter(AMPLITUDE_TARGET, 0.0f);
					}
					else
					{
						synth_set_parameter(FREQUENCY, 440.0f * powf(2.0f, (data_byte1 - 69.0f) / 12.0f));
						synth_set_parameter(AMPLITUDE_TARGET, data_byte2 / 127.0f);
					}
					break;
        		case 0x99:
        			synth_set_parameter(FREQUENCY, 150.0f);
					synth_set_parameter(AMPLITUDE_TARGET, data_byte2 / 127.0f);
					synth_set_parameter(LFO_ACTIVE, 0.0f);
					synth_set_parameter(DRUM_ACTIVE, 1.0f);
					if (data_byte2 ^ 0x00) synth_set_parameter(PITCH_DECAY, 1.0f);
					break;
        	}
        }
        else if ((running_status & 0xF0) == 0xE0) // Pitch bend
        {
            uint16_t pitch_value = (data_byte2 << 7) | data_byte1;
            int32_t centered = (int32_t)pitch_value - 8192;
            synth_set_parameter(PITCH_BEND, (float)centered / 8192.0f);
        }
        else if ((running_status & 0xF0) == 0xB0) // Control change
        {
            if (data_byte1 == 0x15)
            {
            	synth_set_parameter(LFO_ACTIVE, data_byte2 >= 1 ? 1.0f : 0.0f);
            	synth_set_parameter(LFO_FREQUENCY, (float)data_byte2);
            }
            else if (data_byte1 == 0x16)
            {
            	synth_set_parameter(LFO_DEPTH, (float)data_byte2);
            }
        }
    }
}


static void start_midi_reception(void)
{
	HAL_UART_Receive_IT(&huart2, (uint8_t*)&rx_byte, 1);
}


// HAL CALLBACKS
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart -> Instance == USART2)
	{
		process_midi_bytes();
		start_midi_reception();
	}
}

