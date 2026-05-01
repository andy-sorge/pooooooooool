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

#include "Utilities.hpp"

class Server {
public:
    Server() = default;

    ~Server() {
        stop();
    }

    void start(unsigned short port) {
        _running = true;
        _worker = std::thread{&Server::execute, this, port};
    }

    void stop() {
        _running = false;
        if (_worker.joinable()) _worker.join();
    }

    void send(const sf::Packet& packet) {
        auto outgoing = _outgoing.lock();
        outgoing->push(packet);
    }
protected:
    virtual void onReceived(sf::Packet& packet) = 0;
    virtual void onConnection(sf::TcpSocket& connection) = 0;
private:
    sf::TcpListener _listener;
    std::thread _worker;

    std::atomic_bool _running = false;

    Synchronized<std::vector<std::unique_ptr<sf::TcpSocket>>> _connections;
    Synchronized<std::queue<sf::Packet>> _outgoing;

    void execute(unsigned short port) {
        _listener.setBlocking(true);

        sf::Packet recieved;
        while (_running) {
            //std::cout << static_cast<int>(_listener.listen(port)) << std::endl;
            auto listenStatus = _listener.listen(port);
            if (listenStatus != sf::Socket::Status::Done) throw std::runtime_error("fml");
            if (listenStatus == sf::Socket::Status::Done) { // connects clients
                sf::TcpSocket connection;
                auto acceptStatus = _listener.accept(connection);
                if (acceptStatus != sf::Socket::Status::NotReady) throw std::runtime_error("help me");
                // std::unique_ptr<sf::TcpSocket> connection;
                // if (connection == nullptr) std::cout << "nullptr!" << std::endl;
                // auto status = _listener.accept(*connection);
                // std::cout << static_cast<int>(status);
                // if (status == sf::Socket::Status::Done) {
                //     if (!connection) throw std::runtime_error("big bad error, nullptr");
                //     connection->setBlocking(false);
                //     if (connection->getRemoteAddress().has_value()) std::cout << "i have remote address!";
                //     onConnection(*connection);
                //     auto connections = _connections.lock();
                //     connections->emplace_back(std::move(connection));
                // }
            }

            auto connections = _connections.lock();
            for (auto& connection : *connections) if (connection->receive(recieved) != sf::Socket::Status::NotReady) onReceived(recieved);

            auto outgoing = _outgoing.lock();
            while (!outgoing->empty()) { // send queued packets to all clients
                auto packet = outgoing->front();
                std::size_t sent = 0;
                for (auto& connection : *connections) while (connection->send(packet) == sf::Socket::Status::Partial); //resend the same packet if the whole thing didn't reach the serber
                outgoing->pop();
            }
        }
    }
};
