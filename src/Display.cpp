#include <SFML/Graphics/RenderWindow.hpp>
#include <algorithm>
#include <chrono>
#include <stdexcept>

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Socket.hpp>
#include <SFML/Network/SocketHandle.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>

#include "Vector.hpp"
#include "Display.hpp"
#include "Audio.hpp"
#include "Vector.hpp"

namespace {
    constexpr float kPocketRadius = 100.0f;
    constexpr float kPort = 58008;
}

Display::Display(State& state, sf::RenderWindow& window) :
state_(state),
window_(window),
tableTop_(getTableTop(seg)),
tableBorder_(getTableBorder(seg)) {
    this->seg_ = seg; // TODO find segment based on index
    // table border is 205 at top and bottom
    // table border is 217 at left and right
    this->physicalSize_ = this->window_.getSize();
    this->calculateRenderedSize();
    this->calculateRenderedOffset();
    this->calculateScale();
    this->recalculateLayout();

    this->tableBorder_.setPosition({ static_cast<float>(renderedOffset_.x), static_cast<float>(renderedOffset_.y) });
    this->tableBorder_.setScale({ this->scale_, this->scale_});
    this->tableTop_.setPosition({ static_cast<float>(renderedOffset_.x), static_cast<float>(renderedOffset_.y) });
    this->tableTop_.setScale({ this->scale_, this->scale_});

    setupTextures();

    if (state_.role == Role::Host) {
        create_arranged_balls(state_.balls, this->scale_);
        for (Ball& ball : state_.balls) ball.setScale({ this->scale_, this->scale_ });
    }
}

void Display::calculateRenderedSize() {
    this->physicalSize_ = this->window_.getSize();
    this->renderedSize_ = sf::Vector2u({ physicalSize_.x, physicalSize_.x * (9 / 16) });
}

void Display::calculateRenderedOffset() {
    this->renderedOffset_ = sf::Vector2u({ physicalSize_.x - renderedSize_.x, physicalSize_.y - renderedSize_.y });
}

void Display::calculateScale() {
    this->scale_ = (float)this->renderedSize_.x / 1920;
}

void Display::recalculateLayout() {
    unsigned int displays = state_.displays;
    if (displays < 1) displays = 1;

    unsigned int logicalWidth = 1703;
    if (displays == 1) logicalWidth = 1703;
    else if (displays == 2) logicalWidth = 1703 * 2;
    else logicalWidth = 1703 * 2 + 1920 * (displays - 2);

    state_.logicalSpace = sf::Vector2u({ logicalWidth, 670 });

    unsigned int index = state_.index;
    if (index >= displays) index = displays - 1;

    unsigned int displayOffsetX = 0;
    if (index == 0) displayOffsetX = 0;
    else if (index == displays - 1) displayOffsetX = logicalWidth - 1703;
    else displayOffsetX = 1703 + 1920 * (index - 1);

    float baseOffsetX = (index == 0) ? 217.0f : 0.0f;
    this->tableOffset_ = sf::Vector2f({ baseOffsetX - static_cast<float>(displayOffsetX), 205 });
}

void Display::applyNetworkState(const std::vector<BallState>& state) {
    if (state_.balls.size() != state.size()) {
        state_.balls.clear();
        state_.balls.reserve(state.size());
        for (const auto& ball : state) state_.balls.emplace_back(Vector{ball.x, ball.y}, Vector{ball.vx, ball.vy}, ball.number, this->scale_);
    } else {
        for (std::size_t i = 0; i < state.size(); ++i) {
            state_.balls[i].pos = {state[i].x, state[i].y};
            state_.balls[i].vel = {state[i].vx, state[i].vy};
        }
    }
}

std::vector<Vector> Display::pocketCenters() const {
    unsigned int displays = state_.displays;
    if (displays < 1) displays = 1;

    std::vector<Vector> pockets;
    pockets.reserve(displays * 4);

    double offset = 1730.0;
    pockets.push_back(Vector{0.0, 0.0});
    pockets.push_back(Vector{0.0, 670});
    pockets.push_back(Vector{1730.0, 0.0});
    pockets.push_back(Vector{1730.0, 670});
    for (unsigned int i = 1; i < displays; ++i) {
        offset += i == displays - 1 ? 1703.0 : 1920.0;
        pockets.push_back(Vector{0.0 + offset, 0.0});
        pockets.push_back(Vector{0.0 + offset, 670});
    }

    return pockets;
}

bool Display::isPocketed(const Ball& ball) const {
    for (const auto& pocket : pocketCenters()) {
        const auto dist = (ball.pos - pocket).magnitude();
        if (dist <= (kPocketRadius + ball.radius)) return true;
    }

    return false;
}

void Display::drawBall(Ball& ball) {
    ball.setPosition({
        (static_cast<float>ball.pos.x + this->tableOffset_.x) * this->scale_ + this->renderedOffset_.x,
        (static_cast<float>ball.pos.y + this->tableOffset_.y) * this->scale_  + this->renderedOffset_.y,
    });
    this->window_.draw(ball);
}

void Display::update() {
    sf::Clock clock;
    sf::Time dt = sf::Time::Zero;

    int holding_ball = -1;

    while (this->window_.isOpen()) {
        //if (role_ == CLIENT) this->window_.close();
        if (role_ == HOST) recalculateLayout();

        // process window events
        while (const std::optional event = this->window_.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                this->window_.close();

            if (auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape) this->window_.close();

                if (role_ == HOST) {
                    if (key->code == sf::Keyboard::Key::Num1) {
                        tableTop_.setTexture(getTableTop(LEFT));
                        tableBorder_.setTexture(getTableBorder(LEFT));
                    }
                    if (key->code == sf::Keyboard::Key::Num2) {
                        tableTop_.setTexture(getTableTop(CENTER));
                        tableBorder_.setTexture(getTableBorder(CENTER));
                    }
                    if (key->code == sf::Keyboard::Key::Num3) {
                        tableTop_.setTexture(getTableTop(RIGHT));
                        tableBorder_.setTexture(getTableBorder(RIGHT));
                    }
                    if (key->code == sf::Keyboard::Key::Space) {
                        balls.emplace_back(Vector{100, 491.0}, Vector{2800, -70}, 0, this->scale_);
                    }

                    if (key->code == sf::Keyboard::Key::LShift) {
                        balls.clear();
                        create_arranged_balls(balls, this->scale_);
                    }
                    if (key->code == sf::Keyboard::Key::M) {
                        playing_music = true;
                        startMusicLeft();
                    }
                }
            }

            if (role_ == HOST) {
                if (auto* button = event->getIf<sf::Event::MouseButtonPressed>()) {
                   if (button->button == sf::Mouse::Button::Left) {
                       sf::Vector2i mouse = sf::Mouse::getPosition();
                       holding_ball = balls.size();
                       balls.emplace_back(Vector(mouse.x, mouse.y), Vector{0, 0}, 0, this->scale_);
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
        }

        this->window_.clear(sf::Color::Black);

        this->window_.draw(this->tableTop_);
        this->window_.draw(this->tableBorder_);

        // for (const auto& pocket : pocketCenters()) {
        //     sf::CircleShape circle(kPocketRadius * this->scale_);
        //     circle.setFillColor(sf::Color::Transparent);
        //     circle.setOutlineColor(sf::Color::Red);
        //     circle.setOutlineThickness(3.0f);
        //     circle.setOrigin({ kPocketRadius * this->scale_, kPocketRadius * this->scale_ });
        //     circle.setPosition({
        //         ((float)pocket.x + this->tableOffset_.x) * this->scale_ + this->renderedOffset_.x,
        //         ((float)pocket.y + this->tableOffset_.y) * this->scale_ + this->renderedOffset_.y
        //     });
        //     this->window_.draw(circle);
        // }

        // physics
        //
        // clients draw balls
        else for (Ball& ball: balls) this->drawBall(ball);

        this->window_.display();
    }
}
