#ifndef SIGNALPROCESSING2_H
#define SIGNALPROCESSING2_H
#include "Datafetcher.h"

void create_array(Sample SampleArray[]);

int find_index(int mic[]);

double calculate_angle(int mic1[], int mic2[]);

void rotate_camera_loop();

void init_signalprocessing();

double calculate_angle_using_dt(double dt);
int find_index_sample_array(int microphone);
double calculate_angle_2(int index1, int index2, int index3);


#endif // !Datafetcher.h
