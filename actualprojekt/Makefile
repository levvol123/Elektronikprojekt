DIR_Config   = ./lib/Config
DIR_DRIVER      = ./lib/Driver
DIR_Examples = ./examples
DIR_BIN      = ./bin

CC = aarch64-linux-gnu-gcc
CFLAGS = -Wall -Wextra -pthread
LDLIBS = -lgpiod -lm -lcurl

OBJ = Main.o Datafetcher.o Signalprocessing2.o

all: program

program: $(OBJ)
	$(CC) $(CFLAGS) -o compiled_program $(OBJ) $(LDLIBS)

Main.o: Main.c Datafetcher.h Signalprocessing2.h 
	$(CC) $(CFLAGS) -c Main.c

Datafetcher.o: Datafetcher.c Datafetcher.h
	$(CC) $(CFLAGS) -c Datafetcher.c

Signalprocessing2.o: Signalprocessing2.c Signalprocessing2.h
	$(CC) $(CFLAGS) -c Signalprocessing2.c

clean:
	rm -f Main.o Datafetcher.o Signalprocessing2.o compiled_program
