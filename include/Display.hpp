#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <vector>
#include <iostream>
#include "../include/Textures.hpp"
#include "SFML/Graphics.hpp"
#include "Ball.hpp"

typedef struct rect {
    int x;
    int y;
} Rect;

enum Role {
    HOST,
    CLIENT
};

class Display {
public:
    Display(TableSegment seg, Role role, unsigned int totalDisplays);
    ~Display();

    void update();
    
private:
    Role role_;
    TableSegment seg_;

    const sf::Vector2u logicalSize_;
    sf::Vector2u physicalSize_;
    sf::Vector2u renderedSize_;
    
    sf::Sprite tableTop_;
    sf::Sprite tableBorder_;
    
    float scaleFactor_;
    
    sf::RenderWindow window_;
    
    std::vector<Ball> balls;
    std::vector<Vector> initial_ball_velocities; // for physics ✨

    // sf::RenderWindow window;

    void setupDisplay();
    
    void calculateTransform();
    void calculateLogical(int totalDisplays);

};