#include "Datafetcher.h"
#include <pthread.h> //https://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread.h.html
#include <stdatomic.h>

#define BUFFER_SIZE 128

static int current_state;
static int previous_state;

static pthread_t adc_thread;

static Sample circular_buffer[BUFFER_SIZE];

static atomic_int head = 0;
static atomic_int tail = 0;

int f_configure(ADS1263_DRATE samplerate) {
	DEV_Module_Init();
	ADS1263_SetMode(0);
	if (ADS1263_init_ADC1(samplerate) == 1) {
		f_exception_handler();
		return 1;
	}
	return 0;
}
void f_start_loop() {
	current_state = 1;
	previous_state = 1;
	pthread_create(&adc_thread, NULL, f_loop, NULL);
}
static void* f_loop() {
	while(1){
		current_state = DEV_Digital_Read(DEV_DRDY_PIN);
		if(current_state == 0 && previous_state == 1){
			circular_buffer[head].samples[0] = ADS1263_GetChannalValue(0); //Läs från GPIO och skriv till buffer
			circular_buffer[head].samples[1] = ADS1263_GetChannalValue(1);
			circular_buffer[head].samples[2] = ADS1263_GetChannalValue(2);
			atomic_store(&head, (head +1) % BUFFER_SIZE); //increment head
		} 
		previous_state = current_state;
	}
	return NULL;
}
void f_exception_handler() {
	DEV_Module_Exit();
	//exit(0);
}
int get_latest_sample(Sample* latest_sample){
	if(atomic_load(&head) == tail){
		return 0; //inga nya samples
	}
	else{
		*latest_sample = circular_buffer[tail];
		atomic_store(&tail, (tail +1) % BUFFER_SIZE); //increment tail
		return 1;
	}
}
