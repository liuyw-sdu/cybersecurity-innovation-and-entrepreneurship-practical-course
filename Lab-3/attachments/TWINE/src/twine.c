#include "twine.h"
#include <stdio.h>


/*
 permutation
*/

static uint8_t P[16]=
{

0,9,2,11,
4,13,6,15,
8,1,10,3,
12,5,14,7

};





static void twine_round(
        uint8_t state[16],
        uint8_t rk[8])
{


    uint8_t tmp;



    for(int i=0;i<8;i++)
    {


        tmp=
        state[2*i]
        ^
        rk[i];


        tmp=
        twine_sbox(tmp);


        state[2*i+1]
        ^=
        tmp;

    }



    uint8_t t[16];


    for(int i=0;i<16;i++)
        t[i]=state[i];



    for(int i=0;i<16;i++)
        state[i]=t[P[i]];


}





static void twine_inv_round(
        uint8_t state[16],
        uint8_t rk[8])
{


    uint8_t t[16];


    for(int i=0;i<16;i++)
        t[P[i]]=state[i];


    for(int i=0;i<16;i++)
        state[i]=t[i];



    for(int i=0;i<8;i++)
    {

        uint8_t x;


        x=
        state[2*i]
        ^
        rk[i];


        x=
        twine_inv_sbox(x);



        state[2*i+1]
        ^=
        x;

    }

}





void twine_encrypt(
        uint8_t *in,
        uint8_t *out,
        TWINE_KEY *ks)
{
static int encrypt_count=0;


encrypt_count++;


if(encrypt_count%10000==0)
{
    printf("encrypt block = %d\n",
            encrypt_count);

    fflush(stdout);
}

uint8_t state[16];



for(int i=0;i<8;i++)
{

state[2*i]
=
in[i]>>4;


state[2*i+1]
=
in[i]&0xf;

}



for(int r=0;r<36;r++)
{

    twine_round(
        state,
        ks->rk[r]);

}




for(int i=0;i<8;i++)
{

out[i]=
(state[2*i]<<4)
|
state[2*i+1];

}



}





void twine_decrypt(
        uint8_t *in,
        uint8_t *out,
        TWINE_KEY *ks)
{


uint8_t state[16];


for(int i=0;i<8;i++)
{

state[2*i]
=
in[i]>>4;


state[2*i+1]
=
in[i]&0xf;

}




for(int r=35;r>=0;r--)
{

twine_inv_round(
        state,
        ks->rk[r]);

}




for(int i=0;i<8;i++)
{

out[i]
=
(state[2*i]<<4)
|
state[2*i+1];

}


}
