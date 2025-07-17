#include "game.h"
#include "trig_tables.h"
#include <windows.h>
#include <cmath>

int gameMap[MAP_SIZE][MAP_SIZE] = {
    {1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,1},
    {1,0,1,0,0,1,0,1},
    {1,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1},
    {1,0,1,0,0,1,0,1},
    {1,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1}
};

Player player;

void InitGame() {
    player = {2.0f, 2.0f, 0.0f};
}

bool CanMoveTo(float x, float y) {
    int mapX = static_cast<int>(x);
    int mapY = static_cast<int>(y);
    
    return !(mapX < 0 || mapX >= MAP_SIZE || 
             mapY < 0 || mapY >= MAP_SIZE || 
             gameMap[mapX][mapY] != 0);
}

void UpdateGame(float deltaTime) {
    int angleIndex = static_cast<int>(player.angle * 180.0f / 3.14159f * 10) % 3600;
    float lookX = cosTable[angleIndex];
    float lookY = sinTable[angleIndex];
    
    float perpX = lookY;
    float perpY = -lookX;

    float moveX = 0.0f, moveY = 0.0f;
    
    if (GetAsyncKeyState('W') & 0x8000) {
        moveX += lookX * PLAYER_SPEED * deltaTime;
        moveY += lookY * PLAYER_SPEED * deltaTime;
    }
    if (GetAsyncKeyState('S') & 0x8000) {
        moveX -= lookX * PLAYER_SPEED * deltaTime;
        moveY -= lookY * PLAYER_SPEED * deltaTime;
    }
    
    if (GetAsyncKeyState('D') & 0x8000) {
        moveX -= perpX * PLAYER_SPEED * deltaTime;
        moveY -= perpY * PLAYER_SPEED * deltaTime;
    }
    if (GetAsyncKeyState('A') & 0x8000) {
        moveX += perpX * PLAYER_SPEED * deltaTime;
        moveY += perpY * PLAYER_SPEED * deltaTime;
    }
    
    if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
        player.angle -= ROTATION_SPEED * deltaTime;
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        player.angle += ROTATION_SPEED * deltaTime;
    }
    
    player.angle = fmod(player.angle, 2 * 3.14159f);
    if (player.angle < 0) player.angle += 2 * 3.14159f;
    
    if (CanMoveTo(player.x + moveX, player.y)) {
        player.x += moveX;
    }
    if (CanMoveTo(player.x, player.y + moveY)) {
        player.y += moveY;
    }
}