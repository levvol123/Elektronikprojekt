#include "Datafetcher.h"
#include <pthread.h> //https://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread.h.html
#include <stdatomic.h>
#include <time.h>
#include <string.h>

#define BUFFER_SIZE 128
#define CHIP_PATH "/dev/gpiochip0"
#define NUMBER_OF_PINS 3

static int samplerate;
static long interval;
static unsigned int pins[NUMBER_OF_PINS] = {1,2,3};

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

	return 0;
}

void f_start_loop() {
	pthread_create(&adc_thread, NULL, f_loop, NULL);
}

static void* f_loop() {
	struct timespec next;
	clock_gettime(CLOCK_MONOTONIC, &next);
	while(1){
		// v2: replaces gpiod_line_get_value_bulk
		gpiod_line_request_get_values(request, circular_buffer[head].samples);
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

void f_exception_handler(struct gpiod_chip *chip) {
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
		atomic_store(&tail, (tail +1) % BUFFER_SIZE); //increment tail
		return 1;
	}
}

int get_copy_of_buffer(Sample* sample_array){
	int current_head = atomic_load(&head);
	int avaliable_samples = (current_head - tail + BUFFER_SIZE) % BUFFER_SIZE;
	if(avaliable_samples < BUFFER_SIZE){
		return 0; //inga nya samples
	}

	if(tail < current_head){
		memcpy(sample_array, &circular_buffer[current_head], avaliable_samples * sizeof(Sample));
	}
	else{
		int first_chunk = BUFFER_SIZE - tail;
		int second_chunk = current_head;
		memcpy(sample_array, &circular_buffer[tail], first_chunk * sizeof(Sample));
		memcpy(sample_array + first_chunk, &circular_buffer[0], second_chunk * sizeof(Sample));
	}
	return 1;
}