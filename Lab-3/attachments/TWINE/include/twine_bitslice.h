#ifndef TWINE_BITSLICE_H
#define TWINE_BITSLICE_H


#include <stdint.h>


#define BS_ROUNDS 36



/*
 Bitslice状态

 bs[0] bit plane 0
 bs[1] bit plane 1
 bs[2] bit plane 2
 bs[3] bit plane 3

*/


typedef struct
{

uint64_t rk[36][4];


}TWINE_BS_KEY;



void twine_bs_key_schedule(
        uint8_t *key,
        TWINE_BS_KEY *ks);



void twine_bs_encrypt(
        uint64_t state[4],
        uint64_t out[4],
        TWINE_BS_KEY *ks);



void twine_bs_sbox(
        uint64_t x[4]);



#endif
