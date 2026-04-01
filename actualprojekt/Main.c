#include "Signalprocessing2.h"
#include "Datafetcher.h"
#include "ADS1263.h"
#include <pthread.h>
#include <stdio.h>

int main() {
    printf("Initializing");
    f_configure(ADS1263_19200SPS);
    //init singalprocessing2
    printf("Starting tasks");
    f_start_loop();
    //starta signalprocessing2 loop?

    return 0;
}