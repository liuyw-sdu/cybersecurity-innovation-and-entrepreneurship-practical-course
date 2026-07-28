#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>


#include "../include/sm4.h"
#include "../include/sm4_ttable.h"
#include "../include/sm4_simd.h"



#define SIZE (100*1024*1024)



double run_time()
{
    return (double)clock()/CLOCKS_PER_SEC;
}



int main()
{


uint8_t key[16]=
{
0x01,0x23,0x45,0x67,
0x89,0xab,0xcd,0xef,
0xfe,0xdc,0xba,0x98,
0x76,0x54,0x32,0x10
};



uint8_t input[32]={0};

uint8_t output[32]={0};



uint32_t rk[32];



printf("=============================\n");
printf("SM4 Optimization Benchmark\n");
printf("Data size:100 MB\n");
printf("=============================\n\n");



/******** Basic ********/


sm4_setkey(rk,key);


double t1=run_time();


for(int i=0;i<SIZE/16;i++)
{
    sm4_encrypt(
        input,
        output,
        rk
    );
}


double t2=run_time();


printf("SM4 Basic\n");
printf("Time : %.4f s\n",
t2-t1);

printf("Speed: %.2f MB/s\n\n",
100/(t2-t1));




/******** T-table ********/


sm4_ttable_setkey(rk,key);


t1=run_time();


for(int i=0;i<SIZE/16;i++)
{
    sm4_ttable_encrypt(
        input,
        output,
        rk
    );
}


t2=run_time();


printf("SM4 T-table\n");

printf("Time : %.4f s\n",
t2-t1);

printf("Speed: %.2f MB/s\n\n",
100/(t2-t1));




/******** SIMD ********/


sm4_simd_setkey(rk,key);



t1=run_time();


for(int i=0;i<SIZE/32;i++)
{

sm4_simd_encrypt2(
input,
output,
rk
);

}


t2=run_time();



printf("SM4 SIMD AVX2\n");

printf("Time : %.4f s\n",
t2-t1);


printf("Speed: %.2f MB/s\n",
100/(t2-t1));



return 0;

}
