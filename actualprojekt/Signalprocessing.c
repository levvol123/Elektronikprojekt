#include <stdio.h>
#include <math.h>

int main() {
    printf("test\n");
    return 0;
}

int detect_peak(int* data) {
    return 0;
}

float calculate_angle(float delta_time) {
    float c = 340; //ljudhastighet
    float d = 0.01; //avstånd mellan mikrofoner
    return asin(c*delta_time/d);
}