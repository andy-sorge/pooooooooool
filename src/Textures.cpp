#include "Textures.hpp"
#include <SFML/Graphics/Texture.hpp>

std::array<sf::Texture, 16> ballTextures = {
    sf::Texture("graphics/balls/0.png"),
    sf::Texture("graphics/balls/1.png"),
    sf::Texture("graphics/balls/2.png"),
    sf::Texture("graphics/balls/3.png"),
    sf::Texture("graphics/balls/4.png"),
    sf::Texture("graphics/balls/5.png"),
    sf::Texture("graphics/balls/6.png"),
    sf::Texture("graphics/balls/7.png"),
    sf::Texture("graphics/balls/8.png"),
    sf::Texture("graphics/balls/9.png"),
    sf::Texture("graphics/balls/10.png"),
    sf::Texture("graphics/balls/11.png"),
    sf::Texture("graphics/balls/12.png"),
    sf::Texture("graphics/balls/13.png"),
    sf::Texture("graphics/balls/14.png"),
    sf::Texture("graphics/balls/15.png")
};

std::array<sf::Texture, 3> tableTops = {
    sf::Texture("graphics/table/under.png", false, sf::IntRect({0, 0}, {1920, 1080})),
    sf::Texture("graphics/table/under.png", false, sf::IntRect({1920, 0}, {1920, 1080})),
    sf::Texture("graphics/table/under.png", false, sf::IntRect({3840, 0}, {1920, 1080})),
};

std::array<sf::Texture, 3> tableBorders = {
    sf::Texture("graphics/table/over.png", false, sf::IntRect({0, 0}, {1920, 1080})),
    sf::Texture("graphics/table/over.png", false, sf::IntRect({1920, 0}, {1920, 1080})),
    sf::Texture("graphics/table/over.png", false, sf::IntRect({3840, 0}, {1920, 1080})),
};

void setupTextures() {
    for (sf::Texture& t : ballTextures) {
        t.setSmooth(true);
    }
    for (sf::Texture& t : tableTops) {
        t.setSmooth(true);
    }
    for (sf::Texture& t : tableBorders) {
        t.setSmooth(true);
    }
}

sf::Texture& resolveTexture(int ballNumber) {
    return ballTextures.at(ballNumber);
}

sf::Texture& getTableTop(TableSegment seg) {
    return tableTops[seg];
}

sf::Texture& getTableBorder(TableSegment seg) {
    return tableBorders[seg];
}
