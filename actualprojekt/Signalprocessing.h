#ifndef SIGNALPROCESSING_H
#define SIGNALPROCESSING_H
#include "Datafetcher.h"

void init_signalprocessing();
void residuals(double x0, double y0, double r, double res[3]);
double cos_t(double x0, double y0, double r);
void calculate_angle();
void rotate_camera(double x, double y);
void position_calculation_loop();

#endif // !Datafetcher.h
