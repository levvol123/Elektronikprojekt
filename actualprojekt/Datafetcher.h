#ifndef Datafetcher.h
#define Datafetcher.h

int f_configure(int samplerate);

void f_start_loop();

static void* f_loop();

void f_exception_handler();

#endif // !Datafetcher.h
