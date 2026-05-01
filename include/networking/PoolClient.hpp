#pragma once

#include <functional>

#include "SFML/Network/Packet.hpp"

#include "Client.hpp"
#include "Utilities.hpp"
#include <unordered_map>

static inline void interpretBalls(sf::Packet& packet, std::vector<BallState>& balls) { // message sent to packet
    PacketType state;
    packet >> state;
    if (state != PacketType::GameState) throw std::runtime_error("wrong packet type");

    std::uint16_t size;
    packet >> size;

    balls.clear();
    balls.reserve(size);
    for (std::size_t i = 0; i < size; ++i) packet >> balls[i];
}

static inline void interpretDisplays(sf::Packet& packet, std::size_t& displays) { // message sent to client
    PacketType state;
    packet >> state;
    if (state != PacketType::Connection) throw std::runtime_error("wrong packet type");

    uint16_t display_num = 1;
    packet >> display_num;
    displays = display_num;
}

class PoolClient : public Client {
public:
    void registerHandle(PacketType type, std::function<void(sf::Packet& packet)> handle) {
        _handler[type] = std::move(handle);
    }
protected:
    void onReceived(sf::Packet& packet) override {
        PacketType type;
        packet >> type;
        _handler[type](packet);
    }
private:
    std::unordered_map<PacketType, std::function<void(sf::Packet& packet)>> _handler;
};
