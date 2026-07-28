#include "../include/sm4_ttable.h"

#include <stdint.h>


extern uint32_t T0[256];
extern uint32_t T1[256];
extern uint32_t T2[256];
extern uint32_t T3[256];


extern void sm4_init_ttable();



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



static uint32_t rotl(
uint32_t x,
int n
)
{
return (x<<n)|(x>>(32-n));
}



static uint32_t tau_key(uint32_t a)
{

extern const uint8_t SM4_SBOX[256];


return
((uint32_t)SM4_SBOX[a>>24]<<24)|
((uint32_t)SM4_SBOX[(a>>16)&0xff]<<16)|
((uint32_t)SM4_SBOX[(a>>8)&0xff]<<8)|
SM4_SBOX[a&0xff];

}



static uint32_t L_key(uint32_t b)
{

return b^
rotl(b,13)^
rotl(b,23);

}



static uint32_t Tkey(uint32_t x)
{

return L_key(tau_key(x));

}



void sm4_ttable_setkey(
uint32_t rk[32],
const uint8_t key[16]
)
{

uint32_t MK[4];

uint32_t K[36];


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



for(int i=0;i<4;i++)
{
MK[i]=GET32(key+i*4);
K[i]=MK[i]^FK[i];
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



static uint32_t Ttable(
uint32_t x
)
{

return
T0[x>>24]^
T1[(x>>16)&0xff]^
T2[(x>>8)&0xff]^
T3[x&0xff];

}



void sm4_ttable_encrypt(
const uint8_t input[16],
uint8_t output[16],
uint32_t rk[32]
)
{


static int init=0;


if(!init)
{
sm4_init_ttable();
init=1;
}



uint32_t X[36];



for(int i=0;i<4;i++)
{
X[i]=GET32(input+i*4);
}



for(int i=0;i<32;i++)
{

X[i+4]=
X[i]^
Ttable(
X[i+1]^
X[i+2]^
X[i+3]^
rk[i]
);

}



for(int i=0;i<4;i++)
{
PUT32(
output+i*4,
X[35-i]
);
}


}
