#include <string>
#include <vector>
#include <bitset>
#include <algorithm>
#include "PacketFactory.hpp"
#include <iostream>

// -----------------------------------------------------
// ------------------ Packing worklow ------------------
// -----------------------------------------------------

std::vector<std::vector<char>> PacketFactory::convertToBinary(const std::string &encodedString)
{
    std::vector<std::vector<char>> encodedData;
    std::vector<char> encodedChar;
    std::bitset<8> bits;
    for (auto c : encodedString) {
        bits = std::bitset<8>(c);
        for (int i = 0; i < 8; i++) {
            encodedChar.push_back(bits[i]);
        }
        encodedData.push_back(encodedChar);
        encodedChar.clear();
    }
    return encodedData;
}

std::string stringToBinary(const std::string& input)
{
    std::string output;
    for (char c : input) {
        output += std::bitset<8>(c).to_string();
    }
    return output;
}

/**
 * 64 bits ; 8 bytes
 * '10' pattern repeated 32 times
 */
std::string PacketFactory::addSyncPattern() {
    std::string syncPattern = "1010101010101010101010101010101010101010101010101010101010101010";
    return syncPattern;
}

/**
 * Start Frame Delimiter
 * 8 bits ; 1 byte
 * '01111110' pattern
 */
std::string PacketFactory::addSFD() {
    std::string sfd = "01111110";
    return sfd;
}

/**
 * 64 bits ; 8 bytes
 * '1101' pattern repeated 8 times
 */
std::string PacketFactory::addClockSyncBits() {
    std::string pattern = "1101";
    std::string clockSyncBits = pattern + pattern + pattern + pattern + pattern + pattern + pattern + pattern;
    return clockSyncBits;
}

/**
 * 48 bits ; 6 bytes
 * ID of the user the packet is coming from
 */
std::string PacketFactory::addAddressingBits() {
    std::string addressingBits = stringToBinary(keyring.source_identifier);
    return addressingBits;
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
 */
std::string PacketFactory::addControlInformation(std::string packetType, std::vector<char> data) {
    std::string controlInformation = "00000000";
    if (packetType == "ACK") {
        controlInformation = "11111111";
    } else if (packetType == "MESSAGE") {
        controlInformation = "00000000";
    }
    if (data.size() > 200) {
        throw std::runtime_error("Data size is too big");
    }
    // int dataPadding = 200 - data.size();

    return controlInformation;
}

// std::vector<std::string> PacketFactory::pack(const std::string &encodedString)
void PacketFactory::pack(const std::string &encodedString)
{
    std::vector<std::vector<char>>  encodedData = convertToBinary(encodedString);
    //print the size of the encoded data
    std::cout << "Size of encoded data: " << encodedData.size() << std::endl;
    // std::vector<std::string> packets;
}

// ------------------ Unpacking workflow ------------------


//this is just a normal text message i could send, to see if i would need to split stuff at some point because i think just using something regular sized isn't going to work, so i need to test with very big messages just like this one, let's hope i get a big number and am not too dumb to understand how to split it after, we'll see how it goes

std::string PacketFactory::decode(const std::vector<char> &packedPacket)
{
    return "";
}


void PacketFactory::printEncodedData(const std::vector<std::vector<char>>& encodedData)
{
    std::cout << "Printing encoded data: " << std::endl;
    for (size_t i = 0; i < encodedData.size(); ++i) {
        for (size_t j = 0; j < encodedData[i].size(); ++j) {
            std::cout << static_cast<int>(encodedData[i][j]);
        }
        std::cout << " ";
    }
    std::cout << std::endl;
}