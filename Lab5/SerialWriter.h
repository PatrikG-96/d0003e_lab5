
#ifndef SERIALWRITER_H_
#define SERIALWRITER_H_


#define NORTH_GREEN 0
#define NORTH_RED 1
#define SOUTH_GREEN 2
#define SOUTH_RED 3


#include <avr/io.h>
#include "TinyTimber.h"

typedef struct {
	Object super;
	uint8_t output;
}SerialWriter;

#define initSerialWriter() {initObject(), 0}

void usart_write(SerialWriter *self, int arg0);

#endif /* SERIALWRITER_H_ */