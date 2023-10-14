#include <string>
#include <vector>
#include <openssl/evp.h>
#include "KeyManager.hpp"

const int PACKET_SIZE = 200;

class PacketFactory {
    KeyManager keyring;
public:
    explicit PacketFactory(const KeyManager& keyManager) : keyring(keyManager) { }
    ~PacketFactory() { }
    std::vector<std::vector<char>> convertToBinary(const std::string &encodedString);
    std::string addSyncPattern();
    std::string addSFD();
    std::string addClockSyncBits();
    std::string addAddressingBits();
    std::string addControlInformation(std::string packetType, std::vector<char> data);


    // std::vector<std::string> pack(const std::string &encodedString);
    void pack(const std::string &encodedString);


    std::string decode(const std::vector<char>& packedPacket);
    void printEncodedData(const std::vector<std::vector<char>>& encodedData);

};
