/*
 * InterruptHandler.c
 *
 * Created: 2021-03-09 14:43:54
 *  Author: shirt
 */ 

#include "InterruptHandler.h"

void parse_interrupt(InterruptHandler *self, int arg0) {
	
	uint8_t data = UDR0;
	
	if ((data >> NORTH_ARRIVAL_BIT ) & 1) {
		ASYNC(self->controller, enqueue_north, 0);
	}
	if ((data >> NORTH_ENTRY_BIT ) & 1) {
		ASYNC(self->controller, enter_lane, 0);
	}
	if ((data >> SOUTH_ARRIVAL_BIT ) & 1) {
		ASYNC(self->controller, enqueue_south, 0);
	}
	if ((data >> SOUTH_ENTRY_BIT) & 1) {
		ASYNC(self->controller, enter_lane, 0);
	}

	
}

void init(InterruptHandler *self, int arg0) {
	
	unsigned int ubrr = MYUBRR;
	
	// Set BAUD rate
	//UBRR0H = (unsigned char) (ubrr>>8);
	//UBRR0L = (unsigned char) ubrr;
	
	// Enable reciever and transmitter
	//UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	
	// Set frame format: 8 data, 1 stop bit
	//UCSR0C = ~(1<<USBS0) | (3 << UCSZ00);
	UBRR0H = ubrr>>8;
	UBRR0L = ubrr;
	UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (1<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00);
	
	ASYNC(self->controller->gui, start_gui, 0);
	//UDR0 = 5;
}