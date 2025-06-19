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

// Player data structure
struct PlayerData {
    int health;
    char name[16];
};

// Function declarations
DWORD GetModuleBaseAddress(DWORD procId, const wchar_t* modName);
bool ReadPlayerData(HANDLE hProcess, DWORD playerBase, PlayerData* data);

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

inline bool ReadPlayerData(HANDLE hProcess, DWORD playerBase, PlayerData* data) {
    return ReadProcessMemory(hProcess, (LPCVOID)(playerBase + HEALTH), &data->health, sizeof(int), nullptr) &&
        ReadProcessMemory(hProcess, (LPCVOID)(playerBase + PLAYER_NAME), &data->name, sizeof(data->name), nullptr);
}