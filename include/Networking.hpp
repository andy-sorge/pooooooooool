#pragma once

#include "SFML/Network/Packet.hpp"
#include <mutex>
#include <iostream>

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
    Connection,
    Cue
};

inline sf::Packet& operator<<(sf::Packet& packet, const PacketType& type) {
    return packet << static_cast<uint8_t>(type);
}

inline sf::Packet& operator>>(sf::Packet& packet, PacketType& type) {
    return packet >> type;
}

inline sf::Packet& operator<<(sf::Packet& packet, const Ball& ball) {
    return packet << static_cast<uint8_t>(ball.number) << ball.pos.x << ball.pos.y << ball.vel.x << ball.vel.y;
}

inline sf::Packet& operator>>(sf::Packet& packet, Ball& ball) {
    uint8_t number = 0;
    packet >> number >> ball.pos.x >> ball.pos.y >> ball.vel.x >> ball.vel.y;
    ball.number = static_cast<int8_t>(number);
    return packet;
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

    std::vector<Ball> incoming;
    incoming.reserve(size);

    for (std::size_t i = 0; i < size; ++i) {
        uint8_t number = 0;
        unit_t px = 0.0;
        unit_t py = 0.0;
        unit_t vx = 0.0;
        unit_t vy = 0.0;
        packet >> number >> px >> py >> vx >> vy;
        incoming.emplace_back(Vector{px, py}, Vector{vx, vy}, number);
    }

    balls = std::move(incoming);
}

inline void interpret(sf::Packet& packet, std::uint16_t& displays) { // message sent to client
    //PacketType type; // eats the first byte! cannot do
    //packet >> type;
    //if (type != PacketType::Connection) throw std::runtime_error("wrong packet type");

    packet >> displays;
}


[[nodiscard]] inline sf::Packet package(const Vector& aimDir, float power, bool aiming) {
    auto packet = sf::Packet();
    packet << PacketType::Cue << static_cast<float>(aimDir.x) << static_cast<float>(aimDir.y) << power << aiming;
    return packet;
}

inline void interpret(sf::Packet& packet, Vector& aimDir, float& power, bool& aiming) {
    float dx = 0.0f, dy = 0.0f;
    packet >> dx >> dy >> power >> aiming;
    aimDir.x = dx;
    aimDir.y = dy;
}
