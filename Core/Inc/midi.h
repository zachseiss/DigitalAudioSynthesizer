/*
 * midi.h
 *
 *  Created on: May 10, 2025
 *      Author: Zach S
 */

#ifndef INC_MIDI_H_
#define INC_MIDI_H_


// Globals
extern DMA_HandleTypeDef hdma_spi3_tx;
extern UART_HandleTypeDef huart2;
extern uint8_t rx_byte;

// PUBLIC API FUNCTION DECLARATIONS
void init_midi_handlers(void);


#endif /* INC_MIDI_H_ */
