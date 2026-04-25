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
    Rect physicalSize;

    std::vector<Ball> balls;

    // sf::RenderWindow window;

    void setupDisplay();

};