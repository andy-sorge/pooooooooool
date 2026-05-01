#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include "SFML/Graphics.hpp"
#include <SFML/Network.hpp>

#include "Textures.hpp"
#include "Ball.hpp"
#include "PoolServer.hpp"
#include "PoolClient.hpp"


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
    Display(TableSegment& seg, Role& role, unsigned int displays, unsigned int index, std::string& hostAddress);

    void update();
private:
    Role role_;
    TableSegment seg_;

    std::size_t displays_;
    std::size_t index_;

    // display positioning and scale:w
    sf::Vector2u physicalSize_;
    sf::Vector2u renderedSize_;
    sf::Vector2u renderedOffset_;
    sf::Vector2f tableOffset_;
    sf::Vector2u logicalSize_;
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

    bool playing_music;

    void setupDisplay();

    void calculateTransform();
    void calculateLogical(int totalDisplays);

    void setupNetworking(const std::string& hostAddress);
    void broadcastState();
    void applyNetworkState(const std::vector<BallState>& state);
    void recalculateLayout();
    std::vector<Vector> pocketCenters() const;
    bool isPocketed(const Ball& ball) const;

    std::unique_ptr<PoolServer> server_ = nullptr;
    std::unique_ptr<PoolClient> client_ = nullptr;
};
