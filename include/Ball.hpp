#pragma once

#include "SFML/Graphics.hpp"
#include "Physics.hpp"
#include <SFML/Graphics/Texture.hpp>
#include "Textures.hpp"

#define BALL_RADIUS 50

enum class BallType {
    Stripe,
    Solid,
    Eight,
    Cue
};

class Ball : public sf::Sprite {
public:
    // Ball number 0 is the cue ball
    Ball(Vector position, Vector velocity, int8_t number);
    ~Ball() override;

    Vector pos;
    Vector vel;
    unit_t radius;
    unit_t mass;
    int8_t number;
    BallType type;

    void hit(Ball& rhs);
    [[nodiscard]] Vector friction() const;
    void tick_physics(double dt);

private:
    sf::Texture tex_;
    void setCenter(sf::Vector2f pos);
};

void create_arranged_balls(std::vector<Ball>& balls);
