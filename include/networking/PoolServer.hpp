#pragma once

#include <functional>
#include <iostream>

#include <SFML/Network/Packet.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include "Server.hpp"
#include "Utilities.hpp"

static sf::Packet packageBalls(std::vector<BallState>& balls) { // package sent by serber
    auto packet = sf::Packet();
    packet << PacketType::GameState << static_cast<uint16_t>(balls.size());
    for (auto& ball : balls) packet << ball;
    return packet;
}

static sf::Packet packageDisplays(std::size_t& displays) { // package sent by server
    auto packet = sf::Packet();
    packet << PacketType::Connection << static_cast<uint16_t>(displays);
    return packet;
}

class PoolServer : public Server {
public:
    PoolServer(std::uint16_t port) : Server(port) {}

    void registerHandle(PacketType type, std::function<void(sf::Packet& packet)> handle) {
        _handler[type] = std::move(handle);
    }

    void registerConnection(std::function<void()> handle) {
        _connection = std::move(handle);
    }
protected:
    virtual void onReceived(sf::Packet& packet) override {
        PacketType type;
        packet >> type;
        _handler[type](packet);
    }

    virtual void onConnection(sf::TcpSocket& connection) override {
        _connection();
    }
private:
    std::unordered_map<PacketType, std::function<void(sf::Packet& packet)>> _handler;
    std::function<void()> _connection;
};
