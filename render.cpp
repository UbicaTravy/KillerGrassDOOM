#include "render.h"
#include "game.h"
#include "trig_tables.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>

void InitTrigTables() {
    if (tablesInitialized) return;
    
    for (int i = 0; i < 3600; i++) {
        float angle = i * 0.1f * 3.14159f / 180.0f;
        cosTable[i] = std::cos(angle);
        sinTable[i] = std::sin(angle);
    }
    tablesInitialized = true;
}

unsigned int* buffer = nullptr;
int bufferWidth = SCREEN_WIDTH;
int bufferHeight = SCREEN_HEIGHT;
HBITMAP hBitmap = nullptr;
HDC hdcMem = nullptr;

unsigned int wallTexture[TEXTURE_SIZE * TEXTURE_SIZE];
unsigned int floorTexture[TEXTURE_SIZE * TEXTURE_SIZE];

// структуры RayData и precomputedRays больше не нужны - все вычисляется на лету

void GenerateTextures() {
    for (int y = 0; y < TEXTURE_SIZE; y++) {
        for (int x = 0; x < TEXTURE_SIZE; x++) {
            int idx = y * TEXTURE_SIZE + x;
            
            bool brick = (x % 16 < 14) && (y % 16 < 14);
            bool mortar = (x % 16 == 14) || (y % 16 == 14);
            
            if (mortar) {
                wallTexture[idx] = RGB(100, 100, 100);
            } else if (brick) {
                wallTexture[idx] = RGB(180, 80, 60);
            } else {
                wallTexture[idx] = RGB(80, 80, 100);
            }
        }
    }
    
    for (int y = 0; y < TEXTURE_SIZE; y++) {
        for (int x = 0; x < TEXTURE_SIZE; x++) {
            int idx = y * TEXTURE_SIZE + x;
            
            bool tile = (x / 16 % 2) ^ (y / 16 % 2);
            
            if (tile) {
                floorTexture[idx] = RGB(100, 100, 150);
            } else {
                floorTexture[idx] = RGB(120, 120, 170);
            }
        }
    }
}

// функция PrecomputeRays больше не нужна - все вычисляется на лету в Render()

void InitRenderer(HDC hdc) {
    if (buffer) delete[] buffer;
    if (hBitmap) DeleteObject(hBitmap);
    if (hdcMem) DeleteDC(hdcMem);
    
    buffer = new unsigned int[bufferWidth * bufferHeight];
    
    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = bufferWidth;
    bmi.bmiHeader.biHeight = -bufferHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    
    hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&buffer, NULL, 0);
    hdcMem = CreateCompatibleDC(hdc);
    SelectObject(hdcMem, hBitmap);
    
    InitTrigTables();
    GenerateTextures();
}

void CleanupRenderer() {
    if (buffer) delete[] buffer;
    if (hBitmap) DeleteObject(hBitmap);
    if (hdcMem) DeleteDC(hdcMem);
    
    buffer = nullptr;
    hBitmap = nullptr;
    hdcMem = nullptr;
}

void Render() {
    // рендер неба
    unsigned int skyColor = RGB(100, 150, 255);
    for (int y = 0; y < bufferHeight/2; y++) {
        unsigned int* row = buffer + y * bufferWidth;
        std::fill(row, row + bufferWidth, skyColor);
    }

    // рендер пола
    for (int y = bufferHeight/2; y < bufferHeight; y++) {
        float p = y - bufferHeight/2;
        float posZ = 0.5f * bufferHeight;
        float rowDistance = posZ / p;

        // вычисляем направление луча для левого и правого края экрана
        float leftAngle = player.angle - FOV/2;
        float rightAngle = player.angle + FOV/2;
        float leftDirX = cos(leftAngle);
        float leftDirY = sin(leftAngle);
        float rightDirX = cos(rightAngle);
        float rightDirY = sin(rightAngle);

        float floorX = player.x + rowDistance * leftDirX;
        float floorY = player.y + rowDistance * leftDirY;
        float floorStepX = rowDistance * (rightDirX - leftDirX) / bufferWidth;
        float floorStepY = rowDistance * (rightDirY - leftDirY) / bufferWidth;

        for (int x = 0; x < bufferWidth; x++) {
            int texX = int(floorX * TEXTURE_SIZE) & (TEXTURE_SIZE - 1);
            int texY = int(floorY * TEXTURE_SIZE) & (TEXTURE_SIZE - 1);

            unsigned int color = floorTexture[texY * TEXTURE_SIZE + texX];
            float distFactor = 1.0f / (1.0f + rowDistance * rowDistance * 0.02f);
            distFactor = std::max(0.2f, distFactor);
            color = RGB(
                int(GetRValue(color) * distFactor),
                int(GetGValue(color) * distFactor),
                int(GetBValue(color) * distFactor)
            );
            buffer[y * bufferWidth + x] = color;

            floorX += floorStepX;
            floorY += floorStepY;
        }
    }

    // рендер стен - правильная перспектива
    for (int x = 0; x < bufferWidth; x++) {
        // правильный расчет угла луча с учетом перспективы
        float cameraX = 2.0f * x / static_cast<float>(bufferWidth) - 1.0f;
        float rayAngle = player.angle + atan(cameraX * tan(FOV/2.0f));
        
        // используем точные значения cos/sin для максимальной точности
        float rayDirX = cos(rayAngle);
        float rayDirY = sin(rayAngle);
        
        // защита от деления на ноль
        if (std::abs(rayDirX) < 0.0001f) rayDirX = 0.0001f;
        if (std::abs(rayDirY) < 0.0001f) rayDirY = 0.0001f;
        
        float deltaDistX = std::abs(1.0f / rayDirX);
        float deltaDistY = std::abs(1.0f / rayDirY);
        
        int stepX = (rayDirX < 0) ? -1 : 1;
        int stepY = (rayDirY < 0) ? -1 : 1;
        
        int mapX = static_cast<int>(player.x);
        int mapY = static_cast<int>(player.y);
        
        float sideDistX, sideDistY;
        
        if (rayDirX < 0) {
            sideDistX = (player.x - mapX) * deltaDistX;
        } else {
            sideDistX = (mapX + 1.0f - player.x) * deltaDistX;
        }
        
        if (rayDirY < 0) {
            sideDistY = (player.y - mapY) * deltaDistY;
        } else {
            sideDistY = (mapY + 1.0f - player.y) * deltaDistY;
        }
        
        bool hit = false;
        int side = 0;
        float perpWallDist = MAX_DEPTH;
        
        // DDA алгоритм для поиска стен
        for (int i = 0; i < 20; i++) {
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            
            if (mapX < 0 || mapX >= MAP_SIZE || mapY < 0 || mapY >= MAP_SIZE) {
                perpWallDist = MAX_DEPTH;
                hit = true;
                break;
            }
            
            if (gameMap[mapX][mapY] > 0) {
                // исправленный расчет перпендикулярного расстояния
                if (side == 0) {
                    perpWallDist = (mapX - player.x + (1 - stepX)/2) / rayDirX;
                } else {
                    perpWallDist = (mapY - player.y + (1 - stepY)/2) / rayDirY;
                }
                hit = true;
                break;
            }
        }
        
        if (hit && perpWallDist < MAX_DEPTH) {
            // исправленный расчет высоты стены
            int lineHeight = static_cast<int>(bufferHeight / perpWallDist);
            int drawStart = std::max(0, -lineHeight/2 + bufferHeight/2);
            int drawEnd = std::min(bufferHeight - 1, lineHeight/2 + bufferHeight/2);
            
            // вычисление координаты текстуры по X
            float wallX;
            if (side == 0) {
                wallX = player.y + perpWallDist * rayDirY;
            } else {
                wallX = player.x + perpWallDist * rayDirX;
            }
            wallX -= std::floor(wallX);
            
            int texX = static_cast<int>(wallX * TEXTURE_SIZE);
            if ((side == 0 && rayDirX > 0) || (side == 1 && rayDirY < 0)) {
                texX = TEXTURE_SIZE - texX - 1;
            }
            
            // Исправленный расчет шага текстуры
            float step = 1.0f * TEXTURE_SIZE / lineHeight;
            float texPos = (drawStart - bufferHeight/2 + lineHeight/2) * step;
            
            for (int y = drawStart; y < drawEnd; y++) {
                int texY = static_cast<int>(texPos) % TEXTURE_SIZE;
                if (texY < 0) texY += TEXTURE_SIZE;
                texPos += step;
                
                unsigned int color = wallTexture[texY * TEXTURE_SIZE + texX];
                
                // затемнение боковых стен
                if (side == 1) {
                    color = RGB(
                        static_cast<int>(GetRValue(color) * 0.7f),
                        static_cast<int>(GetGValue(color) * 0.7f),
                        static_cast<int>(GetBValue(color) * 0.7f)
                    );
                }
                
                // дополнительное затемнение с расстоянием
                float distFactor = 1.0f / (1.0f + perpWallDist * perpWallDist * 0.01f);
                distFactor = std::max(0.3f, distFactor);
                
                color = RGB(
                    static_cast<int>(GetRValue(color) * distFactor),
                    static_cast<int>(GetGValue(color) * distFactor),
                    static_cast<int>(GetBValue(color) * distFactor)
                );
                
                buffer[y * bufferWidth + x] = color;
            }
        }
    }
}