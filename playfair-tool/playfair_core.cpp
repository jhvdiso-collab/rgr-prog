#include "playfair.h"

vector<unsigned char> encryptDecrypt(const vector<unsigned char>& data,
                                     const vector<unsigned char>& key,
                                     bool encrypt) {
    unsigned char matrix[MATRIX_SIZE][MATRIX_SIZE];
    generateMatrix(key, matrix);
    
    vector<unsigned char> result;
    result.reserve(data.size());
    
    for (size_t i = 0; i < data.size() - 1; i += 2) {
        unsigned char a = data[i];
        unsigned char b = data[i + 1];
        
        int row1, col1, row2, col2;
        findPosition(matrix, a, row1, col1);
        findPosition(matrix, b, row2, col2);
        
        if (row1 == row2) {
            if (encrypt) {
                result.push_back(matrix[row1][(col1 + 1) % MATRIX_SIZE]);
                result.push_back(matrix[row2][(col2 + 1) % MATRIX_SIZE]);
            } else {
                result.push_back(matrix[row1][(col1 - 1 + MATRIX_SIZE) % MATRIX_SIZE]);
                result.push_back(matrix[row2][(col2 - 1 + MATRIX_SIZE) % MATRIX_SIZE]);
            }
        } else if (col1 == col2) {
            if (encrypt) {
                result.push_back(matrix[(row1 + 1) % MATRIX_SIZE][col1]);
                result.push_back(matrix[(row2 + 1) % MATRIX_SIZE][col2]);
            } else {
                result.push_back(matrix[(row1 - 1 + MATRIX_SIZE) % MATRIX_SIZE][col1]);
                result.push_back(matrix[(row2 - 1 + MATRIX_SIZE) % MATRIX_SIZE][col2]);
            }
        } else {
            result.push_back(matrix[row1][col2]);
            result.push_back(matrix[row2][col1]);
        }
    }
    
    if (data.size() % 2 != 0) {
        result.push_back(data.back());
    }
    
    return result;
}
