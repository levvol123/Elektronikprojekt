#include <portaudio.h>
#include <stdio.h>
#include <string.h>
#include<stdlib.h>


#define SAMPLE_RATE 2
#define BUFFER_SIZE 10
#define NUMBER_OF_CHANNELS 2
PaStream *stream;
PaError err;


typedef struct {
	int samples[NUMBER_OF_CHANNELS]; 
} Sample; 

static Sample local_buffer[BUFFER_SIZE];

static int callback_function(const void *input,
                            void *output,
                            unsigned long frameCount,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{
    Sample *input_signal = (Sample*)input;
    if (input == NULL){ //Om input är noll
        return paContinue;
    }
    memcpy(local_buffer, input_signal, sizeof(Sample)*frameCount);
    //kopiera data
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
        2,          /* no input channels */
        0,          /* stereo output */
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
            printf("Channel 1: %d | Channel 2: %d \n", local_buffer[i].samples[0],local_buffer[i].samples[1]);
        }
        Pa_Sleep(3000);
        system("clear");
    }
}