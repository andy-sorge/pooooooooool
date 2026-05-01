#pragma once

#include "Utilities.hpp"

#include <string>

class MainMenu {
public:
    struct Result {
        Role role;
        std::string host;
    };

    MainMenu() = default;

    Result run();
};
