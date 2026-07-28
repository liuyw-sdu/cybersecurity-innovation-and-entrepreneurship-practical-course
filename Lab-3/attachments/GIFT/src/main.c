#include "../include/gift.h"
#include "../include/gift_bitslice.h"

#include <stdio.h>



void print_block(char *name,u8 data[16])
{

    printf("%s:\n",name);


    for(int i=0;i<16;i++)
    {

        printf("%02x ",data[i]);

    }

    printf("\n\n");

}




int main()
{


    u8 plaintext[16]=
    {
        0,1,2,3,
        4,5,6,7,
        8,9,10,11,
        12,13,14,15
    };


    u8 key[16]={0};



    u8 out1[16];

    u8 out2[16];



    /*
       普通版本
    */

    gift128_encrypt(
            plaintext,
            key,
            out1
    );



    /*
       bitslice版本
    */


    gift128_bitslice_encrypt(
            plaintext,
            key,
            out2
    );



    print_block(
            "basic",
            out1
    );


    print_block(
            "bitslice",
            out2
    );



    return 0;

}
