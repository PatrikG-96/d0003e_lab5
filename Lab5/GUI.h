/*
 * GUI.h
 *
 * Created: 2021-03-11 00:35:35
 *  Author: shirt
 */ 


#ifndef GUI_H_
#define GUI_H_

#include "TinyTimber.h"
#include "LCDDrivers.h"

typedef struct {
	Object super;
	uint8_t initialized;
}GUI;

#define initGUI() {initObject(), 0}

void update_north(GUI *self, int arg0);

void update_south(GUI *self, int arg0);

void update_current(GUI *self, int arg0);

void start_gui(GUI *self, int arg0);

#endif /* GUI_H_ */