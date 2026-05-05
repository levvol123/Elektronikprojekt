#include "Signalprocessing2.h"
#include "Datafetcher.h"
#include <pthread.h>
#include <stdio.h>

int main() {
    printf("Initializing\n");
    f_configure(20000);
    init_signalprocessing();
    printf("Starting tasks\n");
    f_start_loop();
    rotate_camera_loop();
    //position_calculation_loop();
    printf("Stopped camera loop");
    f_stop_loop();
    printf("EXIT");
    return 0;
}