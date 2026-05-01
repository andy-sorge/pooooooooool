#pragma once

#include <functional>

#include <SFML/Network/Packet.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include "Server.hpp"
#include "Networking.hpp"

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
        std::cout << "recieved packet!" << std::endl;

        uint8_t type;
        //PacketType type;
        packet >> type;
        auto handle = _handler.find(static_cast<PacketType>(type));
        if (handle != _handler.end() && handle->second) handle->second(packet);
        else std::cerr << "no on received callback for packet type!\n";
    }

    virtual void onConnection(sf::TcpSocket& connection) override {
        if (_connection) _connection();
    }
private:
    std::unordered_map<PacketType, std::function<void(sf::Packet& packet)>> _handler;
    std::function<void()> _connection;
};
