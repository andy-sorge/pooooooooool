#pragma once

#include <cmath>

#include "SFML/System/Vector2.hpp"

typedef double unit_t;

struct Vector {
    unit_t x;
    unit_t y;
    unit_t dot(const Vector& rhs) const { return x*rhs.x + y*rhs.y; }
    [[nodiscard]] unit_t magnitude() const { return std::sqrt(x*x + y*y);  }
    [[nodiscard]] Vector normalized() const { return (*this) / this->magnitude(); }

    Vector operator*(unit_t rhs) const { return Vector { .x = x * rhs, .y = y * rhs }; }
    Vector operator/(unit_t rhs) const { return Vector { .x = x / rhs, .y = y / rhs }; }
    Vector operator+(const Vector& rhs) const { return Vector { .x = x + rhs.x, .y = y + rhs.y }; }
    Vector operator-(const Vector& rhs) const { return Vector { .x = x - rhs.x, .y = y - rhs.y }; }
    Vector operator-() const { return Vector { .x = -x , .y = -y }; }
    Vector& operator+=(const Vector& rhs) { return *this = *this + rhs; }
    Vector& operator-=(const Vector& rhs) { return *this = *this - rhs; }

    [[nodiscard]] sf::Vector2f sf() const { return sf::Vector2f(x, y); }
};
