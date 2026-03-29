#include <stdio.h>
#include <math.h>

int* padd_zero(int* v) {

}


float cross_correlation(int* v1, int* v2, int length) {
    float max_delta_time = 0; //ÄNDRA!!!!
    float t_sample = 0;
    int max_padding = (int) max_delta_time/t_sample;
    int best_index;
    int best_sum = 0;
    int m_v1 = max(v1);
    int m_v2 = max(v2);
    if (m_v1 <= m_v2) {
        for (int current_padding = 0; current_padding < max_padding; current_padding++) {
            int sum = 0;
            for (int i = 0; i < length-1; i++) {
                sum += v1[i]*v2[i];
            }
            if (sum > best_sum) {
                best_index = current_padding;
                best_sum = sum;
            }
            padd_zero(v2);
        }
    } else {
        for (int current_padding = 0; current_padding < max_padding; current_padding++) {
            int sum = 0;
            for (int i = 0; i < length-1; i++) {
                sum += v1[i]*v2[i];
            }
            if (sum > best_sum) {
                best_index = current_padding;
                best_sum = sum;
            }
            padd_zero(v1);
        }
    }


}

double calculate_angle(float delta_time) {
    float c = 340; //ljudhastighet
    float d = 0.01; //avstånd mellan mikrofoner
    return asin((double) (c*delta_time)/d)*180/3.1415926;
}

int main() {
    int* vector1 = {0, 0, 1, 3, 5, 2, 0};
    int* vector2 = {2, 4, 6, 2, 0, 0, 0};

}
