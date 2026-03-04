#define CAMERA_IMPLEMENTATION
#include "Camera.h"

int main()
{
	c_setup();
	c_set_password("root:root");
	c_set_ip("169.254.45.199");
	c_pan(0);
	c_tilt(0);
	return 0;
}