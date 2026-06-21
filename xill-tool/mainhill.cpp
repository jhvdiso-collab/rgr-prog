#include <iostream>
#include <string>
#include <vector>
#include "hill.h"

using namespace std;

int main()
{
    int a, b, c, d;

    cout << "Введите ключ (a b c d): ";
    cin >> a >> b >> c >> d;

    int ia, ib, ic, id;

    if (!inverse_key(a, b, c, d, ia, ib, ic, id))
    {
        cout << "Ключ не подходит (определитель необратим по модулю 256)\n";
        return 1;
    }

    int mode;

    cout << "1 - Шифровать текст\n";
    cout << "2 - Шифровать файл\n";
    cout << "Выбор: ";

    cin >> mode;
    cin.ignore();

    if (mode == 1)
    {
        string text;

        cout << "Введите текст: ";
        getline(cin, text);

        vector<uint8_t> data(text.begin(), text.end());

        vector<uint8_t> encrypted = process_data(data, a, b, c, d);
        vector<uint8_t> decrypted = process_data(encrypted, ia, ib, ic, id);

        cout << "\nЗашифровано (байты):\n";
        for (auto x : encrypted)
            cout << (int)x << " ";

        cout << "\n\nРасшифровано:\n";
        for (auto x : decrypted)
        {
            if (x != 0)  
                cout << (char)x;
        }
        cout << endl;
    }
    else if (mode == 2)
    {
        string filename;

        cout << "Введите путь к файлу: ";
        getline(cin, filename);

        try
        {
            vector<uint8_t> data = read_file(filename);

            vector<uint8_t> encrypted = process_data(data, a, b, c, d);

            string encrypted_name = filename + ".enc";
            write_file(encrypted_name, encrypted);

            cout << "\nФайл зашифрован: " << encrypted_name << endl;

            vector<uint8_t> decrypted = process_data(encrypted, ia, ib, ic, id);

            string decrypted_name = filename + ".dec";
            write_file(decrypted_name, decrypted);

            cout << "Файл расшифрован: " << decrypted_name << endl;
        }
        catch (const exception& e)
        {
            cout << "Ошибка: " << e.what() << endl;
        }
    }
    else
    {
        cout << "Неверный выбор\n";
    }

    return 0;
}
