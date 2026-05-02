#pragma once

#include <cstdint>
#include <optional>

#include "Ball.hpp"

namespace PoolConstants {
    constexpr static float pocketRadius = 100.0f;
    constexpr static std::uint16_t port = 58009;
}

enum Role {
    Host,
    Client
};

enum class PlayerTurn {
    None,
    PlacingCueBall,
    Aiming,
    Physics,
};

struct State {
    Role role;
    std::optional<std::uint16_t> index;
    std::uint16_t displays;

    PlayerTurn turn;

    std::vector<Ball> balls;
    std::vector<Vector> pockets;
    sf::Vector2u logicalSpace;

    State() : role(Role::Host), displays(1), index(std::nullopt) {}
};
