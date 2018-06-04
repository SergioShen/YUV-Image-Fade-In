CC = g++
CFLAGS = -std=c++11 -O1 -mfpmath=387

all: yuv-fade-in-base yuv-fade-in-mmx yuv-fade-in-sse2 yuv-fade-in-avx

yuv-fade-in-base: main.cpp lib-base.o
	$(CC) $(CFLAGS) -o yuv-fade-in-base main.cpp lib-base.o

yuv-fade-in-mmx: main.cpp lib-mmx.o
	$(CC) $(CFLAGS) -o yuv-fade-in-mmx -DMMX main.cpp lib-mmx.o

yuv-fade-in-sse2: main.cpp lib-sse2.o
	$(CC) $(CFLAGS) -o yuv-fade-in-sse2 -DSSE2 main.cpp lib-sse2.o

yuv-fade-in-avx: main.cpp lib-avx.o
	$(CC) $(CFLAGS) -o yuv-fade-in-avx -DAVX main.cpp lib-avx.o

lib-base.o: lib.h lib.cpp
	$(CC) $(CFLAGS) -o lib-base.o -c lib.cpp

lib-mmx.o: lib.h lib.cpp
	$(CC) $(CFLAGS) -o lib-mmx.o -DMMX -c lib.cpp

lib-sse2.o: lib.h lib.cpp
	$(CC) $(CFLAGS) -o lib-sse2.o -c -DSSE2 lib.cpp

lib-avx.o: lib.h lib.cpp
	$(CC) $(CFLAGS) -o lib-avx.o -c -DAVX lib.cpp

clean:
	rm -f yuv-fade-in-* *.o