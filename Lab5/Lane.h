/*
 * Lane.h
 *
 * Created: 2021-03-10 14:56:59
 *  Author: shirt
 */ 


#ifndef LANE_H_
#define LANE_H_

#include "TinyTimber.h"
#include "GUI.h"
#include "Controller.h"

#define MAX_CARS_ON_LANE 5
#define CAR_PASSING_TIME 5000
#define NEXT_CAR_TIME 1000

typedef struct {
	Object super;
	Controller *controller;
	GUI *gui;
	int current_cars;
}Lane;

#define initLane() {initObject(ctrlr, g), ctrlr, g, 0}

void enter_lane(Lane *self, int arg0);

void exit_lane(Lane *self, int arg0);

int get_current_cars(Lane *self, int arg0);

#endif /* LANE_H_ */