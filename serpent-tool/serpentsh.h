#ifndef SERPENT_H
#define SERPENT_H
#include <cstdint>
#include <cstddef>

extern const uint8_t SBOX[8][16];
extern const uint8_t INV_SBOX[8][16];

void serpent_encrypt_block(const uint32_t* round_keys, uint8_t* block);
void serpent_decrypt_block(const uint32_t* round_keys, uint8_t* block);
void serpent_key_schedule(const uint8_t* key, size_t key_len, uint32_t* out_keys);

void generate_random_bytes(uint8_t* buf, size_t len);
void derive_key(const char* password, const uint8_t* salt, uint8_t* key_out);

void process_file(const char* input_path, const char* output_path,
                  const char* password, bool encrypt);

#endif