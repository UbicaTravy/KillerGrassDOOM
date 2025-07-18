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

struct RayData {
    float rayDirX;
    float rayDirY;
    float deltaDistX;
    float deltaDistY;
    int stepX;
    int stepY;
};

RayData precomputedRays[SCREEN_WIDTH];

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

void PrecomputeRays() {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        float rayAngle = player.angle - FOV/2.0f + (static_cast<float>(x)/SCREEN_WIDTH) * FOV;
        rayAngle = fmod(rayAngle + 10*3.14159f, 2*3.14159f);
        int angleIdx = static_cast<int>(rayAngle * 180.0f / 3.14159f * 10) % 3600;
        
        RayData rd;
        rd.rayDirX = cosTable[angleIdx];
        rd.rayDirY = sinTable[angleIdx];
        
        rd.deltaDistX = std::abs(1 / rd.rayDirX);
        rd.deltaDistY = std::abs(1 / rd.rayDirY);
        
        rd.stepX = (rd.rayDirX < 0) ? -1 : 1;
        rd.stepY = (rd.rayDirY < 0) ? -1 : 1;
        
        precomputedRays[x] = rd;
    }
}

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
    PrecomputeRays();

    unsigned int skyColor = RGB(100, 150, 255);
    for (int y = 0; y < bufferHeight/2; y++) {
        unsigned int* row = buffer + y * bufferWidth;
        std::fill(row, row + bufferWidth, skyColor);
    }

    for (int y = bufferHeight/2; y < bufferHeight; y++) {
        float rowDist = (0.5f * bufferHeight) / (y - bufferHeight/2 + 0.1f);
        
        float floorStepX = rowDist * (cosTable[static_cast<int>((player.angle + FOV/2) * 180.0f / 3.14159f * 10) % 3600] - 
                           cosTable[static_cast<int>((player.angle - FOV/2) * 180.0f / 3.14159f * 10) % 3600]) / bufferWidth;
        
        float floorStepY = rowDist * (sinTable[static_cast<int>((player.angle + FOV/2) * 180.0f / 3.14159f * 10) % 3600] - 
                           sinTable[static_cast<int>((player.angle - FOV/2) * 180.0f / 3.14159f * 10) % 3600]) / bufferWidth;
        
        float floorX = player.x + rowDist * cosTable[static_cast<int>((player.angle - FOV/2) * 180.0f / 3.14159f * 10) % 3600];
        float floorY = player.y + rowDist * sinTable[static_cast<int>((player.angle - FOV/2) * 180.0f / 3.14159f * 10) % 3600];
        
        for (int x = 0; x < bufferWidth; x++) {
            int texX = static_cast<int>(floorX * TEXTURE_SIZE) % TEXTURE_SIZE;
            int texY = static_cast<int>(floorY * TEXTURE_SIZE) % TEXTURE_SIZE;
            
            if (texX < 0) texX += TEXTURE_SIZE;
            if (texY < 0) texY += TEXTURE_SIZE;
            
            unsigned int color = floorTexture[texY * TEXTURE_SIZE + texX];
            
            float distFactor = 1.0f / (1.0f + rowDist * 0.1f);
            color = RGB(
                GetRValue(color) * distFactor,
                GetGValue(color) * distFactor,
                GetBValue(color) * distFactor
            );
            
            buffer[y * bufferWidth + x] = color;
            
            floorX += floorStepX;
            floorY += floorStepY;
        }
    }

    for (int x = 0; x < bufferWidth; x++) {
        const RayData& rd = precomputedRays[x];
        
        int mapX = static_cast<int>(player.x);
        int mapY = static_cast<int>(player.y);
        
        float sideDistX, sideDistY;
        
        if (rd.rayDirX < 0) {
            sideDistX = (player.x - mapX) * rd.deltaDistX;
        } else {
            sideDistX = (mapX + 1.0f - player.x) * rd.deltaDistX;
        }
        
        if (rd.rayDirY < 0) {
            sideDistY = (player.y - mapY) * rd.deltaDistY;
        } else {
            sideDistY = (mapY + 1.0f - player.y) * rd.deltaDistY;
        }
        
        bool hit = false;
        int side = 0;
        float perpWallDist = MAX_DEPTH;
        
        for (int i = 0; i < 20; i++) {
            if (sideDistX < sideDistY) {
                sideDistX += rd.deltaDistX;
                mapX += rd.stepX;
                side = 0;
            } else {
                sideDistY += rd.deltaDistY;
                mapY += rd.stepY;
                side = 1;
            }
            
            if (mapX < 0 || mapX >= MAP_SIZE || mapY < 0 || mapY >= MAP_SIZE) {
                perpWallDist = MAX_DEPTH;
                hit = true;
                break;
            }
            
            if (gameMap[mapX][mapY] > 0) {
                if (side == 0) {
                    perpWallDist = (mapX - player.x + (1 - rd.stepX)/2) / rd.rayDirX;
                } else {
                    perpWallDist = (mapY - player.y + (1 - rd.stepY)/2) / rd.rayDirY;
                }
                hit = true;
                break;
            }
        }
        
        if (hit && perpWallDist < MAX_DEPTH) {
            int lineHeight = static_cast<int>(bufferHeight / perpWallDist);
            int drawStart = std::max(0, -lineHeight/2 + bufferHeight/2);
            int drawEnd = std::min(bufferHeight - 1, lineHeight/2 + bufferHeight/2);
            
            float wallX;
            if (side == 0) {
                wallX = player.y + perpWallDist * rd.rayDirY;
            } else {
                wallX = player.x + perpWallDist * rd.rayDirX;
            }
            wallX -= std::floor(wallX);
            
            int texX = static_cast<int>(wallX * TEXTURE_SIZE);
            if ((side == 0 && rd.rayDirX > 0) || (side == 1 && rd.rayDirY < 0)) {
                texX = TEXTURE_SIZE - texX - 1;
            }
            
            float step = 1.0f * TEXTURE_SIZE / lineHeight;
            float texPos = (drawStart - bufferHeight/2 + lineHeight/2) * step;
            
            for (int y = drawStart; y < drawEnd; y++) {
                int texY = static_cast<int>(texPos) % TEXTURE_SIZE;
                texPos += step;
                
                unsigned int color = wallTexture[texY * TEXTURE_SIZE + texX];
                
                if (side == 1) {
                    color = RGB(
                        GetRValue(color) * 0.7f,
                        GetGValue(color) * 0.7f,
                        GetBValue(color) * 0.7f
                    );
                }
                
                buffer[y * bufferWidth + x] = color;
            }
        }
    }
}