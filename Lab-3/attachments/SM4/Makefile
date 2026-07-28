CC=gcc


CFLAGS= -O3 -Wall -mavx2



SRC= \
src/sm4_basic.c \
src/sm4_ttable.c \
src/sm4_ttable_table.c \
src/sm4_simd.c \
src/sm4_sbox.c \
src/benchmark.c



all:

	mkdir -p bin

	$(CC) $(CFLAGS) \
	$(SRC) \
	-Iinclude \
	-o bin/sm4_benchmark



clean:

	rm -rf bin/*
