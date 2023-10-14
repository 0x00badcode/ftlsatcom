#include <string>
#include <vector>
#include <openssl/evp.h>
#include "KeyManager.hpp"

const int PACKET_SIZE = 200;

class PacketFactory {
    KeyManager KeyRing;
public:
    explicit PacketFactory(const KeyManager& keyManager) : KeyRing(keyManager) { }
    ~PacketFactory() { }
    std::vector<std::vector<char>> convertToBinary(const std::string &encodedString);

    // std::vector<std::string> pack(const std::string &encodedString);

    void pack(const std::string &encodedString);

    std::string decode(const std::vector<char>& packedPacket);
    void printEncodedData(const std::vector<std::vector<char>>& encodedData);

};
