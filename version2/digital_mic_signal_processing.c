#include <gpiod.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "digital_mic_signal_processing.h"

static pthread_t adc_thread;
static atomic_int head = 0;

static Sample internal_circular_buffer[DIGITAL_MIC_BUFFER_SIZE];
static Sample calculation_buffer[DIGITAL_MIC_BUFFER_SIZE];

static long interval;

struct gpiod_chip *chip = NULL;
struct gpiod_line_request *request = NULL; // v2: replaces gpiod_line_bulk
static struct gpiod_line_request *switch_request = NULL;

static void* f_loop();

int digital_mic_configure(int samplerate_hz) {
	interval = 1000000000L / samplerate_hz;
    unsigned int pins[2] = {DIGITAL_MIC_PIN_1, DIGITAL_MIC_PIN_2}; 
	chip = gpiod_chip_open(CHIP_PATH);
	if (chip == NULL) {
		return 1;
	}
	// v2: configure line settings (direction = input)
	struct gpiod_line_settings *settings = gpiod_line_settings_new();
	gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);

	// v2: map settings to our pins
	struct gpiod_line_config *line_cfg = gpiod_line_config_new();
	gpiod_line_config_add_line_settings(line_cfg, pins, 2, settings);

	// v2: set consumer name (shows up in gpioinfo)
	struct gpiod_request_config *req_cfg = gpiod_request_config_new();
	gpiod_request_config_set_consumer(req_cfg, "Camera");

	// v2: request the lines
	request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

	// cleanup config objects — request stays alive
	gpiod_line_settings_free(settings);
	gpiod_line_config_free(line_cfg);
	gpiod_request_config_free(req_cfg);
	if (request == NULL || switch_configure() == 1) {
		return 1;
	}
	printf("GIOD done\n");
	return 0;
}

void f_start_loop(){
	pthread_create(&adc_thread, NULL, f_loop, NULL);
}
void f_stop_loop(){
	pthread_join(adc_thread, NULL);
}

int calculate_general_direction(){
    if(switch_get_value() == 1){
        return 1;
        printf("Switch enabled\n");
    }
    for (int i = 0; i < DIGITAL_MIC_BUFFER_SIZE; i++)
    {
        if(calculation_buffer[i].samples[0] == 1){
            return 1; //positiv vinkel
        }
        else if(calculation_buffer[i].samples[1] == 1){
            return 0; //negativ vinkel
        }
    }
    return -1;
}

void save_calculation_buffer(){
    get_full_buffer(calculation_buffer);
}

void get_full_buffer(Sample* sample_array){
    int current_head = atomic_load(&head);
    
    if(current_head == 0){
        // Buffer hasn't wrapped yet, just copy from 0 to head
        memcpy(sample_array, &internal_circular_buffer[0], current_head * sizeof(Sample));
    } else {
        // Copy in two chunks starting from head (oldest data) to get chronological order
        int first_chunk = BUFFER_SIZE - current_head;
        int second_chunk = current_head;
        memcpy(sample_array, &internal_circular_buffer[current_head], first_chunk * sizeof(Sample));
        memcpy(sample_array + first_chunk, &internal_circular_buffer[0], second_chunk * sizeof(Sample));
    }
}

int switch_configure() {
    unsigned int pin = SWITCH_PIN;

    struct gpiod_line_settings *settings = gpiod_line_settings_new();
    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
    gpiod_line_settings_set_bias(settings, GPIOD_LINE_BIAS_PULL_DOWN);

    struct gpiod_line_config *line_cfg = gpiod_line_config_new();
    gpiod_line_config_add_line_settings(line_cfg, &pin, 1, settings);

    struct gpiod_request_config *req_cfg = gpiod_request_config_new();
    gpiod_request_config_set_consumer(req_cfg, "Camera");

    switch_request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);
    if(switch_request == NULL){
        printf("Switch failed\n");
        return 1;
    }
    gpiod_line_settings_free(settings);
    gpiod_line_config_free(line_cfg);
    gpiod_request_config_free(req_cfg);

    return 0;
}

int switch_get_value(){
    if(gpiod_line_request_get_value(switch_request, SWITCH_PIN) == GPIOD_LINE_VALUE_INACTIVE){
        return 0;
    }
    else{
        return 1;
    }
}

static void* f_loop() {
	printf("Digital mic thread created\n");
	struct timespec next;
	clock_gettime(CLOCK_MONOTONIC, &next);
	while(1){
		
		// v2: replaces gpiod_line_get_value_bulk
		if(gpiod_line_request_get_values(request, internal_circular_buffer[head].samples) == -1){
			printf("Line Request Failed");
		}
		atomic_store(&head, (head +1) % BUFFER_SIZE); //increment head

		next.tv_nsec += interval;
		if (next.tv_nsec >= 1000000000L) {
			next.tv_nsec -= 1000000000L;
			next.tv_sec++;
		}
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
	}
	return NULL;
}