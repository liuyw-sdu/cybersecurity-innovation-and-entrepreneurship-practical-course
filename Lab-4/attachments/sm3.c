/*
 * sm3.c — SM3 杂凑算法基础实现(纯通用寄存器,作为正确性基准)
 * 遵循 GB/T 32905-2016 标准。
 */
#include "sm3.h"
#include <stdio.h>
#include <string.h>

/* 循环左移 */
static inline uint32_t rotl(uint32_t x, int n)
{
    return (x << n) | (x >> (32 - n));
}

/* 置换函数 */
#define P0(x) ((x) ^ rotl((x), 9) ^ rotl((x), 17))
#define P1(x) ((x) ^ rotl((x), 15) ^ rotl((x), 23))

/* 布尔函数(用宏以便内联) */
#define FF0(x, y, z) ((x) ^ (y) ^ (z))
#define FF1(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define GG0(x, y, z) ((x) ^ (y) ^ (z))
#define GG1(x, y, z) (((x) & (y)) | (~(x) & (z)))

/* 常量 T_j */
static uint32_t T(int j)
{
    return (j < 16) ? 0x79CC4519u : 0x7A879D8Au;
}

/* 消息扩展:64 字节分组 -> W[0..67] */
void sm3_expand(const uint8_t block[64], uint32_t W[68])
{
    int j;
    for (j = 0; j < 16; j++)
        W[j] = ((uint32_t)block[j * 4] << 24) |
               ((uint32_t)block[j * 4 + 1] << 16) |
               ((uint32_t)block[j * 4 + 2] << 8) |
               ((uint32_t)block[j * 4 + 3]);
    for (j = 16; j < 68; j++)
        W[j] = P1(W[j - 16] ^ W[j - 9] ^ rotl(W[j - 3], 15)) ^ rotl(W[j - 13], 7) ^ W[j - 6];
}

/* 压缩函数:对 state 就地更新 */
void sm3_compress(uint32_t state[8], const uint32_t W[68])
{
    uint32_t A = state[0], B = state[1], C = state[2], D = state[3];
    uint32_t E = state[4], F = state[5], G = state[6], H = state[7];
    uint32_t SS1, SS2, TT1, TT2;
    int j;

    for (j = 0; j < 64; j++)
    {
        uint32_t A12 = rotl(A, 12);
        SS1 = rotl(A12 + E + rotl(T(j), j % 32), 7);
        SS2 = SS1 ^ A12;
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
        C = rotl(B, 9);
        B = A;
        A = TT1;
        H = G;
        G = rotl(F, 19);
        F = E;
        E = P0(TT2);
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

/* 处理单个 64 字节分组 */
static void sm3_process_block(sm3_ctx *ctx, const uint8_t block[64])
{
    uint32_t W[68];
    sm3_expand(block, W);
    sm3_compress(ctx->state, W);
}

void sm3_init(sm3_ctx *ctx)
{
    /* 标准 IV */
    static const uint32_t IV[8] = {
        0x7380166F, 0x4914B2B9, 0x172442D7, 0xDA8A0600,
        0xA96F30BC, 0x163138AA, 0xE38DEE4D, 0xB0FB0E4E};
    memcpy(ctx->state, IV, sizeof(IV));
    ctx->total_len = 0;
    ctx->block_len = 0;
}

void sm3_update(sm3_ctx *ctx, const uint8_t *data, size_t len)
{
    ctx->total_len += len;

    /* 若缓存中有残留,先拼满一个分组 */
    if (ctx->block_len)
    {
        size_t need = 64 - ctx->block_len;
        if (len < need)
        {
            memcpy(ctx->block + ctx->block_len, data, len);
            ctx->block_len += len;
            return;
        }
        memcpy(ctx->block + ctx->block_len, data, need);
        sm3_process_block(ctx, ctx->block);
        data += need;
        len -= need;
        ctx->block_len = 0;
    }
    /* 直接处理完整分组 */
    while (len >= 64)
    {
        sm3_process_block(ctx, data);
        data += 64;
        len -= 64;
    }
    /* 剩余缓存 */
    if (len)
    {
        memcpy(ctx->block, data, len);
        ctx->block_len = len;
    }
}

void sm3_final(sm3_ctx *ctx, uint8_t digest[32])
{
    uint64_t bit_len = ctx->total_len * 8;
    int i;

    /* 填充:1000... 直至距分组尾 8 字节处,再写入 64 位大端长度 */
    ctx->block[ctx->block_len++] = 0x80;
    if (ctx->block_len > 56)
    {
        memset(ctx->block + ctx->block_len, 0, 64 - ctx->block_len);
        sm3_process_block(ctx, ctx->block);
        ctx->block_len = 0;
    }
    memset(ctx->block + ctx->block_len, 0, 56 - ctx->block_len);
    for (i = 0; i < 8; i++)
        ctx->block[56 + i] = (uint8_t)(bit_len >> (56 - 8 * i));
    sm3_process_block(ctx, ctx->block);

    for (i = 0; i < 8; i++)
    {
        digest[i * 4] = (uint8_t)(ctx->state[i] >> 24);
        digest[i * 4 + 1] = (uint8_t)(ctx->state[i] >> 16);
        digest[i * 4 + 2] = (uint8_t)(ctx->state[i] >> 8);
        digest[i * 4 + 3] = (uint8_t)(ctx->state[i]);
    }
}

void sm3_hash(const uint8_t *data, size_t len, uint8_t digest[32])
{
    sm3_ctx ctx;
    sm3_init(&ctx);
    sm3_update(&ctx, data, len);
    sm3_final(&ctx, digest);
}

void sm3_print_hex(const uint8_t digest[32])
{
    int i;
    for (i = 0; i < 32; i++)
        printf("%02x", digest[i]);
    printf("\n");
}
