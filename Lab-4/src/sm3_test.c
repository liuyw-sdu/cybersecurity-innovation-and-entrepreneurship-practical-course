/*
 * sm3_test.c — 正确性测试与性能基准
 *
 * 用法:
 *   ./sm3_test        运行正确性测试(与 GB/T 32905 标准测试向量比对)
 *   ./sm3_test bench  运行性能基准(基础版 vs AVX2 版)
 */
#include "sm3.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/* 各优化实现 */
void sm3_hash_avx2(const uint8_t *data, size_t len, uint8_t digest[32]);
void sm3_hash_neon(const uint8_t *data, size_t len, uint8_t digest[32]);

/* GB/T 32905-2016 附录测试向量 1:"abc" */
static const uint8_t TV1_MSG[] = {'a', 'b', 'c'};
static const uint8_t TV1_MD[32] = {
    0x66, 0xc7, 0xf0, 0xf4, 0x62, 0xee, 0xed, 0xd9,
    0xd1, 0xf2, 0xd4, 0x6b, 0xdc, 0x10, 0xe4, 0xe2,
    0x41, 0x67, 0xc4, 0x87, 0x5c, 0xf2, 0xf7, 0xa2,
    0x29, 0x7d, 0xa0, 0x2b, 0x8f, 0x4b, 0xa8, 0xe0};

/* 测试向量 2:64 字节 "abcd" 重复 16 次 */
static const uint8_t TV2_MD[32] = {
    0xde, 0xbe, 0x9f, 0xf9, 0x22, 0x75, 0xb8, 0xa1,
    0x38, 0x60, 0x48, 0x89, 0xc1, 0x8e, 0x5a, 0x4d,
    0x6f, 0xdb, 0x70, 0xe5, 0x38, 0x7e, 0x57, 0x65,
    0x29, 0x3d, 0xcb, 0xa3, 0x9c, 0x0c, 0x57, 0x32};

static int hex_eq(const uint8_t *a, const uint8_t *b)
{
    return memcmp(a, b, 32) == 0;
}

static void report(const char *name, const uint8_t *got, const uint8_t *want)
{
    printf("  %-22s : ", name);
    sm3_print_hex(got);
    printf("  %-22s : %s\n\n", "结果", hex_eq(got, want) ? "PASS" : "FAIL");
}

int main(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "bench") == 0)
    {
        /* ---------- 性能基准 ---------- */
        const size_t LEN = 64 * 1024; /* 64 KB 数据 */
        const int REP = 2000;         /* 重复次数 */
        uint8_t *buf = malloc(LEN);
        uint8_t d[32];
        size_t i;
        clock_t t0, t1;
        for (i = 0; i < LEN; i++)
            buf[i] = (uint8_t)i;

        t0 = clock();
        for (i = 0; i < REP; i++)
            sm3_hash(buf, LEN, d);
        t1 = clock();
        double base_s = (double)(t1 - t0) / CLOCKS_PER_SEC;
        printf("基础实现  : %.3f s  (%.2f MB/s)\n",
               base_s, (double)LEN * REP / base_s / 1e6);

        t0 = clock();
        for (i = 0; i < REP; i++)
            sm3_hash_avx2(buf, LEN, d);
        t1 = clock();
        double avx2_s = (double)(t1 - t0) / CLOCKS_PER_SEC;
        printf("AVX2 优化 : %.3f s  (%.2f MB/s)\n",
               avx2_s, (double)LEN * REP / avx2_s / 1e6);

#ifdef __aarch64__
        t0 = clock();
        for (i = 0; i < REP; i++)
            sm3_hash_neon(buf, LEN, d);
        t1 = clock();
        double neon_s = (double)(t1 - t0) / CLOCKS_PER_SEC;
        printf("NEON 优化 : %.3f s  (%.2f MB/s)\n",
               neon_s, (double)LEN * REP / neon_s / 1e6);
#endif
        free(buf);
        return 0;
    }

    /* ---------- 正确性测试 ---------- */
    uint8_t d[32];

    /* 向量 1 */
    printf("测试向量 1: \"abc\"\n");
    sm3_hash(TV1_MSG, 3, d);
    report("基础实现 sm3_hash", d, TV1_MD);

    sm3_hash_avx2(TV1_MSG, 3, d);
    report("AVX2 sm3_hash_avx2", d, TV1_MD);

    sm3_hash_neon(TV1_MSG, 3, d);
    report("NEON sm3_hash_neon", d, TV1_MD);

    /* 向量 2:64 字节 */
    {
        uint8_t msg[64];
        int i;
        for (i = 0; i < 16; i++)
            memcpy(msg + i * 4, "abcd", 4);

        printf("测试向量 2: \"abcd\"x16 (64 字节)\n");
        sm3_hash(msg, 64, d);
        report("基础实现 sm3_hash", d, TV2_MD);

        sm3_hash_avx2(msg, 64, d);
        report("AVX2 sm3_hash_avx2", d, TV2_MD);

        sm3_hash_neon(msg, 64, d);
        report("NEON sm3_hash_neon", d, TV2_MD);
    }

    /* 流式(update 分段)一致性 */
    {
        const char *m = "The quick brown fox jumps over the lazy dog";
        sm3_ctx c;
        uint8_t d2[32];
        sm3_init(&c);
        sm3_update(&c, (const uint8_t *)m, 10);
        sm3_update(&c, (const uint8_t *)m + 10, strlen(m) - 10);
        sm3_final(&c, d2);
        sm3_hash((const uint8_t *)m, strlen(m), d);
        printf("流式分段一致性: %s\n", hex_eq(d, d2) ? "PASS" : "FAIL");
    }
    return 0;
}
