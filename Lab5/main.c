/*
 * Lab5.c
 *
 * Created: 2021-03-09 14:42:00
 * Author : shirt
 */ 

#include <avr/io.h>
#include "TinyTimber.h"
#include "Controller.h"
#include "InterruptHandler.h"
#include "GUI.h"

int main(void)
{
	GUI gui = initGUI();
	Controller c = initController(&gui);
    InterruptHandler i = initInterruptHandler(&c);
    INSTALL(&i, parse_interrupt, IRQ_PCINT0);
    //INSTALL(&i, parse_interrupt, IRQ_PCINT1);
    return TINYTIMBER(&i, init, 0);
}

