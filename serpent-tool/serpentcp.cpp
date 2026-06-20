#include "serpentsh.h"
#include <cstring>

using namespace std;

const uint8_t SBOX[8][16] = {
    { 3, 8,15, 1,10, 6, 5,11,14,13, 4, 2, 7, 0, 9,12 },
    {15,12, 2, 7, 9, 0, 5,10, 1,11,14, 8, 6,13, 3, 4 },
    { 8, 6, 7, 9, 3,12,10,15,13, 1,14, 4, 0,11, 5, 2 },
    { 0,15,11, 8,12, 9, 6, 3,13, 1, 2, 4,10, 7, 5,14 },
    { 1,15, 8, 3,12, 0,11, 6, 2, 5, 4,10, 9,14, 7,13 },
    {15, 5, 2,11, 4,10, 9,12, 0, 3,14, 8,13, 6, 7, 1 },
    { 7, 2,12, 5, 8, 4, 6,11,14, 9, 1,15,13, 3,10, 0 },
    { 1,13,15, 0,14, 8, 2,11, 7, 4,12,10, 9, 3, 5, 6 }
};

const uint8_t INV_SBOX[8][16] = {
    {13, 3,11, 0,10, 6, 5,12, 1,14, 4, 7,15, 9, 8, 2 },
    { 5, 8, 2,14,15, 6,12, 3,11, 4, 7, 9, 1,13,10, 0 },
    {12, 9,15, 4,11,14, 1, 2, 0, 3, 6,13, 5, 8,10, 7 },
    { 0, 9,10, 7,11,14, 6,13, 3, 5,12, 2, 4, 8,15, 1 },
    { 5, 0, 8, 3,10, 9, 7,14, 2,12,11, 6, 4,15,13, 1 },
    {14,15, 2, 9, 4, 1,13,12,11, 6, 5, 3, 7, 8,10, 0 },
    {15,10, 1,13, 5, 3, 6, 0, 4, 9,14, 7, 2,12, 8,11 },
    { 3, 0, 6,13, 9,14,15, 8, 5,12,11, 7,10, 1, 4, 2 }
};

static inline uint32_t rotl(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

static inline uint32_t rotr(uint32_t x, int n) {
    return (x >> n) | (x << (32 - n));
}

static void linear_transform(uint32_t* x0, uint32_t* x1, uint32_t* x2, uint32_t* x3) {
    *x0 = rotl(*x0, 13);
    *x2 = rotl(*x2, 3);
    *x1 ^= *x0 ^ *x2;
    *x3 ^= *x2 ^ (*x0 << 3);
    *x1 = rotl(*x1, 1);
    *x3 = rotl(*x3, 7);
    *x0 ^= *x1 ^ *x3;
    *x2 ^= *x3 ^ (*x1 << 7);
    *x0 = rotl(*x0, 5);
    *x2 = rotl(*x2, 22);
}

static void inv_linear_transform(uint32_t* x0, uint32_t* x1, uint32_t* x2, uint32_t* x3) {
    *x2 = rotr(*x2, 22);
    *x0 = rotr(*x0, 5);
    *x0 ^= *x1 ^ *x3;
    *x2 ^= *x3 ^ (*x1 << 7);
    *x1 = rotr(*x1, 1);
    *x3 = rotr(*x3, 7);
    *x1 ^= *x0 ^ *x2;
    *x3 ^= *x2 ^ (*x0 << 3);
    *x0 = rotr(*x0, 13);
    *x2 = rotr(*x2, 3);
}

static void apply_sbox(uint32_t* w0, uint32_t* w1, uint32_t* w2, uint32_t* w3,
                       const uint8_t* box) {
    uint32_t res0 = 0, res1 = 0, res2 = 0, res3 = 0;
    for (int i = 0; i < 32; ++i) {
        uint8_t nibble = ((*w0 >> i) & 1) 
                       | (((*w1 >> i) & 1) << 1)
                       | (((*w2 >> i) & 1) << 2) 
                       | (((*w3 >> i) & 1) << 3);
        uint8_t out = box[nibble];
        res0 |= (uint32_t)(out & 1) << i;
        res1 |= (uint32_t)((out >> 1) & 1) << i;
        res2 |= (uint32_t)((out >> 2) & 1) << i;
        res3 |= (uint32_t)((out >> 3) & 1) << i;
    }
    *w0 = res0; *w1 = res1; *w2 = res2; *w3 = res3;
}

void serpent_encrypt_block(const uint32_t* round_keys, uint8_t* block) {
    uint32_t x0, x1, x2, x3;
    
    memcpy(&x0, block, 4);
    memcpy(&x1, block + 4, 4);
    memcpy(&x2, block + 8, 4);
    memcpy(&x3, block + 12, 4);
    
    for (int r = 0; r < 32; ++r) {
        x0 ^= round_keys[4*r];
        x1 ^= round_keys[4*r+1];
        x2 ^= round_keys[4*r+2];
        x3 ^= round_keys[4*r+3];
        
        apply_sbox(&x0, &x1, &x2, &x3, SBOX[r % 8]);
        
        if (r < 31) {
            linear_transform(&x0, &x1, &x2, &x3);
        }
    }
    
    x0 ^= round_keys[128];
    x1 ^= round_keys[129];
    x2 ^= round_keys[130];
    x3 ^= round_keys[131];
    
    memcpy(block, &x0, 4);
    memcpy(block + 4, &x1, 4);
    memcpy(block + 8, &x2, 4);
    memcpy(block + 12, &x3, 4);
}

void serpent_decrypt_block(const uint32_t* round_keys, uint8_t* block) {
    uint32_t x0, x1, x2, x3;
    
    memcpy(&x0, block, 4);
    memcpy(&x1, block + 4, 4);
    memcpy(&x2, block + 8, 4);
    memcpy(&x3, block + 12, 4);
    
    x0 ^= round_keys[128];
    x1 ^= round_keys[129];
    x2 ^= round_keys[130];
    x3 ^= round_keys[131];
    
    for (int r = 31; r >= 0; --r) {
        apply_sbox(&x0, &x1, &x2, &x3, INV_SBOX[r % 8]);
        
        x0 ^= round_keys[4*r];
        x1 ^= round_keys[4*r+1];
        x2 ^= round_keys[4*r+2];
        x3 ^= round_keys[4*r+3];
        
        if (r > 0) {
            inv_linear_transform(&x0, &x1, &x2, &x3);
        }
    }
    
    memcpy(block, &x0, 4);
    memcpy(block + 4, &x1, 4);
    memcpy(block + 8, &x2, 4);
    memcpy(block + 12, &x3, 4);
}