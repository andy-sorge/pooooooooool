#pragma once

#include <string>
#include "Display.hpp"

class MainMenu {
public:
    struct Result {
        Role role;
        std::string hostAddress;
    };

    Result run();
};
