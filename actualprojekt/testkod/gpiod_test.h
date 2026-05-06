#ifndef GPIODTEST_H
#define GPIODTEST_H
#include <gpiod.h>

#define NUMBER_OF_CHANNELS 3

int f_configure(int samplerate_hz);

void f_loop();

typedef struct {
	int samples[NUMBER_OF_CHANNELS]; 
} Sample; //Struct för ett set av samples (vid tid t)

int get_copy_of_buffer(Sample* sample_array);


#endif // 
