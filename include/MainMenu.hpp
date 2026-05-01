#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <string>

#include "Utilities.hpp"

class MainMenu {
public:
    struct Result {
        Role role;
        std::string host;
    };

    MainMenu(sf::RenderWindow& window);

    Result run();
private:
    sf::RenderWindow& window_;
};
