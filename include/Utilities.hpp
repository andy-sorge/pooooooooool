#pragma once

#include <mutex>
#include <cstdint>

#include "SFML/Network/Packet.hpp"

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

/*
 * connection packet breakdown
 * [uint8_t - number of clients]
 */

enum class PacketType : uint8_t {
    GameState,
    Connection
};

inline sf::Packet& operator<<(sf::Packet& packet, const PacketType& type) {
    return packet << static_cast<uint8_t>(type);
}

inline sf::Packet& operator>>(sf::Packet& packet, PacketType& type) {
    return packet >> type;
}

struct BallState {
    std::int8_t number;
    double x;
    double y;
    double vx;
    double vy;
};

inline sf::Packet& operator<<(sf::Packet& packet, const BallState& state) {
    return packet << state.number << state.x << state.y << state.vx << state.vy;
}

inline sf::Packet& operator>>(sf::Packet& packet, BallState& state) {
    return packet >> state.number >> state.x >> state.y >> state.vx >> state.vy;
}
