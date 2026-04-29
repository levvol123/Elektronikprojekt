#include "Datafetcher_test.h"
#include <pthread.h> //https://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread.h.html
#include <stdatomic.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#define BUFFER_SIZE 128
#define NUMBER_OF_PINS 3

static int samplerate;
static long interval;
static unsigned int pins[NUMBER_OF_PINS] = {1,2,3};

static pthread_t adc_thread;

static Sample circular_buffer[BUFFER_SIZE];

static atomic_int head = 0;
static atomic_int tail = 0;
static void* f_loop();


int f_configure(int samplerate_hz) {
	samplerate = samplerate_hz;
	interval = 1000000000L / samplerate;
	return 0;
}

void f_start_loop() {
    printf("create thread \n");
	int res = pthread_create(&adc_thread, NULL, f_loop, NULL);
    printf("%d\n", res);
}

static void* f_loop() {
    printf("Thread created \n");
    struct timespec next;
	clock_gettime(CLOCK_MONOTONIC, &next);
    for(int i = 0; i < 10; i++){
        printf("loop iteration %d \n", i);
        next.tv_nsec += interval;
		if (next.tv_nsec >= 1000000000L) {
			next.tv_nsec -= 1000000000L;
			next.tv_sec++;
		}
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
    }
	return NULL;
}

int main(){
    printf("Initializing \n");
    f_configure(2);
    printf("Starting tasks \n");
    f_start_loop();
    pthread_join(adc_thread, NULL);
}