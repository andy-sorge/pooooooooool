#include <algorithm>
#include <chrono>
#include <memory>
#include <optional>
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

#include "Physics.hpp"
#include "Display.hpp"
#include "Audio.hpp"
#include "PoolClient.hpp"
#include "PoolServer.hpp"
#include "Utilities.hpp"

namespace {
    constexpr float kPocketRadius = 100.0f;
    constexpr float kPort = 58008;
}

Display::Display(TableSegment seg, Role role, unsigned int displays, unsigned int index, std::string hostAddress):
window_(sf::VideoMode::getDesktopMode(), "POOOOOOOOOOL", sf::State::Fullscreen),
logicalSize_({ 1703, 670 }),
displays_(displays),
index_(index),
tableTop_(getTableTop(seg)),
tableBorder_(getTableBorder(seg)) {
    this->role_ = role;
    this->seg_ = seg;
    // table border is 205 at top and bottom
    // table border is 217 at left and right
    this->physicalSize_ = this->window_.getSize();
    this->calculateRenderedSize();
    this->calculateRenderedOffset();
    this->calculateScale();
    this->recalculateLayout();

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

    if (this->role_ == HOST) {
        create_arranged_balls(balls, this->scale_);
        for (Ball& b : this->balls) {
            b.setScale({ this->scale_, this->scale_ });
        }
    }

    setupNetworking(hostAddress);

    playing_music = false;
    // if (this->role_ == Role::HOST) {
    //     startMusicLeft();
    // }
    // else if (this->role_ == Role::CLIENT) {
    //     startMusicRight();
    // }
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
}

void Display::recalculateLayout() {
    unsigned int displays = this->displays_;
    if (displays < 1) displays = 1;

    unsigned int logicalWidth = 1703;
    if (displays == 1) logicalWidth = 1703;
    else if (displays == 2) logicalWidth = 1703 * 2;
    else logicalWidth = 1703 * 2 + 1920 * (displays - 2);

    this->logicalSize_ = sf::Vector2u({ logicalWidth, 670 });

    unsigned int index = this->index_;
    if (index >= displays) index = displays - 1;

    unsigned int displayOffsetX = 0;
    if (index == 0) displayOffsetX = 0;
    else if (index == displays - 1) displayOffsetX = logicalWidth - 1703;
    else displayOffsetX = 1703 + 1920 * (index - 1);

    float baseOffsetX = (index == 0) ? 217.0f : 0.0f;
    this->tableOffset_ = sf::Vector2f({ baseOffsetX - static_cast<float>(displayOffsetX), 205 });
}

std::vector<Vector> Display::pocketCenters() const {
    unsigned int displays = this->displays_;
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
        if (dist <= (kPocketRadius + ball.radius)) {
            return true;
        }
    }
    return false;
}

void Display::drawBall(Ball& b) {
    b.setPosition({
        ((float)b.pos.x + this->tableOffset_.x) * this->scale_ + this->renderedOffset_.x,
        ((float)b.pos.y + this->tableOffset_.y) * this->scale_  + this->renderedOffset_.y,
    });
    this->window_.draw(b);
}

void Display::setupNetworking(const std::string& hostAddress) {
    if (role_ == HOST) {
        std::cout << "i am a server!" << std::endl;
        server_ = std::make_unique<PoolServer>();
        server_->start(kPort);

        server_->registerConnection([this]() {
            server_->send(packageDisplays(++displays_));
        });
    } else {
        std::cout << "i am a client!" << std::endl;
        client_ = std::make_unique<PoolClient>();
        client_->connect(hostAddress, kPort);

        client_->registerHandle(PacketType::GameState, [this](sf::Packet& packet) {
            std::vector<BallState> state;
            interpretBalls(packet, state);
            applyNetworkState(state);
        });
        client_->registerHandle(PacketType::Connection, [this](sf::Packet& packet) {
            interpretDisplays(packet, displays_);
        });

        auto now = std::chrono::steady_clock::now();
        auto end = now + std::chrono::milliseconds(5000);
        while (std::chrono::steady_clock::now() < end) {
            if (client_->isConnected()) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        if (!client_->isConnected()) throw std::runtime_error("client failed to connect!");

        //this->playing_music = playing_music;
        //if (playing_music && !getMusicStarted()) {
        //    startMusicRight();
        //}
    }
}

void Display::applyNetworkState(const std::vector<BallState>& state) {
    if (balls.size() != state.size()) {
        balls.clear();
        balls.reserve(state.size());
        for (const auto& ball : state) balls.emplace_back(Vector{ball.x, ball.y}, Vector{ball.vx, ball.vy}, ball.number, this->scale_);
    } else {
        for (std::size_t i = 0; i < state.size(); ++i) {
            balls[i].pos = {state[i].x, state[i].y};
            balls[i].vel = {state[i].vx, state[i].vy};
        }
    }
}

void Display::broadcastState() {
    if (!server_) return;

    std::vector<BallState> state;
    state.reserve(balls.size());
    for (const auto& ball : balls)  state.push_back(BallState{ball.number, ball.pos.x, ball.pos.y, ball.vel.x, ball.vel.y});

    server_->send(packageBalls(state));
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
        if (role_ == HOST) {
            if (true/*this->window_.hasFocus()*/) { // physics
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

                        for (int j = i+1; j < balls.size(); ++j) {
                            Ball& ball2 = balls[j];

                            if ((ball2.pos - ball1.pos).magnitude() < ball1.radius + ball2.radius) ball1.hit(ball2, between_frames.asSeconds());
                        }
                    }

                    balls.erase(std::remove_if(balls.begin(), balls.end(), [this](const Ball& ball) {
                        return isPocketed(ball);
                    }), balls.end());

                    broadcastState();
                }

                for (Ball& ball: balls) this->drawBall(ball);
            }

            else clock.stop();
        }
        // clients draw balls
        else for (Ball& ball: balls) this->drawBall(ball);

        this->window_.display();
    }
}
