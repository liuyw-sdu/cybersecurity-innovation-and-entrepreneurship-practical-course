CC=gcc


CFLAGS=-O3 -Wall

AVXFLAGS=-mavx2 -mssse3


TARGET=bin/gift_benchmark



all:

	$(CC) $(CFLAGS) \
	src/gift.c \
	src/gift_bitslice.c \
	src/gift_avx.c \
	src/benchmark.c \
	-Iinclude \
	$(AVXFLAGS) \
	-o $(TARGET)



clean:

	rm -f bin/*

