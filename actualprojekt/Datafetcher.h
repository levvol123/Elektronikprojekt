#ifndef DATAFETCHER_H
#define DATAFETCHER_H
#include <gpiod.h>

#define NUMBER_OF_CHANNELS 3

int f_configure(int samplerate_hz);

void f_start_loop();

void f_stop_loop();

void f_exception_handler(struct gpiod_chip *chip);

typedef struct {
	int samples[NUMBER_OF_CHANNELS]; 
} Sample; //Struct för ett set av samples (vid tid t)

int get_latest_sample(Sample* latest_sample);

int get_copy_of_buffer(Sample* sample_array);

void get_full_buffer(Sample* sample_array);


#endif // !Datafetcher.h
