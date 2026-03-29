#include "Datafetcher.h"
#include <pthread.h> //https://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread.h.html

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
	pthread_create(&adc_thread, NULL, f_loop, NULL);
}
static void* f_loop() {
	//fetcha från adc när DRDY = 0
	//skriv till någon buffer
	return NULL;
}
void f_exception_handler() {
	DEV_Module_Exit();
	//exit(0);
}