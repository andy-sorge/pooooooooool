#pragma once

#include "SFML/Graphics/Texture.hpp"

enum TableSegment { Left, Center, Right };

void setupTextures();

sf::Texture& resolveTexture(std::size_t ball);

sf::Texture& getTableTop(TableSegment segment);
sf::Texture& getTableBorder(TableSegment segment);

sf::Texture& getUiCrown();
sf::Texture& getUiSolids();
sf::Texture& getUiStripes();

sf::Texture& getCue();
sf::Texture& getBallTriangle();
