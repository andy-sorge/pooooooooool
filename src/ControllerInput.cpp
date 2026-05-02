#include "../include/ControllerInput.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <optional>


constexpr float deadband = 5.0f;

float triggerPower(float raw, bool centeredAt50) {
    if (centeredAt50) {
        return std::clamp(std::abs((raw - 50.0f) / 50.0f), 0.0f, 1.0f);
    }
    if (raw < 0.0f) return std::clamp(std::abs(raw / 100.0f), 0.0f, 1.0f);
    return std::clamp(raw / 100.0f, 0.0f, 1.0f);
}

float applyDeadzone(float raw) {
    if (std::abs(raw) < deadband) return 0.0f;
    return raw / 100.0f;
}


ControllerInput::ControllerInput(unsigned int joystickIndex)
    : index_(joystickIndex) {}

void ControllerInput::update(const std::optional<sf::Event>& event) {
    if (!connected()) {
        direction_ = {0.0f, 0.0f};
        power_ = 0.0f;
        hitPressed_ = false;
        prevHit_ = false;
        // std::cout << "not connected" << std::endl;
        return;
    }
    if (auto* joystick_move = event->getIf<sf::Event::JoystickMoved>()) {
        if (joystick_move->axis == sf::Joystick::Axis::X) {
            direction_.x = applyDeadzone(joystick_move->position);
        }
        if (joystick_move->axis == sf::Joystick::Axis::Y) {
            direction_.y = applyDeadzone(joystick_move->position);
        }
    }
    hitPressed_ = false;
    if (auto* button_press = event->getIf<sf::Event::JoystickButtonPressed>()) {
        hitPressed_ = true;
    }

    float power = std::sqrt(direction_.x * direction_.x + direction_.y * direction_.y);
    power_ = std::clamp(power, 0.0f, 1.0f);
}

bool ControllerInput::connected() const {
    return sf::Joystick::isConnected(index_);
}

sf::Vector2f ControllerInput::direction() const {
    return direction_;
}

float ControllerInput::power() const {
    return power_;
}

bool ControllerInput::hitPressed() const {
    return hitPressed_;
}

float ControllerInput::readAxis(sf::Joystick::Axis axis) const {
    return sf::Joystick::getAxisPosition(index_, axis);
}

bool ControllerInput::hasAxis(sf::Joystick::Axis axis) const {
    return sf::Joystick::hasAxis(index_, axis);
}
