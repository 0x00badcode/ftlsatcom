#include <string>
#include <vector>
#include <bitset>
#include <algorithm>
#include "PacketFactory.hpp"
#include <iostream>

std::vector<std::vector<char>> PacketFactory::convertToBinary(const std::string &encodedString)
{
    std::vector<std::vector<char>> encodedData;
    std::vector<char> encodedChar;
    std::bitset<8> bits;
    for (auto c : encodedString)
    {
        bits = std::bitset<8>(c);
        for (int i = 0; i < 8; i++) {
            encodedChar.push_back(bits[i]);
        }
        encodedData.push_back(encodedChar);
        encodedChar.clear();
        // std::cout << c << " = " << bits << std::endl;
    }
    return encodedData;
}

void PacketFactory::pack(const std::string &encodedString)
{
    std::vector<std::vector<char>>  encodedData = convertToBinary(encodedString);
    printEncodedData(encodedData);
}

std::string PacketFactory::decode(const std::vector<char> &packedPacket)
{
    return nullptr;
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