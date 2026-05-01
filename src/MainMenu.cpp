#include "../include/MainMenu.hpp"
#include "../include/Textures.hpp"

#include <SFML/Graphics.hpp>
#include <iostream>

#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Text.hpp"

#include "MainMenu.hpp"

void centerText(sf::Text& text, const sf::Vector2f& center) {
    auto bounds = text.getLocalBounds();
    text.setOrigin({ bounds.position.x + bounds.size.x / 2.0f, bounds.position.y + bounds.size.y / 2.0f });
    text.setPosition(center);
}

MainMenu::MainMenu(sf::RenderWindow& window) : window_(window) {}

MainMenu::Result MainMenu::run() {
    Result result{Role::Host, "127.0.0.1"};

    window_.setFramerateLimit(60);

    sf::Font font;
    if (font.openFromFile("Roboto-Regular.ttf")) std::cerr << "Failed to load Roboto-Regular.ttf for menu\n";

    sf::Sprite crown(getUiCrown());
    crown.setScale({0.4, 0.4});
    crown.setPosition({200, 20});

    sf::Text title(font, "POOOOOOOOOOL", 48);
    title.setFillColor(sf::Color::White);
    centerText(title, {400.0f, 80.0f});

    sf::RectangleShape hostButton({300.0f, 60.0f});
    hostButton.setFillColor(sf::Color(60, 60, 60));
    hostButton.setOutlineThickness(2.0f);
    hostButton.setOutlineColor(sf::Color::White);
    hostButton.setPosition({250.0f, 200.0f});

    sf::RectangleShape joinButton({300.0f, 60.0f});
    joinButton.setFillColor(sf::Color(60, 60, 60));
    joinButton.setOutlineThickness(2.0f);
    joinButton.setOutlineColor(sf::Color::White);
    joinButton.setPosition({250.0f, 290.0f});

    sf::RectangleShape ipBox({420.0f, 50.0f});
    ipBox.setFillColor(sf::Color(30, 30, 30));
    ipBox.setOutlineThickness(2.0f);
    ipBox.setOutlineColor(sf::Color(120, 120, 120));
    ipBox.setPosition({190.0f, 390.0f});

    sf::Text hostText(font, "HOST", 28);
    hostText.setFillColor(sf::Color::White);
    centerText(hostText, {400.0f, 230.0f});

    sf::Text joinText(font, "JOIN", 28);
    joinText.setFillColor(sf::Color::White);
    centerText(joinText, {400.0f, 320.0f});

    sf::Text ipLabel(font, "Host IP", 18);
    ipLabel.setFillColor(sf::Color(200, 200, 200));
    ipLabel.setPosition({190.0f, 360.0f});

    std::string ipInput;
    bool ipActive = false;

    while (window_.isOpen()) {
        while (const auto event = window_.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window_.close();

            if (auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape) window_.close();
                else if (key->code == sf::Keyboard::Key::Enter && ipActive) {
                    result.role = Role::Client;
                    result.host = ipInput.empty() ? "127.0.0.1" : ipInput;
                    return result;
                }
            }

            if (auto* text = event->getIf<sf::Event::TextEntered>()) {
                if (!ipActive) continue;
                if (text->unicode == 8) {
                    if (!ipInput.empty()) ipInput.pop_back();
                } else if (text->unicode < 128) {
                    char c = static_cast<char>(text->unicode);
                    if ((c >= '0' && c <= '9') || c == '.' || c == ':' ) {
                        if (ipInput.size() < 32) ipInput.push_back(c);
                    }
                }
            }

            if (auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouse->button == sf::Mouse::Button::Left) {
                    sf::Vector2f pos(static_cast<float>(mouse->position.x), static_cast<float>(mouse->position.y));
                    if (hostButton.getGlobalBounds().contains(pos)) {
                        result.role = Role::Host;
                        result.host = "127.0.0.1";
                        return result;
                    }
                    if (joinButton.getGlobalBounds().contains(pos)) {
                        result.role = Role::Client;
                        result.host = ipInput.empty() ? "127.0.0.1" : ipInput;
                        return result;
                    }
                    if (ipBox.getGlobalBounds().contains(pos)) {
                        ipActive = true;
                        ipBox.setOutlineColor(sf::Color::White);
                    } else {
                        ipActive = false;
                        ipBox.setOutlineColor(sf::Color(120, 120, 120));
                    }
                }
            }
        }

        sf::Text ipText(font, ipInput.empty() ? "127.0.0.1" : ipInput, 22);
        ipText.setFillColor(ipInput.empty() ? sf::Color(140, 140, 140) : sf::Color::White);
        ipText.setPosition({205.0f, 400.0f});

        window.clear(sf::Color(10, 80, 10));
        window.draw(crown);
        window.draw(title);
        window.draw(hostButton);
        window.draw(joinButton);
        window.draw(ipBox);
        window.draw(hostText);
        window.draw(joinText);
        window.draw(ipLabel);
        window.draw(ipText);
        window.display();
    }

    return result;
}
