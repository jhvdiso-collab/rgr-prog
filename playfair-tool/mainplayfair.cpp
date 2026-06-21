#include "playfair.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 5) {
        cout << "Использование:" << endl;
        cout << "  encrypt <входной_файл> <выходной_файл> <ключ>" << endl;
        cout << "  decrypt <входной_файл> <выходной_файл> <ключ>" << endl;
        cout << endl;
        cout << "Пример:" << endl;
        cout << "  ./playfair encrypt test.txt encrypted.bin mykey" << endl;
        cout << "  ./playfair decrypt encrypted.bin decrypted.txt mykey" << endl;
        return 1;
    }
    
    string mode = argv[1];
    string inputFile = argv[2];
    string outputFile = argv[3];
    string keyStr = argv[4];
    
    vector<unsigned char> key(keyStr.begin(), keyStr.end());
    vector<unsigned char> data = readFile(inputFile);
    
    cout << "Размер файла: " << data.size() << " байт" << endl;
    cout << "Ключ: " << keyStr << endl;
    
    vector<unsigned char> result;
    
    if (mode == "encrypt") {
        cout << "Шифрование..." << endl;
        result = encryptDecrypt(data, key, true);
        cout << "Файл зашифрован!" << endl;
    } else if (mode == "decrypt") {
        cout << "Дешифрование..." << endl;
        result = encryptDecrypt(data, key, false);
        cout << "Файл расшифрован!" << endl;
    } else {
        cerr << "Неверный режим. Используйте 'encrypt' или 'decrypt'" << endl;
        return 1;
    }
    
    writeFile(outputFile, result);
    cout << "Результат сохранен в: " << outputFile << endl;
    
    return 0;
}
