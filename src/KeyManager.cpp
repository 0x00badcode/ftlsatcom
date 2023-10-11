#include "KeyManager.hpp"
#include <sodium.h>
#include <string.h>
#include <stdexcept>

void KeyManager::generate_keys() {
    generate_master_key();
    derive_subkeys();
}

void KeyManager::generate_master_key() {
    if (
        crypto_pwhash(master_key, sizeof(master_key),
                      reinterpret_cast<const char *>(password), strlen(reinterpret_cast<const char *>(password)),
                      pw_salt,
                      crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE,
                      crypto_pwhash_ALG_DEFAULT) != 0)
    {
        throw std::runtime_error("crypto_pwhash() failed");
    }
}

void KeyManager::derive_subkeys() {
    crypto_kdf_derive_from_key(encryption_key, sizeof(encryption_key), 1, "ENCRYPT_KEY", master_key);
    crypto_kdf_derive_from_key(iv, sizeof(iv), 2, "IV_FOR_AES", master_key);
}

void KeyManager::clear_keys() {
    sodium_memzero(master_key, sizeof(master_key));
    sodium_memzero(encryption_key, sizeof(encryption_key));
    sodium_memzero(iv, sizeof(iv));
    sodium_memzero(password, sizeof(password));
}