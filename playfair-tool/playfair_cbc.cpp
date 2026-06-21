#include "playfair.h"
#include <iostream>
#include <fstream>

using namespace std;

vector<unsigned char> readFile(const string& filename) {
    ifstream file(filename, ios::binary | ios::ate);
    if (!file.is_open()) {
        cerr << "Ошибка открытия файла: " << filename << endl;
        exit(1);
    }
    streamsize size = file.tellg();
    file.seekg(0, ios::beg);
    vector<unsigned char> buffer(size);
    file.read(reinterpret_cast<char*>(buffer.data()), size);
    file.close();
    return buffer;
}

void writeFile(const string& filename, const vector<unsigned char>& data) {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Ошибка создания файла: " << filename << endl;
        exit(1);
    }
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    file.close();
}
