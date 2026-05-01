#include "../include/ControllerInput.hpp"

#include <algorithm>
#include <cmath>


constexpr float deadband = 15.0f;

float normalizeAxis(float raw) {
    float clamped = std::clamp(raw, -100.0f, 100.0f);
    return (clamped + 100.0f) / 200.0f;
}

float applyDeadzone(float raw) {
    if (std::abs(raw) < deadband) return 0.0f;
    return raw / 100.0f;
}


ControllerInput::ControllerInput(unsigned int joystickIndex)
    : index_(joystickIndex) {}

void ControllerInput::update() {
    if (!connected()) {
        direction_ = {0.0f, 0.0f};
        power_ = 0.0f;
        hitPressed_ = false;
        prevHit_ = false;
        return;
    }

    float x = hasAxis(sf::Joystick::Axis::X) ? readAxis(sf::Joystick::Axis::X) : 0.0f;
    float y = hasAxis(sf::Joystick::Axis::Y) ? readAxis(sf::Joystick::Axis::Y) : 0.0f;
    direction_ = { applyDeadzone(x), applyDeadzone(y) };

    float power = 0.0f;
    if (hasAxis(sf::Joystick::Axis::R)) power = std::max(power, normalizeAxis(readAxis(sf::Joystick::Axis::R)));
    if (hasAxis(sf::Joystick::Axis::Z)) power = std::max(power, normalizeAxis(readAxis(sf::Joystick::Axis::Z)));
    if (hasAxis(sf::Joystick::Axis::U)) power = std::max(power, normalizeAxis(readAxis(sf::Joystick::Axis::U)));
    if (hasAxis(sf::Joystick::Axis::V)) power = std::max(power, normalizeAxis(readAxis(sf::Joystick::Axis::V)));
    power_ = power;

    bool hit = sf::Joystick::isButtonPressed(index_, 7);
    hitPressed_ = hit && !prevHit_;
    prevHit_ = hit;
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
