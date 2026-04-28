CC = aarch64-linux-gnu-gcc
CFLAGS = -Wall -Wextra -pthread -lcurl -lgpiod
LIBS = -lgpiod -lcurl -lm -lpthread

Datafetcher.o:
	$(CC) $(CFLAGS) -c Datafetcher.c -o /dev/null

Signalprocessing2.o:
	$(CC) $(CFLAGS) -c Signalprocessing2.c -o /dev/null
Main.o:
	$(CC) $(CFLAGS) -c Main.c -o /dev/null
build:
	$(CC) -pthread -I/usr/include/aarch64-linux-gnu -o compiled_program Main.c Datafetcher.c Signalprocessing2.c $(LIBS)