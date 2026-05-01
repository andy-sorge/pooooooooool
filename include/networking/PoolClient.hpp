#pragma once

#include <functional>
#include <unordered_map>
#include <iostream>

#include "SFML/Network/Packet.hpp"

#include "Client.hpp"
#include "Networking.hpp"

class PoolClient : public Client {
public:
    void registerHandle(PacketType type, std::function<void(sf::Packet& packet)> handle) {
        _handler[type] = std::move(handle);
    }
protected:
    void onReceived(sf::Packet& packet) override {
        std::cout << "recieved packet!" << std::endl;

        uint8_t type;
        //PacketType type;
        packet >> type;
        auto it = _handler.find(static_cast<PacketType>(type));
        if (it != _handler.end() && it->second) it->second(packet);
        else std::cerr << "no on received callback for packet type!\n"; // WHAT THE HELL THE NAMES OF THESE VARIABLES AND ORDER OF THINGS DETERMINE WHETHER IT WORKS OR NOT, DO NOT CHANGE
    }
private:
    std::unordered_map<PacketType, std::function<void(sf::Packet& packet)>> _handler;
};
