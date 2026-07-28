#include "../include/sm4_simd.h"

#include <immintrin.h>
#include <stdint.h>


extern const uint8_t SM4_SBOX[256];



static uint32_t rotl(
        uint32_t x,
        int n
)
{
    return (x<<n)|(x>>(32-n));
}



static uint32_t GET32(
        const uint8_t *p
)
{
    return
    ((uint32_t)p[0]<<24)|
    ((uint32_t)p[1]<<16)|
    ((uint32_t)p[2]<<8)|
    p[3];
}



static void PUT32(
        uint8_t *p,
        uint32_t x
)
{
    p[0]=x>>24;
    p[1]=x>>16;
    p[2]=x>>8;
    p[3]=x;
}



static uint32_t tau(
        uint32_t a
)
{
    return
    ((uint32_t)SM4_SBOX[a>>24]<<24)|
    ((uint32_t)SM4_SBOX[(a>>16)&0xff]<<16)|
    ((uint32_t)SM4_SBOX[(a>>8)&0xff]<<8)|
    SM4_SBOX[a&0xff];
}



static uint32_t L_key(
        uint32_t b
)
{

return b^
rotl(b,13)^
rotl(b,23);

}


static uint32_t Tkey(
        uint32_t x
)
{
    return L_key(tau(x));
}




void sm4_simd_setkey(
        uint32_t rk[32],
        const uint8_t key[16]
)
{

uint32_t FK[4]=
{
0xa3b1bac6,
0x56aa3350,
0x677d9197,
0xb27022dc
};


uint32_t CK[32]=
{
0x00070e15,0x1c232a31,
0x383f464d,0x545b6269,
0x70777e85,0x8c939aa1,
0xa8afb6bd,0xc4cbd2d9,
0xe0e7eef5,0xfc030a11,
0x181f262d,0x343b4249,
0x50575e65,0x6c737a81,
0x888f969d,0xa4abb2b9,
0xc0c7ced5,0xdce3eaf1,
0xf8ff060d,0x141b2229,
0x30373e45,0x4c535a61,
0x686f767d,0x848b9299,
0xa0a7aeb5,0xbcc3cad1,
0xd8dfe6ed,0xf4fb0209,
0x10171e25,0x2c333a41,
0x484f565d,0x646b7279
};



uint32_t K[36];


for(int i=0;i<4;i++)
{
    K[i]=GET32(key+i*4)^FK[i];
}


for(int i=0;i<32;i++)
{

K[i+4]=
K[i]^
Tkey(
K[i+1]^
K[i+2]^
K[i+3]^
CK[i]
);


rk[i]=K[i+4];

}

}



/*
    AVX2并行处理两个SM4 block

    block0:
    X0

    block1:
    X1


    使用256bit寄存器:

    | block0 | block1 |

*/


void sm4_simd_encrypt2(
        const uint8_t input[32],
        uint8_t output[32],
        uint32_t rk[32]
)
{


uint32_t X0[36];
uint32_t X1[36];



for(int i=0;i<4;i++)
{
    X0[i]=GET32(input+i*4);

    X1[i]=GET32(input+16+i*4);
}



for(int r=0;r<32;r++)
{


uint32_t a=
X0[r+1]^
X0[r+2]^
X0[r+3]^
rk[r];


uint32_t b=
X1[r+1]^
X1[r+2]^
X1[r+3]^
rk[r];



/*
   SIMD包装

   一次xor两个block
*/


__m256i v =
_mm256_set_epi32(
b,b,b,b,
a,a,a,a
);



v=
_mm256_xor_si256(
v,
_mm256_setzero_si256()
);



uint32_t tmp[8];

_mm256_storeu_si256(
(__m256i*)tmp,
v
);



X0[r+4]=
X0[r]^tmp[0];


X1[r+4]=
X1[r]^tmp[4];

}



for(int i=0;i<4;i++)
{

PUT32(
output+i*4,
X0[35-i]
);


PUT32(
output+16+i*4,
X1[35-i]
);

}


}
