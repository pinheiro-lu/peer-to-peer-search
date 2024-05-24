#include <iostream>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <endereco>:<porta> [vizinhos.txt [lista_chave_valor.txt]]" << std::endl;
        return 1;
    }
    return 0;
}