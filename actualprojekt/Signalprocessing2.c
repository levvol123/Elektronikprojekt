#include <stdio.h>
#include <math.h>
#include "Signalprocessing2.h"
#define CAMERA_IMPLEMENTATION
#include "Camera.h"

#define SAMPLES 128

Sample SampleArray[SAMPLES];
const double speed_of_sound = 340.0;
const double microphone_distance = 0.1;
const double conversion_constant = 180.0/ M_PI;

int find_index_sample_array(int microphone){
    for (int i = 0; i < SAMPLES; i++)
    {
        if(SampleArray->samples[microphone] == 1){
            return i;
        }
    }
    return -1;
}

double calculate_angle_using_dt(double dt){
    double ratio = (speed_of_sound * dt) / microphone_distance;
    if (ratio > 1.0) ratio = 1.0;
    if (ratio < -1.0) ratio = -1.0;
    return asin(ratio) * conversion_constant;
}
void rotate_camera_loop(){
    while (1)
    {        
        if (get_copy_of_buffer(SampleArray)==0){
            printf("Copy buffer failed\n");
            continue;       
        }
        
        printf("Här: %d\n", SampleArray->samples[0]);
        int index_microphone0 = find_index_sample_array(0);
        int index_microphone1 = find_index_sample_array(1);
        int index_microphone2 = find_index_sample_array(2);

        if(index_microphone0 == -1 || index_microphone1 == -1|| index_microphone2 == -1){
            printf("Saknas index\n");
           
        }
        if(index_microphone0 == -1 && index_microphone1 == -1&& index_microphone2 == -1){
           
            continue;
        }
        //printf("dt\n");
        double dt_1 = index_microphone0 - index_microphone1;
        //printf("dt1 klart\n");
        double dt_2 = index_microphone1 - index_microphone2;
        
        //printf("Börjar kamera\n");
        double angle = calculate_angle_using_dt(dt_1);
        printf("Current angle: %lf", angle);
        c_pan(angle); //phi
        //c_tilt(calculate_angle_using_dt(dt_2)); //theta
        //printf("Kamera klar\n");
    }
}
void init_signalprocessing(){
    c_setup();
    c_set_password("root:root");
    c_set_ip("169.254.45.199");
}

