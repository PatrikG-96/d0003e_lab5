
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <sys/select.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <fcntl.h>

#define COM1 "/dev/ttyS0"

#define RED 0
#define GREEN 1
#define NORTH 0
#define SOUTH 1
#define CAR_PASSING_TIME 5000000
#define TIME_TO_WAIT 1000000

#define LIGHT_BIT_N_G 0
#define LIGHT_BIT_N_R 1
#define LIGHT_BIT_S_G 2
#define LIGHT_BIT_S_R 3
 
 #define CAR_BIT_N_A 0
 #define CAR_BIT_N_E 1
 #define CAR_BIT_S_A 2
 #define CAR_BIT_S_E 3

#define REFRESH_RATE 80000


pthread_mutex_t mutex_tl;   // Mutex for accessing, modifying or making decisions based on light status
pthread_mutex_t mutex_sn;   // mutex for accessing, modifying or making decisions based on queue/bridge status

// Contains all information neccessary for all threads, avoids global variables
typedef struct {
    int n_queue;
    int s_queue;
    int n_current;
    int s_current;
    int n_status;
    int s_status;
    int input;
    int output;
    int fd;
}State;

/* Function declarations for thread usage etc */
void *thread_read();
void *run();
void *enter_north();
void *enter_south();
void* gui();
void handle_input();
void serial_write();



int main() {


    pthread_t read_thread, run_thread, gui_thread;
    State* state = (State*)malloc(sizeof(State));   // is freed when all threads have joined or when program is exited

    int fd = open(COM1, O_RDWR);    // file descriptor for COM1 serial port 
    state->fd = fd;

    struct termios t;
    struct termios std;

    tcgetattr(fileno(stdin), &std);
    std.c_lflag &= ~(ICANON) & ~(ECHO);
    tcsetattr(fileno(stdin), TCSANOW, &std);

    tcgetattr(fd, &t);
    tcflush(fd, TCIFLUSH);
    cfsetispeed(&t, B9600);
    cfsetospeed(&t, B9600);
    tcsetattr(fd, TCSANOW, &t);

    system("clear");

    pthread_create(&read_thread, NULL, thread_read, state);
    pthread_create(&run_thread, NULL, run, state);
    pthread_create(&gui_thread, NULL, gui, state);

    pthread_join(read_thread, NULL);
    pthread_join(run_thread, NULL);
    pthread_join(gui_thread, NULL);
    
    free(state);
}

void *gui(void* arg) {

    State* state = (State*)arg;     // Might not need to be casted like this, but it works

    while (1) {
        system("clear");    // Clear the console
        
        // Read data from state, not put in printf statements incase mutex should be used
        // Should mutex be needed if int read is atomic? What if by the time we read current south, traffic light north changed? 
        pthread_mutex_lock(&mutex_tl);
        int nl = state->n_status;
        int sl = state->s_status;
        pthread_mutex_unlock(&mutex_tl);

        pthread_mutex_lock(&mutex_sn);
        int n = state->n_queue;
        int s = state->s_queue;
        int cn = state->n_current;
        int cs = state->s_current;
        pthread_mutex_unlock(&mutex_sn);


        printf("NORTH RED: %d \nNORTH GREEN: %d \nNORTH QUEUE: %d \nNORTH CARS ON BRIDGE: %d", !nl, nl, n, cn);
        printf("\n\nSOUTH RED: %d \nSOUTH GREEN: %d \nSOUTH QUEUE: %d \nSOUTH CARS ON BRIDGE: %d", !sl, sl, s,cs);
  

        fflush(stdout);
        usleep(REFRESH_RATE); // To prevent potential slowdown from eating up mutex time / reduce jittering
    }


}


void *run(void* arg) {

    State* state = (State*)arg;
    while(1) {

        handle_input(state);
      
        // Mutex needed?
        pthread_mutex_lock(&mutex_tl);
        int nl = state->n_status;
        int sl = state->s_status;
        pthread_mutex_unlock(&mutex_tl);

        // If north traffic light is green, create a new car thread
        if(nl == GREEN) {
            pthread_mutex_lock(&mutex_sn);
            if (state->n_queue > 0) {
                pthread_t new_thread;
                pthread_create(&new_thread, NULL, enter_north, state);
            }
            pthread_mutex_unlock(&mutex_sn);
        }
        // If south traffic light is green, create a new car thread
        else if(sl == GREEN) {
            pthread_mutex_lock(&mutex_sn);
            if (state->s_queue > 0) {
                pthread_t new_thread;
                pthread_create(&new_thread, NULL, enter_south,  state);
            }
            pthread_mutex_unlock(&mutex_sn);
            
        }
  
        usleep(TIME_TO_WAIT); //Makes sure we wait 1 second between each entry

    }

}

/* Uses select() to poll stdin and COM1 serial port for reading purposes. */
void *thread_read(void* arg) {
    State* state = (State*)arg;
    fd_set readSet;
    char buffer[2];
    int fd = state->fd;

    while(1) {
        FD_ZERO(&readSet);
        FD_SET(fileno(stdin), &readSet);
        FD_SET(fd, &readSet);


        int max_fd = fileno(stdin) < fd ? fd : fileno(stdin);

        int result = select(max_fd+1, &readSet, NULL, NULL, NULL); // time?

        if (FD_ISSET(fd, &readSet)) {
           
            // Mutex lock to prevent modification of input while run thread is interpreting input
            pthread_mutex_lock(&mutex_tl);
            read(fd, &state->input, sizeof(state->input));
            pthread_mutex_unlock(&mutex_tl);
        }


        if (FD_ISSET(fileno(stdin), &readSet)) {

            fgets(buffer, sizeof(buffer), stdin);
            
            // Depending on keyboard input, enqueue a car north or south
            switch(buffer[0]) {
                case 'n':
                    pthread_mutex_lock(&mutex_sn);
                    state->n_queue++;
                    state->output |= (1 << CAR_BIT_N_A);
                    pthread_mutex_unlock(&mutex_sn);
                    serial_write(state);
                    break;
                case 's':
                    pthread_mutex_lock(&mutex_sn);
                    state->s_queue++;
                    state->output |= (1 << CAR_BIT_S_A);
                    pthread_mutex_unlock(&mutex_sn);
                    serial_write(state);
                    break;
                case 'e':
                    free(state); //Not sure if needed since program shuts down completely, but makes sure memory is freed 
                    exit(0);
                    break;
                default:
                    break;
            }

        }

        fflush(stdin);
        

    }

}


void handle_input(State *state) {

        // Mutex lock as it modifies traffic light states depending on AVR USART input
        pthread_mutex_lock(&mutex_tl);
     
        if((state->input >> LIGHT_BIT_N_G) & 1) {
            state->n_status = GREEN;
        }
        if((state->input >> LIGHT_BIT_N_R) & 1) {
            state->n_status = RED;
        }
        if((state->input >> LIGHT_BIT_S_G) & 1) {
            state->s_status = GREEN;
        }
        if((state->input >> LIGHT_BIT_S_R) & 1) {
            state->s_status = RED;
        }
    
        pthread_mutex_unlock(&mutex_tl);
        
}

void *enter_north(void* arg) {

    State* state = (State*)arg;

    // Car leaves north queue and enters bridge
    pthread_mutex_lock(&mutex_sn);
    state->n_queue--;
    state->n_current++;
    state->output |= (1 << CAR_BIT_N_E);
    pthread_mutex_unlock(&mutex_sn);

    // Signal that a car has entered bridge from the north
    serial_write(state);

    usleep(CAR_PASSING_TIME); // it takes 5 seconds to pass the bridge

    // 5 seconds has passed, remove a car from bridge, we expect the controller to mimic this behaviour
    pthread_mutex_lock(&mutex_sn);
    state->n_current--;
    pthread_mutex_unlock(&mutex_sn);

}


void *enter_south(void* arg) {

    State* state = (State*)arg;

    // Car leaves south queue and enters bridge
    pthread_mutex_lock(&mutex_sn);
    state->s_queue--;
    state->s_current++;
    state->output |= (1 << CAR_BIT_S_E);
    pthread_mutex_unlock(&mutex_sn);

    // Signal that a car has entered bridge from the south
    serial_write(state);
 
    usleep(CAR_PASSING_TIME); // it takes 5 seconds to pass the bridge

    // 5 seconds has passed, remove a car from bridge
    pthread_mutex_lock(&mutex_sn);
    state->s_current--;
    pthread_mutex_unlock(&mutex_sn);

}

void serial_write(State *state) {

    //Take mutex to make sure output isn't modified during writing.
    pthread_mutex_lock(&mutex_sn);
    write(state->fd, &state->output, sizeof(state->output));
    state->output = 0b0000;
    pthread_mutex_unlock(&mutex_sn);


}