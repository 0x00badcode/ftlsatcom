#pragma once
#include <string>
#include <openssl/evp.h>
#include "KeyManager.hpp"

class CryptoManager {
public:
    CryptoManager(const KeyManager& keyManager);
    ~CryptoManager();
    std::string encrypt(const std::string& plaintext);
    std::string decrypt(const std::string& ciphertext);
private:
    KeyManager keyManager;
    EVP_CIPHER_CTX* ctx_enc;
    EVP_CIPHER_CTX* ctx_dec;
};
