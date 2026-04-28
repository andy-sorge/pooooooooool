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

struct BallTextureHolder {
    sf::Texture tex_;
    BallTextureHolder(int8_t number) {
        std::string fname = "graphics/balls/";
        fname += std::to_string(number);
        fname += ".png";
        if (!tex_.loadFromFile(fname))
            throw std::runtime_error("Failed to load ball texture: " + fname);
    }
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
