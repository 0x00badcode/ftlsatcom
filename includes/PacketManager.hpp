#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>


class PacketManager {
    public:

    struct PacketFragment {
        std::string data;
        int packetID;
        int fragmentID;
        int timestamp;
        std::string sender;
        int checksum;
    };

    struct Packet {
        std::unordered_map<int, std::vector<PacketFragment>> fragments;
        int packetID;
        bool complete;
    };


        void addFragment(const PacketFragment& fragment);
        std::string reassemblePacket(int packetID);
    private:
        std::unordered_map<int, std::vector<PacketFragment>> packetStorage;

};
