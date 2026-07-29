#ifndef SM3_H
#define SM3_H

#include <stdint.h>
#include <stddef.h>

#define SM3_DIGEST_SIZE 32
#define SM3_BLOCK_SIZE 64

typedef struct
{
    uint32_t state[8];  /* 链值 A..H */
    uint64_t total_len; /* 已处理总字节数 */
    uint8_t block[64];  /* 未填满的分组缓存 */
    size_t block_len;   /* 缓存中已有字节数 */
} sm3_ctx;

/* 基础(纯通用寄存器)实现 */
void sm3_init(sm3_ctx *ctx);
void sm3_update(sm3_ctx *ctx, const uint8_t *data, size_t len);
void sm3_final(sm3_ctx *ctx, uint8_t digest[32]);
void sm3_hash(const uint8_t *data, size_t len, uint8_t digest[32]);

/* 压缩函数核心:W 为消息扩展后的 68 个字(由调用方准备) */
void sm3_compress(uint32_t state[8], const uint32_t W[68]);

/* 消息扩展:由 64 字节分组生成 W[0..67] */
void sm3_expand(const uint8_t block[64], uint32_t W[68]);

/* 打印 32 字节摘要 */
void sm3_print_hex(const uint8_t digest[32]);

#endif /* SM3_H */
