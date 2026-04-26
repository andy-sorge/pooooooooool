// #include <SFML/Window.hpp>
#include <iostream>

#include "include/Display.hpp"


template <class T>
T dot_product(T x1, T y1, T x2, T y2) {
    return x1*x2 + y1*y2;
}


void test_collision() {
    // total energy should be preserved
    Ball ball1 = Ball({0.0, 0.0}, {0.0, 0.0}, 1);
    Ball ball2 = Ball({0.0, 1.0}, {0.0, -1.0}, 2);
    ball1.mass = 1000;
    ball1.hit(ball2);
    std::cout << ball1.vel.x << ", " << ball1.vel.y << std::endl;
    std::cout << ball2.vel.x << ", " << ball2.vel.y << std::endl;
}


int main() {
    Display d;
    d.update();
}