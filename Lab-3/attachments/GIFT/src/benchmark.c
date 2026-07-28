#include "../include/gift.h"
#include "../include/gift_bitslice.h"
#include "../include/gift_avx.h"

#include <stdio.h>
#include <time.h>


#define LOOP 1000000



/*
====================================
Basic版本测速

16 byte/block
====================================
*/

void benchmark_basic()
{

    u8 plaintext[16]={0};

    u8 key[16]={0};

    u8 ciphertext[16];


    clock_t start,end;



    start=clock();


    for(int i=0;i<LOOP;i++)
    {

        gift128_encrypt(
                plaintext,
                key,
                ciphertext
        );

    }


    end=clock();



    double sec=
        (double)(end-start)
        /
        CLOCKS_PER_SEC;



    double speed=
        (16.0*LOOP)
        /
        (1024*1024)
        /
        sec;



    printf("\nBasic GIFT\n");

    printf(
        "Time : %.6f s\n",
        sec
    );


    printf(
        "Speed: %.2f MB/s\n",
        speed
    );


}







/*
====================================
Bitslice版本测速

16 byte/block

====================================
*/


void benchmark_bitslice()
{

    u8 plaintext[16]={0};

    u8 key[16]={0};

    u8 ciphertext[16];


    clock_t start,end;



    start=clock();


    for(int i=0;i<LOOP;i++)
    {


        gift128_bitslice_encrypt(
                plaintext,
                key,
                ciphertext
        );


    }


    end=clock();



    double sec=
        (double)(end-start)
        /
        CLOCKS_PER_SEC;



    double speed=
        (16.0*LOOP)
        /
        (1024*1024)
        /
        sec;



    printf("\nBitslice GIFT\n");


    printf(
        "Time : %.6f s\n",
        sec
    );


    printf(
        "Speed: %.2f MB/s\n",
        speed
    );


}








/*
====================================
AVX2版本测速

32 byte/block

两个GIFT block并行

====================================
*/


void benchmark_avx()
{


    u8 plaintext[32]={0};

    u8 key[32]={0};

    u8 ciphertext[32];



    clock_t start,end;



    start=clock();



    for(int i=0;i<LOOP;i++)
    {


        gift128_avx_encrypt(
                plaintext,
                key,
                ciphertext
        );


    }



    end=clock();



    double sec=
        (double)(end-start)
        /
        CLOCKS_PER_SEC;



    double speed=
        (32.0*LOOP)
        /
        (1024*1024)
        /
        sec;



    printf("\nAVX2 Shuffle GIFT\n");


    printf(
        "Time : %.6f s\n",
        sec
    );


    printf(
        "Speed: %.2f MB/s\n",
        speed
    );


}






int main()
{


    printf(
    "============================\n"
    " GIFT Optimization Benchmark\n"
    "============================\n"
    );



    benchmark_basic();



    benchmark_bitslice();



    benchmark_avx();



    return 0;

}
