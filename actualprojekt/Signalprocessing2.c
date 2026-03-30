#include <stdio.h>
#include <math.h>
#include "Signalprocessing2.h"
 //kan behöva samla in en del samples för att se när signalen börjar och slutar men det blir samma princip. 
#define SAMPLES 128
 //Hitta den som är störst av de två signalera för att veta hur ska ska subtrahera. 

// en sample så får jag in en 1 eller 0 från alla tre signaler. Sen måste jag själv bygga upp tre vekoterer av detta. Sample är def i datafetcher.h
Sample SampleArray[SAMPLES];
int mic1[SAMPLES];
int mic2[SAMPLES];
int mic3[SAMPLES];
const double speed_of_sound = 340.0;
const double microphone_distance = 0.01;
const double conversion_constant = 180.0/ M_PI;

void create_array(Sample SampleArray[]){
    for (int i =0; i< SAMPLES; i++){
        mic1[i]= SampleArray[i].samples[0];
        mic2[i]= SampleArray[i].samples[1];
        mic3[i] = SampleArray[i].samples[2];
    }
}

int find_index(int mic[]){ 
    for (int i=0; i < SAMPLES; i++){
        if (mic[i]==1){
            return i;
        }
    }
    return -1;
}

double calculate_angle(int mic1[], int mic2[]){
    int index1 = find_index(mic1);
    int index2 = find_index(mic2);
    double d_t = index1-index2; 
    
    double ratio = (speed_of_sound * d_t) / microphone_distance;
    if (ratio > 1.0) ratio = 1.0;
    if (ratio < -1.0) ratio = -1.0;
    return asin(ratio) * conversion_constant;
}
//denna beräknar vinkeln i grader för två mickar. Om förhållandet inte stämmer så kommer vinkeln alltid på +/- 90 grader. 
    

