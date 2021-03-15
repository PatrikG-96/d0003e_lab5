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
#include "SerialWriter.h"

int main(void)
{
	//return 0;
	SerialWriter wr = initSerialWriter();
	GUI gui = initGUI();
	Controller c = initController(&gui, &wr);
    InterruptHandler i = initInterruptHandler(&c);
    INSTALL(&i, parse_interrupt, IRQ_USART0_RX);
    return TINYTIMBER(&i, init, 0);
}

