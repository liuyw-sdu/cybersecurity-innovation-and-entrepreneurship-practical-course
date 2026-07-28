#include <stdint.h>


extern const uint8_t SM4_SBOX[256];


uint32_t T0[256];
uint32_t T1[256];
uint32_t T2[256];
uint32_t T3[256];



static uint32_t rotl(
        uint32_t x,
        int n)
{
    return (x<<n)|(x>>(32-n));
}



static uint32_t L(uint32_t b)
{

return b ^
rotl(b,2)^
rotl(b,10)^
rotl(b,18)^
rotl(b,24);

}



void sm4_init_ttable()
{

for(int i=0;i<256;i++)
{

uint32_t s=SM4_SBOX[i];


uint32_t b0=s<<24;
uint32_t b1=s<<16;
uint32_t b2=s<<8;
uint32_t b3=s;


T0[i]=L(b0);
T1[i]=L(b1);
T2[i]=L(b2);
T3[i]=L(b3);


}


}
