#ifndef TWINE_SSE_H
#define TWINE_SSE_H


#include <stdint.h>
#include <emmintrin.h>


#define SSE_ROUNDS 36



typedef struct
{

    __m128i rk[36][4];


}TWINE_SSE_KEY;



void twine_sse_key_schedule(
        uint8_t *key,
        TWINE_SSE_KEY *ks);



void twine_sse_encrypt(
        __m128i state[4],
        __m128i out[4],
        TWINE_SSE_KEY *ks);



void twine_sse_sbox(
        __m128i state[4]);


#endif
