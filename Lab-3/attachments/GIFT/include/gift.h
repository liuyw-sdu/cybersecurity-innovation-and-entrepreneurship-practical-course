#ifndef GIFT_H
#define GIFT_H


#include <stdint.h>


#define GIFT_ROUNDS 40


typedef uint8_t  u8;
typedef uint32_t u32;
typedef uint64_t u64;



void gift128_encrypt(
        u8 plaintext[16],
        u8 key[16],
        u8 ciphertext[16]
);



#endif
