#include <portaudio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "mems_signal_processing.h"
#include "main.h"
#include "digital_mic_signal_processing.h"

#define CAMERA_IMPLEMENTATION
#include "Camera.h"

static PaStream *stream;
static int callback_function(const void *input,
                            void *output,
                            unsigned long frameCount,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData );

int main(){
    pa_initialize();
    camera_initialize();
    fft_initialize();
    digital_mic_configure(20000);
    f_start_loop();
    while (1)
    {
        float angle = calculate_angle();
        if(angle == -1 || angle == 0){
            continue;
        }
        c_pan(angle);
        Pa_Sleep(200);
    }
    f_stop_loop();
    return 0;
}

void pa_initialize(){
    PaError error_code;
    error_code = Pa_Initialize();
    if(error_code != paNoError ){
        printf("PortAudio Init error: %s\n", Pa_GetErrorText(error_code));
        exception_handler();
    }
    printf("PortAudio init success \n");
    
    /* Open an audio I/O stream. */
    error_code = Pa_OpenDefaultStream(&stream,
        2,          /* 2 input channels */
        0,          /* no output */
        paInt32,  /* 32 bit floating point output */
        SAMPLE_RATE,
        BUFFER_SIZE,        /* frames per buffer, */
        callback_function, /* this is your callback function */
        NULL); /*This is a pointer that will be passed to your callback*/

    if(error_code != paNoError ){
        printf("PortAudio Open Stream error: %s\n", Pa_GetErrorText(error_code));
        exception_handler();
    }
    printf("PortAudio Open Stream success \n");
    error_code = Pa_StartStream(stream);
        if(error_code != paNoError ){
        printf("PortAudio Start Stream error: %s\n", Pa_GetErrorText(error_code));
        exception_handler();
    }
    printf("PortAudio Start Stream success \n");
}
void camera_initialize(){
    if(c_setup() == -1){
        printf("Camera initialize error\n");
        exception_handler();
    }
    c_set_password("root:root");
    c_set_ip("169.254.45.199");
    printf("Camera initialize success\n");
}

void exception_handler(){
    printf("EXIT\n");
    exit(0);

}
static int callback_function(const void *input,
                            void *output,
                            unsigned long frameCount,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{
    (void)output;
    (void)timeInfo;
    (void)statusFlags;
    (void)userData;
    if (input == NULL){ //Om input är noll
        return paContinue;
    }
    load_data((int32_t*)input, frameCount);
    return paContinue;

}