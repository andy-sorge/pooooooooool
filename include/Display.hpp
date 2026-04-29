#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include <atomic>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <asio.hpp>
#include "../include/Textures.hpp"
#include "SFML/Graphics.hpp"
#include "Ball.hpp"
#include "PoolNetwork.hpp"

typedef struct rect {
    int x;
    int y;
} Rect;

enum Role {
    HOST,
    CLIENT
};

class Display {
public:
    Display(TableSegment seg, Role role, unsigned int totalDisplays, std::string hostAddress = "127.0.0.1");
    ~Display();

    void update();
    
private:
    Role role_;
    TableSegment seg_;
    
    // display positioning and scale
    sf::Vector2u physicalSize_;
    sf::Vector2u renderedSize_;
    sf::Vector2u renderedOffset_;
    sf::Vector2f tableOffset_;
    sf::Vector2u logicalSize_;
    int displayOffset_;
    float scale_;
    
    void calculateRenderedSize();
    void calculateRenderedOffset();
    void calculateScale();
        
    sf::Sprite tableTop_;
    sf::Sprite tableBorder_;
    
    sf::RenderWindow window_;
    void drawBall(Ball& b);
    
    std::vector<Ball> balls;
    std::vector<Vector> initial_ball_velocities; // for physics ✨

    void setupDisplay();
    
    void calculateTransform();
    void calculateLogical(int totalDisplays);

    void setupNetworking(const std::string& hostAddress);
    void broadcastState();
    void applyNetworkState(const std::vector<PoolBallState>& state);

    std::unique_ptr<asio::io_context> io_;
    std::unique_ptr<PoolServer> server_;
    std::unique_ptr<PoolClient> client_;
    std::thread networkThread_;
    std::mutex ballsMutex_;
    std::atomic<bool> hasNetworkState_{false};
};
