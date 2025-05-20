#include "Player.h"

void RunHealthMod(HANDLE hProcess, DWORD localPlayer) {
    int newHealth = 999;
    WriteProcessMemory(hProcess, (LPVOID)(localPlayer + HEALTH), &newHealth, sizeof(int), nullptr);
    // Target the process, then take both the localplayer offset + health offset add them together then set that value to the new health value with &newHealth
}

void RunAmmoMod(HANDLE hProcess, DWORD localplayer) {
    int newAmmo = 999;
    //Primary Ammo Mod
    WriteProcessMemory(hProcess, (LPVOID)(localplayer + PRIMARY_AMMO), &newAmmo, sizeof(int), nullptr);
    WriteProcessMemory(hProcess, (LPVOID)(localplayer + PRIMARY_RESERVE), &newAmmo, sizeof(int), nullptr);

    //Secondary Ammo Mod
    WriteProcessMemory(hProcess, (LPVOID)(localplayer + SECONDARY_AMMO), &newAmmo, sizeof(int), nullptr);
    WriteProcessMemory(hProcess, (LPVOID)(localplayer + SECONDARY_RESERVE), &newAmmo, sizeof(int), nullptr);
}