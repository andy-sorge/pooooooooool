#pragma once

#include <atomic>
#include <thread>
#include <string>
#include <queue>
#include <iostream>

#include <SFML/Network.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/Socket.hpp>
#include <SFML/Network/SocketSelector.hpp>

#include "Utilities.hpp"

class Client {
public:
    ~Client() {
        stop();
    }

    void connect(const std::string& address, unsigned short port) {
        auto result = sf::IpAddress::resolve(address);
        if (!result.has_value()) throw std::runtime_error("host address did not resolve");

        _running = true;
        _worker = std::thread{&Client::execute, this, result.value(), port};
    }

    void stop() {
        _running = false;
        _worker.join();
    }

    void send(const sf::Packet& packet) {
        auto outgoing = _outgoing.lock();
        outgoing->push(packet);
    }

    bool isConnected() const {
        return _connected.load();
    }
protected:
    virtual void onReceived(sf::Packet& packet) = 0;
private:
    sf::TcpSocket _client;
    std::thread _worker;

    std::atomic_bool _running = false;
    std::atomic_bool _connected = false;

    Synchronized<std::queue<sf::Packet>> _outgoing;

    void execute(sf::IpAddress address, unsigned short port) {
        auto status = _client.connect(address, port);
        if (status != sf::Socket::Status::Done) {
            std::cout << "did not go" << std::endl;
            return;
        }

        _client.setBlocking(false);

        _connected = true;

        sf::Packet recieved;
        while (_running) {
            // check for incoming packets
            if (_client.receive(recieved) != sf::Socket::Status::NotReady) onReceived(recieved);

            auto outgoing = _outgoing.lock();
            while (!outgoing->empty()) { // sends queued packets to server
                auto packet = outgoing->front();
                std::size_t sent = 0;
                while (_client.send(packet) == sf::Socket::Status::Partial); //resend the same packet if the whole thing didn't reach the serber
                outgoing->pop();
            }
        }
    }
};
