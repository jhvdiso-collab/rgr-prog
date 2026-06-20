#ifndef PLAYFAIR_H
#define PLAYFAIR_H

#include <vector>
#include <string>

using namespace std;

const int MATRIX_SIZE = 16;
const int BYTE_RANGE = 256;

vector<unsigned char> readFile(const string& filename);
void writeFile(const string& filename, const vector<unsigned char>& data);
void generateMatrix(const vector<unsigned char>& key, 
                    unsigned char matrix[MATRIX_SIZE][MATRIX_SIZE]);
void findPosition(unsigned char matrix[MATRIX_SIZE][MATRIX_SIZE], 
                  unsigned char c, int& row, int& col);
vector<unsigned char> encryptDecrypt(const vector<unsigned char>& data,
                                     const vector<unsigned char>& key,
                                     bool encrypt);
#endif
