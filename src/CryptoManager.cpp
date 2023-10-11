#include "CryptoManager.hpp"
#include <openssl/conf.h>
#include <openssl/err.h>
#include <string.h>

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
    unsigned char ciphertext[128];

    // Initialize AES-256-CBC encryption
    EVP_EncryptInit_ex(ctx_enc, EVP_aes_256_cbc(), NULL,
                       keyManager.encryption_key,
                       keyManager.iv);

    // Encrypt the message
    EVP_EncryptUpdate(ctx_enc, ciphertext, &len, (unsigned char*)plaintext.c_str(), plaintext.length());
    ciphertext_len = len;

    // Finalize the encryption
    EVP_EncryptFinal_ex(ctx_enc, ciphertext + len, &len);
    ciphertext_len += len;

    return std::string((char*)ciphertext, ciphertext_len);
}

std::string CryptoManager::decrypt(const std::string& ciphertext) {
    int len;
    int plaintext_len;
    unsigned char plaintext[128];

    // Initialize AES-256-CBC decryption
    EVP_DecryptInit_ex(ctx_dec, EVP_aes_256_cbc(), NULL,
                       keyManager.encryption_key,
                       keyManager.iv);

    // Decrypt the message
    EVP_DecryptUpdate(ctx_dec, plaintext, &len, (unsigned char*)ciphertext.c_str(), ciphertext.length());
    plaintext_len = len;

    // Finalize the decryption
    EVP_DecryptFinal_ex(ctx_dec, plaintext + len, &len);
    plaintext_len += len;

    return std::string((char*)plaintext, plaintext_len);
}