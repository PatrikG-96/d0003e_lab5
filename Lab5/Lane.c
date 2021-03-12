/*
 * Lane.c
 *
 * Created: 2021-03-10 14:57:07
 *  Author: shirt
 */ 

#include "Lane.h"

void enter_lane(Lane *self, int arg0) {
	
	self->current_cars++;	// Another car is currently on the lane
	ASYNC(self->gui, update_current, self->current_cars);
	AFTER(MSEC(CAR_PASSING_TIME), self, exit_lane, 0);	// It takes 5 seconds for the car to cross the bridge
	
}

void exit_lane(Lane *self, int arg0) {
	
	self->current_cars--;	// Car has exited the bridge
	
	if (self->current_cars == 0) {
		ASYNC(self->controller, swap_lights, 0); // Lane is empty, it's safe to swap lights
	}
	ASYNC(self->gui, update_current, self->current_cars);
}

int get_current_cars(Lane *self, int arg0) {
	return self->current_cars;
}