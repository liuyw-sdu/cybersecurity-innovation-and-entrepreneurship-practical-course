#include "../include/gift.h"

#include <string.h>
#include <stdio.h>



/*
 * GIFT-128 Sbox
 *
 * 4-bit substitution
 */

static const u8 GIFT_SBOX[16]=
{

0x1,
0xa,
0x4,
0xc,

0x6,
0xf,
0x3,
0x9,

0x2,
0xd,
0xb,
0x7,

0x5,
0x0,
0x8,
0xe

};




/*
 * Round constants
 */

static const u8 ROUND_CONSTANT[40]=
{

0x01,
0x03,
0x07,
0x0f,
0x1f,
0x3e,
0x3d,
0x3b,

0x37,
0x2f,
0x1e,
0x3c,
0x39,
0x33,
0x27,
0x0e,

0x1d,
0x3a,
0x35,
0x2b,
0x16,
0x2c,
0x18,
0x30,

0x21,
0x02,
0x05,
0x0b,
0x17,
0x2e,
0x1c,
0x38,

0x31,
0x23,
0x06,
0x0d,
0x1b,
0x36,
0x2d,
0x1a

};



/*
 * SubCells
 *
 * 普通查表实现
 *
 */

static void SubCells(
        u8 state[16]
)
{

    for(int i=0;i<16;i++)
    {


        u8 high =
            state[i] >> 4;


        u8 low =
            state[i] & 0x0f;



        state[i]
            =
            (GIFT_SBOX[high]<<4)
            |
            GIFT_SBOX[low];


    }

}




/*
 * PermBits
 *
 * bit permutation
 *
 * P(i)=4*i mod 127
 *
 */


static void PermBits(
        u8 state[16]
)
{

    u8 temp[16]={0};


    for(int i=0;i<128;i++)
    {


        int src_byte=
            i/8;


        int src_bit=
            i%8;



        int bit=
            (state[src_byte]>>src_bit)&1;



        int dst;


        if(i==127)
        {
            dst=127;
        }
        else
        {
            dst=(4*i)%127;
        }



        temp[dst/8]
            |=
            bit<<(dst%8);


    }



    memcpy(
        state,
        temp,
        16
    );

}





/*
 * AddRoundKey
 */

static void AddRoundKey(
        u8 state[16],
        u8 key[16]
)
{

    for(int i=0;i<16;i++)
    {

        state[i]^=key[i];

    }

}





/*
 * 简化版Key Schedule
 *
 * 后续优化版本替换
 *
 */


static void KeySchedule(
        u8 key[16]
)
{

    u8 temp;


    temp=key[0];


    for(int i=0;i<15;i++)
    {

        key[i]=key[i+1];

    }


    key[15]=temp;


}





/*
 * GIFT 一轮
 */

static void GiftRound(
        u8 state[16],
        u8 key[16],
        int round
)
{


    /*
     * 1. SubCells
     */

    SubCells(state);



    /*
     * 2. Permutation
     */

    PermBits(state);



    /*
     * 3. AddKey
     */

    AddRoundKey(
            state,
            key
    );



    /*
     * 4. Round Constant
     */

    state[0]^=
        ROUND_CONSTANT[round];


}





/*
 * GIFT-128 encryption
 *
 * 40 rounds
 */


void gift128_encrypt(
        u8 plaintext[16],
        u8 key[16],
        u8 ciphertext[16]
)
{

    u8 state[16];


    u8 round_key[16];



    memcpy(
            state,
            plaintext,
            16
    );



    memcpy(
            round_key,
            key,
            16
    );




    for(int r=0;r<GIFT_ROUNDS;r++)
    {


        GiftRound(
                state,
                round_key,
                r
        );



        KeySchedule(
                round_key
        );


    }



    memcpy(
            ciphertext,
            state,
            16
    );


}
