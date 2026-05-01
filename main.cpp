// #include <SFML/Window.hpp>
#include <iostream>
#include <string>

#include "include/Display.hpp"
#include "include/MainMenu.hpp"

template <class T>
T dot_product(T x1, T y1, T x2, T y2) {
    return x1*x2 + y1*y2;
}

int main(int argc, char** argv) {
    Role role = HOST;
    TableSegment seg = LEFT;
    unsigned int totalDisplays = 1;
    unsigned int displayIndex = 0;
    std::string hostAddress = "127.0.0.1";
    bool useMenu = (argc == 1);

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--menu") {
            useMenu = true;
        } else if (arg == "--host") {
            role = HOST;
            useMenu = false;
        } else if (arg == "--client") {
            role = CLIENT;
            useMenu = false;
            if (i + 1 < argc) {
                hostAddress = argv[++i];
            }
        } else if (arg == "--segment" && i + 1 < argc) {
            std::string value = argv[++i];
            if (value == "left") seg = LEFT;
            else if (value == "center") seg = CENTER;
            else if (value == "right") seg = RIGHT;
        } else if (arg == "--displays" && i + 1 < argc) {
            totalDisplays = static_cast<unsigned int>(std::stoul(argv[++i]));
        } else if (arg == "--index" && i + 1 < argc) {
            displayIndex = static_cast<unsigned int>(std::stoul(argv[++i]));
        }
    }

    if (useMenu) {
        MainMenu menu;
        auto result = menu.run();
        role = result.role;
        hostAddress = result.hostAddress;
    }

    if (totalDisplays < 1) totalDisplays = 1;
    if (displayIndex >= totalDisplays) displayIndex = totalDisplays - 1;

    if (displayIndex == 0) seg = LEFT;
    else if (displayIndex == totalDisplays - 1) seg = RIGHT;
    else seg = CENTER;

    unsigned int displayCountForRole = (role == HOST) ? 1 : totalDisplays;
    Display d(seg, role, displayCountForRole, displayIndex, hostAddress);
    d.update();
}
