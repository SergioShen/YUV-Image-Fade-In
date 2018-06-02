CC = g++
CFLAGS = -std=c++11

all: yuv-fade-in

yuv-fade-in: main.cpp lib.o
	$(CC) $(CFLAGS) -o yuv-fade-in main.cpp lib.o

lib.o: lib.h lib.cpp
	$(CC) $(CFLAGS) -c lib.cpp

clean:
	rm -f yuv-fade-in *.o