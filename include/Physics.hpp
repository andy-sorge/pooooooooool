#include "Utilities.hpp"

#include "SFML/System/Time.hpp"
#include <SFML/System/Vector2.hpp>

class Physics {
public:
    Physics(State& state) : state_(state) {}

    void step() {
        dt_ += clock_.reset();
        clock_.start();

        sf::Time between_frames = sf::seconds(1.0 / 144); // fixed framerate

        if (dt_ > between_frames) {
            dt_ -= between_frames;

            for (Ball& ball: state_.balls) ball.tick_physics(between_frames.asSeconds());

            for (int i = 0; i < state_.balls.size(); ++i) {
                Ball& ball1 = state_.balls[i];
                if (ball1.pos.y < ball1.radius) {
                    ball1.pos.y = ball1.radius;
                    ball1.vel.y = -ball1.vel.y - ball1.friction().magnitude();
                }

                if (ball1.pos.y > this->state_.logicalSpace.y - ball1.radius) {
                    ball1.pos.y = this->state_.logicalSpace.y - ball1.radius;
                    ball1.vel.y = -ball1.vel.y + ball1.friction().magnitude();
                }

                if (ball1.pos.x < ball1.radius) {
                    ball1.pos.x = ball1.radius;
                    ball1.vel.x = -ball1.vel.x - ball1.friction().magnitude();
                }

                if (ball1.pos.x > this->state_.logicalSpace.x - ball1.radius) {
                    ball1.pos.x = this->state_.logicalSpace.x - ball1.radius;
                    ball1.vel.x = -ball1.vel.x + ball1.friction().magnitude();
                }

                for (int j = i+1; j < state_.balls.size(); ++j) {
                    Ball& ball2 = state_.balls[j];

                    if ((ball2.pos - ball1.pos).magnitude() < ball1.radius + ball2.radius) ball1.hit(ball2, between_frames.asSeconds());
                }
            }
        }
    }
private:
    sf::Clock clock_;
    sf::Time dt_ = sf::Time::Zero;
    State& state_;
}
