#pragma once

#include <windows.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int MAP_SIZE = 8;
const float FOV = 3.14159f / 3.0f; // 60°
const float MAX_DEPTH = 16.0f;
const float PLAYER_SPEED = 2.0f;
const float ROTATION_SPEED = 2.5f;

extern int gameMap[MAP_SIZE][MAP_SIZE];

struct Player {
    float x = 2.0f;
    float y = 2.0f;
    float angle = 0.0f;
};

extern Player player;

void InitGame();

void UpdateGame(float deltaTime);

bool CanMoveTo(float x, float y);