#pragma once
#include <sodium.h>
#include <cstring>
#include <string>

class KeyManager {
    public:
        void generate_keys();
        void generate_master_key();
        void derive_subkeys();
        void clear_keys();
        void generate_source_identifier();
        unsigned char master_key[crypto_kdf_KEYBYTES];
        unsigned char encryption_key[crypto_secretstream_xchacha20poly1305_KEYBYTES];
        unsigned char iv[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
        unsigned char password[256];
        unsigned char pw_salt[crypto_pwhash_SALTBYTES] = { 0x7a, 0x1c, 0x5c, 0x5d, 0x0e, 0x5b, 0x1f, 0x3c, 0x9c, 0x2c, 0x1a, 0x4d, 0x4e, 0x9b, 0x1f, 0x3d };
        unsigned char packet_id_key[crypto_kdf_KEYBYTES];
        std::string source_identifier;
};

