#pragma once

#include <atomic>
#include <cstdint>
#include <stdexcept>
#include <thread>
#include <string>
#include <queue>
#include <iostream>

#include <SFML/Network.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/Socket.hpp>
#include <SFML/Network/SocketSelector.hpp>

#include "Networking.hpp"

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
        std::cout << "got a packet to queue" << std::endl;
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
            recieved.clear();
            // check for incoming packets
            //auto status = _client.receive(recieved);
            //if (status == sf::Socket::Status::Done) onReceived(recieved);
            //else if (status == sf::Socket::Status::Disconnected || status == sf::Socket::Status::Error ) throw std::runtime_error("disconnect or error");

            auto status = sf::Socket::Status::NotReady;
            do status = _client.receive(recieved);
            while (status == sf::Socket::Status::Partial);
            if (status == sf::Socket::Status::Done) onReceived(recieved);

            auto outgoing = _outgoing.lock();
            while (!outgoing->empty()) { // sends queued packets to server
                std::cout << "got a packet to send" << std::endl;
                auto& packet = outgoing->front();
                while (_client.send(packet) == sf::Socket::Status::Partial); //resend the same packet if the whole thing didn't reach the serber
                outgoing->pop();
            }

            //sf::sleep(sf::milliseconds(1));
        }
    }
};
