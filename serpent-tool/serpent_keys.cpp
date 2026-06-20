#include "serpentsh.h"
#include <cstring>
#include <random>
#include <stdexcept>

static inline uint32_t rotl(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

static void apply_sbox_local(uint32_t* w0, uint32_t* w1, uint32_t* w2, uint32_t* w3,
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

void serpent_key_schedule(const uint8_t* key, size_t key_len, uint32_t* out_keys) {
    uint32_t w[8] = {0};
    for (size_t i = 0; i < key_len && i < 32; ++i) {
        w[i / 4] |= (uint32_t)key[i] << (8 * (i % 4));
    }
    
    for (int i = 8; i < 140; ++i) {
        uint32_t tmp = w[(i-8) & 7] ^ w[(i-5) & 7] ^ w[(i-3) & 7]
                     ^ w[(i-1) & 7] ^ 0x9E3779B9 ^ (uint32_t)(i-8);
        w[i & 7] = rotl(tmp, 11);
        out_keys[i - 8] = w[i & 7];
    }
    
    for (int r = 0; r < 33; ++r) {
        apply_sbox_local(&out_keys[4*r], &out_keys[4*r+1],
                         &out_keys[4*r+2], &out_keys[4*r+3], SBOX[r % 8]);
    }
}

void generate_random_bytes(uint8_t* buf, size_t len) {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    static std::uniform_int_distribution<int> dist(0, 255);
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

void pkcs7_pad(uint8_t* data, size_t* data_len, size_t max_len) {
    size_t pad_len = 16 - (*data_len % 16);
    if (*data_len + pad_len > max_len) {
        throw std::runtime_error("Недостаточно места для паддинга");
    }
    for (size_t i = 0; i < pad_len; ++i) {
        data[*data_len + i] = (uint8_t)pad_len;
    }
    *data_len += pad_len;
}

size_t pkcs7_unpad(uint8_t* data, size_t data_len) {
    if (data_len == 0 || data_len % 16 != 0) {
        throw std::runtime_error("Некорректная длина данных");
    }
    uint8_t pad_len = data[data_len - 1];
    if (pad_len == 0 || pad_len > 16) {
        throw std::runtime_error("Некорректный паддинг");
    }
    for (uint8_t i = 1; i <= pad_len; ++i) {
        if (data[data_len - i] != pad_len) {
            throw std::runtime_error("Битый паддинг");
        }
    }
    return data_len - pad_len;
}