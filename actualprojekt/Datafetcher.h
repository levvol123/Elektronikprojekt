#ifndef DATAFETCHER_H
#define DATAFETCHER_H
#include "ADS1263.h"

#define NUMBER_OF_CHANNELS 3

int f_configure(ADS1263_DRATE samplerate);

void f_start_loop();

static void* f_loop();

void f_exception_handler();

typedef struct {
	float samples[NUMBER_OF_CHANNELS]; 
} Sample; //Struct för ett set av samples (vid tid t)

#endif // !Datafetcher.h
