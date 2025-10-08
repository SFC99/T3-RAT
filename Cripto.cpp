//Angel David Morales Palomo
#include "Cripto.h"
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
std::string Cripto::encriptar(const std::string& texto, const std::vector<unsigned char>& llave) {
    if (llave.size() != 16) {
        throw std::invalid_argument("La llave debe ser de 16 bytes para AES-128");
    }
    AES_KEY aesKey;
    if (AES_set_encrypt_key(llave.data(), 128, &aesKey) != 0) {
        throw std::runtime_error("Error al configurar clave de encriptación");
    }
    size_t blockSize = AES_BLOCK_SIZE;
    size_t textLen = texto.length();
    size_t paddedLen = ((textLen + blockSize - 1) / blockSize) * blockSize;
    unsigned char paddingValue = paddedLen - textLen;
    std::vector<unsigned char> input(paddedLen);
    std::memcpy(input.data(), texto.c_str(), textLen);
    for (size_t i = textLen; i < paddedLen; i++) {
        input[i] = paddingValue;
    }
    std::vector<unsigned char> output(paddedLen);
    
    for (size_t i = 0; i < paddedLen; i += AES_BLOCK_SIZE) {
        AES_encrypt(input.data() + i, output.data() + i, &aesKey);
    }
    return std::string(output.begin(), output.end());
}
std::string Cripto::desencriptar(const std::string& textoCifrado, const std::vector<unsigned char>& llave) {
    if (llave.size() != 16) {
        throw std::invalid_argument("La llave debe ser de 16 bytes para AES-128");
    }
    if (textoCifrado.length() % AES_BLOCK_SIZE != 0) {
        throw std::invalid_argument("Texto cifrado no tiene longitud múltiplo de bloque AES");
    }
    AES_KEY aesKey;
    if (AES_set_decrypt_key(llave.data(), 128, &aesKey) != 0) {
        throw std::runtime_error("Error al configurar clave de desencriptación");
    }
    std::vector<unsigned char> input(textoCifrado.begin(), textoCifrado.end());
    std::vector<unsigned char> output(input.size());
    for (size_t i = 0; i < input.size(); i += AES_BLOCK_SIZE) {
        AES_decrypt(input.data() + i, output.data() + i, &aesKey);
    }
    
    if (!output.empty()) {
        unsigned char paddingValue = output.back();
        if (paddingValue > 0 && paddingValue <= AES_BLOCK_SIZE) {
            bool validPadding = true;
            for (size_t i = output.size() - paddingValue; i < output.size(); i++) {
                if (output[i] != paddingValue) {
                    validPadding = false;
                    break;
                }
            }
            if (validPadding) {
                output.resize(output.size() - paddingValue);
            }
        }
    }
    
    return std::string(output.begin(), output.end());
}
