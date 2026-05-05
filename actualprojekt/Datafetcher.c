#include "Datafetcher.h"
#include <pthread.h> //https://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread.h.html
#include <stdatomic.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#define BUFFER_SIZE 4096
#define CHIP_PATH "/dev/gpiochip0"
#define NUMBER_OF_PINS 3

static int samplerate;
static long interval;
static unsigned int pins[NUMBER_OF_PINS] = {24,25,23};

static pthread_t adc_thread;

static Sample circular_buffer[BUFFER_SIZE];

static atomic_int head = 0;
static atomic_int tail = 0;
static void* f_loop();

struct gpiod_chip *chip = NULL;
struct gpiod_line_request *request = NULL; // v2: replaces gpiod_line_bulk

int f_configure(int samplerate_hz) {
	samplerate = samplerate_hz;
	interval = 1000000000L / samplerate;

	chip = gpiod_chip_open(CHIP_PATH);
	if (chip == NULL) {
		f_exception_handler(chip);
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
		f_exception_handler(chip);
		return 1;
	}
	printf("f_configure done\n");
	return 0;
}

void f_start_loop() {
	pthread_create(&adc_thread, NULL, f_loop, NULL);
}
void f_stop_loop(){
	pthread_join(adc_thread, NULL);
}

static void* f_loop() {
	printf("Thread created\n");
	struct timespec next;
	clock_gettime(CLOCK_MONOTONIC, &next);
	while(1){
		
		// v2: replaces gpiod_line_get_value_bulk
		if(gpiod_line_request_get_values(request, circular_buffer[head].samples) == -1){
			printf("Line Request Failed");
		}
		atomic_store(&head, (head +1) % BUFFER_SIZE); //increment head
		//printf("Stored new sample\n");
		//get_latest_sample(&test);
		//printf("%ls", test.samples);
		next.tv_nsec += interval;
		if (next.tv_nsec >= 1000000000L) {
			next.tv_nsec -= 1000000000L;
			next.tv_sec++;
		}
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
	}
	return NULL;
}

void f_exception_handler(struct gpiod_chip *chip) {
	printf("Exception\n");
	if (request != NULL) {
		gpiod_line_request_release(request); // v2: replaces gpiod_line_release_bulk
		request = NULL;
	}
	if (chip != NULL) {
		gpiod_chip_close(chip);
	}
	//exit(0);
}

int get_latest_sample(Sample* latest_sample){
	if(atomic_load(&head) == tail){
		return 0; //inga nya samples
	}
	else{
		*latest_sample = circular_buffer[tail];
		printf("Getting latest sample\n");
		printf("Head: %d, Tail: %d\n", atomic_load(&head), tail);
		atomic_store(&tail, (tail +1) % BUFFER_SIZE); //increment tail
		return 1;
	}
}

int get_copy_of_buffer(Sample* sample_array){
	int current_head = atomic_load(&head);
	int avaliable_samples = (current_head - tail + BUFFER_SIZE) % BUFFER_SIZE;
	//printf("Avaliable samples: %d \n",avaliable_samples);
	if(avaliable_samples == 0){
		return 0; //inga nya samples

	}

	if(tail < current_head){
		//printf("func 1\n");
		//printf("DEBUG: tail=%d, head=%d, avaliable_samples=%d\n", tail, current_head, avaliable_samples);
		memcpy(sample_array, &circular_buffer[tail], avaliable_samples * sizeof(Sample));
		atomic_store(&tail, (tail + avaliable_samples) % BUFFER_SIZE); //increment tail
		
		//printf("DEBUG2: tail=%d, head=%d, avaliable_samples=%d\n", tail, current_head, avaliable_samples);
	}else{
		//printf("func 2\n");
		int first_chunk = BUFFER_SIZE - tail;
		int second_chunk = current_head;
		memcpy(sample_array, &circular_buffer[tail], first_chunk * sizeof(Sample));
		memcpy(sample_array + first_chunk, &circular_buffer[0], second_chunk * sizeof(Sample));
		atomic_store(&tail, (tail + avaliable_samples) % BUFFER_SIZE);
	}
	//tail = (tail + avaliable_samples) % BUFFER_SIZE;
	//printf("return\n");
	return 1;
}