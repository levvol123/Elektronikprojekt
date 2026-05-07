#include <portaudio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <limits.h>
#include <fftw3.h>
#define CAMERA_IMPLEMENTATION
#include "Camera.h"



#define SAMPLE_RATE 48000
#define BUFFER_SIZE 4096
#define F_BUFFER_SIZE (2*BUFFER_SIZE)
#define NUMBER_OF_CHANNELS 2
PaStream *stream;
PaError err;

const float conversion_constant = 180.0/ M_PI;
const float microphone_distance_meters = 0.1f;
const float speed_of_sound = 343.0f;
const float max_delay = microphone_distance_meters / speed_of_sound;

static int32_t mic1[BUFFER_SIZE];
static int32_t mic2[BUFFER_SIZE];

float calculate_angle();
void cross_correlate(int n);
void init_fftw();

fftw_complex in1[F_BUFFER_SIZE];
fftw_complex out1[F_BUFFER_SIZE];
fftw_plan plan1; 

fftw_complex in2[F_BUFFER_SIZE];
fftw_complex out2[F_BUFFER_SIZE];
fftw_plan plan2; 

fftw_complex cross_array[F_BUFFER_SIZE];
fftw_plan plan_inverse;
fftw_complex result[F_BUFFER_SIZE];

static int callback_function(const void *input,
                            void *output,
                            unsigned long frameCount,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{
    //Sample *input_signal = (Sample*)input;
    if (input == NULL){ //Om input är noll
        return paContinue;
    }
    //memcpy(local_buffer, input_signal, sizeof(Sample)*frameCount);
    //kopiera data
    int32_t *input_samples = (int32_t*)input;
    for(int i = 0; i < frameCount; i++){
        mic1[i] = input_samples[i * 2];      // even samples = channel 1
        mic2[i] = input_samples[i * 2 + 1];  // odd samples = channel 2
    }
    return paContinue;
}

int main(){

    err = Pa_Initialize();
    if( err != paNoError ){
        printf("PortAudio Init error: %s\n", Pa_GetErrorText(err));
    }
    printf("PortAudio init success \n");
    
    /* Open an audio I/O stream. */
    err = Pa_OpenDefaultStream(&stream,
        2,          /* 2 input channels */
        0,          /* no output */
        paInt32,  /* 32 bit floating point output */
        SAMPLE_RATE,
        BUFFER_SIZE,        /* frames per buffer, */
        callback_function, /* this is your callback function */
        NULL); /*This is a pointer that will be passed to your callback*/

    if( err != paNoError ){
        printf("PortAudio Open Stream error: %s\n", Pa_GetErrorText(err));
    }
    printf("PortAudio Open Stream success \n");
    err = Pa_StartStream(stream);
        if( err != paNoError ){
        printf("PortAudio Start Stream error: %s\n", Pa_GetErrorText(err));
    }
    printf("PortAudio Start Stream success \n");
    init_fftw();
    c_setup();
    c_set_password("root:root");
    c_set_ip("169.254.45.199");
    while (1)
    {
        float angle = calculate_angle();
        if(angle == -1 || angle == 0)
        {
            continue;
        }
        
        printf("Angle : %f \n", angle);
        c_pan(angle);
        Pa_Sleep(200);
        //system("clear");
    }
}

void cross_correlate(int n) { 
    for (int i = 0; i < n; i++) {
        // Complex Conjugate Multiplication: (a+bi) * (c-di)
        double real = out1[i][0] * out2[i][0] + out1[i][1] * out2[i][1];
        double imag = out1[i][1] * out2[i][0] - out1[i][0] * out2[i][1];
        
        double mag = sqrt(real * real + imag * imag);
        
        // PHAT Weighting: Normalize by magnitude
        if (mag > 1e-9) { 
            cross_array[i][0] = real / mag;
            cross_array[i][1] = imag / mag;
        } else {
            cross_array[i][0] = 0;
            cross_array[i][1] = 0;
        }
    }
}

float calculate_angle(){
    // Load data into complex input arrays
    for (int i = 0; i < F_BUFFER_SIZE; i++)
    {
        in1[i][0] = 0; in1[i][1] = 0;
        in2[i][0] = 0; in2[i][1] = 0;
    }
    FILE *f1 = fopen("mic1.csv", "w");
    FILE *f2 = fopen("mic2.csv", "w");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        in1[i][0] = mic1[i]; in1[i][1] = 0;
        in2[i][0] = mic2[i]; in2[i][1] = 0;
        fprintf(f1,"%d, %d \n",i,mic1[i]);
        fprintf(f2,"%d, %d \n",i,mic2[i]);
    }
    fclose(f1);
    fclose(f2);
    //exit(0);
    //FILE *f2 = fopen("mic2.txt", "w");
    fftw_execute(plan1);// Calculate fft1
    fftw_execute(plan2);// Calculate fft2
    cross_correlate(F_BUFFER_SIZE);
    fftw_execute(plan_inverse); // Calculate inverse
    int best_index = 0;
    double best_value = 0;
    for(int i = 0; i< F_BUFFER_SIZE; i++){
        result[i][0] /= F_BUFFER_SIZE;
        result[i][1] /= F_BUFFER_SIZE;
        double value = result[i][0]*result[i][0] + result[i][1]*result[i][1];

        if(value > best_value){
            best_value = value;
            best_index = i; 
        }
    }
    if(best_index > F_BUFFER_SIZE/2){
        best_index -= F_BUFFER_SIZE;
    }

    float delay = (float)best_index/(float)SAMPLE_RATE;  // seconds
    if(delay > max_delay || delay < -max_delay){
        //printf("Stor delay\n");
        return -1;
    }
    //printf("Delay: %f\n", delay);
    float angle = asin(delay * speed_of_sound / microphone_distance_meters);  // radians
    // if(angle != 0){
    //     printf("%f",angle*conversion_constant);
    //     exit(0);
    // }
    return angle*conversion_constant; //degrees
}

void init_fftw(){
    plan1 = fftw_plan_dft_1d(F_BUFFER_SIZE, in1,out1, FFTW_FORWARD, FFTW_ESTIMATE);
    plan2 = fftw_plan_dft_1d(F_BUFFER_SIZE, in2,out2, FFTW_FORWARD, FFTW_ESTIMATE);
    plan_inverse = fftw_plan_dft_1d(F_BUFFER_SIZE, cross_array ,result, FFTW_BACKWARD, FFTW_ESTIMATE);
    printf("Plan Created\n");
}