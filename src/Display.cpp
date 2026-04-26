#include "../include/Physics.hpp"
#include "../include/Display.hpp"

Display::Display()
// window(sf::VideoMode({800, 600}), "Game Window")
{
    // for (int i = 0; i <= 8; ++i) {
    //     balls.emplace_back(Vector{100.5*i,200.0}, Vector{500.0*i,1000.0}, i);
    // }
    create_arranged_balls(balls);
}


Display::~Display() {

}

void Display::setupDisplay() {

}

void Display::update() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "My window", sf::State::Fullscreen);

    auto window_size = window.getSize();
    std::cout << window_size.x << " " << window.getSize().y << std::endl;

    sf::Clock clock;
    sf::Time dt = sf::Time::Zero;
    while (window.isOpen())
    {

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape) {
                    window.close();
                }
                if (key->code == sf::Keyboard::Key::Space) {
                    balls.emplace_back(Vector{100, 491.0}, Vector{2800, -70}, 0);
                    // balls.back().mass = 0.5; // messing with mass for fun
                }

                if (key->code == sf::Keyboard::Key::LShift) {
                    balls.clear();
                    create_arranged_balls(balls);
                }
            }
        }

        window.clear(sf::Color(0x08, 0x33, 0x00));

        if (window.hasFocus()) { // physics
            dt += clock.reset();
            clock.start();
            sf::Time between_frames = sf::seconds(1.0 / 144); // fixed framerate
            if (dt > between_frames) {
                dt -= between_frames;
                initial_ball_velocities.clear();
                for (Ball& ball: balls) {
                    initial_ball_velocities.push_back(ball.vel);
                    window.draw(ball);
                    ball.tick_physics(between_frames.asSeconds());
                }
                for (int i = 0; i < balls.size(); ++i) {
                    Ball& ball1 = balls[i];
                    // flip if it hits the walls (currently just edge of screen)
                    // needs to correctly move the ball backwards in time out of the wall instead of just setting position lowk
                    if (ball1.pos.y < 0 + ball1.radius ) {
                        ball1.pos.y = 0 + ball1.radius;
                        ball1.vel.y = -ball1.vel.y - ball1.friction().magnitude();
                    }
                    if (ball1.pos.y > window_size.y - ball1.radius) {
                        ball1.pos.y = window_size.y - ball1.radius;
                        ball1.vel.y = -ball1.vel.y + ball1.friction().magnitude();
                    }
                    if (ball1.pos.x < 0 + ball1.radius) {
                        ball1.pos.x = 0 + ball1.radius;
                        ball1.vel.x = -ball1.vel.x - ball1.friction().magnitude();
                    }
                    if (ball1.pos.x > window_size.x - ball1.radius) {
                        ball1.pos.x = window_size.x - ball1.radius;
                        ball1.vel.x = -ball1.vel.x + ball1.friction().magnitude();
                    }
                    // collisions with other balls (could be improved by correcting the position before doing the hit)
                    for (int j = i+1; j < balls.size(); ++j) {
                        Ball& ball2 = balls[j];
                        if ((ball2.pos - ball1.pos).magnitude() < ball1.radius + ball2.radius) {
                            // TODO: play hit sound effect
                            ball1.hit(ball2);
                        }
                    }
                }
            }
            for (Ball& ball: balls) {
                window.draw(ball);
            }
        }

        window.display();
    }

}