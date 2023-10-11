#pragma once
#include <sodium.h>
#include <cstring>

class KeyManager {
    public:
        void generate_keys();
        void generate_master_key();
        void derive_subkeys();
        void clear_keys();

        // // Getters
        // const unsigned char* get_master_key() const { return master_key; }
        // const unsigned char* get_encryption_key() const { return encryption_key; }
        // const unsigned char* get_iv() const { return iv; }
        // const unsigned char* get_password() const { return password; }

        // // Setters
        // void set_master_key(const unsigned char* new_master_key) { memcpy(master_key, new_master_key, crypto_kdf_KEYBYTES); }
        // void set_encryption_key(const unsigned char* new_encryption_key) { memcpy(encryption_key, new_encryption_key, crypto_secretstream_xchacha20poly1305_KEYBYTES); }
        // void set_iv(const unsigned char* new_iv) { memcpy(iv, new_iv, crypto_secretstream_xchacha20poly1305_HEADERBYTES); }
        // void set_password(const unsigned char* new_password) { memcpy(password, new_password, 256); }

        unsigned char master_key[crypto_kdf_KEYBYTES];
        unsigned char encryption_key[crypto_secretstream_xchacha20poly1305_KEYBYTES];
        unsigned char iv[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
        unsigned char password[256];
        unsigned char pw_salt[crypto_pwhash_SALTBYTES] = { 0x7a, 0x1c, 0x5c, 0x5d, 0x0e, 0x5b, 0x1f, 0x3c, 0x9c, 0x2c, 0x1a, 0x4d, 0x4e, 0x9b, 0x1f, 0x3d };
    // private:
};

