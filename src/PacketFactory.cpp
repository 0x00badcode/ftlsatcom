#include <string>
#include <vector>
#include <bitset>
#include <algorithm>
#include "PacketFactory.hpp"
#include <iostream>
#include <numeric>
#include <time.h>

// -----------------------------------------------------
// ------------------ Packing worklow ------------------
// -----------------------------------------------------

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

//------------------ Packing workflow ------------------

/**
 * 32 bits ; 4 bytes
 * '10' pattern repeated 16 times
 */
std::string PacketFactory::addSyncPattern()
{
    std::string syncPattern = "10101010101010101010101010101010";
    return syncPattern;
}

/**
 * Start Frame Delimiter
 * 8 bits ; 1 byte
 * '01111110' pattern
 */
std::string PacketFactory::addSFD()
{
    std::string sfd = "01111110";
    return sfd;
}

/**
 * 64 bits ; 8 bytes
 * '1101' pattern repeated 8 times
 */
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

/**
 * 64 bits ; 8 bytes
 * ID of the user the packet is coming from
 */
std::string PacketFactory::addAddressingBits()
{
    std::string addressingBits = stringToBinary(keyring.source_identifier);
    std::cout << "addressingBits: " << addressingBits << std::endl;
    return addressingBits;
}

std::string PacketFactory::addPacketHeader()
{
    std::string packetHeader = addSyncPattern() + addSFD() + addClockSyncBits() + addAddressingBits();
    return packetHeader;
}

/**
 * - padding :
 * 8 bits ; 1 byte
 * 00000000
 * - packet type
 * 8 bits ; 1 byte
 * 00000000 for message packet
 * 11111111 for acknoledgement packet ACK
 * - Timestamp ;
 * 32 bits ; 4 bytes
 * - Sequence Number (16 bits for up to 65,535) ;
 * 16 bits ; 2 bytes
 * - Data Length Field
 * 16 bits ; 2 bytes
 * represents the number of bytes in the data field ("200" -> 200 bytes of data)
 * - KEY ;
 * 32 bits ; 4 bytes
 * an id to identify the packets we can decrypt from the packets we can't
 * - padding :
 * 8 bits ; 1 byte
 */
std::string PacketFactory::addControlInformation(std::string messageType, int dataSize, int seqNumber, std::string key)
{
    std::cout << "seqNumber: " << seqNumber << std::endl;
    std::string controlInformation;

    std::string padding = "00000000";

    std::string packetType = (messageType == "ACK") ? "11111111" : "00000000";

    if (dataSize > 200)
    {
        throw std::runtime_error("Data size is too big");
    }

    std::string timeStamp = std::bitset<32>(time(NULL)).to_string();

    std::string dataLength = std::bitset<16>(dataSize).to_string();

    std::string sequenceNumber = std::bitset<16>(seqNumber).to_string();

    std::string idKey = stringToBinary(key);

    controlInformation = padding + packetType + timeStamp + dataLength + sequenceNumber + idKey + padding;

    return controlInformation;
}




/**
 * 16 bits ; 2 bytes
 * checksum of the data
 */
std::string PacketFactory::addChecksum(std::string data)
{
    unsigned int checksum = 0;
    for (char c : data) {
        checksum += static_cast<unsigned char>(c);
    }

    std::string binaryChecksum = std::bitset<16>(checksum).to_string();

    return binaryChecksum;
}

/**
 * 24 bits ; 3 bytes
 * '000111000111000111000111' pattern
 */
std::string PacketFactory::addEndingBits()
{
    std::string endingBits = "000111000111000111000111";
    return endingBits;
}

// std::vector<std::string> PacketFactory::pack(const std::string &encodedString)
void PacketFactory::pack(const std::string &encodedString)
{
    std::vector<std::vector<char>> binaryData = convertToBinary(encodedString);
    const int dataSize = binaryData.size();
    const int fullPacketCount = dataSize / PACKET_SIZE;
    const int lastPacketSize = dataSize % PACKET_SIZE;

    std::vector<std::string> packets;
    packets.reserve(fullPacketCount + (lastPacketSize > 0 ? 1 : 0));

    for (int i = 0; i < fullPacketCount; ++i)
    {
        std::string packetData;
        for (int j = 0; j < PACKET_SIZE; ++j)
        {
            packetData += std::string(binaryData[i * PACKET_SIZE + j].begin(), binaryData[i * PACKET_SIZE + j].end());
        }
        std::string header = addPacketHeader();
        std::string controlInfo = addControlInformation("MSG", PACKET_SIZE, i, keyring.source_identifier);
        std::string checksum = addChecksum(packetData);
        std::string endingBits = addEndingBits();
        printAndHighlightPacket(header + controlInfo + packetData + checksum + endingBits);
        packets.push_back(header + controlInfo + packetData);
    }

    if (lastPacketSize > 0)
    {
        std::string lastPacketData;
        for (int j = 0; j < lastPacketSize; ++j)
        {
            lastPacketData += std::string(binaryData[fullPacketCount * PACKET_SIZE + j].begin(), binaryData[fullPacketCount * PACKET_SIZE + j].end());
        }

        // Pad the last packet
        lastPacketData.append((PACKET_SIZE - lastPacketSize) * 8, '0');

        std::string header = addPacketHeader();
        std::string controlInfo = addControlInformation("MSG", lastPacketSize, fullPacketCount, keyring.source_identifier);
        std::string checksum = addChecksum(lastPacketData);
        std::string endingBits = addEndingBits();
        printAndHighlightPacket(header + controlInfo + lastPacketData + checksum + endingBits);

        packets.push_back(header + controlInfo + lastPacketData + checksum + endingBits);
    }
}

// ------------------ Unpacking workflow ------------------

std::string PacketFactory::decode(const std::vector<char> &packedPacket)
{
    return "";
}

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
    for (size_t i = 0; i < packets.size(); ++i)
    {
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

    std::cout << RED_COLOR << "Sync pattern," << GREEN_COLOR << "SFD," << YELLOW_COLOR << "clock sync bits," << BLUE_COLOR << "addressing bits," << MAGENTA_COLOR << "padding," << CYAN_COLOR << "packet type," << RED_COLOR << "timestamp," << GREEN_COLOR << "data length," << YELLOW_COLOR << "sequence number," << BLUE_COLOR << "key," << MAGENTA_COLOR << "padding," << CYAN_COLOR << "data," << RED_COLOR << "checksum," << GREEN_COLOR << "ending bits" << RESET_COLOR << std::endl;
    try {
        std::cout << "Printing packet: " << std::endl;
        std::cout << RED_COLOR << packet.substr(0, 32)      // sync pattern (32 bits)
        << GREEN_COLOR << packet.substr(32, 8)    // SFD (8 bits)
        << YELLOW_COLOR << packet.substr(40, 64) // clock sync bits (64 bits)
        << BLUE_COLOR << packet.substr(104, 64)   // addressing bits (64 bits)
        << MAGENTA_COLOR << packet.substr(168, 8) // padding (8 bits)
        << CYAN_COLOR << packet.substr(176, 8)    // packet type (8 bits)
        << RED_COLOR << packet.substr(184, 32)    // timestamp (32 bits)
        << GREEN_COLOR << packet.substr(216, 16)  // sequence number (16 bits)
        << YELLOW_COLOR << packet.substr(232, 16) // data length (16 bits)
        << BLUE_COLOR << packet.substr(248, 64)   // key (64 bits)
        << MAGENTA_COLOR << packet.substr(312, 8) // padding (8 bits)
        << CYAN_COLOR << packet.substr(320, 1600) // data (1600 bits)
        << RED_COLOR << packet.substr(1920, 16)   // checksum (16 bits)
        << GREEN_COLOR << packet.substr(1936, 24) // ending bits (24 bits)
        << RESET_COLOR << std::endl;
    } catch (const std::out_of_range &e) {
        std::cerr << "Packet is too short" << std::endl;
    }
}