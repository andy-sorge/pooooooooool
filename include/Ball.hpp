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
    Ball(Vector position, Vector velocity, int8_t number, float scale);
    ~Ball() override;

    sf::Clock ball_hit_sound_cooldown;

    Vector pos;
    Vector vel;
    unit_t radius;
    unit_t mass;
    int8_t number;
    BallType type;

    void hit(Ball& rhs, double dt);
    [[nodiscard]] Vector friction() const;
    void tick_physics(double dt);
    // void setCenter(sf::Vector2f pos);

private:
    sf::Texture tex_;
};

void create_arranged_balls(std::vector<Ball>& balls, float scale);
