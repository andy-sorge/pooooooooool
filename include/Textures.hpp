#pragma once

#include <array>
#include <SFML/Graphics/Texture.hpp>

enum TableSegment {
    LEFT,
    CENTER,
    RIGHT
};

sf::Texture& resolveTexture(int ballNumber);

sf::Texture& getTableTop(TableSegment seg);

sf::Texture& getTableBorder(TableSegment seg);