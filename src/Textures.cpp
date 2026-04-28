#include "../include/Textures.hpp"

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

sf::Texture& resolveTexture(int ballNumber) {
    return ballTextures.at(ballNumber);
}