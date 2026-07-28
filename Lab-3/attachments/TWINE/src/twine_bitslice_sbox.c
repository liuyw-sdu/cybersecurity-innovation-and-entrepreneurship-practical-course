#include "twine_bitslice.h"



/*
 TWINE Sbox bitslice版本

 输入:

 x[0]-x[3]

 输出:

 x[0]-x[3]

*/


void twine_bs_sbox(
        uint64_t x[4])
{


uint64_t a,b,c,d;


a=x[0];
b=x[1];
c=x[2];
d=x[3];



/*
 以下为逻辑门实现

 只包含:

 XOR
 AND
 NOT

*/


uint64_t t0,t1,t2,t3;


t0 =
a ^ b;


t1 =
c & d;


t2 =
t0 ^ t1;


t3 =
a & c;



x[0]
=
t2 ^ t3;



x[1]
=
b ^ t1;



x[2]
=
c ^ t0;



x[3]
=
d ^ t2;



}
