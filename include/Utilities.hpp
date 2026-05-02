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

enum class PlayerTurn : uint8_t {
    None,
    PlacingCueBall,
    Aiming,
    Physics,
    End
};

struct State {
    Role role;
    std::optional<std::uint16_t> index;
    std::uint16_t displays;

    PlayerTurn turn = PlayerTurn::None;

    std::vector<Ball> balls;
    std::vector<Vector> pockets;
    sf::Vector2u logicalSpace;

    // cue (aim) state shared from host to clients so the cue animates everywhere
    Vector cueDir{0.0, 0.0};
    float cuePower{0.0f};
    bool cueAiming{false};

    // ball placement state
    Vector ballPlacementPos{0.0, 0.0};

    State() : role(Role::Host), displays(1), index(std::nullopt) {}
};
