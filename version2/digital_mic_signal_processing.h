#ifndef DIGITAL_MIC_SIGNAL_PROCESSING_H
#define DIGITAL_MIC_SIGNAL_PROCESSING_H
typedef struct {
	int samples[2]; 
} Sample; //Struct för ett set av samples (vid tid t)

int digital_mic_configure(int samplerate_hz);
void f_start_loop();
void f_stop_loop();
int calculate_general_direction();
void save_calculation_buffer();
void get_full_buffer(Sample* sample_array);

#endif