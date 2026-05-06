#include "gpiod_test.h"
#include <stdatomic.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#define BUFFER_SIZE 128
#define CHIP_PATH "/dev/gpiochip0"
#define NUMBER_OF_PINS 3

static int samplerate;
static long interval;
static unsigned int pins[NUMBER_OF_PINS] = {24,25,23};

static Sample circular_buffer[BUFFER_SIZE];
static Sample test[BUFFER_SIZE];

static int head = 0;

struct gpiod_chip *chip = NULL;
struct gpiod_line_request *request = NULL; // v2: replaces gpiod_line_bulk

int f_configure(int samplerate_hz) {
	samplerate = samplerate_hz;
	interval = 1000000000L / samplerate;

	chip = gpiod_chip_open(CHIP_PATH);
	if (chip == NULL) {
		return 1;
	}

	// v2: configure line settings (direction = input)
	struct gpiod_line_settings *settings = gpiod_line_settings_new();
	gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);

	// v2: map settings to our pins
	struct gpiod_line_config *line_cfg = gpiod_line_config_new();
	gpiod_line_config_add_line_settings(line_cfg, pins, NUMBER_OF_PINS, settings);

	// v2: set consumer name (shows up in gpioinfo)
	struct gpiod_request_config *req_cfg = gpiod_request_config_new();
	gpiod_request_config_set_consumer(req_cfg, "Camera");

	// v2: request the lines
	request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

	// cleanup config objects — request stays alive
	gpiod_line_settings_free(settings);
	gpiod_line_config_free(line_cfg);
	gpiod_request_config_free(req_cfg);

	if (request == NULL) {
		//perror("Failed to request lines");
		return 1;
	}
	printf("f_configure done\n");
	return 0;
}

void f_loop() {
	printf("Thread created\n");
	struct timespec next;
	clock_gettime(CLOCK_MONOTONIC, &next);
	while(1){
        //get_copy_of_buffer(test);
        if(gpiod_line_request_get_values(request, circular_buffer[head].samples) == -1){
			printf("Line Request Failed");
		}
        if(circular_buffer[head].samples[0] != 0 || circular_buffer[head].samples[1] != 0 || circular_buffer[head].samples[2] != 0 ){
        printf("Mic1: %d | Mic2: %d | Mic3 %d \n", 
            circular_buffer[head].samples[0], circular_buffer[head].samples[1], circular_buffer[head].samples[2]);
        }
        head++;
		next.tv_nsec += interval;
		if (next.tv_nsec >= 1000000000L) {
			next.tv_nsec -= 1000000000L;
			next.tv_sec++;
		}
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
	}
}

int main(){
    f_configure(5000);
    f_loop();
}