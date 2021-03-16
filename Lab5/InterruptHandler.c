
#include "InterruptHandler.h"

void parse_interrupt(InterruptHandler *self, int arg0) {
	
	
	/* Reads from USART port and interprets input according to standards in lab specs*/
	while ( !(UCSR0A & (1 << RXC0)) );
	uint8_t data = UDR0;
	
	if ((data >> NORTH_ARRIVAL_BIT ) & 1) {
		ASYNC(self->controller, enqueue_north, 0);
	}
	if ((data >> NORTH_ENTRY_BIT ) & 1) {
		ASYNC(self->controller, entry_north, 0);
	}
	if ((data >> SOUTH_ARRIVAL_BIT ) & 1) {
		ASYNC(self->controller, enqueue_south, 0);
	}
	if ((data >> SOUTH_ENTRY_BIT) & 1) {
		ASYNC(self->controller, entry_south, 0);
	}

	
}

void init(InterruptHandler *self, int arg0) {
	
	// Initialize USART
	unsigned int ubrr = MYUBRR;
	
	UBRR0H = ubrr>>8;
	UBRR0L = ubrr;
	UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (1<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00);
	
	ASYNC(self->controller->gui, start_gui, 0); // init the gui
}