#pragma once

#include "SFML/Network/Packet.hpp"
#include <mutex>

#include "Ball.hpp"
#include "Vector.hpp"

template <typename T>
class Synchronized {
public:
    template <typename... Args>
    explicit Synchronized(Args&& ...args) : _value(std::forward<Args>(args)...) {}

    class Guard {
    public:
        Guard(T& reference, std::mutex& mutex) : _reference(reference), _lock(mutex) {}

        T& operator*() {
            return _reference;
        }

        T* operator->() {
            return &_reference;
        }
    private:
        T& _reference;
        std::lock_guard<std::mutex> _lock;
    };

    [[nodiscard]] Guard lock() {
        return Guard(_value, _mutex);
    }
private:
    mutable std::mutex _mutex;
    T _value;
};

enum class PacketType : uint8_t {
    Balls,
    Connection
};

inline sf::Packet& operator<<(sf::Packet& packet, const PacketType& type) {
    return packet << static_cast<uint8_t>(type);
}

inline sf::Packet& operator>>(sf::Packet& packet, PacketType& type) {
    return packet >> type;
}

inline sf::Packet& operator<<(sf::Packet& packet, const Ball& ball) {
    return packet << static_cast<uint16_t>(ball.number) << ball.pos.x << ball.pos.y << ball.vel.x << ball.vel.y;
}

inline sf::Packet& operator>>(sf::Packet& packet, Ball& ball) {
    return packet >> ball.number >> ball.pos.x >> ball.pos.y >> ball.vel.x >> ball.vel.y;
}

[[nodiscard]] inline sf::Packet package(std::vector<Ball>& balls) { // package sent by serber
    auto packet = sf::Packet();
    packet << PacketType::Balls << static_cast<uint16_t>(balls.size());
    for (auto& ball : balls) packet << ball;
    return packet;
}

[[nodiscard]] inline sf::Packet package(std::uint16_t& displays) { // package sent by server
    auto packet = sf::Packet();
    packet << PacketType::Connection << static_cast<uint16_t>(displays);
    return packet;
}

inline void interpret(sf::Packet& packet, std::vector<Ball>& balls) { // message sent to packet
    //PacketType type; // eats the first byte! cannot do
    //packet >> type;
    //if (type != PacketType::Balls) throw std::runtime_error("wrong packet type");

    std::uint16_t size;
    packet >> size;

    if (balls.size() != size) balls.reserve(size);
    for (std::size_t i = 0; i < size; ++i) packet >> balls[i];
}

inline void interpret(sf::Packet& packet, std::uint16_t& displays) { // message sent to client
    //PacketType type; // eats the first byte! cannot do
    //packet >> type;
    //if (type != PacketType::Connection) throw std::runtime_error("wrong packet type");

    packet >> displays;
}
