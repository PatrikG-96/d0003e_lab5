/*
 * Controller.c
 *
 * Created: 2021-03-09 14:42:40
 *  Author: shirt
 */ 

#include "Controller.h"

void enqueue_north(Controller *self, int arg0) {

	self->queues[NORTH]++;
	ASYNC(self->gui, update_north, self->queues[NORTH]);
	if (!self->active) {
		self->active = true;
	}
	
}

void enqueue_south(Controller *self, int arg0) {

	self->queues[SOUTH]++;
	ASYNC(self->gui, update_south, self->queues[SOUTH]);
	if (!self->active) {
		self->active = true;
	}
	
}

void swap_lights(Controller *self, int arg0) {
	
	if (self->current_direction == NORTH) {
		self->current_direction = SOUTH;
	}
	else {
		self->current_direction = NORTH;
	}
	self->traffic_lights[self->current_direction] = TRUE;
	self->cars_allowed = MAX_CARS_ON_LANE;

}

void manage_lights(Controller *self, int arg0) {
	
	// If both queues are empty
	if (self->queues[NORTH]==0 && self->queues[SOUTH] == 0) {
		self->active = false;
		self->traffic_lights[NORTH] = false;
		self->traffic_lights[SOUTH] = false;
		//send to writer
	}
	
	// If current queue is empty or we've allowed a specified number of cars in a row 
	if (self->queues[self->current_direction] == 0) {
		self->traffic_lights[self->current_direction] = false;
		// send to writer
	}
	
	// If we let in the maximum amount of cars allowed from one direction
	if (self->cars_allowed == 0) {
		// If there is a queue in the other direction, turn off the light 
		if (self->queues[!self->current_direction] > 0) {
			self->traffic_lights[self->current_direction] = false;
			//send to writer
		}
		else {
			// If we let in the max amount but there are no cars in the other queue, allow another car
			self->cars_allowed++;
			// When we let in another car, cars_allowed will be 0 again, so if there's a queue in the other direction then, we turn off light
		}
		
	}

}

void enter_lane(Controller *self, int arg0) {
	self->queues[self->current_direction]--;
	self->cars_allowed--;
	self->current_cars++;
	ASYNC(self->gui, update_current, self->current_cars);
	AFTER(MSEC(CAR_PASSING_TIME), self, exit_lane, 0);
	manage_lights(self, 0);
}

void exit_lane(Controller *self, int arg0) {
	self->current_cars--;
	if (self->current_cars == 0) {
		ASYNC(self, swap_lights, 0);
	}
	ASYNC(self->gui, update_current, self->current_cars);
	manage_lights(self, 0);
}