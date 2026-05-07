#include <portaudio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <limits.h>
#include <fftw3.h>

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
void cross_correlate(int n);
void init_fftw();

fftw_complex in1[BUFFER_SIZE*2];
fftw_complex out1[BUFFER_SIZE*2];
fftw_plan plan1; 

fftw_complex in2[BUFFER_SIZE*2];
fftw_complex out2[BUFFER_SIZE*2];
fftw_plan plan2; 

fftw_complex cross_array[BUFFER_SIZE*2];
fftw_plan plan_inverse;
fftw_complex result[BUFFER_SIZE*2];

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

    while (1)
    {
        float angle = calculate_angle();
        if(angle = -1)
        {
            continue;
        }
        printf("Angle : %f \n", angle);
        Pa_Sleep(1000);
        system("clear");
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
    for (int i = 0; i < BUFFER_SIZE*2; i++)
    {
        in1[i][0] = 0; in1[i][1] = 0;
        in2[i][0] = 0; in2[i][1] = 0;
    }
    
    for (int i = 0; i < BUFFER_SIZE; i++) {
        in1[i][0] = mic1[i]; in1[i][1] = 0;
        in2[i][0] = mic2[i]; in2[i][1] = 0;
    }

    fftw_execute(plan1);// Calculate fft1
    fftw_execute(plan2);// Calculate fft2
    cross_correlate(BUFFER_SIZE);
    fftw_execute(plan_inverse); // Calculate inverse
    int best_index = 0;
    double best_value = 0;
    for(int i = 0; i< BUFFER_SIZE; i++){
        result[i][0] /= BUFFER_SIZE;
        result[i][1] /= BUFFER_SIZE;
        double value = result[i][0]*result[i][0] + result[i][1]*result[i][1];

        if(value > best_value){
            best_value = value;
            best_index = i; 
        }
    }
    if(best_index > BUFFER_SIZE/2){
        best_index -= BUFFER_SIZE;
    }

    float delay = best_index/SAMPLE_RATE;  // seconds
    if(delay > 0.001){
        printf("Stor delay\n");
        return -1;
    }
    float angle = asin(delay * speed_of_sound / microphone_distance_meters);  // radians
    return angle*conversion_constant; //degrees
}

void init_fftw(){
    plan1 = fftw_plan_dft_1d(BUFFER_SIZE, in1,out1, FFTW_FORWARD, FFTW_ESTIMATE);
    plan2 = fftw_plan_dft_1d(BUFFER_SIZE, in1,out1, FFTW_FORWARD, FFTW_ESTIMATE);
    plan_inverse = fftw_plan_dft_1d(BUFFER_SIZE, cross_array ,result, FFTW_BACKWARD, FFTW_ESTIMATE);
    printf("Plan Created\n");
}