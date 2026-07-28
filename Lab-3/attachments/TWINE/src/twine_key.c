#include "twine.h"



static uint8_t RC[36]=
{

0x01,0x02,0x04,0x08,
0x10,0x20,0x3b,0x36,
0x2c,0x19,0x32,0x25,
0x0b,0x16,0x2d,0x1a,
0x34,0x29,0x12,0x24,
0x09,0x13,0x26,0x0d,
0x1a,0x35,0x2b,0x17,
0x2e,0x1d,0x3a,0x33,
0x27,0x0f,0x1e,0x3c

};




void twine_key_schedule(
        uint8_t *key,
        TWINE_KEY *ks)
{

    uint8_t k[20];


    /*
       byte 转 nibble
    */

    for(int i=0;i<10;i++)
    {

        k[2*i]=
        key[i]>>4;


        k[2*i+1]=
        key[i]&0xf;

    }



    for(int r=0;r<TWINE_ROUNDS;r++)
    {

        /*
          round key
        */

        for(int i=0;i<8;i++)
        {

            ks->rk[r][i]
            =
            k[i];

        }



        /*
          更新密钥状态
        */


        k[0]^=
        twine_sbox(k[1]);



        k[1]^=
        RC[r]&0xf;



        /*
          nibble循环移位
        */

        uint8_t temp=k[19];


        for(int i=19;i>0;i--)
        {
            k[i]=k[i-1];
        }


        k[0]=temp;


    }

}
