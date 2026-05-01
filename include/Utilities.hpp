#pragma once

#include <cstdint>

#include "Ball.hpp"

namespace PoolConstants {
    constexpr static float pocketRadius = 100.0f;
    constexpr static std::uint16_t port = 58009;
}

enum Role {
    Host,
    Client
};

enum PlayerTurn {
    None,
    Player1,
    Player2
};

struct State {
    Role role;
    std::uint16_t index;
    std::uint16_t displays;

    std::vector<Ball> balls;
    std::vector<Vector> pockets;
    sf::Vector2u logicalSpace;

    State() : role(Role::Host), displays(1), index(0) {}
};
