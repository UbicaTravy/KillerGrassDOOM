#pragma once

#include <windows.h>

const int TEXTURE_SIZE = 64;

// Буфер отрисовки
extern unsigned int* buffer;
extern int bufferWidth;
extern int bufferHeight;
extern HBITMAP hBitmap;
extern HDC hdcMem;

// Текстуры
extern unsigned int wallTexture[TEXTURE_SIZE * TEXTURE_SIZE];
extern unsigned int floorTexture[TEXTURE_SIZE * TEXTURE_SIZE];

// Инициализация рендерера
void InitRenderer(HDC hdc);

// Очистка ресурсов рендерера
void CleanupRenderer();

// Основная функция рендеринга
void Render();

// Генерация текстур
void GenerateTextures();