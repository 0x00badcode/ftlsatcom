#include "KeyManager.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <sodium.h>


void KeyManager::generate_keys() {
    generate_master_key();
    derive_subkeys();
    generate_source_identifier();
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
    crypto_kdf_derive_from_key(packet_id_key, sizeof(packet_id_key), 3, "PACKET_ID_KEY", master_key);
}

void KeyManager::clear_keys() {
    sodium_memzero(master_key, sizeof(master_key));
    sodium_memzero(encryption_key, sizeof(encryption_key));
    sodium_memzero(iv, sizeof(iv));
    sodium_memzero(password, sizeof(password));
    sodium_memzero(packet_id_key, sizeof(packet_id_key));
}

void KeyManager::generate_source_identifier() {
    const size_t id_size = 16;
    std::vector<unsigned char> id(id_size);
    randombytes_buf(id.data(), id_size);
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < id.size(); i++) {
        ss << std::setw(2) << static_cast<unsigned>(id[i]);
    }
    std::cout << "Source Identifier: " << ss.str() << std::endl;
    source_identifier = ss.str();
}