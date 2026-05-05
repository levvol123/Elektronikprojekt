#include <stdio.h>
#include <math.h>
#include "Signalprocessing.h"
#define CAMERA_IMPLEMENTATION
#include "Camera.h"

#define ITERATIONS 10000
#define LEARNING_RATE 1e-7
#define SAMPLES 128
#define C 330.0

double ax=0, ay=0;
double bx=0, by=0;
double cx=0, cy=0;
double ta=0, tb=0, tc=0;
Sample SampleArray[SAMPLES];

void init_signalprocessing(){
    c_setup();
    c_set_password("root:root");
    c_set_ip("169.254.45.199");
}

void residuals(double x0, double y0, double r, double res[3]) {
    res[0] = (ax-x0)*(ax-x0) + (ay-y0)*(ay-y0) - 
             (r + C*ta)*(r + C*ta);

    res[1] = (bx-x0)*(bx-x0) + (by-y0)*(by-y0) -
             (r + C*tb)*(r + C*tb);

    res[2] = (cx-x0)*(cx-x0) + (cy-y0)*(cy-y0) -
             (r + C*tc)*(r + C*tc);
}

double cos_t(double x0, double y0, double r) {
    double res[3];
    residuals(x0,y0,r,res);

    return res[0]*res[0] + res[1]*res[1] + res[2]*res[2];
}

void calculate_angle(){
    double x0 = 0;
    double y0 = 0;
    double r  = 1000;

    for(int i=0;i<ITERATIONS;i++){ 
        double eps = 1e-6;
        double gx = (cos_t(x0+eps,y0,r)-cos_t(x0,y0,r))/eps;
        double gy = (cos_t(x0,y0+eps,r)-cos_t(x0,y0,r))/eps;
        double gr = (cos_t(x0,y0,r+eps)-cos_t(x0,y0,r))/eps;
        x0 -= LEARNING_RATE * gx;
        y0 -= LEARNING_RATE * gy;
        r  -= LEARNING_RATE * gr;
        if(r < 0){
            r = 0;
        }
    }
}

void rotate_camera(double x, double y){
    c_pan(atan(y/x));
    //c_tilt();
}

void position_calculation_loop() {
    while(1){
        double dt1 = 0;
        double dt2 = 0;

        if (get_copy_of_buffer(SampleArray)==0){
            printf("Copy buffer failed\n");
            continue;       
        }
        
        int index_microphone0 = find_index_sample_array(0);
        int index_microphone1 = find_index_sample_array(1);
        int index_microphone2 = find_index_sample_array(2);

        if(index_microphone0 == -1 || index_microphone1 == -1|| index_microphone2 == -1){
            printf("Saknas index\n");
        }
        if(index_microphone0 == -1 && index_microphone1 == -1&& index_microphone2 == -1){
            continue;
        }

        if (index_microphone0 >= index_microphone1 && index_microphone0 >= index_microphone2) {
            //mic1 störst
            ta = 0;
            dt1 = index_microphone0 - index_microphone1;
            dt2 = index_microphone0 - index_microphone2;
            tb = dt1;
            tc = dt2;
        } else if (index_microphone1 >= index_microphone0 && index_microphone1 >= index_microphone2) {
            //mic2 störst
            tb = 0;
            dt1 = index_microphone1 - index_microphone0;
            dt2 = index_microphone1 - index_microphone2;
            ta = dt1;
            tc = dt2;
        } else {
            //mic3 störst
            tc = 0;
            dt1 = index_microphone2 - index_microphone0;
            dt2 = index_microphone2 - index_microphone1;
            ta = dt1;
            tb = dt2;
        }
        calculate_angle();
    }
}