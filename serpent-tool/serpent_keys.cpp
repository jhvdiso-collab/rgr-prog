#include "serpentsh.h"
#include <cstring>
#include <random>
#include <stdexcept>

using namespace std;

static inline uint32_t rotl(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

static void apply_sbox_local(uint32_t* w0, uint32_t* w1, uint32_t* w2, uint32_t* w3,
                             const uint8_t* box) {
    uint32_t res[4] = {0, 0, 0, 0};
    for (int i = 0; i < 32; ++i) {
        uint8_t in = ((*w0 >> i) & 1) | (((*w1 >> i) & 1) << 1) |
                     (((*w2 >> i) & 1) << 2) | (((*w3 >> i) & 1) << 3);
        uint8_t out = box[in];
        res[0] |= (uint32_t)(out & 1) << i;
        res[1] |= (uint32_t)((out >> 1) & 1) << i;
        res[2] |= (uint32_t)((out >> 2) & 1) << i;
        res[3] |= (uint32_t)((out >> 3) & 1) << i;
    }
    *w0 = res[0]; *w1 = res[1]; *w2 = res[2]; *w3 = res[3];
}

void serpent_key_schedule(const uint8_t* key, size_t key_len, uint32_t* out_keys) {
    uint32_t w[8] = {0};

    size_t bytes = (key_len < 32) ? key_len : 32;
    for (size_t i = 0; i < bytes; ++i) {
        w[i / 4] |= (uint32_t)key[i] << (8 * (i % 4));
    }

    if (key_len < 32) {
        w[key_len / 4] |= (uint32_t)1 << (8 * (key_len % 4));
    }

    uint32_t prekeys[140];
    for (int i = 0; i < 8; ++i) {
        prekeys[i] = w[i];
    }
    
    for (int i = 8; i < 140; ++i) {
        prekeys[i] = rotl(prekeys[i-8] ^ prekeys[i-5] ^ prekeys[i-3] ^ prekeys[i-1] ^ 0x9E3779B9 ^ (uint32_t)(i-8), 11);
    }

    for (int r = 0; r < 33; ++r) {
        int sbox_idx = (3 - r) % 8;
        if (sbox_idx < 0) sbox_idx += 8;
        
        out_keys[4*r]   = prekeys[4*r];
        out_keys[4*r+1] = prekeys[4*r+1];
        out_keys[4*r+2] = prekeys[4*r+2];
        out_keys[4*r+3] = prekeys[4*r+3];
        
        apply_sbox_local(&out_keys[4*r], &out_keys[4*r+1],
                         &out_keys[4*r+2], &out_keys[4*r+3], SBOX[sbox_idx]);
    }
}

void generate_random_bytes(uint8_t* buf, size_t len) {
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<int> dist(0, 255);
    for (size_t i = 0; i < len; ++i) {
        buf[i] = (uint8_t)dist(gen);
    }
}

void derive_key(const char* password, const uint8_t* salt, uint8_t* key_out) {
    size_t pass_len = strlen(password);
    for (int i = 0; i < 32; ++i) {
        key_out[i] = password[i % pass_len] ^ salt[i % 16] ^ (i * 0x1B);
    }
}