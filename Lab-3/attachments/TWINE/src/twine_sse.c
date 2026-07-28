#include "twine_sse.h"



static inline void
sse_round(
        __m128i state[4],
        __m128i rk[4])
{


for(int i=0;i<4;i++)
{

    state[i]
    =
    _mm_xor_si128(
        state[i],
        rk[i]);

}



twine_sse_sbox(state);


}



void twine_sse_encrypt(
        __m128i state[4],
        __m128i out[4],
        TWINE_SSE_KEY *ks)

{


for(int r=0;r<36;r++)
{


    sse_round(
        state,
        ks->rk[r]);

}



for(int i=0;i<4;i++)
{

    out[i]=state[i];

}


}
