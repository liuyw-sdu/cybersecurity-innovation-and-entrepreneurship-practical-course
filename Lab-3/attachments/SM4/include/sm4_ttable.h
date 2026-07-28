#ifndef SM4_TTABLE_H
#define SM4_TTABLE_H


#include <stdint.h>


void sm4_ttable_setkey(
    uint32_t rk[32],
    const uint8_t key[16]
);



void sm4_ttable_encrypt(
    const uint8_t input[16],
    uint8_t output[16],
    uint32_t rk[32]
);


#endif
