/*
 * Controller.c
 *
 * Created: 2021-03-09 14:42:40
 *  Author: shirt
 */ 

#include "Controller.h"

void enqueue_north(Controller *self, int arg0) {

	self->queues[NORTH]++;	// car is added to north queue
	ASYNC(self->gui, update_north, self->queues[NORTH]);
	// If controller isn't active, both lights are red and bridge is empty, start it up again
	if(!self->active) {
		self->active = true;
		self->lights[NORTH] = GREEN;
		self->cars_allowed = MAX_CARS_ON_LANE;
		self->curr_dir = NORTH;
		manage_lights(self, 0);
	}
	
}

void enqueue_south(Controller *self, int arg0) {

	self->queues[SOUTH]++;
	ASYNC(self->gui, update_south, self->queues[SOUTH]);
	if(!self->active) {
		self->active = true;
		self->lights[SOUTH] = GREEN;
		self->cars_allowed = MAX_CARS_ON_LANE;
		self->curr_dir = SOUTH;
		manage_lights(self, 0);
	}
	
	
	
}


void empty_bridge(Controller *self, int arg0) {
	// If bridge is empty
	if(self->current_cars == 0) {
		
		// If queue in opposite direction isn't empty
		if(self->queues[!self->curr_dir] > 0) {
			self->curr_dir = !self->curr_dir;			// Swap direction
			self->lights[self->curr_dir] = GREEN;		// Turn on light
			self->cars_allowed = MAX_CARS_ON_LANE;		// Reset starvation counter
		}
		
	}
	manage_lights(self, 0);
}

void manage_lights(Controller *self, int arg0) {
	
	
	/***********************/
	/* Empty current queue */
	/***********************/
	
	// If current queue is empty
	if (self->queues[self->curr_dir] == 0) {
		
		if(self->queues[!self->curr_dir] > 0) {
			self->lights[self->curr_dir] = RED;	
		}
		
		
		//If queue in opposite direction is empty and no cars are on the bridge
		if(self->queues[!self->curr_dir] == 0 && self->current_cars == 0) {
			self->lights[NORTH] = RED;
			self->lights[SOUTH] = RED;
			self->active = false;				// deactivate
		}
		
	}
	

	/********************/
	/* Starvation logic */
	/********************/
	
	// If we have allowed the maximum cars allowed to avoid starvation
	if(self->cars_allowed == 0) {
		
		// If queue from opposite direction is empty
		if(self->queues[!self->curr_dir] == 0) {
			self->cars_allowed ++;						// Allow another car from this direction
		}
		// If queue from opposite direction is not empty
		else {
			self->lights[self->curr_dir] = RED;			// Set this light to red
		}
	}	
		
	send_lightstatus(self, 0);
	
	
}



void entry_north(Controller *self, int arg0) {
	
	self->queues[NORTH]--;	// dequeue car
	self->cars_allowed--;	// decrement starvation counter
	self->current_cars++;	// new car on bridge
	// Update relevant parts of the gui
	ASYNC(self->gui, update_current, self->current_cars);
	ASYNC(self->gui, update_north, self->queues[NORTH]);
	
	// After a set amount of time, 
	AFTER(MSEC(CAR_PASSING_TIME), self, exit_bridge, 0);
	manage_lights(self, 0);
}


void entry_south(Controller *self, int arg0) {
	
	self->queues[SOUTH]--;	
	self->cars_allowed--;
	self->current_cars++;
	ASYNC(self->gui, update_current, self->current_cars);
	ASYNC(self->gui, update_south, self->queues[SOUTH]);
	
	AFTER(MSEC(CAR_PASSING_TIME), self, exit_bridge, 0);
	manage_lights(self, 0);
}


void exit_bridge(Controller *self, int arg0) {
	self->current_cars--;	// a car has left the bridge

	ASYNC(self->gui, update_current, self->current_cars);
	if (self->current_cars == 0) {
		empty_bridge(self, 0);
	}
}

void send_lightstatus(Controller *self, int arg0) {
		
		// Form output according to standard given
		if (self->lights[NORTH] == GREEN) {
			self->output |= (1<<NORTH_GREEN);
		}
		else {
			self->output |= (1<<NORTH_RED);
		}
		if (self->lights[SOUTH] == GREEN) {
			self->output |= (1<<SOUTH_GREEN);
		}
		else {
			self->output |= (1<<SOUTH_RED);
		}
		
		// Send output to serial writer and write to port
		ASYNC(self->writer, usart_write, self->output);
		self->output = 0;	// reset output
}

