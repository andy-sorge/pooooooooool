#include "../include/Ball.hpp"

Ball::Ball(sf::Color color) {
    this->setRadius(100);
    this->setFillColor(color);
    this->setCenter({200.f, 300.f});
}

Ball::~Ball() {

}

void Ball::setCenter(sf::Vector2f pos) {
    this->setPosition({pos.x - this->getRadius(), pos.y - this->getRadius()});
}
