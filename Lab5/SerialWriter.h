/*
 * SerialWriter.h
 *
 * Created: 2021-03-09 14:46:06
 *  Author: shirt
 */ 


#ifndef SERIALWRITER_H_
#define SERIALWRITER_H_


#define LIGHT_BIT_N_G 0
#define LIGHT_BIT_N_R 1
#define LIGHT_BIT_S_G 2
#define LIGHT_BIT_S_R 3


#include <avr/io.h>
#include "TinyTimber.h"

typedef struct {
	Object super;
	int output;
}SerialWriter;

#define initSerialWriter() {initObject(), 0}

void set_output(SerialWriter *self, int arg0);

#endif /* SERIALWRITER_H_ */