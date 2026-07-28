#include "../include/gift_bitslice.h"

#include <string.h>



/*
================================================

GIFT Bitslice S-box

输入:

x0 x1 x2 x3

输出:

y0 y1 y2 y3


全部使用逻辑门

无查表

================================================
*/


static void gift_bitslice_sbox(
        u64 *x0,
        u64 *x1,
        u64 *x2,
        u64 *x3
)
{

    u64 y0;
    u64 y1;
    u64 y2;
    u64 y3;



    y0 =
        (*x0 & *x1)
        ^
        *x2
        ^
        *x3;



    y1 =
        *x0
        ^
        (*x1 & *x3);



    y2 =
        *x1
        ^
        (*x2 & *x3);



    y3 =
        (*x0 & *x2)
        ^
        *x3;



    *x0=y0;
    *x1=y1;
    *x2=y2;
    *x3=y3;

}




/*
================================================

普通数据

16 byte

转换为

bit planes


================================================
*/


static void transpose(
        u8 input[16],
        bitslice_state *state
)
{

    state->x0=0;
    state->x1=0;
    state->x2=0;
    state->x3=0;



    for(int i=0;i<16;i++)
    {


        state->x0 |=
        ((input[i]&1ULL)<<i);



        state->x1 |=
        (((input[i]>>1)&1ULL)<<i);



        state->x2 |=
        (((input[i]>>2)&1ULL)<<i);



        state->x3 |=
        (((input[i]>>3)&1ULL)<<i);



    }

}





/*
================================================

恢复普通byte

================================================
*/


static void inverse_transpose(
        bitslice_state *state,
        u8 output[16]
)
{


    for(int i=0;i<16;i++)
    {

        output[i]=0;


        output[i]
        |=
        ((state->x0>>i)&1);



        output[i]
        |=
        (((state->x1>>i)&1)<<1);



        output[i]
        |=
        (((state->x2>>i)&1)<<2);



        output[i]
        |=
        (((state->x3>>i)&1)<<3);


    }


}




/*
================================================

bitslice permutation

================================================
*/


static void bitslice_permutation(
        bitslice_state *state
)
{


    state->x0 =
        (state->x0<<1)
        |
        (state->x0>>63);



    state->x1 =
        (state->x1<<1)
        |
        (state->x1>>63);



    state->x2 =
        (state->x2<<1)
        |
        (state->x2>>63);



    state->x3 =
        (state->x3<<1)
        |
        (state->x3>>63);


}




/*
================================================

单轮

================================================
*/


static void bitslice_round(
        bitslice_state *state
)
{

    gift_bitslice_sbox(
            &state->x0,
            &state->x1,
            &state->x2,
            &state->x3
    );


    bitslice_permutation(state);

}






/*
================================================

GIFT bitslice encryption

================================================
*/


void gift128_bitslice_encrypt(
        u8 plaintext[16],
        u8 key[16],
        u8 ciphertext[16]
)
{


    bitslice_state state;



    /*
        byte -> bitslice
    */

    transpose(
            plaintext,
            &state
    );




    /*
        40轮
    */

    for(int r=0;r<GIFT_ROUNDS;r++)
    {

        bitslice_round(
                &state
        );

    }



    /*
        bitslice -> byte
    */

    inverse_transpose(
            &state,
            ciphertext
    );


}

