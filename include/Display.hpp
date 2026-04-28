#pragma once

#include <vector>
#include <iostream>

#include "SFML/Graphics.hpp"
#include "Ball.hpp"

typedef struct rect {
    int x;
    int y;
} Rect;

class Display {
public:
    Display();
    ~Display();

    void update();
    
private:
    const sf::Vector2u logicalSize_;
    sf::Vector2u physicalSize_;
    sf::Vector2u renderedSize_;
    
    float scaleFactor_;
    
    sf::RenderWindow window_;
    
    std::vector<Ball> balls;
    std::vector<Vector> initial_ball_velocities; // for physics ✨

    // sf::RenderWindow window;

    void setupDisplay();
    
    void calculateTransform();

};