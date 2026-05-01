#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Joystick.hpp>

class ControllerInput {
public:
    explicit ControllerInput(unsigned int joystickIndex = 0);

    void update();

    bool connected() const;
    sf::Vector2f direction() const;
    float power() const;
    bool hitPressed() const;

private:
    unsigned int index_;
    sf::Vector2f direction_{0.0f, 0.0f};
    float power_{0.0f};
    bool hitPressed_{false};
    bool prevHit_{false};

    float readAxis(sf::Joystick::Axis axis) const;
    bool hasAxis(sf::Joystick::Axis axis) const;
};
