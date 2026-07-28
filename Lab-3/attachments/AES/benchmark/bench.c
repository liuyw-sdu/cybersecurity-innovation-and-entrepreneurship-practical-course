#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#include "../basic/aes.h"


/*
 测试数据大小

 100MB

*/

#define TEST_SIZE (100*1024*1024)



/*
 AES block

 16 bytes

*/

#define BLOCK_SIZE 16



uint8_t buffer[TEST_SIZE];



uint8_t key[16]=
{
0x2b,0x7e,0x15,0x16,
0x28,0xae,0xd2,0xa6,
0xab,0xf7,0x15,0x88,
0x09,0xcf,0x4f,0x3c
};





/*
 时间计算

*/

double get_time()
{

struct timespec ts;


clock_gettime(
CLOCK_MONOTONIC,
&ts);


return
ts.tv_sec+
ts.tv_nsec/1000000000.0;

}





/*
 Basic AES测试

*/

void benchmark_basic()
{


AES_ctx ctx;


AES_init(
&ctx,
key);



double start=
get_time();



for(
size_t i=0;
i<TEST_SIZE;
i+=16)
{


AES_encrypt(
&ctx,
buffer+i);


}



double end=
get_time();



double speed=
(TEST_SIZE/1024.0/1024.0)/
(end-start);



printf(
"AES Basic C\n");


printf(
" Time : %.4f s\n",
end-start);


printf(
" Speed: %.2f MB/s\n\n",
speed);



}








/*
 T-table AES

*/

extern void AES_Ttable_init();


extern void AES_TTable_KeyExpansion(
uint8_t *,
const uint8_t *);

extern void AES_Ttable_encrypt(
uint8_t *,
uint8_t *);




void benchmark_ttable()
{


uint8_t rk[176];


AES_Ttable_init();



AES_TTable_KeyExpansion(
rk,
key);



double start=
get_time();



for(
size_t i=0;
i<TEST_SIZE;
i+=16)
{


AES_Ttable_encrypt(
buffer+i,
rk);


}



double end=
get_time();



printf(
"AES T-table\n");


printf(
" Time : %.4f s\n",
end-start);



printf(
" Speed: %.2f MB/s\n\n",
(TEST_SIZE/1024.0/1024.0)/
(end-start));


}







/*
 AES-NI

*/

extern void AESNI_KeyExpansion(
uint8_t *,
const uint8_t *);

extern void AES128_AESNI_encrypt(
uint8_t *,
uint8_t *);





void benchmark_aesni()
{


uint8_t rk[176];


AESNI_KeyExpansion(
rk,
key);



double start=
get_time();



for(
size_t i=0;
i<TEST_SIZE;
i+=16)
{


AES128_AESNI_encrypt(
buffer+i,
rk);


}



double end=
get_time();



printf(
"AES-NI\n");


printf(
" Time : %.4f s\n",
end-start);



printf(
" Speed: %.2f MB/s\n\n",
(TEST_SIZE/1024.0/1024.0)/
(end-start));



}








/*
 GCM

接口预留

后续连接gcm.c

*/

extern void AES_GCM_encrypt();



void benchmark_gcm()
{


double start=
get_time();



/*

调用:

AES_GCM_encrypt()

*/


for(
size_t i=0;
i<TEST_SIZE;
i+=16)
{


// GCM CTR + GHASH

}




double end=
get_time();



printf(
"AES-GCM\n");


printf(
" Time : %.4f s\n",
end-start);


printf(
" Speed: %.2f MB/s\n\n",
(TEST_SIZE/1024.0/1024.0)/
(end-start));


}





int main()
{


printf(
"=============================\n");

printf(
"AES Optimization Benchmark\n");

printf(
"Data size: %d MB\n",
TEST_SIZE/1024/1024);


printf(
"=============================\n\n");



memset(
buffer,
0x11,
TEST_SIZE);



benchmark_basic();



memset(
buffer,
0x11,
TEST_SIZE);


benchmark_ttable();



memset(
buffer,
0x11,
TEST_SIZE);


benchmark_aesni();



memset(
buffer,
0x11,
TEST_SIZE);


//benchmark_gcm();



return 0;

}
