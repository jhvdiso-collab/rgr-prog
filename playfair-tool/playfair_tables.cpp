#include "playfair.h"

void generateMatrix(const vector<unsigned char>& key, 
                    unsigned char matrix[MATRIX_SIZE][MATRIX_SIZE]) {
    vector<unsigned char> allBytes;
    for (int i = 0; i < BYTE_RANGE; i++) {
        allBytes.push_back(static_cast<unsigned char>(i));
    }
    
    vector<unsigned char> keyBytes;
    bool seen[BYTE_RANGE] = {false};
    
    for (unsigned char c : key) {
        if (!seen[c]) {
            keyBytes.push_back(c);
            seen[c] = true;
        }
    }
    
    for (unsigned char c : allBytes) {
        if (!seen[c]) {
            keyBytes.push_back(c);
            seen[c] = true;
        }
    }
    
    int idx = 0;
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            matrix[i][j] = keyBytes[idx++];
        }
    }
}

void findPosition(unsigned char matrix[MATRIX_SIZE][MATRIX_SIZE], 
                  unsigned char c, int& row, int& col) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            if (matrix[i][j] == c) {
                row = i;
                col = j;
                return;
            }
        }
    }
    row = -1;
    col = -1;
}
