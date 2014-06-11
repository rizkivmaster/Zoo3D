#include <math.h>
#define PI 3.141592

void swap(double *x, double *y) {
	double temp = *x;
	*x = *y;
	*y = temp;
}

double sin_d(double degree) {

	return sin(degree*PI/180.0);

}

double cos_d(double degree) {
	return cos(degree*PI/180.0);
}