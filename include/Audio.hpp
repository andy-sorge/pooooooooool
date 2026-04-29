#pragma once
// Sound volumes range from 0 to 100

// Two balls hitting eachother
void playBallHitBall(float volume=100.0);

// Cue hitting a ball (controlled by a player)
void playCueHitBall(float volume=100.0);

// Ball hitting a wall
void playBallHitCushions(float volume=100.0);

// Ball sunk into a pocket
void playBallSunk(int num_balls_already_sunk);

void startMusicLeft();

void startMusicRight();

bool getMusicStarted();
