#include "hill.h"
#include <fstream>
#include <stdexcept>

using namespace std;

vector<uint8_t> read_file(const std::string& filename)
{
    ifstream file(filename, ios::binary);

    if (!file)
        throw runtime_error("Не удалось открыть файл: " + filename);

    return vector<uint8_t>(
        istreambuf_iterator<char>(file),
        istreambuf_iterator<char>());
}

void write_file(const string& filename, const vector<uint8_t>& data)
{
    ofstream file(filename, ios::binary);

    if (!file)
        throw runtime_error("Не удалось создать файл: " + filename);

    file.write(
        reinterpret_cast<const char*>(data.data()),
        data.size());
}
