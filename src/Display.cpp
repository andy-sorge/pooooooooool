#include <iostream>
#include "../include/Physics.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include <cmath>
#include "../include/Display.hpp"
#include "../include/Audio.hpp"

#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/RenderWindow.hpp"

#include "Textures.hpp"
#include "Utilities.hpp"
#include "Vector.hpp"
#include "Display.hpp"
#include "Vector.hpp"

Display::Display(Synchronized<State>& state, sf::RenderWindow& window) :
cueTexture_("graphics/other/cue.png"),
cueSprite_(cueTexture_),
state_(state), window_(window), border_(sf::Sprite(getTableBorder(TableSegment::Left))), top_(sf::Sprite(getTableTop(TableSegment::Left))) {
    update();
}

void Display::update() {
    {
        auto state = state_.lock();
        if (state->index == 0) segment_ = TableSegment::Left;
        else if (state->index == state->displays - 1) segment_ = TableSegment::Right;
        else segment_ = TableSegment::Center;
    }
    // table border is 205 at top and bottom
    // table border is 217 at left and right

    this->calculateRenderedSize();
    this->calculateRenderedOffset();
    this->calculateScale();
    this->calculateLayout();

    this->top_ = sf::Sprite(getTableTop(segment_));
    this->border_ = sf::Sprite(getTableBorder(segment_));

    this->border_.setPosition({ static_cast<float>(renderedOffset_.x), static_cast<float>(renderedOffset_.y) });
    this->border_.setScale({ this->scale_, this->scale_});
    this->top_.setPosition({ static_cast<float>(renderedOffset_.x), static_cast<float>(renderedOffset_.y) });
    this->top_.setScale({ this->scale_, this->scale_});
    
    cueSprite_.setScale({ this->scale_, this->scale_ });
    auto size = cueTexture_.getSize();
    cueSprite_.setOrigin({ static_cast<float>(size.x) / 2.0f, 0 });
}

void Display::scaleBalls(std::vector<Ball>& balls) {
    for (auto& ball : balls) ball.setScale({ scale_, scale_ });
}

void Display::drawBall(Ball& ball) {
    ball.setPosition({
        static_cast<float>(ball.pos.x + this->tableOffset_.x) * this->scale_ + this->renderedOffset_.x,
        static_cast<float>(ball.pos.y + this->tableOffset_.y) * this->scale_  + this->renderedOffset_.y,
    });
    this->window_.draw(ball);
}

void Display::render() {
    sf::Clock clock;
    sf::Time dt = sf::Time::Zero;

    this->window_.clear(sf::Color::Black);

    this->window_.draw(this->top_);
    this->window_.draw(this->border_);
    
    if (auto state = state_.lock(); state->role == Host) {
        controller_.update();
        sf::Vector2f dir = controller_.direction();
        Vector shotDir{-dir.x, -dir.y};
        aimDir_ = dir;
        aimPower_ = std::clamp(controller_.power(), 0.0f, 1.0f);
        aiming_ = shotDir.magnitude() > 0.05f;

        if (controller_.hitPressed()) {


            if (shotDir.magnitude() > 0.05) {
                float speed = 3000.0f * std::max(0.1f, aimPower_);

                shotDir = shotDir.normalized() * speed;
                for (Ball& ball : balls) {
                    if (ball.number == 0) {
                        ball.vel = shotDir;
                        break;
                    }
                }
            }
        }
    }


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
    {
        auto state = state_.lock();
        for (Ball& ball: state->balls) this->drawBall(ball);
    }

    window_.display();
}

// calculations
void Display::calculateRenderedSize() {
    this->physicalSize_ = this->window_.getSize();
    this->renderedSize_ = sf::Vector2u({ physicalSize_.x, static_cast<unsigned int>(physicalSize_.x * (9.0 / 16.0)) });
}

void Display::calculateRenderedOffset() {
    this->renderedOffset_ = sf::Vector2u({ physicalSize_.x - renderedSize_.x, physicalSize_.y - renderedSize_.y });
}

void Display::calculateScale() {
    this->scale_ = (float)this->renderedSize_.x / 1920;
}

void Display::calculateLayout() {
    auto state = state_.lock();
    unsigned int logicalWidth = 1703;
    if (state->displays == 1) logicalWidth = 1703;
    else if (state->displays == 2) logicalWidth = 1703 * 2;
    else logicalWidth = 1703 * 2 + 1920 * (state->displays - 2);

    state->logicalSpace = sf::Vector2u({ logicalWidth, 670 });

    unsigned int displayOffsetX = 0;
    if (state->index == 0) displayOffsetX = 0;
    else if (state->index == state->displays - 1) displayOffsetX = logicalWidth - 1703;
    else displayOffsetX = 1703 + 1920 * (state->index - 1);

    float baseOffsetX = (state->index == 0) ? 217.0f : 0.0f;
    this->tableOffset_ = sf::Vector2f({ baseOffsetX - static_cast<float>(displayOffsetX), 205 });
}
