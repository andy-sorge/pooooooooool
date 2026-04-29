#include "../include/Physics.hpp"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include "../include/Display.hpp"

Display::Display(TableSegment seg, Role role, unsigned int totalDisplays):
window_(sf::VideoMode::getDesktopMode(), "POOOOOOOOOOL", sf::State::Fullscreen),
logicalSize_({ 1703 * 2 + (1920 * totalDisplays - 2), 670 }),
tableTop_(getTableTop(seg)),
tableBorder_(getTableBorder(seg))
{
    this->role_ = role;
    this->seg_ = seg;
    // table border is 205 at top and bottom
    // table border is 217 at left and right
    this->physicalSize_ = this->window_.getSize();
    this->calculateRenderedSize();
    this->calculateRenderedOffset();
    this->tableOffset_ = sf::Vector2u({ 217, 205 }); // set to { 0, 205 } for middle or right
    this->logicalSize_ = sf::Vector2u({ 1703, 670 }); // comment this out for multi display, set to { 1920, 670 } to test middle
    this->calculateScale();

    this->tableBorder_.setPosition({
        (float)this->renderedOffset_.x,
        (float)this->renderedOffset_.y
    });
    this->tableBorder_.setScale({ this->scale_, this->scale_});
    this->tableTop_.setPosition({
        (float)this->renderedOffset_.x,
        (float)this->renderedOffset_.y
    });
    this->tableTop_.setScale({ this->scale_, this->scale_});
    
    setupTextures();

    // for (int i = 0; i <= 8; ++i) {
    //     balls.emplace_back(Vector{100.5*i,200.0}, Vector{500.0*i,1000.0}, i);
    // }
    create_arranged_balls(balls);
    for (Ball& b : this->balls) {
        b.setScale({ this->scale_, this->scale_ });
    }
}

void Display::calculateRenderedSize() {
    this->physicalSize_ = this->window_.getSize();
    this->renderedSize_ = sf::Vector2u({
        this->physicalSize_.x,
        this->physicalSize_.x * 9 / 16
    });
}

void Display::calculateRenderedOffset() {
    this->renderedOffset_ = sf::Vector2u({
        this->physicalSize_.x - this->renderedSize_.x,
        this->physicalSize_.y - this->renderedSize_.y
    });
}

void Display::calculateScale() {
    this->scale_ = (float)this->renderedSize_.x / 1920;
    std::cout << this->scale_ << std::endl;
}

void Display::drawBall(Ball& b) {
    b.setPosition({
        (float)b.pos.x + this->tableOffset_.x + this->renderedOffset_.x,
        (float)b.pos.y + this->tableOffset_.y + this->renderedOffset_.y,
    });
    this->window_.draw(b);
}

Display::~Display() {

}

void Display::update() {
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

        this->window_.clear(sf::Color::Black);

        this->window_.draw(this->tableTop_);
        this->window_.draw(this->tableBorder_);

        if (this->window_.hasFocus()) { // physics
            dt += clock.reset();
            clock.start();
            sf::Time between_frames = sf::seconds(1.0 / 144); // fixed framerate
            if (dt > between_frames) {
                dt -= between_frames;
                initial_ball_velocities.clear();
                for (Ball& ball: balls) {
                    initial_ball_velocities.push_back(ball.vel);
                    this->drawBall(ball);
                    ball.tick_physics(between_frames.asSeconds());
                }
                for (int i = 0; i < balls.size(); ++i) {
                    Ball& ball1 = balls[i];
                    // flip if it hits the walls (currently just edge of screen)
                    // needs to correctly move the ball backwards in time out of the wall instead of just setting position lowk
                    if (ball1.pos.y < ball1.radius) {
                        ball1.pos.y = ball1.radius;
                        ball1.vel.y = -ball1.vel.y - ball1.friction().magnitude();
                    }
                    if (ball1.pos.y > this->logicalSize_.y - ball1.radius) {
                        ball1.pos.y = this->logicalSize_.y - ball1.radius;
                        ball1.vel.y = -ball1.vel.y + ball1.friction().magnitude();
                    }
                    if (ball1.pos.x < ball1.radius) {
                        ball1.pos.x = ball1.radius;
                        ball1.vel.x = -ball1.vel.x - ball1.friction().magnitude();
                    }
                    if (ball1.pos.x > this->logicalSize_.x - ball1.radius) {
                        ball1.pos.x = this->logicalSize_.x - ball1.radius;
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
                this->drawBall(ball);
            }
        }

        this->window_.display();
    }

}
