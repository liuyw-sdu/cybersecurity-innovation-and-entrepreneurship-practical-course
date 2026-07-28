#include "../include/gift_avx.h"

#include <immintrin.h>
#include <string.h>



/*
================================================

AVX2 GIFT SBOX

使用 VPSHUFB

一次处理32 byte


================================================
*/


static __m256i gift_avx_sbox(
        __m256i input
)
{


    /*
       GIFT SBOX table

       0 1 2 3 ...
    */


    const __m256i table =
    _mm256_set_epi8(

        0xe,0x8,0x0,0x5,
        0x7,0xb,0xd,0x2,
        0x9,0x3,0xf,0x6,
        0xc,0x4,0xa,0x1,


        0xe,0x8,0x0,0x5,
        0x7,0xb,0xd,0x2,
        0x9,0x3,0xf,0x6,
        0xc,0x4,0xa,0x1
    );


    /*
        对应汇编:

        VPSHUFB

    */


    return
    _mm256_shuffle_epi8(
            table,
            input
    );


}




/*
================================================

AVX permutation

使用：

VPERMQ


================================================
*/


static __m256i gift_avx_perm(
        __m256i state
)
{


    return
    _mm256_permute4x64_epi64(
            state,
            0x93
    );

}




/*
================================================

AVX GIFT encrypt

================================================
*/


void gift128_avx_encrypt(
        u8 plaintext[32],
        u8 key[32],
        u8 ciphertext[32]
)
{


    __m256i state;


    /*
        加载128bit数据

    */


    state =
    _mm256_loadu_si256(
        (__m256i*)plaintext
    );




    for(int r=0;r<40;r++)
    {


        /*
            SBOX

            VPSHUFB

        */

        state=
        gift_avx_sbox(state);



        /*
            permutation

            VPERMQ

        */

        state=
        gift_avx_perm(state);



        /*
            AddKey

        */

        state =
        _mm256_xor_si256(
                state,
                _mm256_loadu_si256(
                (__m256i*)key)
        );


    }



    _mm256_storeu_si256(
        (__m256i*)ciphertext,
        state
    );


}
