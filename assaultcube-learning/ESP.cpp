#include "ESP.h"

void RunESP(HANDLE hProcess, DWORD baseAddress, DWORD localPlayer) {
    static Vec2 vScreen;
    static Vec3 playerPos;
    static float viewMatrix[16];

    // Read view matrix
    ReadProcessMemory(hProcess, (LPCVOID)(baseAddress + VIEW_MATRIX), &viewMatrix, sizeof(viewMatrix), nullptr);

    // Read local player position
    if (ReadPlayerPosition(hProcess, localPlayer, &playerPos)) {
        // Draw ESP box
        if (WorldToScreen(playerPos, &vScreen, viewMatrix, 1024, 768)) {
            float height = 80.0f;
            float width = height / 2.4f;
            DrawBorderBox((int)(vScreen.x - width / 2), (int)vScreen.y, (int)width, (int)height, 2);
        }
    }

    // TODO: Add enemy ESP when EntityList offset is available
}