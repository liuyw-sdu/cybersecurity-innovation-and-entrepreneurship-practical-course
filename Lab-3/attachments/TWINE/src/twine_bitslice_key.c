#include "twine_bitslice.h"



void twine_bs_key_schedule(
        uint8_t *key,
        TWINE_BS_KEY *ks)

{



/*
 简化处理

 将普通key扩展成bitslice round key

 */


for(int r=0;r<36;r++)
{


for(int i=0;i<4;i++)
{


ks->rk[r][i]
=
((uint64_t)key[i])
<<32
|
key[i+4];


}


}



}
