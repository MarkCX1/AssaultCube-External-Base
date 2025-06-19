#include "ESP.h"
#include "offsets.h"
#include <windows.h>
#include <iostream>
#include <cmath>

bool WorldToScreen(Vec3 pos, Vec2& screen, float matrix[16], int width, int height) {
    float clipX = pos.x * matrix[0] + pos.y * matrix[4] + pos.z * matrix[8] + matrix[12];
    float clipY = pos.x * matrix[1] + pos.y * matrix[5] + pos.z * matrix[9] + matrix[13];
    float clipW = pos.x * matrix[3] + pos.y * matrix[7] + pos.z * matrix[11] + matrix[15];
    if (clipW < 0.1f) return false;
    screen.x = (width / 2) * (1 + clipX / clipW);
    screen.y = (height / 2) * (1 - clipY / clipW);
    return true;
}

void DrawESP(HANDLE hProcess, HWND gameWindow, DWORD baseAddress, DWORD entityList) {
    static HWND overlay = nullptr;
    if (!overlay) {
        WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, DefWindowProc, 0, 0, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"Overlay", nullptr };
        RegisterClassEx(&wc);
        RECT clientRect;
        GetClientRect(gameWindow, &clientRect);
        POINT topLeft = { 0, 0 };
        ClientToScreen(gameWindow, &topLeft);
        overlay = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST, L"Overlay", L"ESP",
            WS_POPUP, topLeft.x, topLeft.y, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top,
            nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
        SetLayeredWindowAttributes(overlay, RGB(0, 0, 0), 0, LWA_COLORKEY);
        ShowWindow(overlay, SW_SHOW);
    }

    HDC hdc = GetDC(overlay);
    SetBkMode(hdc, TRANSPARENT);
    RECT rect;
    GetClientRect(overlay, &rect);
    HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(hdc, &rect, brush);
    DeleteObject(brush);

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    // The FOV circle
    const float fovAngle = 30.0f; // Aimbot and ESP FOV pixel size
    float fovRadius = tan(fovAngle * 3.14159265f / 360.0f) * width * 0.5f; // Approximate radius in pixels
    HPEN fovPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    HBRUSH nullBrush = (HBRUSH)GetStockObject(NULL_BRUSH); 
    SelectObject(hdc, fovPen);
    SelectObject(hdc, nullBrush);
    Ellipse(hdc, (int)(width / 2 - fovRadius), (int)(height / 2 - fovRadius),
        (int)(width / 2 + fovRadius), (int)(height / 2 + fovRadius));
    DeleteObject(fovPen);
    DeleteObject(nullBrush);

    float viewMatrix[16];
    ReadProcessMemory(hProcess, (LPCVOID)(baseAddress + VIEW_MATRIX), viewMatrix, sizeof(viewMatrix), nullptr);

    // Aimbot vars
    bool aimbotActive = GetAsyncKeyState(VK_XBUTTON1) & 0x8000; // Mouse Button 4
    Vec2 closestTarget = { -1, -1 };
    float closestDist = fovRadius + 1; // Initialize beyond FOV

    for (int i = 0; i < 32; i++) {
        DWORD playerAddr;
        if (ReadProcessMemory(hProcess, (LPCVOID)(entityList + i * 4), &playerAddr, sizeof(DWORD), nullptr) && playerAddr) {
            PlayerData data;
            Vec3 feet;
            Vec2 feetScreen;
            if (ReadPlayerData(hProcess, playerAddr, &data) &&
                ReadProcessMemory(hProcess, (LPCVOID)(playerAddr + HEAD_X), &feet.x, sizeof(float), nullptr) &&
                ReadProcessMemory(hProcess, (LPCVOID)(playerAddr + HEAD_Y), &feet.y, sizeof(float), nullptr) &&
                ReadProcessMemory(hProcess, (LPCVOID)(playerAddr + HEAD_Z), &feet.z, sizeof(float), nullptr)) {
                if (WorldToScreen(feet, feetScreen, viewMatrix, width, height)) {
                    // Draw ESP dot with upward offset
                    float espDotY = feetScreen.y - 50.0f; // ESP dot position offset 
                    HBRUSH dotBrush = CreateSolidBrush(data.health > 0 ? RGB(0, 255, 0) : RGB(255, 0, 0));
                    SelectObject(hdc, dotBrush);
                    Ellipse(hdc, (int)feetScreen.x - 5, (int)espDotY - 5, (int)feetScreen.x + 5, (int)espDotY + 5);
                    DeleteObject(dotBrush);

                    // Aimbot target selection for living players
                    if (aimbotActive && data.health > 0) {
                        float dist = sqrt(pow(feetScreen.x - width / 2, 2) + pow(feetScreen.y - height / 2, 2));
                        if (dist < fovRadius && dist < closestDist) {
                            closestDist = dist;
                            closestTarget = feetScreen;
                            closestTarget.y -= -25.0f; // set offset for the aimbot (aims 25 pixels lower than the ESP dot)
                        }
                    }
                }
            }
        }
    }

    // Aimbot logic
    if (aimbotActive && closestTarget.x != -1 && closestTarget.y != -1) {
        // Get current mouse position
        POINT currentPos;
        GetCursorPos(&currentPos);

        // Calculate mouse movement
        int deltaX = (int)(closestTarget.x - width / 2);
        int deltaY = (int)(closestTarget.y - height / 2);

        // Smoothing
        const float smoothing = 5.0f;
        deltaX = (int)(deltaX / smoothing);
        deltaY = (int)(deltaY / smoothing);

        // Move mouse
        if (deltaX != 0 || deltaY != 0) {
            mouse_event(MOUSEEVENTF_MOVE, deltaX, deltaY, 0, 0);
        }
    }

    ReleaseDC(overlay, hdc);
}