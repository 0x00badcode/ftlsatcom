#include <string>
#include <vector>
#include <bitset>
#include <algorithm>
#include "PacketFactory.hpp"
#include <iostream>
#include <numeric>
#include <time.h>

#include <iomanip>
#include <sstream>

// ------------------ Utils------------------

std::vector<std::vector<char>> PacketFactory::convertToBinary(const std::string &encodedString)
{
    std::vector<std::vector<char>> encodedData;
    for (auto c : encodedString)
    {
        std::vector<char> encodedChar;
        std::bitset<8> bits(c);
        for (int i = 7; i >= 0; --i)
        {
            encodedChar.push_back(bits[i] + '0');
        }
        encodedData.push_back(std::move(encodedChar));
    }
    return encodedData;
}

std::string stringToBinary(const std::string &input)
{
    std::string output;
    for (char c : input) {
        output += std::bitset<8>(c).to_string();
    }
    return output;
}

std::string PacketFactory::generatePacketId() {
    const size_t id_size = 2; // 2 bytes -> 16 bits
    std::vector<unsigned char> id(id_size);
    randombytes_buf(id.data(), id_size);
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < id.size(); i++) {
        ss << std::setw(2) << static_cast<unsigned>(id[i]);
    }
    return ss.str();
}

//------------------ PACKET HEADER ------------------

std::string PacketFactory::addSyncPattern()
{
    std::string syncPattern = "10101010101010101010101010101010";
    return syncPattern;
}

std::string PacketFactory::addSFD()
{
    std::string sfd = "01111110";
    return sfd;
}

std::string PacketFactory::addClockSyncBits()
{
    const int PATTERN_REPEAT_COUNT = 16; // 8 bytes / 0.5 byte = 16
    const std::string CLOCK_SYNC_PATTERN = "1101";

    std::string clockSyncBits = "";
    for(int i = 0; i < PATTERN_REPEAT_COUNT; ++i) {
        clockSyncBits += CLOCK_SYNC_PATTERN;
    }

    return clockSyncBits;
}

std::string PacketFactory::addAddressingBits()
{
    std::string addressingBits = stringToBinary(keyring.source_identifier);
    return addressingBits;
}

std::string PacketFactory::addPacketHeader()
{
    std::string packetHeader = addSyncPattern() + addSFD() + addClockSyncBits() + addAddressingBits();
    return packetHeader;
}

//------------------ PACKET CONTROL INFO ------------------

std::string PacketFactory::addControlInformation(std::string messageType, int dataLength, int sequenceNumber, std::string key, std::string packetId, std::string totalNumberOfPackets)
{
    std::string controlInformation;

    std::string padding = "00000000";

    std::string packetType = (messageType == "ACK") ? "11111111" : "00000000";

    if (dataLength > 200) {
        throw std::runtime_error("Data size is too big");
    }

    std::string timeStamp = std::bitset<32>(time(NULL)).to_string();

    std::string _dataLength = std::bitset<16>(dataLength).to_string();

    std::string _sequenceNumber = std::bitset<16>(sequenceNumber).to_string();

    std::string idKey = stringToBinary(key);
    std::string _packetId = stringToBinary(packetId);

    controlInformation = padding + packetType + timeStamp + _dataLength + _sequenceNumber + totalNumberOfPackets + _packetId + idKey + padding;

    return controlInformation;
}

//------------------ PACKET TAIL ------------------

std::string PacketFactory::addFooter(std::string data)
{
    std::string footer = addChecksum(data) + addEndingBits();
    return footer;
}

std::string PacketFactory::addChecksum(std::string data)
{
    unsigned int checksum = 0;
    for (char c : data) {
        checksum += static_cast<unsigned char>(c);
    }

    std::string binaryChecksum = std::bitset<16>(checksum).to_string();

    return binaryChecksum;
}

std::string PacketFactory::addEndingBits()
{
    std::string endingBits = "000111000111000111000111";
    return endingBits;
}

//------------------ Packing workflow ------------------

void PacketFactory::pack(const std::string &encodedString)
{
    auto binaryData = convertToBinary(encodedString);
    auto packetId = generatePacketId();
    const int dataSize = binaryData.size();
    const int fullPacketCount = dataSize / PACKET_SIZE;
    const int lastPacketSize = dataSize % PACKET_SIZE;
    int totalNumberOfPackets = fullPacketCount + (lastPacketSize > 0 ? 1 : 0);
    auto binaryNumberOfPackets = std::bitset<16>(totalNumberOfPackets).to_string();

    std::vector<std::string> packets;
    packets.reserve(totalNumberOfPackets);

    for (int i = 0; i < totalNumberOfPackets; ++i) {
        int currentPacketSize = (i == fullPacketCount) ? lastPacketSize : PACKET_SIZE;

        std::string packetData;
        for (int j = 0; j < currentPacketSize; ++j) {
            packetData += std::string(binaryData[i * PACKET_SIZE + j].begin(), binaryData[i * PACKET_SIZE + j].end());
        }

        if (i == fullPacketCount && lastPacketSize > 0) {
            packetData.append((PACKET_SIZE - lastPacketSize) * 8, '0');
        }

        std::string header = addPacketHeader();
        std::string controlInfo = addControlInformation("MSG", currentPacketSize, i, keyring.source_identifier, packetId, binaryNumberOfPackets);
        std::string footer = addFooter(packetData);

        printAndHighlightPacket(header + controlInfo + packetData + footer); // debug
        packets.push_back(header + controlInfo + packetData + footer);
    }
}

// ------------------ Unpacking workflow ------------------

std::vector<std::string> PacketFactory::decode(const std::vector<char> &packedPacket)
{
    std::string packet = "";


    return std::vector<std::string>();
}



// ------------------- debug -------------------

void PacketFactory::printEncodedData(const std::vector<std::vector<char>> &encodedData)
{
    for (size_t i = 0; i < encodedData.size(); ++i)
    {
        for (size_t j = 0; j < encodedData[i].size(); ++j)
        {
            std::cout << static_cast<int>(encodedData[i][j]);
        }
        std::cout << " ";
    }
    std::cout << std::endl;
}

void PacketFactory::printPackets(const std::vector<std::string> &packets)
{
    std::cout << "Printing packets: " << std::endl;
    for (size_t i = 0; i < packets.size(); ++i) {
        std::cout << packets[i] << std::endl;
    }
}

void PacketFactory::printAndHighlightPacket(const std::string &packet)
{
    // Define color escape sequences
    const std::string RESET_COLOR = "\033[0m";
    const std::string RED_COLOR = "\033[31m";
    const std::string GREEN_COLOR = "\033[32m";
    const std::string YELLOW_COLOR = "\033[33m";
    const std::string BLUE_COLOR = "\033[34m";
    const std::string MAGENTA_COLOR = "\033[35m";
    const std::string CYAN_COLOR = "\033[36m";

    std::cout << RED_COLOR << "sync pattern," << GREEN_COLOR << "SFD," << YELLOW_COLOR << "clock sync bits," << BLUE_COLOR << "addressing bits," << MAGENTA_COLOR << "padding," << CYAN_COLOR << "packet type," << RED_COLOR << "timestamp," << GREEN_COLOR << "data length," << RED_COLOR << "sequence number," << MAGENTA_COLOR << "total number of packets," << YELLOW_COLOR << "packet ID," << BLUE_COLOR << "key," << MAGENTA_COLOR << "padding," << CYAN_COLOR << "data," << RED_COLOR << "checksum," << GREEN_COLOR << "ending bits" << RESET_COLOR << std::endl;
    try {
         std::cout << "Printing packet: " << std::endl;
        std::cout << RED_COLOR << packet.substr(0, SYNC_PATTERN_SIZE)           // sync pattern (32 bits)
        << GREEN_COLOR << packet.substr(32, SFD_SIZE)                           // SFD (8 bits)
        << YELLOW_COLOR << packet.substr(40, CLOCK_SYNC_BITS_SIZE)              // clock sync bits (64 bits)
        << BLUE_COLOR << packet.substr(104, ADDRESSING_BITS_SIZE)               // addressing bits (64 bits)
        << MAGENTA_COLOR << packet.substr(168, PADDING_SIZE)                    // padding (8 bits)
        << CYAN_COLOR << packet.substr(176, PACKET_TYPE_SIZE)                   // packet type (8 bits)
        << RED_COLOR << packet.substr(184, TIMESTAMP_SIZE)                      // timestamp (32 bits)
        << GREEN_COLOR << packet.substr(216, DATA_LENGTH_SIZE)                  // data length (16 bits)
        << RED_COLOR << packet.substr(232, SEQUENCE_NUMBER_SIZE)                // sequence number (16 bits)
        << MAGENTA_COLOR << packet.substr(248, TOTAL_NUMBER_OF_PACKETS_SIZE)    // total number of packets (16 bits)
        << YELLOW_COLOR << packet.substr(264, PACKET_ID_SIZE)                   // packet ID (16 bits)
        << BLUE_COLOR << packet.substr(296, KEY_SIZE)                           // key (64 bits)
        << MAGENTA_COLOR << packet.substr(360, PADDING_SIZE)                    // padding (8 bits)
        << CYAN_COLOR << packet.substr(368, DATA_SIZE)                          // data (1600 bits)
        << RED_COLOR << packet.substr(1968, CHECKSUM_SIZE)                      // checksum (16 bits)
        << GREEN_COLOR << packet.substr(1984, ENDING_BITS_SIZE)                 // ending bits (24 bits)
        << RESET_COLOR << std::endl;
    } catch (const std::out_of_range &e) {
        std::cerr << "Packet is too short" << std::endl;
    }
}
