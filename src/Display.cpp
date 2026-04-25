#include "../include/Display.hpp"

Display::Display()
// window(sf::VideoMode({800, 600}), "Game Window")
{
    balls.emplace_back(sf::Color::Yellow);
}

Display::~Display() {
    
}

void Display::setupDisplay() {
    
}

void Display::update() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "My window", sf::State::Fullscreen);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape) {
                    window.close();
                }
            }
        }

        window.clear(sf::Color::Black);

        for (Ball& ball: balls) {
            window.draw(ball);
        }

        window.display();
        std::cout << window.getSize().x << " " << window.getSize().y << std::endl;
    }

}