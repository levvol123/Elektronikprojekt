#ifndef MEMS_SIGNAL_PROCESSING_H
#define MEMS_SIGNAL_PROCESSING_H

#include <stdint.h>

void fft_initialize();
float calculate_angle();
void load_data(int32_t* input_samples, int n);

#endif //MEMS_SIGNAL_PROCESSING_H