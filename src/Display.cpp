#include "../include/Physics.hpp"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include "../include/Display.hpp"
#include "../include/Audio.hpp"

Display::Display(TableSegment seg, Role role, unsigned int totalDisplays, std::string hostAddress):
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
    this->calculateScale();
    if (this->seg_ == LEFT) {
           this->tableOffset_ = sf::Vector2f({ 217, 205 });
    } else {
        this->tableOffset_ = sf::Vector2f({ 0, 205 });
    }
    if (this->seg_ == LEFT) {
        this->displayOffset_ = 0;
    } else if (this->seg_ == CENTER) {
        this->displayOffset_ = 1920;
    } else if (this->seg_ == RIGHT) {
        this->displayOffset_ = 1920;
    }
    // this->tableOffset_ = sf::Vector2f({ 217, 205 }); // set to { 0, 205 } for middle or right
    // this->logicalSize_ = sf::Vector2u({ 1703, 670 }); // comment this out for multi display, set to { 1920, 670 } to test middle

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

void Display::drawBall(Ball& b) {
    b.setPosition({
        (((float)b.pos.x + this->tableOffset_.x) * this->scale_ + this->renderedOffset_.x) - this->displayOffset_,
        ((float)b.pos.y + this->tableOffset_.y) * this->scale_  + this->renderedOffset_.y,
    });
    this->window_.draw(b);
}

Display::~Display() {
    if (io_) io_->stop();
    if (networkThread_.joinable()) networkThread_.join();
}

void Display::setupNetworking(const std::string& hostAddress) {
    io_ = std::make_unique<asio::io_context>();

    if (role_ == HOST) {
        asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), kPoolPort);
        server_ = std::make_unique<PoolServer>(*io_, endpoint);
    } else {
        asio::ip::tcp::resolver resolver(*io_);
        auto endpoints = resolver.resolve(hostAddress, kPoolPortString);
        client_ = std::make_unique<PoolClient>(*io_, endpoints, [this](const std::vector<PoolBallState>& state) {
            applyNetworkState(state);
        });
    }

    networkThread_ = std::thread([this]() {
        io_->run();
    });
}

void Display::applyNetworkState(const std::vector<PoolBallState>& state) {
    std::lock_guard<std::mutex> lock(ballsMutex_);

    if (balls.size() != state.size()) {
        balls.clear();
        balls.reserve(state.size());
        for (const auto& ball : state) {
            balls.emplace_back(Vector{ball.x, ball.y}, Vector{ball.vx, ball.vy}, ball.number, this->scale_);
        }
    } else {
        for (std::size_t i = 0; i < state.size(); ++i) {
            balls[i].pos = Vector{state[i].x, state[i].y};
            balls[i].vel = Vector{state[i].vx, state[i].vy};
        }
    }

    hasNetworkState_ = true;
}

void Display::broadcastState() {
    if (!server_) return;

    std::vector<PoolBallState> state;
    state.reserve(balls.size());
    for (const auto& ball : balls) {
        state.push_back(PoolBallState{
            .number = ball.number,
            .x = ball.pos.x,
            .y = ball.pos.y,
            .vx = ball.vel.x,
            .vy = ball.vel.y
        });
    }

    server_->broadcast(makeStateMessage(state));
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

        if (role_ == HOST) {
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
                            if ((ball2.pos - ball1.pos).magnitude() < ball1.radius + ball2.radius) {
                                ball1.hit(ball2, between_frames.asSeconds());
                            }
                        }
                    }

                    broadcastState();
                }
                for (Ball& ball: balls) {
                    this->drawBall(ball);
                }
            }
            else clock.stop();
        } else {
            std::lock_guard<std::mutex> lock(ballsMutex_);
            if (hasNetworkState_) {
                for (Ball& ball: balls) {
                    this->drawBall(ball);
                }
            }
        }

        this->window_.display();
    }
}
