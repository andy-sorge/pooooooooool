#include "SFML/Audio/Sound.hpp"
#include "SFML/Audio/SoundBuffer.hpp"
#include <random>
#include <vector>
#include <deque>

std::array<sf::SoundBuffer, 11> ball_hit_sounds = {
    sf::SoundBuffer("sfx/ball-collision/0.wav"),
    sf::SoundBuffer("sfx/ball-collision/1.wav"),
    sf::SoundBuffer("sfx/ball-collision/2.wav"),
    sf::SoundBuffer("sfx/ball-collision/3.wav"),
    sf::SoundBuffer("sfx/ball-collision/4.wav"),
    sf::SoundBuffer("sfx/ball-collision/5.wav"),
    sf::SoundBuffer("sfx/ball-collision/6.wav"),
    sf::SoundBuffer("sfx/ball-collision/7.wav"),
    sf::SoundBuffer("sfx/ball-collision/8.wav"),
    sf::SoundBuffer("sfx/ball-collision/9.wav"),
    sf::SoundBuffer("sfx/ball-collision/10.wav"),
};
std::array<sf::SoundBuffer, 1> ball_hit_bumper_sounds = {
    sf::SoundBuffer("sfx/ball-collision/0.wav"),
};

// rand stuff yay
std::default_random_engine generator;
template <size_t l>
const sf::SoundBuffer& random_sound(const std::array<sf::SoundBuffer, l>& sounds) {
    std::uniform_int_distribution<int> distr(0,l-1);
    return sounds[distr(generator)];
}

size_t sound_index = 0;
std::array<std::optional<sf::Sound>, 40> sounds = {};

std::deque<sf::Sound> sound_queue;

void playBallHit(float volume) {
    // float pitch_variation = sf::Randomizer::Random(0.9f, 1.1f);
    // std::cout <<
    const auto& sound_used = random_sound(ball_hit_sounds);

    sound_queue.emplace_back(sound_used); // TODO: queue either needs seen to completion or use the old system below
    sound_queue.back().setVolume(volume);

    // sounds[sound_index % sounds.max_size()].emplace(sf::Sound(sound_used));
    // auto& sound = sounds[sound_index % sounds.max_size()];
    // sound->setVolume(volume);
    // sound->play();
    //
    // // sounds[(sound_index+1) % sounds.max_size()].emplace(sf::Sound(sound_used));
    // // auto& sound_up = sounds[(sound_index+1) % sounds.max_size()];
    // // sound_up->setVolume(volume);
    // // sound_up->setPitch(1.8);
    //
    // sound_index += 1;
}
