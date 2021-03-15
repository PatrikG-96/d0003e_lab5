/*
 * SerialWriter.c
 *
 * Created: 2021-03-09 14:46:18
 *  Author: shirt
 */ 

#include "SerialWriter.h"

void usart_write(SerialWriter *self, int arg0) {
	self->output = arg0;
	UDR0 = self->output;
}