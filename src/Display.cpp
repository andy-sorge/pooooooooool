#include <iostream>

#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/RenderWindow.hpp"

#include "Textures.hpp"
#include "Vector.hpp"
#include "Display.hpp"
#include "Vector.hpp"

Display::Display(State& state, sf::RenderWindow& window) :
state_(state), window_(window), border_(sf::Sprite(getTableBorder(TableSegment::Left))), top_(sf::Sprite(getTableTop(TableSegment::Left))) {
    update();
}

void Display::update() {
    if (state_.index == 0) segment_ = TableSegment::Left;
    else if (state_.index == state_.displays - 1) segment_ = TableSegment::Right;
    else segment_ = TableSegment::Center;
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
}

void Display::scaleBalls() {
    for (auto& ball : state_.balls) ball.setScale({ scale_, scale_ });
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
    for (Ball& ball: state_.balls) this->drawBall(ball);

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
    unsigned int logicalWidth = 1703;
    if (state_.displays == 1) logicalWidth = 1703;
    else if (state_.displays == 2) logicalWidth = 1703 * 2;
    else logicalWidth = 1703 * 2 + 1920 * (state_.displays - 2);

    state_.logicalSpace = sf::Vector2u({ logicalWidth, 670 });

    unsigned int displayOffsetX = 0;
    if (state_.index == 0) displayOffsetX = 0;
    else if (state_.index == state_.displays - 1) displayOffsetX = logicalWidth - 1703;
    else displayOffsetX = 1703 + 1920 * (state_.index - 1);

    float baseOffsetX = (state_.index == 0) ? 217.0f : 0.0f;
    this->tableOffset_ = sf::Vector2f({ baseOffsetX - static_cast<float>(displayOffsetX), 205 });
}
