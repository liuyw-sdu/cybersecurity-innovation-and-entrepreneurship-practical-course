#ifndef GIFT_BITSLICE_H
#define GIFT_BITSLICE_H


#include <stdint.h>


#define GIFT_ROUNDS 40


typedef uint8_t u8;
typedef uint64_t u64;



typedef struct
{

    u64 x0;
    u64 x1;
    u64 x2;
    u64 x3;


}bitslice_state;



void gift128_bitslice_encrypt(
        u8 plaintext[16],
        u8 key[16],
        u8 ciphertext[16]
);



#endif
