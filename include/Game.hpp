#pragma once

#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>
#include <functional>
#include <iostream>
#include <stdexcept>
//#include <variant>

#include "SFML/Window/VideoMode.hpp"
#include "SFML/Network/Packet.hpp"

#include "MainMenu.hpp"
#include "Display.hpp"
#include "PoolServer.hpp"
#include "PoolClient.hpp"
#include "Physics.hpp"
#include "Vector.hpp"
#include "Textures.hpp"
#include "Utilities.hpp"
#include "Networking.hpp"
#include "ControllerInput.hpp"


class Game {
public:
    Game() : window_(sf::VideoMode({800, 600}), "POOOOOOOOOOL", sf::Style::Titlebar | sf::Style::Close) {}

    void run() {
        setupTextures();

        // startup, get all values
        MainMenu menu(window_);
        auto result = menu.run();

        window_.close();

        window_.create(sf::VideoMode::getDesktopMode(), "POOOOOOOOOOL", sf::State::Fullscreen);
        //sf::RenderWindow window2(sf::VideoMode::getDesktopMode(), "POOOOOOOOOOL", sf::State::Fullscreen);
        auto& display = display_.emplace(state_, window_);
        auto& physics = physics_.emplace(state_);

        if (result.role == Role::Host) {
            host();
            auto state = state_.lock();
            state->index = 0;
            std::cout << "help me\n";
            create_arranged_balls(state->balls);
            display.scaleBalls(state->balls);
        } else client(result.host);


        sf::Clock clock;
        const sf::Time interval = sf::milliseconds(100); // 20hz, tune as needed
        sf::Time last = sf::Time::Zero;

        {
            this->state_.lock()->turn = PlayerTurn::PlacingCueBall;
        }

        while(window_.isOpen()) {
            display.render();
            stateBasedActions();

            if (result.role == Role::Host) {
                auto elapsed = clock.getElapsedTime();
                if (elapsed - last >= interval) {
                    auto state = state_.lock();
                    server_->send(package(state->balls));
                    // also send cue state
                    server_->send(package(state->cueDir, state->cuePower, state->cueAiming));
                    // also send real gamer state
                    server_->send(package(state->turn));
                    last = elapsed;
                }
            }

            input(); // this can be blocking!
        }

        if (server_) server_->stop();
        if (client_) client_->stop();
    }
private:
    Synchronized<State> state_; // game state, ball positions, role, etc.

    sf::RenderWindow window_;
    std::optional<Display> display_;
    std::optional<Physics> physics_;
    ControllerInput controller_;

    std::optional<std::reference_wrapper<Ball>> cue_;

    std::shared_ptr<PoolServer> server_;
    std::shared_ptr<PoolClient> client_;

    std::queue<sf::Packet> _connection;


    void stateBasedActions() {
        PlayerTurn turn;
        {
            auto state = this->state_.lock();
            turn = state->turn;
        }
        // exit physics when physics have played out
        if (turn == PlayerTurn::Physics) {
            {
                auto state = state_.lock();
                state->turn = PlayerTurn::Aiming;
                for (const Ball& ball : state->balls) {
                    if (ball.vel.magnitude() > 0.0001) {
                        state->turn = PlayerTurn::Physics; }
                    break;
                }
                // std::cout << "physics" << std::endl;
            }
            physics_->step();
        }
        else { physics_->pauseTime(); }

        if (turn == PlayerTurn::Aiming) {
            // functionality can be found within the input method
            // std::cout << "aiming" << std::endl;
        }
        if (turn == PlayerTurn::None) {
            // std::cout << "turn none" << std::endl;
        }
        if (turn == PlayerTurn::PlacingCueBall) {
            // std::cout << "turn placing" << std::endl;
            if (!cue_.has_value()) {
                auto state = state_.lock();
                Ball cue(Vector(200, state->logicalSpace.y/2), Vector{0, 0}, 0);
                state->balls.insert(state->balls.begin(), std::move(cue));
                cue_ = state->balls[0];
                display_->scale(*cue_);
            }
        }
        if (turn == PlayerTurn::End) {
            std::cout << "turn end" << std::endl;
        }
    }


    void host() {
        std::cout << "i am a server!" << std::endl;
        server_ = std::make_unique<PoolServer>(PoolConstants::port);

        server_->registerConnection([this]() {
            std::cout << "client connected" << std::endl;
            {
                auto state = state_.lock();
                _connection.push(package(++state->displays));
                // TODO: reloads balls to be farther right
            }
            if (display_.has_value()) display_.value().update();
        });

        server_->registerHandle(PacketType::Connection, [this](sf::Packet& packet) {
            std::cout << "recieved connection packet!" << std::endl;
            if (!_connection.empty()) {
                server_->send(_connection.front());
                _connection.pop();
            }
        });

        server_->start();
    }

    void client(std::string& host) {
        std::cout << "i am a client!" << std::endl;
        client_ = std::make_unique<PoolClient>();

        client_->registerHandle(PacketType::Balls, [this](sf::Packet& packet) {
            std::cout << "recieved balls packet!" << std::endl;
            auto state = state_.lock();
            interpret(packet, state->balls);
            display_.value().scaleBalls(state->balls);
        });
        client_->registerHandle(PacketType::Connection, [this](sf::Packet& packet) {
            std::cout << "recieved connection packet!" << std::endl;
            {
                auto state = state_.lock();
                interpret(packet, state->displays);
                if (!state->index.has_value()) state->index = state->displays - 1;
            }
            if (display_.has_value()) display_.value().update();
            else std::cerr << "display not ready!";
        });

        client_->registerHandle(PacketType::Cue, [this](sf::Packet& packet) {   
            auto state = state_.lock();
            interpret(packet, state->cueDir, state->cuePower, state->cueAiming);
        });

        client_->registerHandle(PacketType::PlayerTurn, [this](sf::Packet& packet) {
            auto state = state_.lock();
            interpret(packet, state->turn);
        });

        client_->connect(host, PoolConstants::port);

        auto now = std::chrono::steady_clock::now();
        auto end = now + std::chrono::milliseconds(5000);
        while (std::chrono::steady_clock::now() < end) {
            if (client_->isConnected()) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        if (!client_->isConnected()) throw std::runtime_error("client failed to connect!");

        sf::Packet packet;
        packet << PacketType::Connection;
        client_->send(packet);
    }

    // game calculations
    void computePocketCenters() {
        std::vector<Vector> pockets;
        auto state = state_.lock();
        state->pockets.clear();
        state->pockets.reserve(state->displays * 4);

        double offset = 1730.0;
        state->pockets.push_back(Vector{0.0, 0.0});
        state->pockets.push_back(Vector{0.0, 670});
        state->pockets.push_back(Vector{1730.0, 0.0});
        state->pockets.push_back(Vector{1730.0, 670});

        for (unsigned int i = 1; i < state->displays; ++i) {
            offset += i == state->displays - 1 ? 1703.0 : 1920.0;
            state->pockets.push_back(Vector{0.0 + offset, 0.0});
            state->pockets.push_back(Vector{0.0 + offset, 670});
        }
    }

    bool isPocketed(const Ball& ball) {
        auto state = state_.lock();
        for (const auto& pocket : state->pockets) {
            const auto dist = (ball.pos - pocket).magnitude();
            if (dist <= (PoolConstants::pocketRadius + ball.radius)) return true;
        }

        return false;
    }



    void input() {
        while (const std::optional event = this->window_.pollEvent()) {
            auto state = state_.lock();
            controller_.update(event);
            if (event->is<sf::Event::Closed>())
                this->window_.close();

            if (auto* click = event->getIf<sf::Event::MouseButtonPressed>()) {
                    if (state->turn == PlayerTurn::PlacingCueBall);
                    // TODO: display should show a picture of the ball
                    //cue_.emplace(state->balls.emplace_back(Vector(mouse.x, mouse.y), Vector{0, 0}, 0));
            }

            if (auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape) this->window_.close();

                if (state->role != Role::Host) continue;

                switch (key->code) {
                // case sf::Keyboard::Key::Space:
                //         display_.value().scale(state->balls.emplace_back(Vector{100, 491.0}, Vector{2800, -70}, 0));
                //         break;
                case sf::Keyboard::Key::LShift:
                        create_arranged_balls(state->balls);
                        display_.value().scaleBalls(state->balls);
                        cue_.reset();
                        state->turn = PlayerTurn::PlacingCueBall;
                        return;
                //case sf::Keyboard::Key::M: startMusicLeft(); break; // TODO toggle music
                default: break;
                }
            }

            if (auto* button = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (button->button == sf::Mouse::Button::Left) {
                    sf::Vector2i mouse = sf::Mouse::getPosition();
                    cue_.emplace(state->balls.emplace_back(Vector(mouse.x, mouse.y), Vector{0, 0}, 0));
                    display_.value().scale(cue_.value());
                }
            }
            if (auto* button = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (button->button == sf::Mouse::Button::Left) {
                    if (!cue_.has_value()) continue;
                    auto& cue = cue_->get();

                    sf::Vector2i mouse = sf::Mouse::getPosition();
                    Vector mouse_pos = Vector(mouse.x, mouse.y);
                    cue.vel += (cue.pos - Vector(mouse.x, mouse.y)) * 14;
                }
            }
        }
        auto state = state_.lock();
        if (state->role == Role::Host) {
            sf::Vector2f dir = controller_.direction();
            Vector stickDir{-dir.x, -dir.y};
            state->cueDir = Vector{static_cast<unit_t>(dir.x), static_cast<unit_t>(dir.y)};
            state->cuePower = std::clamp(controller_.power(), 0.0f, 1.0f);
            state->cueAiming = stickDir.magnitude() > 0.05f;

            if (state->turn == PlayerTurn::Aiming && controller_.hitPressed() && stickDir.magnitude() > 0.05f) {
                state->turn = PlayerTurn::Physics;
                float speed = 3000.0f * std::max(0.1f, state->cuePower);
                stickDir = stickDir.normalized() * speed;
                for (Ball& ball : state->balls) {
                    if (ball.number == 0) { ball.vel = stickDir; break; }
                }
            }
            if (state->turn == PlayerTurn::PlacingCueBall) {
                cue_->get().pos -= stickDir * 4;
                if (controller_.hitPressed()) state->turn = PlayerTurn::Aiming;
            }
        }
    }
};
