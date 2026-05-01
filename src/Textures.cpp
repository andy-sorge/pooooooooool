#include <array>
#include <cstdlib>

#include "SFML/Graphics/Texture.hpp"

#include "Textures.hpp"

// UI
sf::Texture uiCrown("graphics/other/crown.png");
sf::Texture uiSolids("graphics/other/solids.png");
sf::Texture uiStripes("graphics/other/stripes.png");

// On The Table
sf::Texture cue("graphics/other/cue.png");
sf::Texture ballTriangle("graphics/other/ball-triangle.png");

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
    uiCrown.setSmooth(true);
    uiSolids.setSmooth(true);
    uiStripes.setSmooth(true);
    cue.setSmooth(true);
    ballTriangle.setSmooth(true);

    for (sf::Texture& texture : ballTextures) texture.setSmooth(true);
    for (sf::Texture& texture : tableTops) texture.setSmooth(true);
    for (sf::Texture& texture : tableBorders) texture.setSmooth(true);
}

sf::Texture& resolveTexture(std::size_t ball) {
    return ballTextures[ball];
}

sf::Texture& getTableTop(TableSegment segment) {
    return tableTops[segment];
}

sf::Texture& getTableBorder(TableSegment segment) {
    return tableBorders[segment];
}

sf::Texture& getUiCrown() {
    return uiCrown;
}
sf::Texture& getUiSolids() {
    return uiSolids;
}
sf::Texture& getUiStripes() {
    return uiStripes;
}

sf::Texture& getCue() {
    return cue;
}
sf::Texture& getBallTriangle() {
    return ballTriangle;
}
