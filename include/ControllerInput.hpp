#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Joystick.hpp>
#include <optional>

class ControllerInput {
public:
    explicit ControllerInput(unsigned int joystickIndex = 0);

    void update(const std::optional<sf::Event> & event);

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
    bool triggerModeSet_{false};
    bool triggerCenteredAt50_{false};

    float readAxis(sf::Joystick::Axis axis) const;
    bool hasAxis(sf::Joystick::Axis axis) const;
};
