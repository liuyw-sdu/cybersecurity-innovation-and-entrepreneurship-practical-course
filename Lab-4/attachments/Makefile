# Lab-4 SM3 软件实现与优化 — Makefile
CC      ?= gcc
CFLAGS  ?= -O2 -Wall -Wextra
# 自动检测:若 CPU 支持 AVX2 则加上 -mavx2
ARCH_FLAG := $(shell grep -qm1 avx2 /proc/cpuinfo 2>/dev/null && echo -mavx2)

SRCS = sm3.c sm3_avx2.c sm3_neon.c sm3_test.c
OBJS = $(SRCS:.c=.o)

all: sm3_test

%.o: %.c sm3.h
	$(CC) $(CFLAGS) $(ARCH_FLAG) -c $< -o $@

sm3_test: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

test: sm3_test
	./sm3_test

bench: sm3_test
	./sm3_test bench

clean:
	rm -f $(OBJS) sm3_test

.PHONY: all test bench clean
