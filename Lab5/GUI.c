
#include "GUI.h"

/* Print north queue at positions 0 and 1 */
void update_north(GUI *self, int arg0) {
	printAt(arg0, 0);
}

/* Print current bridge cars at position 2 and 3 */
void update_current(GUI *self, int arg0) {	
	printAt(arg0, 2);
}

/* Print south queue at positions 4 and 5*/
void update_south(GUI *self, int arg0) {
	printAt(arg0, 4);
}

// Makes sure something is printed before any reaction has taken place
void start_gui(GUI *self, int arg0) {
	if (!self->initialized) {
		initLCD();
		self->initialized = 1;
	}
	printAt(0, 0);
	printAt(0, 2);
	printAt(0, 4);
}