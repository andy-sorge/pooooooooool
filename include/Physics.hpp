#pragma once

#include "Networking.hpp"
#include "SFML/System/Time.hpp"
#include "SFML/System/Vector2.hpp"

#include "Utilities.hpp"
#include "Networking.hpp"

class Physics {
public:
    Physics(Synchronized<State>& state) : state_(state) {
        pauseTime();
    }

    void pauseTime() {
        clock_.reset();
    }

    void step() {
        dt_ += clock_.reset();
        clock_.start();

        sf::Time between_frames = sf::seconds(1.0 / 60); // fixed framerate

        if (dt_ > between_frames) {
            dt_ -= between_frames;

            auto state = state_.lock();

            for (Ball& ball: state->balls) ball.tick_physics(between_frames.asSeconds());

            for (int i = 0; i < state->balls.size(); ++i) {
                Ball& ball1 = state->balls[i];
                if (ball1.pos.y < ball1.radius) {
                    ball1.pos.y = ball1.radius;
                    ball1.vel.y = -ball1.vel.y - ball1.friction().magnitude();
                }

                if (ball1.pos.y > state->logicalSpace.y - ball1.radius) {
                    ball1.pos.y = state->logicalSpace.y - ball1.radius;
                    ball1.vel.y = -ball1.vel.y + ball1.friction().magnitude();
                }

                if (ball1.pos.x < ball1.radius) {
                    ball1.pos.x = ball1.radius;
                    ball1.vel.x = -ball1.vel.x - ball1.friction().magnitude();
                }

                if (ball1.pos.x > state->logicalSpace.x - ball1.radius) {
                    ball1.pos.x = state->logicalSpace.x - ball1.radius;
                    ball1.vel.x = -ball1.vel.x + ball1.friction().magnitude();
                }

                for (int j = i+1; j < state->balls.size(); ++j) {
                    Ball& ball2 = state->balls[j];

                    if ((ball2.pos - ball1.pos).magnitude() < ball1.radius + ball2.radius) ball1.hit(ball2, between_frames.asSeconds());
                }
            }
        }
        
        
    }
private:
    sf::Clock clock_;
    sf::Time dt_ = sf::Time::Zero;
    Synchronized<State>& state_;
};
