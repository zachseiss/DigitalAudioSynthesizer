/*
 * midi.c
 *
 *  Created on: May 10, 2025
 *      Author: Zach S
 */

#include "synth.h"
#include "midi.h"

// PRIVATE DEFS
#define TYPE_BITMASK    0xF0
#define SUBTYPE_BITMASK 0x0F
#define KEY_NOTE        0x00
#define DRUM_NOTE       0x09
#define CLK_MSG         0xF8
#define DATA_BYTE_CEIL  0x7F
#define NOTE_ON         0x90
#define PITCH_BEND      0xE0
#define CONTROL_CHANGE  0xB0
#define CC_021          0x15
#define CC_022          0x16
#define CC_023          0x17
#define CC_024          0x18


// TYPEDEFS
typedef void (*MidiControlFunction)(uint8_t value);
typedef enum
{
	WAIT_STATUS,
	WAIT_DATA1,
	WAIT_DATA2
} MidiStateMachine;

// PRIVATE VARIABLES
MidiControlFunction control_change_handlers[128] = {0};

// PRIVATE FUNCTION DEFINITIONS
static void process_midi_bytes(void)
{
	static MidiStateMachine midi_state = WAIT_STATUS;
    static uint8_t running_status = 0;
    static uint8_t data_byte1 = 0;
    static uint8_t current_note = 0;
    static float lfo_active_save_state; // saves previous LFO on/off state when we need to turn it off for drums

    if (rx_byte == CLK_MSG) return;  // ignore clock bytes


    if (rx_byte > DATA_BYTE_CEIL)  // rx_byte is a status byte
    {
        running_status = rx_byte;
        midi_state = WAIT_DATA1;
        return;
    }

    switch (midi_state)
    {
    	case WAIT_STATUS:
    		// data byte but no status byte, no choice but to ignore
    		break;

    	case WAIT_DATA1:
    		data_byte1 = rx_byte;
    		midi_state = WAIT_DATA2;
    		break;

    	case WAIT_DATA2:
    		uint8_t data_byte2 = rx_byte;

    		// at this point we have a full 2 or 3 byte message that can be processed
    		uint8_t status_type = running_status & TYPE_BITMASK;
    		switch (status_type)
    		{
    			case NOTE_ON:
    				// now need to determine what kind of note (key, drum, etc)
    				uint8_t status_subtype = running_status & SUBTYPE_BITMASK;
    				switch (status_subtype)
    				{
    					case KEY_NOTE:
    	        			synth_set_parameter(DRUM_ACTIVE, 0.0f);

    						if (data_byte2 == 0 && data_byte1 == current_note)
    						{
    							synth_set_parameter(AMPLITUDE_TARGET, 0.0f);
    						}
    						else if (data_byte2 != 0)
    						{
    							synth_set_parameter(FREQUENCY, 440.0f * powf(2.0f, (data_byte1 - 69.0f) / 12.0f));
    							synth_set_parameter(AMPLITUDE_TARGET, data_byte2 / 127.0f);
    							current_note = data_byte1;
    						}
    						break;

    					case DRUM_NOTE:
    	        			synth_set_parameter(FREQUENCY, 150.0f);
    						synth_set_parameter(AMPLITUDE_TARGET, data_byte2 / 127.0f);
    						lfo_active_save_state = synth_get_parameter(LFO_ACTIVE);
    						synth_set_parameter(LFO_ACTIVE, 0.0f);
    						synth_set_parameter(DRUM_ACTIVE, 1.0f);
    						if (data_byte2 ^ 0x00)
    						{
    							synth_set_parameter(PITCH_DECAY, 1.0f);
    							synth_set_parameter(LFO_ACTIVE, lfo_active_save_state);
    						}
    						break;
    				}

    			case PITCH_BEND:
    	            uint16_t pitch_value = (data_byte2 << 7) | data_byte1;
    	            int32_t centered = (int32_t)pitch_value - 8192;
    	            synth_set_parameter(PITCH_BEND, (float)centered / 8192.0f);

    			case CONTROL_CHANGE:
    	        	if (control_change_handlers[data_byte1])
    	        	{
    	        		control_change_handlers[data_byte1](data_byte2);
    	        	}
    		}

    }
}


static void start_midi_reception(void)
{
	HAL_UART_Receive_IT(&huart2, (uint8_t*)&rx_byte, 1);
}

static void handle_lfo_frequency(uint8_t value)
{
	synth_set_parameter(LFO_ACTIVE, value >= 1 ? 1.0f : 0.0f);
	synth_set_parameter(LFO_FREQUENCY, (float)value);
}

static void handle_lfo_depth(uint8_t value)
{
	synth_set_parameter(LFO_DEPTH, (float)value);
}

static void handle_attack(uint8_t value)
{
	synth_set_parameter(ATTACK, value == 0 ? 0.005f : 0.000025 * value);
}

static void handle_decay(uint8_t value)
{
	synth_set_parameter(DECAY, value == 0 ? .99995f : 1.0f - .0003 * value);
}

// PUBLIC API FUNCTION DEFINITIONS
void init_midi_handlers(void)
{
	control_change_handlers[CC_021] = handle_lfo_frequency;
	control_change_handlers[CC_022] = handle_lfo_depth;
	control_change_handlers[CC_023] = handle_attack;
	control_change_handlers[CC_024] = handle_decay;
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

