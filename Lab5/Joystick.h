/*
 * Joystick.h
 *
 * Created: 2021-03-01 02:18:17
 *  Author: shirt
 */ 


#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <avr/io.h>

#define STICK_CENTER 0
#define STICK_RIGHT 1
#define STICK_LEFT 2
#define STICK_UP 3
#define STICK_DOWN 4
#define STICK_PRESSED 5


int joystick();

void initJoystick();


#endif /* JOYSTICK_H_ */