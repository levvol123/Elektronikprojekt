#include "Datafetcher.h"
#include "ADS1263.h"

void f_init(ADS1263_DRATE samplerate) {
	DEV_Module_Init();
	ADS1263_SetMode(0);
	if (ADS1263_init_ADC1(samplerate) == 1) {
		f_exception_handler();
	}

}
void f_loop() {

}
void f_exception_handler() {
	DEV_Module_Exit();
	exit(0);
}


