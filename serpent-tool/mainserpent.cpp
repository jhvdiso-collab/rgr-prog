#include <iostream>
#include <cstring>
#include "serpentsh.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 5) {
        cout << "Serpent File Encryptor\n\n"
                  << "Использование:\n"
                  << "  Шифрование:   " << argv[0] << " enc <вход> <выход> <пароль>\n"
                  << "  Расшифрование: " << argv[0] << " dec <вход> <выход> <пароль>\n";
        return 1;
    }
    
    try {
        bool encrypt = (strcmp(argv[1], "enc") == 0);
        if (!encrypt && strcmp(argv[1], "dec") != 0) {
            cerr << "Ошибка: укажите 'enc' или 'dec'\n";
            return 1;
        }
        
        process_file(argv[2], argv[3], argv[4], encrypt);
        cout << (encrypt ? "Шифрование" : "Расшифрование") 
                  << " успешно завершено!\n";
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << '\n';
        return 1;
    }
    
    return 0;
}