#ifndef CRIPTO_H
#define CRIPTO_H
#include <string>
#include <vector>
class Cripto {
public:
    static std::string encriptar(const std::string& texto, const std::vector<unsigned char>& llave);
    static std::string desencriptar(const std::string& textoCifrado, const std::vector<unsigned char>& llave);
};
#endif
