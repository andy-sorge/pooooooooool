#include "../include/Physics.hpp"
#include "../include/Display.hpp"

Display::Display():
window_(sf::VideoMode::getDesktopMode(), "POOOOOOOOOOL", sf::State::Fullscreen),
logicalSize_({ 1600, 1000 })
{
    this->physicalSize_ = this->window_.getSize();
    
    this->calculateTransform();
    // for (int i = 0; i <= 8; ++i) {
    //     balls.emplace_back(Vector{100.5*i,200.0}, Vector{500.0*i,1000.0}, i);
    // }
    create_arranged_balls(balls);
    
}

void Display::calculateTransform() {
    
}


Display::~Display() {

}

void Display::setupDisplay() {

}

void Display::update() {
    // sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "My window", sf::State::Fullscreen);

    auto window_size = this->window_.getSize();
    std::cout << window_size.x << " " << this->window_.getSize().y << std::endl;

    sf::Clock clock;
    sf::Time dt = sf::Time::Zero;
    int holding_ball = -1;
    while (this->window_.isOpen())
    {

        while (const std::optional event = this->window_.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                this->window_.close();

            if (auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape) {
                    this->window_.close();
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
            if (auto* button = event->getIf<sf::Event::MouseButtonPressed>()) {
               if (button->button == sf::Mouse::Button::Left) {
                   sf::Vector2i mouse = sf::Mouse::getPosition();
                   holding_ball = balls.size();
                   balls.emplace_back(Vector(mouse.x, mouse.y), Vector{0, 0}, 0);
               }
            }
            if (auto* button = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (button->button == sf::Mouse::Button::Left) {
                    sf::Vector2i mouse = sf::Mouse::getPosition();
                    Vector mouse_pos = Vector(mouse.x, mouse.y);
                    balls[holding_ball].vel += (balls[holding_ball].pos - mouse_pos) * 10;
                }
            }
        }

        this->window_.clear(sf::Color(0x08, 0x33, 0x00));

        if (this->window_.hasFocus()) { // physics
            dt += clock.reset();
            clock.start();
            sf::Time between_frames = sf::seconds(1.0 / 144); // fixed framerate
            if (dt > between_frames) {
                dt -= between_frames;
                initial_ball_velocities.clear();
                for (Ball& ball: balls) {
                    initial_ball_velocities.push_back(ball.vel);
                    this->window_.draw(ball);
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
                this->window_.draw(ball);
            }
        }

        this->window_.display();
    }

}