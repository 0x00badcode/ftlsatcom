#include "CryptoManager.hpp"
#include "KeyManager.hpp"
#include "PacketFactory.hpp"
// #include <openssl/conf.h>
// #include <openssl/err.h>
#include <string.h>
// #include <sodium.h>
#include <iostream>
#include <string>
#include <random>

std::string get_user_input() {
    std::string input;
    std::cout << "> ";
    std::getline(std::cin, input);
    return input;
}

void chat(CryptoManager& cryptoManager, KeyManager& keyring) {
    std::string message;
    PacketFactory packetFactory(keyring);
    while(true) {
        message = get_user_input();
        if (sodium_memcmp(message.data(), "exit", 4) == 0) { break; }

        std::string encrypted_msg = cryptoManager.encrypt(message);

        //std::cout << "Encrypted message: '" << encrypted_msg << "'" << std::endl;


        packetFactory.pack(encrypted_msg);

        //std::string decrypted_msg = cryptoManager.decrypt(encrypted_msg);
        //std::cout << "Decrypted message: " << decrypted_msg << std::endl;
    }
}

int main() {
    if (sodium_init() == -1) {
        std::cerr << "sodium_init() failed" << std::endl;
        return 1;
    }

    KeyManager keyring;

    std::cout << "Enter password: ";
    std::cin.getline(reinterpret_cast<char*>(keyring.password), sizeof(keyring.password));

    try {
        keyring.generate_keys();
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    CryptoManager cryptoManager(keyring);

    std::cout << "Welcome to the satellite chat!" << std::endl;
    std::cout << "Type 'exit' to leave the chat." << std::endl;

    chat(cryptoManager, keyring);

    keyring.clear_keys();

    return 0;
}
