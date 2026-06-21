#include "hill.h"

using namespace std;

int mod_inverse(int value)
{
    value %= 256;

    if (value < 0)
        value += 256;

    for (int i = 1; i < 256; i++)
    {
        if ((value * i) % 256 == 1)
            return i;
    }

    return -1;
}

int determinant(int a, int b, int c, int d)
{
    return a * d - b * c;
}

void process_block(
    uint8_t x1, uint8_t x2,
    uint8_t& y1, uint8_t& y2,
    int a, int b, int c, int d)
{
    y1 = (a * x1 + b * x2) % 256;
    y2 = (c * x1 + d * x2) % 256;
}

vector<uint8_t> process_data(
    const vector<uint8_t>& data,
    int a, int b, int c, int d)
{
    vector<uint8_t> result = data;


    if (result.size() % 2 != 0)
        result.push_back(0);

    for (size_t i = 0; i < result.size(); i += 2)
    {
        uint8_t r1, r2;

        process_block(
            result[i], result[i + 1],
            r1, r2,
            a, b, c, d);

        result[i] = r1;
        result[i + 1] = r2;
    }

    return result;
}

