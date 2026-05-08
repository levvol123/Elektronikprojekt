#include <fftw3.h>
#include <math.h>
#include <semaphore.h>
#include "config.h"
#include "mems_signal_processing.h"
#include "digital_mic_signal_processing.h"

static fftw_complex microphone_1[F_BUFFER_SIZE];
static fftw_complex microphone_1_fft[F_BUFFER_SIZE];
static fftw_plan microphone_1_plan; 
 
static fftw_complex microphone_2[F_BUFFER_SIZE];
static fftw_complex microphone_2_fft[F_BUFFER_SIZE];
static fftw_plan microphone_2_plan; 

static fftw_complex cross_correlation[F_BUFFER_SIZE];
static fftw_complex final_result[F_BUFFER_SIZE];
static fftw_plan inverse_plan;

static sem_t data_semaphore;

void fft_initialize(){
    microphone_1_plan = fftw_plan_dft_1d(F_BUFFER_SIZE, microphone_1, microphone_1_fft, FFTW_FORWARD, FFTW_ESTIMATE);
    microphone_2_plan = fftw_plan_dft_1d(F_BUFFER_SIZE, microphone_2, microphone_2_fft, FFTW_FORWARD, FFTW_ESTIMATE);
    inverse_plan = fftw_plan_dft_1d(F_BUFFER_SIZE, cross_correlation, final_result, FFTW_BACKWARD, FFTW_ESTIMATE);
    printf("FFT plans created\n");

    for (int i = 0; i < F_BUFFER_SIZE; i++)
    {
        microphone_1[i][0] = 0; microphone_1[i][1] = 0;
        microphone_2[i][0] = 0; microphone_2[i][1] = 0;
    }
    printf("Microphone arrays set to zero\n");
    sem_init(&data_semaphore,0,1);
}

static void cross_correlate(int n) { 
    for (int i = 0; i < n; i++) {
        // Complex Conjugate Multiplication: (a+bi) * (c-di)
        double real = microphone_1_fft[i][0] * microphone_2_fft[i][0] + microphone_1_fft[i][1] * microphone_2_fft[i][1];
        double imag = microphone_1_fft[i][1] * microphone_2_fft[i][0] - microphone_1_fft[i][0] * microphone_2_fft[i][1];
        
        double mag = sqrt(real * real + imag * imag);
        
        // PHAT Weighting: Normalize by magnitude
        if (mag > 1e-9) { 
            cross_correlation[i][0] = real / mag;
            cross_correlation[i][1] = imag / mag;
        } else {
            cross_correlation[i][0] = 0;
            cross_correlation[i][1] = 0;
        }
    }
}

float calculate_angle(){
    sem_wait(&data_semaphore);
    fftw_execute(microphone_1_plan);// Calculate fft1
    fftw_execute(microphone_2_plan);// Calculate fft2
    sem_post(&data_semaphore);
    save_calculation_buffer();
    cross_correlate(F_BUFFER_SIZE);
    fftw_execute(inverse_plan); // Calculate inverse
    int best_index = 0;
    double best_value = 0;
    for(int i = 0; i< F_BUFFER_SIZE; i++){
        final_result[i][0] /= F_BUFFER_SIZE;
        final_result[i][1] /= F_BUFFER_SIZE;
        double value = final_result[i][0]*final_result[i][0] + final_result[i][1]*final_result[i][1];

        if(value > best_value){
            best_value = value;
            best_index = i; 
        }
    }
    if(best_index > F_BUFFER_SIZE/2){
        best_index -= F_BUFFER_SIZE;
    }

    float delay = (float)best_index/(float)SAMPLE_RATE;  // seconds
    if(delay > MAX_DELAY || delay < -MAX_DELAY){
        return -1;
    }
    int direction = calculate_general_direction();
    float angle = CONVERSION_CONSTANT*(delay * SPEED_OF_SOUND / MICROPHONE_DISTANCE_METERS);  //i grader
    if (direction == 1)
    {
        return angle;
    }
    else if(direction == 0){
        if (angle >= 0){
            return 180.0f - angle;
        }
        else{
            return -180.0f - angle;
        }
    }
    else
    {
        return -1;
    }
}

void load_data(int32_t* input_samples, int n){
    if(sem_trywait(&data_semaphore) == 0){
        return;
    }
    else{
        for(int i = 0; i < n; i++){
            microphone_1[i][0] = input_samples[i * 2];    
            microphone_2[i][0] = input_samples[i * 2 + 1];
        }
        sem_post(&data_semaphore);
    }
}