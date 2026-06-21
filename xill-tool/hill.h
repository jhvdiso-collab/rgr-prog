#ifndef HILL_H
#define HILL_H
#include <cstdint>
#include <vector>
#include <string>

using namespace std;

int mod_inverse(int value);
int determinant(int a, int b, int c, int d);

void process_block(
    uint8_t x1, uint8_t x2,
    uint8_t& y1, uint8_t& y2,
    int a, int b, int c, int d);


vector<uint8_t> process_data(
    const vector<uint8_t>& data,
    int a, int b, int c, int d);


bool inverse_key(
    int a, int b, int c, int d,
    int& ia, int& ib, int& ic, int& id);


vector<uint8_t> read_file(const std::string& filename);
void write_file(const string& filename, const vector<uint8_t>& data);

#endif
