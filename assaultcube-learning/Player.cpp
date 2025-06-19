#include "Player.h"

void RunHealthMod(HANDLE hProcess, DWORD localPlayer) {
    int newHealth = 999;
    WriteProcessMemory(hProcess, (LPVOID)(localPlayer + HEALTH), &newHealth, sizeof(int), nullptr);
    WriteProcessMemory(hProcess, (LPVOID)(localPlayer + ARMOUR), &newHealth, sizeof(int), nullptr);
    // Target the process, then take both the localplayer offset + health offset add them together and then set that value to the new health value with &newHealth simple and shit
}
 
void RunAmmoMod(HANDLE hProcess, DWORD localplayer) {
    int newAmmo = 999;
    //Assault Ammo Mod
    WriteProcessMemory(hProcess, (LPVOID)(localplayer + ASSAULT_AMMO), &newAmmo, sizeof(int), nullptr);
    WriteProcessMemory(hProcess, (LPVOID)(localplayer + ASSAULT_RESERVE), &newAmmo, sizeof(int), nullptr);

    //Pistol Ammo Mod
    WriteProcessMemory(hProcess, (LPVOID)(localplayer + PISTOL_AMMO), &newAmmo, sizeof(int), nullptr);
    WriteProcessMemory(hProcess, (LPVOID)(localplayer + PISTOL_RESERVE), &newAmmo, sizeof(int), nullptr);

    // Sniper Ammo Mod
	WriteProcessMemory(hProcess, (LPVOID)(localplayer + SNIPER_AMMO), &newAmmo, sizeof(int), nullptr);
	WriteProcessMemory(hProcess, (LPVOID)(localplayer + SNIPER_RESERVE), &newAmmo, sizeof(int), nullptr);

    // Submachine Ammo Mod
	WriteProcessMemory(hProcess, (LPVOID)(localplayer + SMG_AMMO), &newAmmo, sizeof(int), nullptr);
	WriteProcessMemory(hProcess, (LPVOID)(localplayer + SMG_RESERVE), &newAmmo, sizeof(int), nullptr);

	// Shotgun Ammo Mod
	WriteProcessMemory(hProcess, (LPVOID)(localplayer + SHOTGUN_AMMO), &newAmmo, sizeof(int), nullptr);
	WriteProcessMemory(hProcess, (LPVOID)(localplayer + SHOTGUN_RESERVE), &newAmmo, sizeof(int), nullptr);

	// Carbine Ammo Mod
	WriteProcessMemory(hProcess, (LPVOID)(localplayer + CARBINE_AMMO), &newAmmo, sizeof(int), nullptr);
	WriteProcessMemory(hProcess, (LPVOID)(localplayer + CARBINE_RESERVE), &newAmmo, sizeof(int), nullptr);

    //Grenade Ammo Mod
    WriteProcessMemory(hProcess, (LPVOID)(localplayer + GRENADE_AMMO), &newAmmo, sizeof(int), nullptr);

}

