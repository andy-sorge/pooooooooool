#pragma once

#include <vector>

#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Clock.hpp"

#include "Vector.hpp"

#define BALL_RADIUS 50
#define BALL_SCALE 50

class Ball : public sf::Sprite {
public:
    enum class Type {
        Stripe,
        Solid,
        Eight,
        Cue
    };

    // Ball number 0 is the cue ball
    Ball(Vector position, Vector velocity, uint8_t number);
    ~Ball() override;

    sf::Clock ball_hit_sound_cooldown;

    Vector pos;
    Vector vel;
    unit_t radius;
    unit_t mass;
    int8_t number;
    Type type;

    void hit(Ball& rhs, double dt);
    [[nodiscard]] Vector friction() const;
    void tick_physics(double dt);
    // void setCenter(sf::Vector2f pos);
private:
    sf::Texture texture_;
};

void create_arranged_balls(std::vector<Ball>& balls);
