#include <stdio.h>
#include <math.h>

int max(int* v, int length) {
    int max_value = v[0];
    for (int i = 1; i < length; i++) {
        if (v[i] > max_value) {
            max_value = v[i];
        }
    }
    return max_value;
}

int* padd_zero(int* v, int length) {
    for (int i=(length-1); i >= 0; i--){
        if (i==0){
            v[0]=0;
        }
        else{
            v[i]=v[i-1];
        }
    }
}



float cross_correlation(int* v1, int* v2, int length) {
    float max_delta_time = 4; 
    float t_sample = 1;
    int max_padding = (int) max_delta_time/t_sample;
    int best_padding = 0;
    int best_sum = 0;
    int m_v1 = max(v1, length);
    int m_v2 = max(v2, length);
    if (m_v1 <= m_v2) {
        for (int current_padding = 0; current_padding < max_padding; current_padding++) {
            int sum = 0;
            for (int i = 0; i < length; i++) {
                sum += v1[i]*v2[i];
            }
            if (sum > best_sum) {
                best_padding = current_padding;
                best_sum = sum;
            }
            padd_zero(v2, length);
        }
    } else {
        for (int current_padding = 0; current_padding < max_padding; current_padding++) {
            int sum = 0;
            for (int i = 0; i < length; i++) {
                sum += v1[i]*v2[i];
            }
            if (sum > best_sum) {
                best_padding = current_padding;
                best_sum = sum;
            }
            padd_zero(v1, length);
        }
    }
    float delta_time = best_padding * t_sample;
    return delta_time;
}

double calculate_angle(float delta_time) {
    const double c = 340.0;
    const double d = 0.01;
    double ratio = (c * delta_time) / d;
    if (ratio > 1.0) ratio = 1.0;
    if (ratio < -1.0) ratio = -1.0;
    return asin(ratio) * 180.0 / 3.14159265;
}

int main() {
    int vector1[7] = {0, 0, 1, 3, 5, 2, 0};
    int vector2[7] = {2, 4, 6, 2, 0, 0, 0};
    float delta_time = cross_correlation(vector1, vector2, 7);
    double angle = calculate_angle(delta_time);
    printf("delta_time = %.3f s, angle = %.2f°\n", delta_time, angle);
    return 0;
}
