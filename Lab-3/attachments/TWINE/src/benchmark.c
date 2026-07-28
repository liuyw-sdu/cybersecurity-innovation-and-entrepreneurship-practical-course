#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>


#include "twine.h"
#include "twine_bitslice.h"
#include "twine_sse.h"



#define TEST_SIZE (1024*1024)


#define BLOCK_SIZE 8



uint8_t *plaintext;

uint8_t *ciphertext;



/*
------------------------------------------------

Basic TWINE

------------------------------------------------
*/


double benchmark_basic()
{


uint8_t key[10]=
{
0x00,0x11,0x22,0x33,0x44,
0x55,0x66,0x77,0x88,0x99
};



TWINE_KEY ks;



twine_key_schedule(
key,
&ks);



clock_t start,end;



start=clock();



for(size_t i=0;
    i<TEST_SIZE;
    i+=BLOCK_SIZE)
{


twine_encrypt(
        &plaintext[i],
        &ciphertext[i],
        &ks);

}



end=clock();



return 
(double)(end-start)/CLOCKS_PER_SEC;


}





/*
------------------------------------------------

Bitslice

------------------------------------------------
*/


double benchmark_bitslice()
{


uint8_t key[10]=
{
1,2,3,4,5,
6,7,8,9,10
};



TWINE_BS_KEY ks;


twine_bs_key_schedule(
key,
&ks);



uint64_t state[4];

uint64_t out[4];



clock_t start,end;



start=clock();



for(size_t i=0;
i<TEST_SIZE;
i+=64)
{


state[0]=
0xffffffffffffffff;


state[1]=
0xaaaaaaaaaaaaaaaa;


state[2]=
0x5555555555555555;


state[3]=
0;



twine_bs_encrypt(
state,
out,
&ks);


}



end=clock();



return
(double)(end-start)/CLOCKS_PER_SEC;


}







/*
------------------------------------------------

SSE

------------------------------------------------
*/


double benchmark_sse()
{


uint8_t key[10]=
{
1,2,3,4,5,
6,7,8,9,10
};



TWINE_SSE_KEY ks;



twine_sse_key_schedule(
key,
&ks);



__m128i input[4];

__m128i output[4];



clock_t start,end;



start=clock();



for(size_t i=0;
i<TEST_SIZE;
i+=16)
{


for(int j=0;j<4;j++)
{

input[j]
=
_mm_set1_epi32(i+j);

}



twine_sse_encrypt(
input,
output,
&ks);


}



end=clock();



return
(double)(end-start)/CLOCKS_PER_SEC;


}


int main()
{


printf("START\n");
fflush(stdout);


printf("==============================\n");
printf(" TWINE Optimization Benchmark\n");
printf("==============================\n");


printf("Data size : %d MB\n",
TEST_SIZE/1024/1024);


plaintext =
malloc(TEST_SIZE);


ciphertext =
malloc(TEST_SIZE);


if(plaintext==NULL || ciphertext==NULL)
{
    printf("malloc failed\n");
    return -1;
}



memset(
plaintext,
0x11,
TEST_SIZE);


/*
 Basic
*/
printf("Running Basic...\n");
fflush(stdout);



double t1=
benchmark_basic();


double speed1=
(TEST_SIZE/1024.0/1024.0)
/
t1;


/*
 Bitslice
*/
printf("Running Bitslice...\n");
fflush(stdout);

double t2=
benchmark_bitslice();


double speed2=
(TEST_SIZE/1024.0/1024.0)
/
t2;



/*
 SSE
*/

printf("Running SSE...\n");
fflush(stdout);

double t3=
benchmark_sse();


double speed3=
(TEST_SIZE/1024.0/1024.0)
/
t3;





printf("\n");


printf("---------------------------------\n");


printf(
"TWINE Basic C\n");

printf(
" Time : %.6f s\n",
t1);


printf(
" Speed: %.2f MB/s\n",
speed1);




printf("\n");


printf(
"TWINE Bitslice\n");


printf(
" Time : %.6f s\n",
t2);


printf(
" Speed: %.2f MB/s\n",
speed2);



printf("\n");


printf(
"TWINE SSE\n");


printf(
" Time : %.6f s\n",
t3);


printf(
" Speed: %.2f MB/s\n",
speed3);





printf("\n");

printf("---------------------------------\n");



printf(
"Bitslice Speedup : %.2fx\n",
speed2/speed1);



printf(
"SSE Speedup      : %.2fx\n",
speed3/speed1);



printf("---------------------------------\n");

free(plaintext);
free(ciphertext);


return 0;

}
