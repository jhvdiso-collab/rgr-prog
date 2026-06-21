#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdint>
#include <fstream>
#include <random>
#include <stdexcept>

using namespace std;

// ==========================================
// ШИФР 1: ПЛЕЙФЕР 
// ==========================================
const int MATRIX_SIZE = 16;
const int BYTE_RANGE = 256;

void generateMatrix(const vector<unsigned char>& key,
                    unsigned char matrix[MATRIX_SIZE][MATRIX_SIZE]) {
    vector<unsigned char> allBytes;
    for (int i = 0; i < BYTE_RANGE; i++)
        allBytes.push_back(static_cast<unsigned char>(i));

    vector<unsigned char> keyBytes;
    bool seen[BYTE_RANGE] = {false};
    for (unsigned char c : key) {
        if (!seen[c]) { keyBytes.push_back(c); seen[c] = true; }
    }
    for (unsigned char c : allBytes) {
        if (!seen[c]) { keyBytes.push_back(c); seen[c] = true; }
    }

    int idx = 0;
    for (int i = 0; i < MATRIX_SIZE; i++)
        for (int j = 0; j < MATRIX_SIZE; j++)
            matrix[i][j] = keyBytes[idx++];
}

void findPosition(unsigned char matrix[MATRIX_SIZE][MATRIX_SIZE],
                  unsigned char c, int& row, int& col) {
    for (int i = 0; i < MATRIX_SIZE; i++)
        for (int j = 0; j < MATRIX_SIZE; j++)
            if (matrix[i][j] == c) { row = i; col = j; return; }
    row = -1; col = -1;
}

vector<unsigned char> playfairEncryptDecrypt(const vector<unsigned char>& data,
                                             const vector<unsigned char>& key,
                                             bool encrypt) {
    unsigned char matrix[MATRIX_SIZE][MATRIX_SIZE];
    generateMatrix(key, matrix);

    vector<unsigned char> result;
    result.reserve(data.size());

    for (size_t i = 0; i + 1 < data.size(); i += 2) {
        unsigned char a = data[i], b = data[i + 1];
        int r1, c1, r2, c2;
        findPosition(matrix, a, r1, c1);
        findPosition(matrix, b, r2, c2);

        if (r1 == r2) {
            if (encrypt) {
                result.push_back(matrix[r1][(c1 + 1) % MATRIX_SIZE]);
                result.push_back(matrix[r2][(c2 + 1) % MATRIX_SIZE]);
            } else {
                result.push_back(matrix[r1][(c1 - 1 + MATRIX_SIZE) % MATRIX_SIZE]);
                result.push_back(matrix[r2][(c2 - 1 + MATRIX_SIZE) % MATRIX_SIZE]);
            }
        } else if (c1 == c2) {
            if (encrypt) {
                result.push_back(matrix[(r1 + 1) % MATRIX_SIZE][c1]);
                result.push_back(matrix[(r2 + 1) % MATRIX_SIZE][c2]);
            } else {
                result.push_back(matrix[(r1 - 1 + MATRIX_SIZE) % MATRIX_SIZE][c1]);
                result.push_back(matrix[(r2 - 1 + MATRIX_SIZE) % MATRIX_SIZE][c2]);
            }
        } else {
            result.push_back(matrix[r1][c2]);
            result.push_back(matrix[r2][c1]);
        }
    }
    if (data.size() % 2 != 0) result.push_back(data.back());
    return result;
}

vector<unsigned char> readFilePlayfair(const string& filename) {
    ifstream file(filename, ios::binary | ios::ate);
    if (!file) { cerr << "Ошибка открытия: " << filename << endl; exit(1); }
    streamsize size = file.tellg();
    file.seekg(0, ios::beg);
    vector<unsigned char> buffer(size);
    file.read(reinterpret_cast<char*>(buffer.data()), size);
    return buffer;
}

void writeFilePlayfair(const string& filename, const vector<unsigned char>& data) {
    ofstream file(filename, ios::binary);
    if (!file) { cerr << "Ошибка создания: " << filename << endl; exit(1); }
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

// ==========================================
// ШИФР 2: ХИЛЛ 
// ==========================================
int mod_inverse(int value) {
    value %= 256; if (value < 0) value += 256;
    for (int i = 1; i < 256; i++)
        if ((value * i) % 256 == 1) return i;
    return -1;
}

int determinant(int a, int b, int c, int d) { return a * d - b * c; }

void process_block(uint8_t x1, uint8_t x2, uint8_t& y1, uint8_t& y2,
                   int a, int b, int c, int d) {
    y1 = (a * x1 + b * x2) % 256;
    y2 = (c * x1 + d * x2) % 256;
}

vector<uint8_t> process_data(const vector<uint8_t>& data,
                             int a, int b, int c, int d) {
    vector<uint8_t> result = data;
    if (result.size() % 2 != 0) result.push_back(0);
    for (size_t i = 0; i < result.size(); i += 2) {
        uint8_t r1, r2;
        process_block(result[i], result[i + 1], r1, r2, a, b, c, d);
        result[i] = r1; result[i + 1] = r2;
    }
    return result;
}

bool inverse_key(int a, int b, int c, int d, int& ia, int& ib, int& ic, int& id) {
    int det = determinant(a, b, c, d);
    det %= 256; if (det < 0) det += 256;
    int det_inv = mod_inverse(det);
    if (det_inv == -1) return false;

    ia = ( d * det_inv) % 256;
    ib = ((-b + 256) * det_inv) % 256;
    ic = ((-c + 256) * det_inv) % 256;
    id = ( a * det_inv) % 256;
    if (ia < 0) ia += 256; if (ib < 0) ib += 256;
    if (ic < 0) ic += 256; if (id < 0) id += 256;
    return true;
}

vector<uint8_t> read_file_hill(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file) throw runtime_error("Не удалось открыть файл: " + filename);
    return vector<uint8_t>(istreambuf_iterator<char>(file), istreambuf_iterator<char>());
}

void write_file_hill(const string& filename, const vector<uint8_t>& data) {
    ofstream file(filename, ios::binary);
    if (!file) throw runtime_error("Не удалось создать файл: " + filename);
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

// ==========================================
// ШИФР 3: SERPENT
// ==========================================
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
    { 8,15, 2, 9, 4, 1,13,14,11, 6, 5, 3, 7,12,10, 0 },
    {15,10, 1,13, 5, 3, 6, 0, 4, 9,14, 7, 2,12, 8,11 },
    { 3, 0, 6,13, 9,14,15, 8, 5,12,11, 7,10, 1, 4, 2 }
};

static inline uint32_t rotl(uint32_t x, int n) { return (x << n) | (x >> (32 - n)); }
static inline uint32_t rotr(uint32_t x, int n) { return (x >> n) | (x << (32 - n)); }

static void linear_transform(uint32_t* x0, uint32_t* x1, uint32_t* x2, uint32_t* x3) {
    *x0 = rotl(*x0, 13); *x2 = rotl(*x2, 3);
    *x1 ^= *x0 ^ *x2; *x3 ^= *x2 ^ (*x0 << 3);
    *x1 = rotl(*x1, 1); *x3 = rotl(*x3, 7);
    *x0 ^= *x1 ^ *x3; *x2 ^= *x3 ^ (*x1 << 7);
    *x0 = rotl(*x0, 5); *x2 = rotl(*x2, 22);
}

static void inv_linear_transform(uint32_t* x0, uint32_t* x1, uint32_t* x2, uint32_t* x3) {
    *x2 = rotr(*x2, 22); *x0 = rotr(*x0, 5);
    *x0 ^= *x1 ^ *x3; *x2 ^= *x3 ^ (*x1 << 7);
    *x1 = rotr(*x1, 1); *x3 = rotr(*x3, 7);
    *x1 ^= *x0 ^ *x2; *x3 ^= *x2 ^ (*x0 << 3);
    *x0 = rotr(*x0, 13); *x2 = rotr(*x2, 3);
}

static void apply_sbox(uint32_t* w0, uint32_t* w1, uint32_t* w2, uint32_t* w3, const uint8_t* box) {
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

void serpent_encrypt_block(const uint32_t* rk, uint8_t* block) {
    uint32_t x0, x1, x2, x3;
    memcpy(&x0, block, 4); memcpy(&x1, block+4, 4);
    memcpy(&x2, block+8, 4); memcpy(&x3, block+12, 4);
    
    for (int r = 0; r < 31; ++r) {
        x0 ^= rk[4*r]; x1 ^= rk[4*r+1]; x2 ^= rk[4*r+2]; x3 ^= rk[4*r+3];
        apply_sbox(&x0, &x1, &x2, &x3, SBOX[r % 8]);
        linear_transform(&x0, &x1, &x2, &x3);
    }
    x0 ^= rk[124]; x1 ^= rk[125]; x2 ^= rk[126]; x3 ^= rk[127];
    apply_sbox(&x0, &x1, &x2, &x3, SBOX[7]);
    x0 ^= rk[128]; x1 ^= rk[129]; x2 ^= rk[130]; x3 ^= rk[131];
    
    memcpy(block, &x0, 4); memcpy(block+4, &x1, 4);
    memcpy(block+8, &x2, 4); memcpy(block+12, &x3, 4);
}

void serpent_decrypt_block(const uint32_t* rk, uint8_t* block) {
    uint32_t x0, x1, x2, x3;
    memcpy(&x0, block, 4); memcpy(&x1, block+4, 4);
    memcpy(&x2, block+8, 4); memcpy(&x3, block+12, 4);
    
    x0 ^= rk[128]; x1 ^= rk[129]; x2 ^= rk[130]; x3 ^= rk[131];
    apply_sbox(&x0, &x1, &x2, &x3, INV_SBOX[7]);
    x0 ^= rk[124]; x1 ^= rk[125]; x2 ^= rk[126]; x3 ^= rk[127];
    
    for (int r = 30; r >= 0; --r) {
        inv_linear_transform(&x0, &x1, &x2, &x3);
        apply_sbox(&x0, &x1, &x2, &x3, INV_SBOX[r % 8]);
        x0 ^= rk[4*r]; x1 ^= rk[4*r+1]; x2 ^= rk[4*r+2]; x3 ^= rk[4*r+3];
    }
    
    memcpy(block, &x0, 4); memcpy(block+4, &x1, 4);
    memcpy(block+8, &x2, 4); memcpy(block+12, &x3, 4);
}

static void apply_sbox_local(uint32_t* w0, uint32_t* w1, uint32_t* w2, uint32_t* w3, const uint8_t* box) {
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
    for (size_t i = 0; i < bytes; ++i)
        w[i / 4] |= (uint32_t)key[i] << (8 * (i % 4));
    if (key_len < 32)
        w[key_len / 4] |= (uint32_t)1 << (8 * (key_len % 4));

    uint32_t prekeys[140];
    for (int i = 0; i < 8; ++i) prekeys[i] = w[i];
    for (int i = 8; i < 140; ++i)
        prekeys[i] = rotl(prekeys[i-8] ^ prekeys[i-5] ^ prekeys[i-3] ^ prekeys[i-1] ^ 0x9E3779B9 ^ (uint32_t)(i-8), 11);

    for (int r = 0; r < 33; ++r) {
        int sbox_idx = (3 - r) % 8;
        if (sbox_idx < 0) sbox_idx += 8;
        out_keys[4*r] = prekeys[4*r]; out_keys[4*r+1] = prekeys[4*r+1];
        out_keys[4*r+2] = prekeys[4*r+2]; out_keys[4*r+3] = prekeys[4*r+3];
        apply_sbox_local(&out_keys[4*r], &out_keys[4*r+1], &out_keys[4*r+2], &out_keys[4*r+3], SBOX[sbox_idx]);
    }
}

void generate_random_bytes(uint8_t* buf, size_t len) {
    random_device rd; mt19937_64 gen(rd());
    uniform_int_distribution<int> dist(0, 255);
    for (size_t i = 0; i < len; ++i) buf[i] = (uint8_t)dist(gen);
}

void derive_key(const char* password, const uint8_t* salt, uint8_t* key_out) {
    size_t pass_len = strlen(password);
    for (int i = 0; i < 32; ++i)
        key_out[i] = password[i % pass_len] ^ salt[i % 16] ^ (i * 0x1B);
}

size_t serpent_cbc_encrypt(const uint8_t* plaintext, size_t plain_len, const uint32_t* round_keys,
                           const uint8_t* iv, uint8_t* ciphertext, size_t cipher_max) {
    size_t pad_len = 16 - (plain_len % 16);
    size_t padded_len = plain_len + pad_len;
    if (padded_len > cipher_max) throw runtime_error("Буфер шифротекста слишком мал");
    memcpy(ciphertext, plaintext, plain_len);
    for (size_t i = 0; i < pad_len; ++i) ciphertext[plain_len + i] = (uint8_t)pad_len;

    uint8_t prev_ct[16]; memcpy(prev_ct, iv, 16);
    for (size_t i = 0; i < padded_len; i += 16) {
        for (int j = 0; j < 16; ++j) ciphertext[i + j] ^= prev_ct[j];
        serpent_encrypt_block(round_keys, ciphertext + i);
        memcpy(prev_ct, ciphertext + i, 16);
    }
    return padded_len;
}

size_t serpent_cbc_decrypt(const uint8_t* ciphertext, size_t cipher_len, const uint32_t* round_keys,
                           const uint8_t* iv, uint8_t* plaintext, size_t plain_max) {
    if (cipher_len % 16 != 0 || cipher_len == 0) throw runtime_error("Длина шифротекста не кратна 16");
    if (plain_max < cipher_len) throw runtime_error("Буфер для расшифровки слишком мал");
    memcpy(plaintext, ciphertext, cipher_len);

    uint8_t prev_ct[16]; memcpy(prev_ct, iv, 16);
    for (size_t i = 0; i < cipher_len; i += 16) {
        uint8_t current_block[16]; memcpy(current_block, plaintext + i, 16);
        serpent_decrypt_block(round_keys, plaintext + i);
        for (int j = 0; j < 16; ++j) plaintext[i + j] ^= prev_ct[j];
        memcpy(prev_ct, current_block, 16);
    }

    uint8_t pad_len = plaintext[cipher_len - 1];
    if (pad_len == 0 || pad_len > 16) throw runtime_error("Некорректный паддинг");
    for (uint8_t i = 1; i <= pad_len; ++i)
        if (plaintext[cipher_len - i] != pad_len) throw runtime_error("Битый паддинг");
    return cipher_len - pad_len;
}

void process_file_serpent(const char* input_path, const char* output_path,
                          const char* password, bool encrypt) {
    ifstream in(input_path, ios::binary);
    if (!in) throw runtime_error("Невозможно открыть входной файл");
    vector<uint8_t> data;
    in.seekg(0, ios::end); data.resize(in.tellg());
    in.seekg(0, ios::beg); in.read((char*)data.data(), data.size());
    in.close();

    vector<uint8_t> result;
    if (encrypt) {
        uint8_t salt[16], iv[16];
        generate_random_bytes(salt, 16); generate_random_bytes(iv, 16);
        uint8_t key[32]; derive_key(password, salt, key);
        uint32_t round_keys[140]; serpent_key_schedule(key, 32, round_keys);
        uint64_t orig_size = data.size();
        result.resize(40);
        memcpy(result.data(), salt, 16);
        memcpy(result.data() + 16, iv, 16);
        memcpy(result.data() + 32, &orig_size, 8);
        vector<uint8_t> cipher_buf(data.size() + 16);
        size_t cipher_len = serpent_cbc_encrypt(data.data(), data.size(), round_keys, iv,
                                                cipher_buf.data(), cipher_buf.size());
        result.insert(result.end(), cipher_buf.begin(), cipher_buf.begin() + cipher_len);
    } else {
        if (data.size() < 40) throw runtime_error("Файл повреждён");
        uint8_t salt[16], iv[16]; uint64_t orig_size;
        memcpy(salt, data.data(), 16);
        memcpy(iv, data.data() + 16, 16);
        memcpy(&orig_size, data.data() + 32, 8);
        uint8_t key[32]; derive_key(password, salt, key);
        uint32_t round_keys[140]; serpent_key_schedule(key, 32, round_keys);
        size_t cipher_len = data.size() - 40;
        vector<uint8_t> plain_buf(cipher_len);
        size_t plain_len = serpent_cbc_decrypt(data.data() + 40, cipher_len, round_keys, iv,
                                               plain_buf.data(), plain_buf.size());
        result.assign(plain_buf.begin(), plain_buf.begin() + plain_len);
    }
    ofstream out(output_path, ios::binary);
    if (!out) throw runtime_error("Невозможно создать выходной файл");
    out.write((char*)result.data(), result.size());
}

// ==========================================
// MAIN
// ==========================================
int main() {
    cout << "Выберите шифр:\n";
    cout << "1 - Плейфер (16x16)\n";
    cout << "2 - Хилл (2x2)\n";
    cout << "3 - Serpent\n";
    cout << "Ваш выбор: ";

    int choice;
    cin >> choice;
    cin.ignore();

    if (choice == 1) {
        cout << "\n=== ПЛЕЙФЕР ===\n";
        cout << "1 - Шифровать файл\n";
        cout << "2 - Расшифровать файл\n";
        cout << "Выбор: ";
        int mode; cin >> mode; cin.ignore();

        string input, output, keyStr;
        cout << "Входной файл: "; getline(cin, input);
        cout << "Выходной файл: "; getline(cin, output);
        cout << "Ключ (строка): "; getline(cin, keyStr);

        vector<unsigned char> key(keyStr.begin(), keyStr.end());
        auto data = readFilePlayfair(input);
        auto result = playfairEncryptDecrypt(data, key, mode == 1);
        writeFilePlayfair(output, result);
        cout << "Готово!\n";
    }
    else if (choice == 2) {
        cout << "\n=== ХИЛЛ ===\n";
        int a, b, c, d;
        cout << "Введите ключ (a b c d): ";
        cin >> a >> b >> c >> d;

        int ia, ib, ic, id;
        if (!inverse_key(a, b, c, d, ia, ib, ic, id)) {
            cout << "Ключ необратим!\n";
            return 1;
        }

        cout << "1 - Шифровать файл\n";
        cout << "2 - Расшифровать файл\n";
        cout << "Выбор: ";
        int mode; cin >> mode; cin.ignore();

        string input, output;
        cout << "Входной файл: "; getline(cin, input);
        cout << "Выходной файл: "; getline(cin, output);

        auto data = read_file_hill(input);
        vector<uint8_t> result;
        if (mode == 1)
            result = process_data(data, a, b, c, d);
        else
            result = process_data(data, ia, ib, ic, id);
        write_file_hill(output, result);
        cout << "Готово!\n";
    }
    else if (choice == 3) {
        cout << "\n=== SERPENT ===\n";
        cout << "1 - Шифровать файл\n";
        cout << "2 - Расшифровать файл\n";
        cout << "Выбор: ";
        int mode; cin >> mode; cin.ignore();

        string input, output, password;
        cout << "Входной файл: "; getline(cin, input);
        cout << "Выходной файл: "; getline(cin, output);
        cout << "Пароль: "; getline(cin, password);

        process_file_serpent(input.c_str(), output.c_str(), password.c_str(), mode == 1);
        cout << "Готово!\n";
    }
    else {
        cout << "Неверный выбор\n";
    }

    return 0;
}