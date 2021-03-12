/*
 * SerialWriter.h
 *
 * Created: 2021-03-09 14:46:06
 *  Author: shirt
 */ 


#ifndef SERIALWRITER_H_
#define SERIALWRITER_H_

#include <avr/io.h>

typedef struct {
	Object super;
}SerialWriter;

void USART_write(SerialWriter *self, int arg0);

#endif /* SERIALWRITER_H_ */