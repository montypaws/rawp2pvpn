#ifndef __TEA.H__
#define __TEA.H__

#include <stdint.h>

void encrypt (uint32_t* v, uint32_t* k);
void decrypt (uint32_t* v, uint32_t* k);
void encryptBlock(uint8_t * data, uint32_t * len, uint32_t * key);
void decryptBlock(uint8_t * data, uint32_t * len, uint32_t * key);

#endif
