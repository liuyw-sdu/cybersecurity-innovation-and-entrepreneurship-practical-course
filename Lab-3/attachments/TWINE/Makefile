CC=gcc


TARGET=bin/twine_benchmark


# 调试编译选项
CFLAGS= -O0 -g3 -Wall \
        -fno-omit-frame-pointer \
        -fsanitize=address \
        -fsanitize=undefined \
        -msse2


INCLUDE=-Iinclude



SRC= \
src/twine.c \
src/twine_key.c \
src/twine_sbox.c \
\
src/twine_bitslice.c \
src/twine_bitslice_sbox.c \
src/twine_bitslice_key.c \
\
src/twine_sse.c \
src/twine_sse_sbox.c \
src/twine_sse_key.c \
\
src/benchmark.c




all:
	mkdir -p bin

	$(CC) \
	$(CFLAGS) \
	$(SRC) \
	$(INCLUDE) \
	-o $(TARGET)




clean:

	rm -f bin/*



run:

	./$(TARGET)
