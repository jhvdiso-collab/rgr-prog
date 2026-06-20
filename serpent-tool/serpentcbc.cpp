#include "serpentsh.h"
#include <cstring>
#include <stdexcept>
#include <fstream>
#include <vector>

using namespace std;

size_t serpent_cbc_encrypt(const uint8_t* plaintext, size_t plain_len,
                           const uint32_t* round_keys, const uint8_t* iv,
                           uint8_t* ciphertext, size_t cipher_max) {
    size_t padded_len = plain_len;
    if (cipher_max < plain_len + 16) {
        throw runtime_error("Буфер шифротекста слишком мал");
    }
    memcpy(ciphertext, plaintext, plain_len);
    
    pkcs7_pad(ciphertext, &padded_len, cipher_max);
    
    uint8_t prev_ct[16];
    memcpy(prev_ct, iv, 16);
    
    for (size_t i = 0; i < padded_len; i += 16) {
        for (int j = 0; j < 16; ++j) {
            ciphertext[i + j] ^= prev_ct[j];
        }
        serpent_encrypt_block(round_keys, ciphertext + i);
        memcpy(prev_ct, ciphertext + i, 16);
    }
    
    return padded_len;
}

size_t serpent_cbc_decrypt(const uint8_t* ciphertext, size_t cipher_len,
                           const uint32_t* round_keys, const uint8_t* iv,
                           uint8_t* plaintext, size_t plain_max) {
    if (cipher_len % 16 != 0 || cipher_len == 0) {
        throw runtime_error("Длина шифротекста не кратна 16");
    }
    if (plain_max < cipher_len) {
        throw runtime_error("Буфер для расшифровки слишком мал");
    }
    
    memcpy(plaintext, ciphertext, cipher_len);
    
    uint8_t prev_ct[16];
    memcpy(prev_ct, iv, 16);
    
    for (size_t i = 0; i < cipher_len; i += 16) {
        uint8_t current_block[16];
        memcpy(current_block, plaintext + i, 16);
        
        serpent_decrypt_block(round_keys, plaintext + i);
        
        for (int j = 0; j < 16; ++j) {
            plaintext[i + j] ^= prev_ct[j];
        }
        
        memcpy(prev_ct, current_block, 16);
    }
    
    return pkcs7_unpad(plaintext, cipher_len);
}

void process_file(const char* input_path, const char* output_path,
                  const char* password, bool encrypt) {
    ifstream in(input_path, ios::binary);
    if (!in) throw runtime_error("Невозможно открыть входной файл");
    
    vector<uint8_t> data;
    in.seekg(0, ios::end);
    data.resize(in.tellg());
    in.seekg(0, ios::beg);
    in.read((char*)data.data(), data.size());
    in.close();
    
    vector<uint8_t> result;
    
    if (encrypt) {
        uint8_t salt[16], iv[16];
        generate_random_bytes(salt, 16);
        generate_random_bytes(iv, 16);
        
        uint8_t key[32];
        derive_key(password, salt, key);
        
        uint32_t round_keys[140];
        serpent_key_schedule(key, 32, round_keys);
        
        uint64_t orig_size = data.size();
        result.resize(40);
        memcpy(result.data(), salt, 16);
        memcpy(result.data() + 16, iv, 16);
        memcpy(result.data() + 32, &orig_size, 8);
        
        std::vector<uint8_t> cipher_buf(data.size() + 16);
        size_t cipher_len = serpent_cbc_encrypt(data.data(), data.size(),
                                                round_keys, iv,
                                                cipher_buf.data(), cipher_buf.size());
        result.insert(result.end(), cipher_buf.begin(), cipher_buf.begin() + cipher_len);
    } else {
        if (data.size() < 40) {
            throw runtime_error("Файл повреждён");
        }
        
        uint8_t salt[16], iv[16];
        uint64_t orig_size;
        memcpy(salt, data.data(), 16);
        memcpy(iv, data.data() + 16, 16);
        memcpy(&orig_size, data.data() + 32, 8);
        
        uint8_t key[32];
        derive_key(password, salt, key);
        
        uint32_t round_keys[140];
        serpent_key_schedule(key, 32, round_keys);
        
        size_t cipher_len = data.size() - 40;
        vector<uint8_t> plain_buf(cipher_len);
        size_t plain_len = serpent_cbc_decrypt(data.data() + 40, cipher_len,
                                               round_keys, iv,
                                               plain_buf.data(), plain_buf.size());
        result.assign(plain_buf.begin(), plain_buf.begin() + plain_len);
    }
    
    ofstream out(output_path, ios::binary);
    if (!out) throw runtime_error("Невозможно создать выходной файл");
    out.write((char*)result.data(), result.size());
}