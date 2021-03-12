/*
 * Joystick.c
 *
 * Created: 2021-03-01 02:18:27
 *  Author: shirt
 */ 

#include "Joystick.h"

int joystick() {
	
	
	if (~PINB & (1 << 7)) {
		return STICK_DOWN;
	}
	
	if (~PINB & (1 << 4)) {
		return STICK_PRESSED;
	}
	
	if (~PINB & (1 << 6)) {
		return STICK_UP;
	}
	
	if (~PINE & (1 << 3)) {
		return STICK_RIGHT;
	}
	
	if (~PINE & (1 << 2)) {
		return STICK_LEFT;
	}
	
	return STICK_CENTER;
}

void initJoystick() {

	PORTB = PORTB | ((1 << 7)| (1 << 6 ) | (1 << 4));
	PORTE = PORTE | ( (1 << 2) | ( 1 << 3));
	
	
	// Enables external interrupt and sets the interrupt to PCINT15..8
	EIMSK |= (1 << PCIE1) | (1 << PCIE0);
	
	// Enables interrupt on PCINT15
	//PCMSK1 |= (1 << PCINT15)|(1 << PCINT14)|(1 << PCINT12);

	//PCMSK0 |= (1 << PCINT3)|(1 << PCINT2);
	PCMSK0 |= (1 << PCINT2);
	// Sets interrupt control to generate an interruption on a falling edge
	EICRA |= (1 << ISC01);
	
}