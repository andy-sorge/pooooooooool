#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "Vector.hpp"

#define BALL_RADIUS 50

class Ball : public sf::Sprite {
public:

    enum class Type {
        Stripe,
        Solid,
        Eight,
        Cue
    };

    // Ball number 0 is the cue ball
    Ball(Vector position, Vector velocity, int8_t number, float scale);
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
    sf::Texture tex_;
};

void create_arranged_balls(std::vector<Ball>& balls, float scale);
