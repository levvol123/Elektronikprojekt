#include "Datafetcher.h"
#include <pthread.h> //https://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread.h.html

static int current_state;
static int previous_state;

static pthread_t adc_thread;

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
			//skriv till buffer
		} 
		previous_state = current_state;
	}
	//fetcha från adc när DRDY = 0
	//skriv till någon buffer
	return NULL;
}
void f_exception_handler() {
	DEV_Module_Exit();
	//exit(0);
}