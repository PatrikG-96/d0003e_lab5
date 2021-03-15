
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <sys/select.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdbool.h>
#include <string.h>

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

#define SEC_IN_USEC 1000000
#define UPDATE_RATE 1000
#define REFRESH_RATE SEC_IN_USEC/UPDATE_RATE


pthread_mutex_t mutex_tl;   // Mutex for accessing, modifying or making decisions based on light status
pthread_mutex_t mutex_sn;   // mutex for accessing, modifying or making decisions based on queue/bridge status

sem_t sem_n;
sem_t sem_s;

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
void *north_car();
void *south_car();
void* gui();
void handle_input();
void serial_write();
void unlock_north();
void unlock_south();

char dbg[] = "DEBUG INFO:\n";

bool debug;


int main() {


    
    debug = 0;
    pthread_t read_thread, gui_thread;
    State* state = (State*)malloc(sizeof(State));   // is freed when all threads have joined or when program is exited

    int fd = open(COM1, O_RDWR);    // file descriptor for COM1 serial port 
    state->fd = fd;
    state->n_queue = 0;
    state->n_current = 0;
    state->s_queue = 0;
    state->s_current = 0;
    state->n_status = 0;
    state->s_status = 0;
    state->input = 0;
    state->output = 0;

    pthread_mutex_init(&mutex_sn, NULL);
    pthread_mutex_init(&mutex_tl, NULL);

    sem_init(&sem_s, 0, 1);
    sem_init(&sem_n, 0, 1);

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

    sem_wait(&sem_n);
    sem_wait(&sem_s);


    pthread_create(&read_thread, NULL, thread_read, state);
    pthread_create(&gui_thread, NULL, gui, state);

  
    pthread_join(read_thread, NULL);
    pthread_join(gui_thread, NULL);

    
    
    free(state);
}

void *gui(State* state) {

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
        printf("\n\nSOUTH RED: %d \nSOUTH GREEN: %d \nSOUTH QUEUE: %d \nSOUTH CARS ON BRIDGE: %d\n", !sl, sl, s,cs);

        fflush(stdout);
        usleep(REFRESH_RATE); // To prevent potential slowdown from eating up mutex time / reduce jittering
    }


}


/* Uses select() to poll stdin and COM1 serial port for reading purposes. */
void *thread_read(State* state) {

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
           
            // Mutex lock to prevent modification of input while read thread is interpreting input
            pthread_mutex_lock(&mutex_tl);
            read(fd, &state->input, sizeof(state->input));
            pthread_mutex_unlock(&mutex_tl);
            handle_input(state);
        }


        if (FD_ISSET(fileno(stdin), &readSet)) {

            fgets(buffer, sizeof(buffer), stdin);

            // Depending on keyboard input, enqueue a car north or south
            switch(buffer[0]) {
                case 'n':
                {
                    pthread_t n_car;
                    pthread_create(&n_car, NULL, north_car, state);
                }
                    break;
                case 's':
                {
                    pthread_t s_car;
                    pthread_create(&s_car, NULL, south_car, state);
                }
                    break;
                case 'e':
                    free(state); //Not sure if needed since program shuts down completely
                    if (debug) {
                        printf(dbg);
                    }
                    exit(0);
                    break;
            }

        }

        fflush(stdin);
        

    }

}


void handle_input(State *state) {

        pthread_mutex_lock(&mutex_tl);
        int input = state->input;
        int nl = state->n_status;
        int sl = state->s_status;
        pthread_mutex_unlock(&mutex_tl);

        if((input >> LIGHT_BIT_N_G) & 1) {
            if (nl != GREEN) { // Light state has changed
               
                state->n_status = GREEN;
                sem_post(&sem_n);            // Traffic light has turned green, let cars pass
               
                if (debug) {
                    strcat(dbg, "Setting light to green, unlocking north light\n");
                }
            }
           
        }
        if((input >> LIGHT_BIT_N_R) & 1) {
            if (nl!= RED) {

                state->n_status = RED;
                sem_trywait(&sem_n);

                if (debug) {
                    strcat(dbg, "Setting light to red, locking north light\n");
                }

            }
        }
        if((input >> LIGHT_BIT_S_G) & 1) {
            if (sl != GREEN) {
                
                state->s_status = GREEN;
                sem_post(&sem_s);
       
                if (debug) {
                    strcat(dbg, "Setting light to green, unlocking south light\n");
                }
            }

        }
        if((input >> LIGHT_BIT_S_R) & 1) {
            if (sl != RED) {

                state->s_status = RED;
                sem_trywait(&sem_s);        // For the special case of both semaphores being avaiable when 0 queues remain

                if (debug) {
                    strcat(dbg, "Setting light to red, locking south light\n");
                }

            }

        }
        pthread_mutex_lock(&mutex_tl);
        state->input = 0;
        pthread_mutex_unlock(&mutex_tl);
        
}


void *north_car(State* state) {

    pthread_mutex_lock(&mutex_sn);
    state->n_queue++;
    serial_write(state, (1 << CAR_BIT_N_A));
    pthread_mutex_unlock(&mutex_sn);



    if (debug) {
        strcat(dbg, "New north car is added\n");
    }

    sem_wait(&sem_n);
    

    if (debug) {
        strcat(dbg, "North car is entering bridge\n");
    }


    pthread_mutex_lock(&mutex_sn);
    state->n_queue--;
    state->n_current++;
    serial_write(state, (1 << CAR_BIT_N_E));
    pthread_mutex_unlock(&mutex_sn);

    usleep(TIME_TO_WAIT);

    unlock_north(state);

    usleep(CAR_PASSING_TIME-TIME_TO_WAIT); // it takes 5 seconds to pass the bridge

    pthread_mutex_lock(&mutex_sn);
    state->n_current--;
    pthread_mutex_unlock(&mutex_sn);

    if (debug) {
        strcat(dbg, "North car is leaving bridge\n");
        if (!state->n_current) {
            strcat(dbg, "North no longer has cars on the bridge\n");
        }
    }


}


void *south_car(State* state) {

    pthread_mutex_lock(&mutex_sn);
    state->s_queue++;
    pthread_mutex_unlock(&mutex_sn);

    serial_write(state, (1 << CAR_BIT_S_A));


    if (debug) {
        strcat(dbg, "New south car is added\n");
    }

    
    sem_wait(&sem_s);

    if (debug) {
        strcat(dbg, "South car is entering bridge\n");
    }

    pthread_mutex_lock(&mutex_sn);
    state->s_queue--;
    state->s_current++;
    serial_write(state, (1 << CAR_BIT_S_E));
    pthread_mutex_unlock(&mutex_sn);


    usleep(TIME_TO_WAIT);
    unlock_south(state);
 
    usleep(CAR_PASSING_TIME-TIME_TO_WAIT); // it takes 5 seconds to pass the bridge

    // 5 seconds has passed, remove a car from bridge
    pthread_mutex_lock(&mutex_sn);
    state->s_current--;
    pthread_mutex_unlock(&mutex_sn);
 
    if (debug) {
        strcat(dbg, "South car is leaving bridge\n");
        if (!state->s_current) {
            strcat(dbg, "South no longer has cars on the bridge\n");
        }
    }

}

/* Assumed any neccessary mutex is taken before calling this function */
void serial_write(State *state, int output) {

    write(state->fd, &output, sizeof(output));

}



void unlock_north(State *state) {
    if (state->n_status == GREEN) {
        sem_post(&sem_n);
        
        if (debug) {
            strcat(dbg, "Unlocking north semaphore\n");
        }
        
    }
}

void unlock_south(State* state) {
    if (state->s_status == GREEN) {
        sem_post(&sem_s);
        
        if (debug) {
            strcat(dbg, "Unlocking south semaphore\n");
        }
    }
}