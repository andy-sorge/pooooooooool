#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Network.hpp>

#include "Ball.hpp"
#include "PoolServer.hpp"
#include "PoolClient.hpp"

class Display {
public:
    Display(State& state);
    void render();
private:
    State& state_;
    sf::RenderWindow& window_;

    // display positioning and scale
    sf::Vector2u physicalSize_;
    sf::Vector2u renderedSize_;
    sf::Vector2u renderedOffset_;
    sf::Vector2f tableOffset_;
    float scale_;

    void calculateRenderedSize();
    void calculateRenderedOffset();
    void calculateScale();

    sf::Sprite tableTop_;
    sf::Sprite tableBorder_;

    void drawBall(Ball& b);

    void setupDisplay();

    void calculateTransform();
    void calculateLogical(int totalDisplays);

    void recalculateLayout();
    bool isPocketed(const Ball& ball) const;
};
