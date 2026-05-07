#ifndef MEMS_SIGNAL_PROCESSING_H
#define MEMS_SIGNAL_PROCESSING_H

#include <stdint.h>

void fft_initialize();
float calculate_angle();
void load_data(int32_t* input_samples, int n);

extern volatile int ready_for_calculation;

#endif //MEMS_SIGNAL_PROCESSING_H