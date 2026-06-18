#include <iostream>
#include <cstdint>

using namespace std;

void process_block(
    uint8_t x1, uint8_t x2,
    uint8_t& y1, uint8_t& y2,
    int a, int b, int c, int d)
{
    y1 = (a * x1 + b * x2) % 256;
    y2 = (c * x1 + d * x2) % 256;
}

int main()
{
    uint8_t r1, r2;

    process_block(
        'H', 'I',
        r1, r2,
        3, 5, 2, 7);

    cout << (int)r1 << " "
         << (int)r2 << endl;
}
