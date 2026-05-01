#pragma once
#include "Display.hpp"
#include "Vector.hpp"

enum GameState {
    Lobby, // default state
    UserInput, // selecting how to hit ball
    Balling, // balls movin & allat
    GameEnd // game over, display winner & loser
};

enum PlayerTurn {
    None,
    Player1,
    Player2
};

// things to sync over the network
struct Sync {
    GameState state = GameState::Lobby;
    PlayerTurn turn = None;
    uint16_t totalDisplays = 1;
    bool playingMusic = false;
    std::vector<Ball> balls;
};

class Game {
    Sync sync_;
    uint16_t displayIndex_ = 0;
    std::optional<Display> display_;

    void startDisplay();
    void startGame();
    void syncState();
public:
    void mainloop();
};