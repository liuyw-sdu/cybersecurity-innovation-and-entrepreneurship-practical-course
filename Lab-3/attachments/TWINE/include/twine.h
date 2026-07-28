#ifndef TWINE_H
#define TWINE_H


#include <stdint.h>


#define TWINE_ROUNDS 36


typedef struct
{

    uint8_t rk[TWINE_ROUNDS][8];

}TWINE_KEY;



/*
 * 密钥扩展
 */
void twine_key_schedule(
        uint8_t *key,
        TWINE_KEY *ks);



/*
 * 加密
 */
void twine_encrypt(
        uint8_t *in,
        uint8_t *out,
        TWINE_KEY *ks);



/*
 * 解密
 */
void twine_decrypt(
        uint8_t *in,
        uint8_t *out,
        TWINE_KEY *ks);



/*
 * S盒
 */
uint8_t twine_sbox(
        uint8_t x);



uint8_t twine_inv_sbox(
        uint8_t x);


#endif
