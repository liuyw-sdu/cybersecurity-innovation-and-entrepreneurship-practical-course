#ifndef AES_H
#define AES_H

#include <stdint.h>


#define AES_BLOCK_SIZE 16
#define AES_KEY_SIZE 16
#define AES_ROUND_KEY_SIZE 176


/*
 AES-128上下文结构

 RoundKey:
 11轮密钥 × 16字节
 =176 bytes

*/
typedef struct
{

    uint8_t RoundKey[AES_ROUND_KEY_SIZE];

} AES_ctx;



/*
 AES-128密钥初始化

 输入:
 ctx  AES上下文
 key  16字节密钥

*/
void AES_init(
        AES_ctx *ctx,
        const uint8_t *key);



/*
 AES-128加密

 输入:
 ctx:
     已初始化密钥

 input:
     16字节明文

 输出:
     input被替换为16字节密文

*/
void AES_encrypt(
        AES_ctx *ctx,
        uint8_t *input);



/*
 AES-128解密

 (后续实现)

*/
void AES_decrypt(
        AES_ctx *ctx,
        uint8_t *input);



#endif
