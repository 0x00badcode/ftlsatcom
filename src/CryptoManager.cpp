#include "CryptoManager.hpp"
#include <openssl/conf.h>
#include <openssl/err.h>
#include <string.h>
#include <iostream>

CryptoManager::CryptoManager(const KeyManager& keyManager)
    : keyManager(keyManager) {
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    ctx_enc = EVP_CIPHER_CTX_new();
    ctx_dec = EVP_CIPHER_CTX_new();
}

CryptoManager::~CryptoManager() {
    EVP_CIPHER_CTX_free(ctx_enc);
    EVP_CIPHER_CTX_free(ctx_dec);
    ERR_free_strings();
    EVP_cleanup();
}

std::string CryptoManager::encrypt(const std::string& plaintext) {
    int len;
    int ciphertext_len;

    int block_size = EVP_CIPHER_block_size(EVP_aes_256_cbc());

    std::vector<unsigned char> ciphertext(plaintext.length() + block_size);

    if (1 != EVP_EncryptInit_ex(ctx_enc, EVP_aes_256_cbc(), NULL, keyManager.encryption_key, keyManager.iv)) {
        std::cerr << "EVP_EncryptInit_ex() failed" << std::endl;
    }

    if (1 != EVP_EncryptUpdate(ctx_enc, ciphertext.data(), &len, reinterpret_cast<const unsigned char*>(plaintext.c_str()), plaintext.length())) {
        std::cerr << "EVP_EncryptUpdate() failed" << std::endl;
    }
    ciphertext_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx_enc, ciphertext.data() + len, &len)) {
        std::cerr << "EVP_EncryptFinal_ex() failed" << std::endl;
    }
    ciphertext_len += len;

    return std::string(ciphertext.begin(), ciphertext.begin() + ciphertext_len);
}

std::string CryptoManager::decrypt(const std::string& ciphertext) {
    int len;
    int plaintext_len;

    std::vector<unsigned char> plaintext(ciphertext.length());

    if (1 != EVP_DecryptInit_ex(ctx_dec, EVP_aes_256_cbc(), NULL, keyManager.encryption_key, keyManager.iv)) {
        std::cerr << "EVP_DecryptInit_ex() failed" << std::endl;
    }

    if (1 != EVP_DecryptUpdate(ctx_dec, plaintext.data(), &len, reinterpret_cast<const unsigned char*>(ciphertext.c_str()), ciphertext.length())) {
        std::cerr << "EVP_DecryptUpdate() failed" << std::endl;
    }
    plaintext_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx_dec, plaintext.data() + len, &len)) {
        std::cerr << "EVP_DecryptFinal_ex() failed" << std::endl;
    }
    plaintext_len += len;

    return std::string(plaintext.begin(), plaintext.begin() + plaintext_len);
}