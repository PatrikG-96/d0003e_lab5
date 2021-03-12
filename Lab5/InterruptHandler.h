/*
 * InterruptHandler.h
 *
 * Created: 2021-03-09 14:43:45
 *  Author: shirt
 */ 


#ifndef INTERRUPTHANDLER_H_
#define INTERRUPTHANDLER_H_

#include "TinyTimber.h"
#include "Joystick.h"
#include "Controller.h"
#include <avr/io.h>

#define FOSC 8000000
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

#define NORTH_ARRIVAL_BIT 0
#define NORTH_ENTRY_BIT 1
#define SOUTH_ARRIVAL_BIT 2
#define SOUTH_ENTRY_BIT 3

typedef struct {
	Object super;
	Controller *controller;
}InterruptHandler;

#define initInterruptHandler(c) {initObject(), c}

void parse_interrupt(InterruptHandler *self, int arg0);
void init(InterruptHandler *self, int arg0);

#endif /* INTERRUPTHANDLER_H_ */