CC = g++
CFLAGS = -std=c++11

all: yuv-fade-in-base yuv-fade-in-mmx yuv-fade-in-sse2 yuv-fade-in-avx

yuv-fade-in-base: main.cpp lib.o
	$(CC) $(CFLAGS) -o yuv-fade-in-base main.cpp lib.o

yuv-fade-in-mmx: main.cpp lib.o
	$(CC) $(CFLAGS) -o yuv-fade-in-mmx -DMMX main.cpp lib.o

yuv-fade-in-sse2: main.cpp lib.o
	$(CC) $(CFLAGS) -o yuv-fade-in-sse2 -DSSE2 main.cpp lib.o

yuv-fade-in-avx: main.cpp lib.o
	$(CC) $(CFLAGS) -o yuv-fade-in-avx -DAVX main.cpp lib.o

lib.o: lib.h lib.cpp
	$(CC) $(CFLAGS) -c lib.cpp

clean:
	rm -f yuv-fade-in-* *.o