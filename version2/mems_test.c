#include <portaudio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <limits.h>

#define SAMPLE_RATE 48000
#define BUFFER_SIZE 512
#define NUMBER_OF_CHANNELS 2
PaStream *stream;
PaError err;

const float conversion_constant = 180.0/ M_PI;
const float microphone_distance_meters = 0.1f;
const float speed_of_sound = 343.0f;

static int32_t mic1[BUFFER_SIZE];
static int32_t mic2[BUFFER_SIZE];

float calculate_angle();
int cross_correlate(int32_t *mic1, int32_t *mic2, int n);

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

    while (1)
    {

        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            printf("Channel 1: %d | Channel 2: %d\n", 
                mic1[i], mic2[i]);
        }
        float angle = calculate_angle();
        if(angle == -1)
        {
            continue;
        }
        printf("Angle : %f \n", angle);
        Pa_Sleep(1000);
        system("clear");
    }
}

int cross_correlate(int32_t *mic1, int32_t *mic2, int n) { //claude
    int best_lag = 0;
    long long best_val = LLONG_MIN;

    for (int lag = -(n-1); lag < n; lag++) {
        long long sum = 0;
        for (int i = 0; i < n; i++) {
            int j = i + lag;
            if (j >= 0 && j < n)
                sum += (long long)mic1[i] * mic2[j];
        }
        if (sum > best_val) {
            best_val = sum;
            best_lag = lag;
        }
    }
    return best_lag;  // in samples
}

float calculate_angle(){
    long long energy = 0;
    for(int i = 0; i < BUFFER_SIZE; i++)
        energy += (long long)mic1[i] * mic1[i];
    
    printf("Energy: %lld\n", energy);  // add this temporarily
    
/*     if(energy < SOME_THRESHOLD){
        return -1.0f;
    } */
    int best_guess = cross_correlate(mic1,mic2, BUFFER_SIZE);
    if(best_guess == 0){
        printf("No match \n");
        return -1;
    }
    float delay = (float)best_guess / SAMPLE_RATE;  // seconds
    float angle = asin(delay * speed_of_sound / microphone_distance_meters);  // radians
    return angle*conversion_constant; //degrees
}