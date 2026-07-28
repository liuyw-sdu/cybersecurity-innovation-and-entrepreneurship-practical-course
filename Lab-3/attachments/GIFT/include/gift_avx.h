#ifndef GIFT_AVX_H
#define GIFT_AVX_H


#include <stdint.h>


typedef uint8_t u8;



void gift128_avx_encrypt(
        u8 plaintext[32],
        u8 key[32],
        u8 ciphertext[32]
);



#endif
