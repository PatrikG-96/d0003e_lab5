/*
 * Controller.h
 *
 * Created: 2021-03-09 14:42:32
 *  Author: shirt
 */ 


#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#define NORTH 0
#define SOUTH 1
#define MAX_CARS_ON_LANE 5
#define CAR_PASSING_TIME 5000
#define NEXT_CAR_TIME 1000
#define TRUE 1
#define FALSE 0
#define RED 0
#define GREEN 1



#include <stdint-gcc.h>
#include <stdbool.h>

#include "TinyTimber.h"
#include "GUI.h"
#include "SerialWriter.h"

typedef struct {
	Object super;
	GUI *gui;
	SerialWriter *writer;
	int queues[2];
	uint8_t traffic_lights[2];
	uint8_t current_direction;
	uint8_t cars_allowed;
	int current_cars;
	bool active;
}Controller;

#define initController(gui, wr) {initObject(), gui, wr, {0,0}, {0,0} , 0, 0, 0}
	
void enqueue_north(Controller *self, int arg0);

void enqueue_south(Controller *self, int arg0);

void swap_lights(Controller *self, int arg0);

void manage_lights(Controller *self, int arg0);

void enter_lane(Controller *self, int arg0);

void exit_lane(Controller *self, int arg0);


#endif /* CONTROLLER_H_ */