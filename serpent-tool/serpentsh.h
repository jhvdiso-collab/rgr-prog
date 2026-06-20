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
void pkcs7_pad(uint8_t* data, size_t* data_len, size_t max_len);
size_t pkcs7_unpad(uint8_t* data, size_t data_len);

size_t serpent_cbc_encrypt(const uint8_t* plaintext, size_t plain_len,
                           const uint32_t* round_keys, const uint8_t* iv,
                           uint8_t* ciphertext, size_t cipher_max);

size_t serpent_cbc_decrypt(const uint8_t* ciphertext, size_t cipher_len,
                           const uint32_t* round_keys, const uint8_t* iv,
                           uint8_t* plaintext, size_t plain_max);

void process_file(const char* input_path, const char* output_path,
                  const char* password, bool encrypt);

#endif 