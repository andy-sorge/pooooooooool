#pragma once


#include <atomic>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <optional>

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Vector2.hpp"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Texture.hpp>
#include "SFML/Graphics.hpp"

#include "Textures.hpp"
#include "Ball.hpp"

typedef struct rect {
    int x;
    int y;
} Rect;

#include "Ball.hpp"
#include "Textures.hpp"
#include "Utilities.hpp"
#include "Networking.hpp"

class Display {
public:
    Display(Synchronized<State>& state, sf::RenderWindow& window);

    void render();

    void scale(sf::Sprite& sprite) {
        sprite.setScale({ scale_, scale_ });
    }

    void setupDisplay();

    void update();

    void scaleBalls(std::vector<Ball>& balls);
private:
    Synchronized<State>& state_;
    sf::RenderWindow& window_;

    // display positioning and scale
    sf::Vector2u physicalSize_;
    sf::Vector2u renderedSize_;
    sf::Vector2u renderedOffset_;
    sf::Vector2f tableOffset_;
    float scale_;
    TableSegment segment_;

    void calculateRenderedSize();
    void calculateRenderedOffset();
    void calculateScale();

    // sf::Sprite tableTop_;
    // sf::Sprite tableBorder_;
    sf::Texture cueTexture_;
    sf::Sprite cueSprite_;

    // sf::RenderWindow window_;
    void drawBall(Ball& b);

    std::vector<Ball> balls;
    std::vector<Vector> initial_ball_velocities; // for physics ✨
    sf::Vector2f aimDir_{0.0f, 0.0f};
    float aimPower_{0.0f};
    bool aiming_{false};

    sf::Sprite top_;
    sf::Sprite border_;

    // void drawBall(Ball& b);

    void calculateTransform();
    void calculateLogical();

    void calculateLayout();
    bool isPocketed(const Ball& ball) const;
};
