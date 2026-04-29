#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
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
    
    // display positioning and scale
    sf::Vector2u physicalSize_;
    sf::Vector2u renderedSize_;
    sf::Vector2u renderedOffset_;
    sf::Vector2u tableOffset_;
    sf::Vector2u logicalSize_;
    float scale_;
    
    void calculateRenderedSize();
    void calculateRenderedOffset();
    void calculateScale();
        
    sf::Sprite tableTop_;
    sf::Sprite tableBorder_;
    
    float scaleFactor_;
    
    sf::RenderWindow window_;
    void drawBall(Ball& b);
    
    std::vector<Ball> balls;
    std::vector<Vector> initial_ball_velocities; // for physics ✨

    void setupDisplay();
    
    void calculateTransform();
    void calculateLogical(int totalDisplays);

};