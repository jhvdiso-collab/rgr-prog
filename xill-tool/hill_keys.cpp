#include "hill.h"

using namespace std;

bool inverse_key(
    int a, int b, int c, int d,
    int& ia, int& ib, int& ic, int& id)
{
    int det = determinant(a, b, c, d);

    det %= 256;

    if (det < 0)
        det += 256;

    int det_inv = mod_inverse(det);

    if (det_inv == -1)
        return false;

 

    ia = ( d * det_inv) % 256;
    ib = ((-b + 256) * det_inv) % 256;
    ic = ((-c + 256) * det_inv) % 256;
    id = ( a * det_inv) % 256;

    if (ia < 0) ia += 256;
    if (ib < 0) ib += 256;
    if (ic < 0) ic += 256;
    if (id < 0) id += 256;

    return true;
}
