#ifndef SIGNALPROCESSING2_H
#define SIGNALPROCESSING2_H
#include "Datafetcher.h"

void create_array(Sample SampleArray[]);

int find_index(int mic[]);

double calculate_angle(int mic1[], int mic2[]);

void rotate_camera_loop();

void init_signalprocessing();

#endif // !Datafetcher.h
