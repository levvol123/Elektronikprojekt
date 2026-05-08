CC = gcc
CFLAGS = -Wall -Wextra
LDLIBS = -lfftw3 -lm -lcurl -lportaudio -pthread

OBJ = main.o mems_signal_processing.o

all: program

program: $(OBJ)
	$(CC) $(CFLAGS) -o compiled_program $(OBJ) $(LDLIBS)

clean:
	rm -f $(OBJ) compiled_program