#include "twine_bitslice.h"





static void bs_round(
        uint64_t state[4],
        uint64_t rk[4])
{


/*
 XOR round key

 */

for(int i=0;i<4;i++)
{

state[i]^=
rk[i];

}



/*
 bitslice Sbox

 */

twine_bs_sbox(state);



}






void twine_bs_encrypt(
        uint64_t state[4],
        uint64_t out[4],
        TWINE_BS_KEY *ks)
{


for(int r=0;r<36;r++)
{


bs_round(
state,
ks->rk[r]);


}



for(int i=0;i<4;i++)

out[i]=state[i];


}
