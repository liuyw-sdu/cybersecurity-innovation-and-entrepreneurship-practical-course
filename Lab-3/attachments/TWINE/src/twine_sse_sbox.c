#include "twine_sse.h"



/*
 SSE版本SBOX

 使用SIMD逻辑门

*/


void twine_sse_sbox(
        __m128i state[4])
{


__m128i a,b,c,d;


a=
_mm_srli_epi16(
state[0],0);


b=
_mm_srli_epi16(
state[1],0);



c=
_mm_srli_epi16(
state[2],0);


d=
_mm_srli_epi16(
state[3],0);



__m128i t0,t1,t2;



t0=
_mm_xor_si128(
a,b);



t1=
_mm_and_si128(
c,d);



t2=
_mm_xor_si128(
t0,t1);



state[0]
=
_mm_xor_si128(
t2,
_mm_and_si128(a,c)
);



state[1]
=
_mm_xor_si128(
b,t1);



state[2]
=
_mm_xor_si128(
c,t0);



state[3]
=
_mm_xor_si128(
d,t2);



}
