/*
 * sm3_avx2.c — SM3 的 x86 AVX2 优化实现
 *
 * 优化思路(SIMD 寄存器与通用寄存器混合):
 *  1. 消息扩展阶段:用 256 位 YMM 寄存器对"无前后依赖"的异或大项做
 *     4 路并行运算(ROTL、XOR);含依赖链的 P1 递推用 GPR 串行完成。
 *  2. 压缩阶段:8 个链接变量 A..H 放通用寄存器(GPR)中串行推进,
 *     因为 SM3 压缩是强数据依赖链,每轮结果立即参与下一轮,不宜向量化。
 *  3. "扩展用 SIMD、压缩用 GPR"的混合,避免数据在 SIMD/GPR 间频繁倒腾,
 *     是 SM3 在通用 CPU 上最现实的优化路径。
 */
#include "sm3.h"
#include <immintrin.h>
#include <string.h>

/* ---------- GPR 工具 ---------- */
static inline uint32_t rotl32(uint32_t x, int n)
{
    return (x << n) | (x >> (32 - n));
}
#define P0g(x) ((x) ^ rotl32((x), 9) ^ rotl32((x), 17))
#define FF0(x, y, z) ((x) ^ (y) ^ (z))
#define FF1(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define GG0(x, y, z) ((x) ^ (y) ^ (z))
#define GG1(x, y, z) (((x) & (y)) | (~(x) & (z)))

static uint32_t T(int j) { return (j < 16) ? 0x79CC4519u : 0x7A879D8Au; }

/* ---------- SIMD 工具:8 路 32 位循环左移 ---------- */
static inline __m256i rotl256(__m256i x, int n)
{
    return _mm256_or_si256(_mm256_slli_epi32(x, n),
                           _mm256_srli_epi32(x, 32 - n));
}

/*
 * 消息扩展:SIMD 计算外部异或项,GPR 串行补依赖链。
 *   W[j] = P1(W[j-16]^W[j-9]^ROTL(W[j-3],15)) ^ ROTL(W[j-13],7) ^ W[j-6]
 * 每批 4 个:SIMD 先算 ROTL(W[j-13],7)^W[j-6] 与 W[j-16]^W[j-9] 的并行异或
 * (这些对当前批全部已知),再 GPR 串行算含 W[j-3] 的 P1 项并合并。
 */
static void expand_avx2(const uint8_t block[64], uint32_t W[68])
{
    int j, k;
    for (j = 0; j < 16; j++)
        W[j] = ((uint32_t)block[j * 4] << 24) |
               ((uint32_t)block[j * 4 + 1] << 16) |
               ((uint32_t)block[j * 4 + 2] << 8) |
               ((uint32_t)block[j * 4 + 3]);

    for (j = 16; j < 68; j += 4)
    {
        /* 载入窗口(loadu 允许未对齐) */
        __m256i w16 = _mm256_loadu_si256((const __m256i *)&W[j - 16]);
        __m256i w9 = _mm256_loadu_si256((const __m256i *)&W[j - 9]);
        __m256i w13 = _mm256_loadu_si256((const __m256i *)&W[j - 13]);
        __m256i w6 = _mm256_loadu_si256((const __m256i *)&W[j - 6]);
        /* 并行:ext = (W[j-16]^W[j-9]) ^ (ROTL(W[j-13],7) ^ W[j-6]) */
        __m256i ext = _mm256_xor_si256(
            _mm256_xor_si256(w16, w9),
            _mm256_xor_si256(rotl256(w13, 7), w6));
        uint32_t ext_lane[8];
        _mm256_storeu_si256((__m256i *)ext_lane, ext);
        /* GPR 串行:P1 依赖项(含 W[j-3]);与 SIMD 结果合并:
         * W = P1(inner) ^ ROTL(W[j-13],7) ^ W[j-6]
         *   = P1(inner) ^ (ext ^ W[j-16] ^ W[j-9])          */
        for (k = 0; k < 4 && (j + k) < 68; k++)
        {
            uint32_t inner = W[j + k - 16] ^ W[j + k - 9] ^ rotl32(W[j + k - 3], 15);
            uint32_t p1 = inner ^ rotl32(inner, 15) ^ rotl32(inner, 23);
            W[j + k] = p1 ^ (ext_lane[k] ^ W[j + k - 16] ^ W[j + k - 9]);
        }
    }
}

/* 压缩(纯 GPR,与基础实现相同) */
static void compress_gpr(uint32_t state[8], const uint32_t W[68])
{
    uint32_t A = state[0], B = state[1], C = state[2], D = state[3];
    uint32_t E = state[4], F = state[5], G = state[6], H = state[7];
    int j;
    for (j = 0; j < 64; j++)
    {
        uint32_t A12 = rotl32(A, 12);
        uint32_t SS1 = rotl32(A12 + E + rotl32(T(j), j % 32), 7);
        uint32_t SS2 = SS1 ^ A12;
        uint32_t TT1, TT2;
        if (j < 16)
        {
            TT1 = FF0(A, B, C) + D + SS2 + (W[j] ^ W[j + 4]);
            TT2 = GG0(E, F, G) + H + SS1 + W[j];
        }
        else
        {
            TT1 = FF1(A, B, C) + D + SS2 + (W[j] ^ W[j + 4]);
            TT2 = GG1(E, F, G) + H + SS1 + W[j];
        }
        D = C;
        C = rotl32(B, 9);
        B = A;
        A = TT1;
        H = G;
        G = rotl32(F, 19);
        F = E;
        E = P0g(TT2);
    }
    state[0] ^= A;
    state[1] ^= B;
    state[2] ^= C;
    state[3] ^= D;
    state[4] ^= E;
    state[5] ^= F;
    state[6] ^= G;
    state[7] ^= H;
}

/* 对单块数据(<=55 字节)做完整 SM3。为演示混合优化,
 * 这里只支持一次性短消息(与测试向量长度一致),长消息退化为逐块基础处理。 */
void sm3_hash_avx2(const uint8_t *data, size_t len, uint8_t digest[32])
{
    uint32_t state[8] = {
        0x7380166F, 0x4914B2B9, 0x172442D7, 0xDA8A0600,
        0xA96F30BC, 0x163138AA, 0xE38DEE4D, 0xB0FB0E4E};
    uint8_t block[64];
    uint32_t W[68];
    size_t full = len / 64;
    size_t rem = len % 64;
    size_t i;
    int k;

    /* 完整分组 */
    for (i = 0; i < full; i++)
    {
        expand_avx2(data + i * 64, W);
        compress_gpr(state, W);
    }
    /* 尾部分组(含填充) */
    memset(block, 0, sizeof(block));
    memcpy(block, data + full * 64, rem);
    block[rem] = 0x80;
    if (rem > 55)
    {
        expand_avx2(block, W);
        compress_gpr(state, W);
        memset(block, 0, 56);
    }
    {
        uint64_t bit_len = (uint64_t)len * 8;
        for (k = 0; k < 8; k++)
            block[56 + k] = (uint8_t)(bit_len >> (56 - 8 * k));
    }
    expand_avx2(block, W);
    compress_gpr(state, W);

    for (k = 0; k < 8; k++)
    {
        digest[k * 4] = (uint8_t)(state[k] >> 24);
        digest[k * 4 + 1] = (uint8_t)(state[k] >> 16);
        digest[k * 4 + 2] = (uint8_t)(state[k] >> 8);
        digest[k * 4 + 3] = (uint8_t)(state[k]);
    }
}
