#ifndef CAMERA_H
#define CAMERA_H
#include <curl/curl.h>
#include <string.h>
#include <stdio.h>
void c_pan(int degrees);
void c_tilt(int degrees);
int c_setup();
void c_set_ip(char ip[]);
void c_set_password(char user_password[]);
void c_exit();

#if defined(CAMERA_IMPLEMENTATION)
static CURL* curl;
static char ip_adress[16];
static char password[64];

//setup för curl, bör köras en gång i början av koden.
int c_setup() {
	curl = curl_easy_init();
	if (!curl) {
		return -1;
	}

	return 0;
}
//cleanup av curl, friar minnet.
void c_exit() {
	curl_easy_cleanup(curl);
}
//funkar med värden från -360 till 360.
void c_pan(int degrees) {
	CURLcode result;
	char buffer[128];
	snprintf(buffer, sizeof(buffer), "http://%s/axis-cgi/com/ptz.cgi?pan=%d&camera=1", ip_adress, degrees);
	curl_easy_setopt(curl, CURLOPT_URL, buffer);
	curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_DIGEST);
	curl_easy_setopt(curl, CURLOPT_USERPWD, password);
	result = curl_easy_perform(curl);
	if (result != CURLE_OK)
	{
		printf("%s\n", curl_easy_strerror(result));
	}
}
//c_tilt funkar bara mellan 0 och 90. Allt över 90 blir 90, och allt under 0 blir 0.
void c_tilt(int degrees) {
	CURLcode result;
	char buffer[128];
	snprintf(buffer, sizeof(buffer), "http://%s/axis-cgi/com/ptz.cgi?tilt=%d&camera=1", ip_adress, degrees);
	curl_easy_setopt(curl, CURLOPT_URL, buffer);
	curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_DIGEST);
	curl_easy_setopt(curl, CURLOPT_USERPWD, password);
	result = curl_easy_perform(curl);
	if (result != CURLE_OK)
	{
		printf("%s\n", curl_easy_strerror(result));
	}
}
// Uppdaterar ip:n, där ip:n är en string (char[])
void c_set_ip(char ip[]) {
	strcpy_s(ip_adress, sizeof(ip_adress), ip); //kanske inte funkar på raspberry pi, vi får se! Kanske också lägga till att vi checkar returnvalue av strcpy för att se om vi får felmeddelanden
	//strcpy(ip_adress, ip);
}
// Uppdaterar lösenordet, där lösenordet är en string (char[])
void c_set_password(char user_password[]) {
	strcpy_s(password, sizeof(password), user_password);
}
#endif //CAMERA_IMPLEMENTATION

#endif //CAMERA_H