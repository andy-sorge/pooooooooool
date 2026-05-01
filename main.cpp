// #include <SFML/Window.hpp>
#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include <span>

#include "message.hpp"
#include "server.hpp"
#include "client.hpp"
#include "Game.hpp"

#define ADDRESS "127.0.0.1"
#define PORT 58008
#define PORT_STRING "58008"

// example of a protocol, has only one message type
struct TestProtocol {
	static constexpr std::size_t headerLength = 5;

	enum class MessageType : uint8_t {
		Test  = 0x01
	};

	static MessageType type(std::span<const uint8_t, headerLength> header) {
		return static_cast<MessageType>(header[0]);
	}

	static uint32_t bodyLength(std::span<const uint8_t, headerLength> header) {
		return (header[1] << 24) | (header[2] << 16) | (header[3] << 8) | header[4];
	}

	static void encodeHeader(std::array<uint8_t, 5>& header, MessageType type, uint32_t bodyLength) {
		header[0] = static_cast<uint8_t>(type);
		header[1] = bodyLength >> 24;
		header[2] = bodyLength >> 16;
		header[3] = bodyLength >> 8;
		header[4] = bodyLength;
	}
};

#include "include/Display.hpp"
#include "include/MainMenu.hpp"

template <class T>
T dot_product(T x1, T y1, T x2, T y2) {
    return x1*x2 + y1*y2;
}

int main(int argc, char** argv) {
    Game game;
    game.mainloop();
    return 0;

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
