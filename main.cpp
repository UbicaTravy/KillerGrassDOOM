#include "game.h"
#include "render.h"
#include "trig_tables.h"
#include <windows.h>
#include <ctime>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"KGDOOM";
    
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    
    RegisterClassW(&wc);
    
    RECT rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    int winWidth = rect.right - rect.left;
    int winHeight = rect.bottom - rect.top;
    
    HWND hwnd = CreateWindowExW(0, CLASS_NAME, L"KG DOOM v0.1",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        winWidth, winHeight,
        NULL, NULL, hInstance, NULL);
    
    if (!hwnd) return 0;
    
    SetWindowTextW(hwnd, L"KillerGrass DOOM v0.1");
    
    InitTrigTables();
    InitGame();
    HDC hdc = GetDC(hwnd);
    InitRenderer(hdc);
    ReleaseDC(hwnd, hdc);
    
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    clock_t lastTime = clock();
    MSG msg = {};
    
    while (true) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                break;
            
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            clock_t currentTime = clock();
            float deltaTime = static_cast<float>(currentTime - lastTime) / CLOCKS_PER_SEC;
            lastTime = currentTime;
            
            UpdateGame(deltaTime);
            Render();
            
            HDC hdc = GetDC(hwnd);
            BitBlt(hdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, hdcMem, 0, 0, SRCCOPY);
            ReleaseDC(hwnd, hdc);
        }
    }
    
    CleanupRenderer();
    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                PostQuitMessage(0);
            }
            return 0;
            
        case WM_GETMINMAXINFO: {
            MINMAXINFO* mmi = (MINMAXINFO*)lParam;
            RECT rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
            mmi->ptMinTrackSize.x = rect.right - rect.left;
            mmi->ptMinTrackSize.y = rect.bottom - rect.top;
            mmi->ptMaxTrackSize.x = rect.right - rect.left;
            mmi->ptMaxTrackSize.y = rect.bottom - rect.top;
            return 0;
        }
            
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}