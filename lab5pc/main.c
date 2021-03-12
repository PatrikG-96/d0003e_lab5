
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <sys/select.h>
#include <unistd.h>
#include <termios.h>

#define COM1 "/dev/ttyS0"

#define RED 0
#define GREEN 1
#define NORTH 0
#define SOUTH 1
#define CAR_PASSING_TIME 5000
#define TIME_TO_WAIT 1000

#define LIGHT_BIT_N_G 0
#define LIGHT_BIT_N_R 1
#define LIGHT_BIT_S_G 2
#define LIGHT_BIT_S_R 3
 
 #define CAR_BIT_N_A 0
 #define CAR_BIT_N_E 1
 #define CAR_BIT_S_A 2
 #define CAR_BIT_S_E 3

pthread_mutex_t mutex_tl;
pthread_mutex_t mutex_sn;

int north = 0;
int south = 0;
int current = 0;
int output = 0b0000;
int input = 0b0000;
int fd;

int traffic_lights[2] = {RED, RED};

int main() {


    //pthread_t keyboard_thread, input_thread, run_thread;
    pthread_t RW_thread, run_thread;

    fd = open(COM1, O_RDWR);

    struct termios t;

    tcgetattr(fd, &t);

    tcflush(fd, TCIFLUSH);
    cfsetispeed(&t, B9600);
    cfsetospeed(&t, B9600);

    tcsetattr(fd, TCSANOW, &t); 

    pthread_create(&RW_thread, NULL, read_and_write, NULL);
    pthread_create(&run_thread, NULL, run, NULL);

    pthread_join(&RW_thread, NULL);
}


void run() {

    while(1) {

        handle_input();
        // If north traffic light is green, create a new car thread
        pthread_mutex_lock(&mutex_tl);
        if(traffic_lights[NORTH] == GREEN) {
            pthread_mutex_lock(&mutex_sn);
            if (north > 0) {
                pthread_mutex_unlock(&mutex_sn);
                pthread_t new_thread;
                pthread_create(&new_thread, NULL, enter_north, NULL);
            }
        }
        // If south traffic light is green, create a new car thread
        else if(traffic_lights[SOUTH] == GREEN) {
            pthread_mutex_lock(&mutex_sn);
            if (south > 0) {
                pthread_mutex_unlock(&mutex_sn);
                pthread_t new_thread;
                pthread_create(&new_thread, NULL, enter_south, NULL);
            }
            
        }
        pthread_mutex_unlock(&mutex_tl);
        usleep(TIME_TO_WAIT);

    }

}

void read_and_write() {

    fd_set readSet;
    fd_set writeSet;
    char[2] buffer;

    while(1) {
        FD_ZERO(&readSet);
        FD_ZERO(&writeSet);
        FD_SET(fileno(stdin), &readSet);
        FD_SET(fd, &readSet);
        FD_SET(fd, &writeSet);


        int max_fd = fileno(stdin) < fd ? fd : fileno(stdin);

        int result = select(max_fd+1, &readSet, &writeSet, NULL, NULL); // time?

        if (FD_ISSET(fileno(stdin), &readSet)) {
            fgets(buffer, sizeof(buffer), stdin);

            switch(buffer[0]) {
                case 'n':
                    pthread_mutex_lock(&mutex_sn);
                    north++;
                    output |= (1 << CAR_BIT_N_A);
                    pthread_mutex_unlock(&mutex_sn);
                    break;
                case 's':
                    pthread_mutex_lock(&mutex_sn);
                    south++;
                    output |= (1 << CAR_BIT_S_A);
                    pthread_mutex_unlock(&mutex_sn);
                    break;
                case 'e':
                    exit(0);
                    break;
                default:
                    break;
            }

        }

        if (FD_ISSET(fd, &readSet)) {

            pthread_mutex_lock(&mutex_tl);
            read(fd, &input, 1);
            pthread_mutex_unlock(&mutex_tl);
        }

        if (FD_ISSET(fd, &writeSet)) {

            pthread_mutex_lock(&mutex_sn);
            write(fd, &output, 1);
            output = 0b0000;
            pthread_mutex_unlock(&mutex_sn);

        }

    }

}

void handle_input() {

        
        pthread_mutex_lock(&mutex_tl);

        if(input >> LIGHT_BIT_N_G) {
            traffic_lights[NORTH] = GREEN;
        }
        if(input >> LIGHT_BIT_N_R) {
            traffic_lights[NORTH] = RED;
        }
        if(input >> LIGHT_BIT_S_G) {
            traffic_lights[SOUTH] = GREEN;
        }
        if(input >> LIGHT_BIT_S_R) {
            traffic_lights[SOUTH] = RED;
        }
    
        pthread_mutex_unlock(&mutex_tl);
        
}

void enter_north() {
    // Car leaves north queue and enters bridge
    pthread_mutex_lock(&mutex_sn);
    north--;
    current++;
    output |= (1 << CAR_BIT_N_E);
    pthread_mutex_unlock(&mutex_sn);

    // send signal to avr - a car has entered the bridge

    usleep(CAR_PASSING_TIME); // it takes 5 seconds to pass the bridge

    // 5 seconds has passed, remove a car from bridge
    pthread_mutex_lock(&mutex_sn);
    current--;
    pthread_mutex_unlock(&mutex_sn);

}


void enter_south() {
    // Car leaves south queue and enters bridge
    pthread_mutex_lock(&mutex_sn);
    south--;
    current++;
    output |= (1<<CAR_BIT_S_E);
    pthread_mutex_unlock(&mutex_sn);

    // send signal to avr - a car has entered the bridge from the south

    usleep(CAR_PASSING_TIME); // it takes 5 seconds to pass the bridge

    // 5 seconds has passed, remove a car from bridge
    pthread_mutex_lock(&mutex_sn);
    current--;
    output
    pthread_mutex_unlock(&mutex_sn);

}

void init() {

}