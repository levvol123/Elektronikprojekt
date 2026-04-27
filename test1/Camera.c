#define CAMERA_IMPLEMENTATION
#include "Camera.h"

int main()
{
	srand(time(NULL));
	c_setup();
	c_set_password("root:root");
	c_set_ip("169.254.45.199");
	c_pan(rand()% 90);
	c_tilt(rand()% 90);
	c_exit();
	return 0;
}