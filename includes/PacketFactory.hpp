#include <string>
#include <vector>
#include <openssl/evp.h>
#include "KeyManager.hpp"

// size in bits
constexpr int PACKET_SIZE = 200;
constexpr int FULL_PACKET_SIZE = 1960;
constexpr int PADDING_SIZE = 8;
constexpr int SYNC_PATTERN_SIZE = 32;
constexpr int SFD_SIZE = 8;
constexpr int CLOCK_SYNC_BITS_SIZE = 64;
constexpr int ADDRESSING_BITS_SIZE = 32;
constexpr int PACKET_TYPE_SIZE = 8;
constexpr int TIMESTAMP_SIZE = 32;
constexpr int SEQUENCE_NUMBER_SIZE = 16;
constexpr int DATA_LENGTH_SIZE = 16;
constexpr int KEY_SIZE = 32;
constexpr int DATA_SIZE = 1600;
constexpr int CHECKSUM_SIZE = 16;
constexpr int ENDING_BITS_SIZE = 24;

class PacketFactory {
    KeyManager keyring;
public:
    explicit PacketFactory(const KeyManager& keyManager) : keyring(keyManager) { }
    ~PacketFactory() { }
    std::vector<std::vector <char> > convertToBinary(const std::string &encodedString);
    std::string generatePacketId();
    std::string addSyncPattern();
    std::string addSFD();
    std::string addClockSyncBits();
    std::string addAddressingBits();
    std::string addPacketHeader();
    std::string addControlInformation(std::string packetType, int dataSize, int sequenceNumber, std::string key, std::string packetId, std::string totalNumberOfPackets);
    std::string addChecksum(std::string data);
    std::string addEndingBits();

    // std::vector<std::string> pack(const std::string &encodedString);
    void pack(const std::string &encodedString);


    std::vector<std::string> decode(const std::vector<char> &packedPacket);
    void printEncodedData(const std::vector<std::vector <char> >& encodedData);
    void printPackets(const std::vector<std::string> &packets);
    void printAndHighlightPacket(const std::string &packet);
};
