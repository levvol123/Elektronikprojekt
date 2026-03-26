#include <stdio.h>
#include <math.h>

#define ITERATIONS 10000
#define LEARNING_RATE 1e-7
#define C 330.0

double ax=0, ay=0;
double bx=0, by=0;
double cx=0, cy=0;
double tb=0, tc=0;

void residuals(double x0, double y0, double r, double res[3]) {
    res[0] = (ax-x0)*(ax-x0) + (ay-y0)*(ay-y0) - r*r;

    res[1] = (bx-x0)*(bx-x0) + (by-y0)*(by-y0) -
             (r + C*tb)*(r + C*tb);

    res[2] = (cx-x0)*(cx-x0) + (cy-y0)*(cy-y0) -
             (r + C*tc)*(r + C*tc);
}

double cost(double x0, double y0, double r) {
    double res[3];
    residuals(x0,y0,r,res);

    return res[0]*res[0] + res[1]*res[1] + res[2]*res[2];
}

int main() {

    double x0 = 0;
    double y0 = 0;
    double r  = 1000;

    for(int i=0;i<ITERATIONS;i++) {

        double eps = 1e-6;

        double gx = (cost(x0+eps,y0,r)-cost(x0,y0,r))/eps;
        double gy = (cost(x0,y0+eps,r)-cost(x0,y0,r))/eps;
        double gr = (cost(x0,y0,r+eps)-cost(x0,y0,r))/eps;

        x0 -= LEARNING_RATE * gx;
        y0 -= LEARNING_RATE * gy;
        r  -= LEARNING_RATE * gr;

        if(r < 0) r = 0;
    }

    printf("Solution:\n");
    printf("x0 = %f\n",x0);
    printf("y0 = %f\n",y0);
    printf("r  = %f\n",r);

    return 0;
}
