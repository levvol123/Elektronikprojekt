#pragma once
#include <curl/curl.h>
#include <string.h>
#include <stdio.h>
void c_pan(int degrees);
void c_tilt(int degrees);
int c_setup();
void c_set_ip(char ip[]);
static CURL* curl;
static char ip_adress[16];

#if defined(CAMERA_IMPLEMENTATION)
int c_setup() {
	curl = curl_easy_init();
	if (!curl) {
		return -1;
	}

	return 0;
}
void c_pan(int degrees) {
	CURLcode result;
	char buffer[128];
	snprintf(buffer, sizeof(buffer), "http://%s/axis-cgi/com/ptz.cgi?pan=%d&camera=1", ip_adress, degrees);
	curl_easy_setopt(curl, CURLOPT_URL, buffer);
	result = curl_easy_perform(curl);
	if (result != CURLE_OK)
	{
		printf("%s\n", curl_easy_strerror(result));
	}
}
void c_tilt(int degrees) {

}
void c_set_ip(char ip[]) {
	strcpy(ip_adress, ip);
}
#endif