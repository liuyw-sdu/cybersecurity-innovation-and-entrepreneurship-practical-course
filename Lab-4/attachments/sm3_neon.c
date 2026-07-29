/*
 * sm3_neon.c — SM3 的 ARM64 (AArch64) NEON 优化实现
 *
 * 优化思路(与 x86 AVX2 版同源,SIMD 寄存器与通用寄存器混合):
 *  1. 消息扩展:用 128 位 NEON 寄存器(uint32x4_t)对无依赖的异或大项
 *     做 4 路并行 ROTL/XOR;含依赖链的 P1 递推由 GPR 串行完成。
 *  2. 压缩阶段:8 个链接变量放 GPR 串行推进(强数据依赖,不宜向量化)。
 *
 * 本文件仅在 __aarch64__ 下编译有效;其它平台编译为空实现以便链接通过。
 */
#include "sm3.h"
#include <string.h>

#ifdef __aarch64__
#include <arm_neon.h>

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

/* ---------- NEON 工具:4 路 32 位循环左移 ---------- */
static inline uint32x4_t rotl128(uint32x4_t x, int n)
{
    return vorrq_u32(vshlq_n_u32(x, n), vshrq_n_u32(x, 32 - n));
}

/* 消息扩展:NEON 算外部项,GPR 补 P1 依赖(与 AVX2 版相同策略) */
static void expand_neon(const uint8_t block[64], uint32_t W[68])
{
    int j, k;
    for (j = 0; j < 16; j++)
        W[j] = ((uint32_t)block[j * 4] << 24) |
               ((uint32_t)block[j * 4 + 1] << 16) |
               ((uint32_t)block[j * 4 + 2] << 8) |
               ((uint32_t)block[j * 4 + 3]);

    for (j = 16; j < 68; j += 4)
    {
        uint32x4_t w16 = vld1q_u32(&W[j - 16]);
        uint32x4_t w9 = vld1q_u32(&W[j - 9]);
        uint32x4_t w13 = vld1q_u32(&W[j - 13]);
        uint32x4_t w6 = vld1q_u32(&W[j - 6]);
        /* ext = (W[j-16]^W[j-9]) ^ (ROTL(W[j-13],7) ^ W[j-6]) */
        uint32x4_t ext = veorq_u32(veorq_u32(w16, w9),
                                   veorq_u32(rotl128(w13, 7), w6));
        uint32_t ext_lane[4];
        vst1q_u32(ext_lane, ext);
        for (k = 0; k < 4 && (j + k) < 68; k++)
        {
            uint32_t inner = W[j + k - 16] ^ W[j + k - 9] ^ rotl32(W[j + k - 3], 15);
            uint32_t p1 = inner ^ rotl32(inner, 15) ^ rotl32(inner, 23);
            W[j + k] = p1 ^ (ext_lane[k] ^ W[j + k - 16] ^ W[j + k - 9]);
        }
    }
}

/* 压缩:纯 GPR */
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

/* 完整一次性 SM3(NEON 混合优化版) */
void sm3_hash_neon(const uint8_t *data, size_t len, uint8_t digest[32])
{
    uint32_t state[8] = {
        0x7380166F, 0x4914B2B9, 0x172442D7, 0xDA8A0600,
        0xA96F30BC, 0x163138AA, 0xE38DEE4D, 0xB0FB0E4E};
    uint8_t block[64];
    uint32_t W[68];
    size_t full = len / 64, rem = len % 64, i;
    int k;

    for (i = 0; i < full; i++)
    {
        expand_neon(data + i * 64, W);
        compress_gpr(state, W);
    }
    memset(block, 0, sizeof(block));
    memcpy(block, data + full * 64, rem);
    block[rem] = 0x80;
    if (rem > 55)
    {
        expand_neon(block, W);
        compress_gpr(state, W);
        memset(block, 0, 56);
    }
    {
        uint64_t bit_len = (uint64_t)len * 8;
        for (k = 0; k < 8; k++)
            block[56 + k] = (uint8_t)(bit_len >> (56 - 8 * k));
    }
    expand_neon(block, W);
    compress_gpr(state, W);

    for (k = 0; k < 8; k++)
    {
        digest[k * 4] = (uint8_t)(state[k] >> 24);
        digest[k * 4 + 1] = (uint8_t)(state[k] >> 16);
        digest[k * 4 + 2] = (uint8_t)(state[k] >> 8);
        digest[k * 4 + 3] = (uint8_t)(state[k]);
    }
}

#else /* !__aarch64__:占位实现,便于在非 ARM 平台编译通过 */

void sm3_hash_neon(const uint8_t *data, size_t len, uint8_t digest[32])
{
    /* 非 ARM 平台退化为参考实现 */
    sm3_hash(data, len, digest);
}

#endif /* __aarch64__ */
