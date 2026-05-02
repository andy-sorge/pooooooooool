#pragma once

#include <atomic>
#include <memory>
#include <stdexcept>
#include <thread>
#include <queue>
#include <iostream>

#include <SFML/Network.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/Socket.hpp>
#include <SFML/Network/SocketSelector.hpp>

#include "Networking.hpp"

class Server {
public:
    Server(std::uint16_t port) : _port(port) {}

    ~Server() {
        stop();
    }

    void start() {
        _running = true;
        _worker = std::thread{&Server::execute, this};
        _listen = std::thread{&Server::listen, this};
    }

    void stop() {
        _running = false;
        if (_worker.joinable()) _worker.join();
        if (_listen.joinable()) _listen.join();
        _listener.close();
    }

    void send(const sf::Packet& packet) {
        // std::cout << "got a packet to queue" << std::endl;
        auto outgoing = _outgoing.lock();
        outgoing->push(packet);
    }
protected:
    virtual void onReceived(sf::Packet& packet) = 0;
    virtual void onConnection(sf::TcpSocket& connection) = 0;
private:
    sf::TcpListener _listener;
    std::thread _listen;
    std::thread _worker;

    std::atomic_bool _running = false;
    std::atomic_uint16_t _port;

    Synchronized<std::vector<std::unique_ptr<sf::TcpSocket>>> _connections;
    Synchronized<std::queue<sf::Packet>> _outgoing;

    void execute() {
        sf::Packet recieved;
        while (_running) {
            auto connections = _connections.lock();
            for (auto& connection : *connections) {
                auto status = sf::Socket::Status::NotReady;
                do status = connection->receive(recieved);
                while (status == sf::Socket::Status::Partial);
                if (status == sf::Socket::Status::Done) onReceived(recieved);
            }

            auto outgoing = _outgoing.lock();
            while (!outgoing->empty()) { // send queued packets to all clients
                // std::cout << "got a packet to send" << std::endl;
                auto packet = outgoing->front();
                for (auto& connection : *connections) while (connection->send(packet) == sf::Socket::Status::Partial); //resend the same packet if the whole thing didn't reach the serber
                outgoing->pop();
            }
        }
    }

    void listen() {
        _listener.setBlocking(false);

        if (_listener.listen(_port) != sf::Socket::Status::Done) throw std::runtime_error("failed to bind");

        while (_running) {
                auto connection = std::make_unique<sf::TcpSocket>();
                if (_listener.accept(*connection) == sf::Socket::Status::Done) {
                    if (!connection) throw std::runtime_error("big bad error, nullptr");
                    connection->setBlocking(false);
                    onConnection(*connection);
                    auto connections = _connections.lock();
                    connections->emplace_back(std::move(connection));
                }
        }

        _listener.close();
    }
};
