/*
 * GUI.c
 *
 * Created: 2021-03-11 00:35:11
 *  Author: shirt
 */ 
#include "GUI.h"

void update_north(GUI *self, int arg0) {

	printAt(arg0, 0);
	
}

void update_current(GUI *self, int arg0) {
	

	
	printAt(arg0, 2);
	
}


void update_south(GUI *self, int arg0) {
	

	
	printAt(arg0, 4);
	
}

void start_gui(GUI *self, int arg0) {
	if (!self->initialized) {
		initLCD();
	}
	printAt(0, 0);
	printAt(0, 2);
	printAt(0, 4);
}