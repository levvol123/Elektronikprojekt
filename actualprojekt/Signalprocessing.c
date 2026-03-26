#include <stdio.h>
#include <math.h>


float calculate_delta_time() {
    
}


double calculate_angle(float delta_time) {
    float c = 340; //ljudhastighet
    float d = 0.01; //avstånd mellan mikrofoner
    return asin((double) (c*delta_time)/d)*180/3.1415926;
}


int main() {
    float t1 = 0.00001;
    float t2 = 0.00003;
    float d_t = t2 - t1;
    printf("%lf\n", calculate_angle(d_t));
}
