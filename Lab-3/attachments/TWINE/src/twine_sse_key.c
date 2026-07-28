#include "twine_sse.h"


void twine_sse_key_schedule(
        uint8_t *key,
        TWINE_SSE_KEY *ks)

{


for(int r=0;r<36;r++)
{


for(int i=0;i<4;i++)
{

ks->rk[r][i]
=
_mm_set1_epi32(
key[i]);

}


}


}

