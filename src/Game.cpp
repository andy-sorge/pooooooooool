#include "../include/Game.hpp"

void Game::startDisplay() {
    // TODO: refactor display to use the info from sync instead of copying it
    std::string host = "127.0.0.1";
    display_.emplace(TableSegment::CENTER, Role::HOST, sync_.totalDisplays, displayIndex_, host);
}

void Game::startGame() {
    startDisplay();
    sync_.state = UserInput;
}

void Game::syncState() {
    // TODO: push to clients / receive from server
}

void Game::mainloop() {
    syncState();
    switch (sync_.state) {
        case GameState::Lobby:
            // show menu for connecting
            // take input to start game & change thingy
            if (display_.has_value()) display_.reset();
            startGame();
            break;    GameState state = GameState::Lobby;
            PlayerTurn turn = None;
        case GameState::UserInput:
            // clearly display correct user's turn (depends on what sunk last time of course)
            break;
        case GameState::Balling:
            // so here we just simulate the balls moving & set state for next player input
            break;
        case GameState::GameEnd:
            break;
        default:
            // bad
            break;
    }

    if (display_.has_value()) {
        display_->update();
    }
}
