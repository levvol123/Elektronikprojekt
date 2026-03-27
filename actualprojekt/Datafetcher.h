#ifndef DATAFETCHER_H
#define DATAFETCHER_H
#include "ADS1263.h"


int f_configure(ADS1263_DRATE samplerate);

void f_start_loop();

static void* f_loop();

void f_exception_handler();

#endif // !Datafetcher.h
