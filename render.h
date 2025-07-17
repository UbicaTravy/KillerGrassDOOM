#pragma once

#include <windows.h>

const int TEXTURE_SIZE = 64;

extern unsigned int* buffer;
extern int bufferWidth;
extern int bufferHeight;
extern HBITMAP hBitmap;
extern HDC hdcMem;

extern unsigned int wallTexture[TEXTURE_SIZE * TEXTURE_SIZE];
extern unsigned int floorTexture[TEXTURE_SIZE * TEXTURE_SIZE];

void InitRenderer(HDC hdc);

void CleanupRenderer();

void Render();

void GenerateTextures();