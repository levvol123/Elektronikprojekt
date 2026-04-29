#ifndef DATAFETCHERT_H
#define DATAFETCHERT_H

#define NUMBER_OF_CHANNELS 3

int f_configure(int samplerate_hz);

void f_start_loop();

typedef struct {
	int samples[NUMBER_OF_CHANNELS]; 
} Sample; //Struct för ett set av samples (vid tid t)

#endif // !Datafetcher.h
