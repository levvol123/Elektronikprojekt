#include "Signalprocessing2.h"
#include "Datafetcher.h"
#include <pthread.h>
#include <stdio.h>

int main() {
    printf("Initializing");
    f_configure(100);
    init_signalprocessing();
    printf("Starting tasks");
    f_start_loop();
    rotate_camera_loop();
    f_stop_loop();

    return 0;
}