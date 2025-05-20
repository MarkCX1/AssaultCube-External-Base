#pragma once
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include "offsets.h"
using namespace std;

// Vector structures
typedef struct {
    float x, y, z, w;
} Vec4;

typedef struct {
    float x, y, z;
} Vec3;

typedef struct {
    float x, y;
} Vec2;

// Global variables
extern HDC hDC;
extern HBRUSH color;

// Player data structure
struct PlayerData {
    int health;
    char name[16];
};

// Function declarations
DWORD GetModuleBaseAddress(DWORD procId, const wchar_t* modName);
int WorldToScreen(Vec3 pos, Vec2* screen, float matrix[16], int windowWidth, int windowHeight);
void DrawBorderBox(int x, int y, int w, int h, int thickness);
void DrawFilledRect(int x, int y, int w, int h, HBRUSH color);
bool ReadPlayerData(HANDLE hProcess, DWORD playerBase, PlayerData* data);
bool ReadPlayerPosition(HANDLE hProcess, DWORD playerBase, Vec3* pos);

// Function implementations
inline DWORD GetModuleBaseAddress(DWORD procId, const wchar_t* modName) {
    DWORD modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry)) {
            do {
                if (!_wcsicmp(modEntry.szModule, modName)) {
                    modBaseAddr = (DWORD)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
        CloseHandle(hSnap);
    }
    return modBaseAddr;
}

inline int WorldToScreen(Vec3 pos, Vec2* screen, float matrix[16], int windowWidth, int windowHeight) {
    Vec4 clipCoords;
    clipCoords.x = pos.x * matrix[0] + pos.y * matrix[4] + pos.z * matrix[8] + matrix[12];
    clipCoords.y = pos.x * matrix[1] + pos.y * matrix[5] + pos.z * matrix[9] + matrix[13];
    clipCoords.z = pos.x * matrix[2] + pos.y * matrix[6] + pos.z * matrix[10] + matrix[14];
    clipCoords.w = pos.x * matrix[3] + pos.y * matrix[7] + pos.z * matrix[11] + matrix[15];

    if (clipCoords.w < 0.1f)
        return 0;

    Vec3 NDC;
    NDC.x = clipCoords.x / clipCoords.w; 
    NDC.y = clipCoords.y / clipCoords.w;
    NDC.z = clipCoords.z / clipCoords.w;

    screen->x = (windowWidth / 2.0f * NDC.x) + (NDC.x + windowWidth / 2.0f);
    screen->y = -(windowHeight / 2.0f * NDC.y) + (NDC.y + windowHeight / 2.0f);
    return 1;
}

inline void DrawFilledRect(int x, int y, int w, int h, HBRUSH color) {
    RECT rect = { x, y, x + w, y + h };
    FillRect(hDC, &rect, color);
}

inline void DrawBorderBox(int x, int y, int w, int h, int thickness) {
    DrawFilledRect(x, y, w, thickness, color); // Top
    DrawFilledRect(x, y, thickness, h, color); // Left
    DrawFilledRect(x + w - thickness, y, thickness, h, color); // Right
    DrawFilledRect(x, y + h - thickness, w, thickness, color); // Bottom
}

inline bool ReadPlayerPosition(HANDLE hProcess, DWORD playerBase, Vec3* pos) {
    return ReadProcessMemory(hProcess, (LPCVOID)(playerBase + PLAYER_X), &pos->x, sizeof(float), nullptr) &&
        ReadProcessMemory(hProcess, (LPCVOID)(playerBase + PLAYER_Y), &pos->y, sizeof(float), nullptr) &&
        ReadProcessMemory(hProcess, (LPCVOID)(playerBase + PLAYER_Z), &pos->z, sizeof(float), nullptr);
}

inline bool ReadPlayerData(HANDLE hProcess, DWORD playerBase, PlayerData* data) {
    return ReadProcessMemory(hProcess, (LPCVOID)(playerBase + HEALTH), &data->health, sizeof(int), nullptr) &&
        ReadProcessMemory(hProcess, (LPCVOID)(playerBase + PLAYER_NAME), &data->name, sizeof(data->name), nullptr);
}