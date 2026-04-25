// #include <SFML/Window.hpp>
#include <cmath>
#include <iostream>

#include "include/Display.hpp"

typedef double unit_t;

template <class T>
T dot_product(T x1, T y1, T x2, T y2) {
    return x1*x2 + y1*y2;
}

struct Vector {
    unit_t x;
    unit_t y;
    unit_t dot(Vector& rhs) const { return x*rhs.x + y*rhs.y; }
    [[nodiscard]] unit_t magnitude() const { return std::sqrt(x*x + y*y);  }
    [[nodiscard]] Vector normalized() const { return (*this) / this->magnitude(); }

    Vector operator*(unit_t rhs) const { return Vector { .x = x * rhs, .y = y * rhs }; }
    Vector operator/(unit_t rhs) const { return Vector { .x = x / rhs, .y = y / rhs }; }
    Vector operator+(Vector rhs) const { return Vector { .x = x + rhs.x, .y = y + rhs.y }; }
    Vector operator-(Vector rhs) const { return Vector { .x = x - rhs.x, .y = y - rhs.y }; }
    Vector operator-() const { return Vector { .x = -x , .y = -y }; }
    Vector& operator+=(const Vector& rhs) { return *this = *this + rhs; }
    Vector& operator-=(const Vector& rhs) { return *this = *this - rhs; }
};

// class Sphere {
// public:
//     Vector pos;
//     Vector vel;
//     unit_t radius;
//     unit_t mass;
// };
//
// enum class BallType {
//     Stripe,
//     Solid,
//     Eight,
//     Cue
// };
//
// class Ball : public Sphere {
// public:
//     int number;
//     BallType type;
//
//     Ball(Vector position, Vector velocity) {
//         pos = position;
//         vel = velocity;
//         radius = 1;
//         mass = 1;
//         type = BallType::Solid;
//     }
//     void hit(Ball& rhs) {
//         // so we get the normal from the centers of our balls
//         // then
//         Vector dmomentum = vel * mass - rhs.vel * rhs.mass; // delta momentum
//         Vector normal = (pos - rhs.pos).normalized();
//         Vector force = normal * dmomentum.dot(normal);
//         rhs.vel += force / rhs.mass;
//         this->vel -= force / mass;
//         // I think I fundamentally misunderstand collision physics
//     }
//     [[nodiscard]] Vector friction() const {
//         return vel.normalized() * mass * 0.01;
//     }
//     void tick_physics(double dt) { // dt in seconds
//         Vector dpos = vel * dt - friction() * (dt*dt/2);
//         this->pos += dpos;
//         // if (intersects other ball) {
//         //     this->hit(other_ball);
//         // }
//     }
// };

// void test_collision() {
//     // total energy should be preserved
//     Ball ball1 = Ball({0.0, 0.0}, {0.0, 0.0});
//     Ball ball2 = Ball({0.0, 1.0}, {0.0, -1.0});
//     ball1.mass = 1000;
//     ball1.hit(ball2);
//     std::cout << ball1.vel.x << ", " << ball1.vel.y << std::endl;
//     std::cout << ball2.vel.x << ", " << ball2.vel.y << std::endl;
// }


int main() {
    Display d;
    d.update();
}