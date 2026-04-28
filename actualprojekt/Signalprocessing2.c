#include <stdio.h>
#include <math.h>
#include "Signalprocessing2.h"
#define CAMERA_IMPLEMENTATION
#include "Camera.h"
 //kan behöva samla in en del samples för att se när signalen börjar och slutar men det blir samma princip. 
#define SAMPLES 128
 //Hitta den som är störst av de två signalera för att veta hur ska ska subtrahera. 

// en sample så får jag in en 1 eller 0 från alla tre signaler. Sen måste jag själv bygga upp tre vekoterer av detta. Sample är def i datafetcher.h
Sample SampleArray[SAMPLES];
//När jag skapar mina micar så måste jag hämta från Levs data, plocka varje rad. 
//int mic1[SAMPLES];
//int mic2[SAMPLES];
//int mic3[SAMPLES];
const double speed_of_sound = 340.0;
const double microphone_distance = 0.1;
const double conversion_constant = 180.0/ M_PI;



/* void create_array(Sample SampleArray[]){
    for (int i =0; i< SAMPLES; i++){
        mic1[i]= SampleArray[i].samples[0];
        mic2[i]= SampleArray[i].samples[1];
        mic3[i] = SampleArray[i].samples[2];
    }
} */

int find_index(int mic[]){ 
    for (int i=0; i < SAMPLES; i++){
        if (mic[i]==1){
            return i;
        }
    }
    return -1;
}

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
double calculate_angle(int mic1[], int mic2[]){
    int index_microphone0 = find_index_sample_array(0);
    int index_microphone1 = find_index_sample_array(1);

    if(index_microphone0 == -1 || index_microphone1 == -1){
        return 0;
    }
    double d_t = index_microphone0 - index_microphone1;
    //int index1 = find_index(mic1);
    //int index2 = find_index(mic2);
/*     if (index1 == -1 || index2 == -1){
        return 0;
    }
    double d_t = index1-index2;  */
    
    double ratio = (speed_of_sound * d_t) / microphone_distance;
    if (ratio > 1.0) ratio = 1.0;
    if (ratio < -1.0) ratio = -1.0;
    return asin(ratio) * conversion_constant;
}
//denna beräknar vinkeln i grader för två mickar. Om förhållandet inte stämmer så kommer vinkeln alltid på +/- 90 grader. 
void rotate_camera_loop(){
    while (1)
    {
        if (get_copy_of_buffer(SampleArray)==0){
            return;
        }
        int index_microphone0 = find_index_sample_array(0);
        int index_microphone1 = find_index_sample_array(1);
        int index_microphone2 = find_index_sample_array(2);

        if(index_microphone0 == -1 || index_microphone1 == -1|| index_microphone2 == -1){
            printf("Saknas index");
        }
        double dt_1 = index_microphone0 - index_microphone1;
        double dt_2 = index_microphone1 - index_microphone2;
    
        //double angle_phi = calculate_angle(SampleArray->samples[0], SampleArray->samples[1]);
        //double angle_theta = calculate_angle(SampleArray->samples[1], SampleArray->samples[2]);
        c_pan(calculate_angle_using_dt(dt_1)); //phi
        c_tilt(calculate_angle_using_dt(dt_2)); //theta
        //kolla på alla fall som kan bli fel
        //Lev funktion ger mig en matris med tre rader. Funkar index för detta?
    }
}
void init_signalprocessing(){
    c_setup();
    c_set_password("root:root");
    c_set_ip("169.254.45.199");

    //bla bla bla
    //Det som är kvar att göra är att hämnta varje rad från Levs matris och skapa konstanter av dem.  
}

