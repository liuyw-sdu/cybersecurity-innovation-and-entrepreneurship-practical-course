#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <wmmintrin.h>
#include <emmintrin.h>


#define AES128_ROUNDS 10


static __m128i AES128_KeyAssist(
        __m128i key,
        int rcon)
{

    __m128i temp;


    temp =
        _mm_aeskeygenassist_si128(
                key,
                rcon);


    temp =
        _mm_shuffle_epi32(
                temp,
                0xff);



    key =
        _mm_xor_si128(
                key,
                _mm_slli_si128(key,4));


    key =
        _mm_xor_si128(
                key,
                _mm_slli_si128(key,4));


    key =
        _mm_xor_si128(
                key,
                _mm_slli_si128(key,4));


    key =
        _mm_xor_si128(
                key,
                temp);


    return key;
}



void AESNI_KeyExpansion(
        uint8_t *roundKey,
        const uint8_t *key)
{




    __m128i *rk =
        (__m128i*)roundKey;



    rk[0]=
        _mm_loadu_si128(
            (const __m128i*)key);


    rk[1]=
        AES128_KeyAssist(
                rk[0],
                0x01);



    rk[2]=
        AES128_KeyAssist(
                rk[1],
                0x02);



    rk[3]=
        AES128_KeyAssist(
                rk[2],
                0x04);



    rk[4]=
        AES128_KeyAssist(
                rk[3],
                0x08);



    rk[5]=
        AES128_KeyAssist(
                rk[4],
                0x10);



    rk[6]=
        AES128_KeyAssist(
                rk[5],
                0x20);



    rk[7]=
        AES128_KeyAssist(
                rk[6],
                0x40);



    rk[8]=
        AES128_KeyAssist(
                rk[7],
                0x80);



    rk[9]=
        AES128_KeyAssist(
                rk[8],
                0x1b);



    rk[10]=
        AES128_KeyAssist(
                rk[9],
                0x36);


}


void AES128_AESNI_encrypt(
        uint8_t *input,
        uint8_t *roundKey)
{


    __m128i state;


    __m128i *rk =
        (__m128i*)roundKey;



    state =
        _mm_loadu_si128(
            (__m128i*)input);


    state =
        _mm_xor_si128(
                state,
                rk[0]);




    for(int i=1;i<10;i++)
    {


        state =
            _mm_aesenc_si128(
                    state,
                    rk[i]);

    }




    state =
        _mm_aesenclast_si128(
                state,
                rk[10]);



    _mm_storeu_si128(
        (__m128i*)input,
        state);


}


int AESNI_check()
{

#if defined(__AES__)

    return 1;

#else

    return 0;

#endif

}



