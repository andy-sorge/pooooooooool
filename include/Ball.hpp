#pragma once

#include "SFML/Graphics.hpp"

class Ball : public sf::CircleShape {
public:
    Ball(sf::Color color);

    ~Ball();

private:
    void setCenter(sf::Vector2f pos);
};