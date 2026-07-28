#ifndef SM4_H
#define SM4_H

#include <stdint.h>


#define SM4_BLOCK_SIZE 16
#define SM4_ROUNDS 32


void sm4_setkey(
    uint32_t rk[32],
    const uint8_t key[16]
);


void sm4_encrypt(
    const uint8_t input[16],
    uint8_t output[16],
    uint32_t rk[32]
);


#endif
