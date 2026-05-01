#pragma once

#include <stdexcept>
#include <variant>

#include "Client.hpp"
#include "Display.hpp"
#include "Vector.hpp"
#include "Physics.hpp"
#include "Utilities.hpp"
#include "PoolServer.hpp"
#include "PoolClient.hpp"
#include "Network.hpp"
#include "MainMenu.hpp"

class Game {
public:
    void run() {
        // startup, get all values
        MainMenu menu;
        auto result = menu.run();

        if (result.role == Role::Host) host();
        else client(result.host);

        auto running = true;
        while(running) {
            display_.update()
            if (state_.role == Role::Host) broadcastState();
        }
    }
private:
    enum Scene { // state machine
        Menu, // default state
        UserInput, // selecting how to hit ball
        Balling, // balls movin & allat
        GameEnd // game over, display winner & loser
    };

    Scene scene_ = Game::Scene::Menu;
    State state_; // game state, ball positions, role, etc.

    sf::RenderWindow window_;
    Display display_;
    Physics physics_;

    std::shared_ptr<PoolServer> server_;
    std::shared_ptr<PoolClient> client_;

    void host() {
        server_ = std::make_unique<PoolServer>(Network::port);
        server_->start();

        server_->registerConnection([this]() {
            std::cout << "client connected" << std::endl;
            server_->send(packageDisplays(state_.displays));
        });
    }

    void client(std::string& host) {
        std::cout << "i am a client!" << std::endl;
        client_ = std::make_unique<PoolClient>();
        client_->connect(host, Network::port);

        auto now = std::chrono::steady_clock::now();
        auto end = now + std::chrono::milliseconds(5000);
        while (std::chrono::steady_clock::now() < end) {
            if (client_->isConnected()) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        if (!client_->isConnected()) throw std::runtime_error("client failed to connect!");

        client_->registerHandle(PacketType::GameState, [this](sf::Packet& packet) {
            std::vector<BallState> state;
            interpretBalls(packet, state);
            applyNetworkState(state);
        });
        client_->registerHandle(PacketType::Connection, [this](sf::Packet& packet) {
            interpretDisplays(packet, state_.displays);
            stat_.index = state_.displays;
            recalculateLayout();
        });
    }

    void startDisplay() {
        // TODO: refactor display to use the info from sync instead of copying it
        std::string host = "127.0.0.1";
        display_.emplace(TableSegment::Center, Role::Host, state_.displays, state_.index, host);
    }

    void broadcastState() {
        if (state_.role != Role::Host) throw std::runtime_error("only host can broadcast state");

        std::vector<BallState> state;
        state.reserve(balls.size());
        for (const auto& ball : balls)  state.push_back(BallState{ball.number, ball.pos.x, ball.pos.y, ball.vel.x, ball.vel.y});

        server_->send(packageBalls(state));
    }

    // game calculations
    void computePocketCenters() const {
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
};
