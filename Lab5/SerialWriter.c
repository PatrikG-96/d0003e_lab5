
#include "SerialWriter.h"

void usart_write(SerialWriter *self, int arg0) {
	self->output = arg0;
	while ( !(UCSR0A & ( 1<< UDRE0)) );  
	UDR0 = self->output;
}