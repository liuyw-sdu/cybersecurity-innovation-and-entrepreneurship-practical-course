#ifndef SM4_SIMD_H
#define SM4_SIMD_H


#include <stdint.h>


void sm4_simd_setkey(
        uint32_t rk[32],
        const uint8_t key[16]
);



void sm4_simd_encrypt2(
        const uint8_t input[32],
        uint8_t output[32],
        uint32_t rk[32]
);


#endif
