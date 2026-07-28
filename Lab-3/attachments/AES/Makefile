CC=gcc


CFLAGS=-O3 -Wall


AES_FLAGS=-maes -msse4 -mpclmul



TARGET=aes_benchmark



BASIC_SRC=basic/aes.c


TTABLE_SRC=ttable/aes_ttable.c


AESNI_SRC=aesni/aesni.c


GCM_SRC=gcm/gcm.c


BENCH_SRC=benchmark/bench.c





all:
	$(CC) $(CFLAGS) \
	$(BASIC_SRC) \
	$(TTABLE_SRC) \
	$(AESNI_SRC) \
	$(GCM_SRC) \
	$(BENCH_SRC) \
	$(AES_FLAGS) \
	-o $(TARGET)





basic:

	$(CC) $(CFLAGS) \
	$(BASIC_SRC) \
	$(BENCH_SRC) \
	-o aes_basic





ttable:

	$(CC) $(CFLAGS) \
	$(TTABLE_SRC) \
	$(BENCH_SRC) \
	-o aes_ttable





aesni:

	$(CC) $(CFLAGS) \
	$(AES_FLAGS) \
	$(AESNI_SRC) \
	$(BENCH_SRC) \
	-o aesni_test





gcm:

	$(CC) $(CFLAGS) \
	$(AES_FLAGS) \
	$(AESNI_SRC) \
	$(GCM_SRC) \
	$(BENCH_SRC) \
	-o aes_gcm





clean:

	rm -f aes_benchmark
	rm -f aes_basic
	rm -f aes_ttable
	rm -f aesni_test
	rm -f aes_gcm
