#include "Header.h"
#include "ESP.h"
#include "Player.h"
#include <iostream>
#include <Windows.h>

HDC hDC;
HBRUSH color;

int main() {
    // ESP Box color
    color = CreateSolidBrush(RGB(255, 0, 255));

    // Find the window
    HWND hwnd = FindWindowA(NULL, "AssaultCube");
    if (!hwnd) {
        std::cout << "Window not found!" << std::endl;
        return 1;
    }
    std::cout << "Window found!" << std::endl;

    // Get device context for drawing
    hDC = GetDC(hwnd);

    // Get process ID
    DWORD pID;
    GetWindowThreadProcessId(hwnd, &pID);
    if (!pID) {
        std::cout << "Process ID not found!" << std::endl;
        return 1;
    }
    std::cout << "Process ID: " << pID << std::endl;

    // Open process
    HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
    if (!handle) {
        std::cout << "Failed to open process!" << std::endl;
        return 1;
    }

    // Get base address of ac_client.exe
    DWORD baseAddress = GetModuleBaseAddress(pID, L"ac_client.exe");
    if (!baseAddress) {
        std::cout << "Base address not found!" << std::endl;
        return 1;
    }
    std::cout << "Base address: 0x" << std::hex << baseAddress << std::endl;

    // Get LOCAL_PLAYER address
    DWORD localPlayer;
    ReadProcessMemory(handle, (LPCVOID)(baseAddress + LOCAL_ENTITY), &localPlayer, sizeof(DWORD), nullptr);
    if (!localPlayer) {
        std::cout << "Local player not found!" << std::endl;
        return 1;
    }
    std::cout << "Local player address: 0x" << std::hex << localPlayer << std::endl;

    // Get ENTITY_LIST address
    DWORD entityList;
    ReadProcessMemory(handle, (LPCVOID)(baseAddress + ENTITY_LIST), &entityList, sizeof(DWORD), nullptr);
    if (!entityList) {
        std::cout << "Entity list not found!" << std::endl;
        return 1;
    }
    std::cout << "Entity list address: 0x" << std::hex << entityList << std::endl;
   
    // Get VIEW_MATRIX address
	DWORD viewMatrix;
	ReadProcessMemory(handle, (LPCVOID)(baseAddress + VIEW_MATRIX), &viewMatrix, sizeof(DWORD), nullptr);
	if (!viewMatrix) {
		std::cout << "View matrix not found!" << std::endl;
		return 1;
	}
	std::cout << "View matrix address: 0x" << std::hex << viewMatrix << std::endl;

    // Count valid players and print their names
    int playerCount = 0;
    std::cout << std::endl; 
    for (int i = 0; i < 32; ++i) {
        DWORD playerPtr;
        if (ReadProcessMemory(handle, (LPCVOID)(entityList + (i * 4)), &playerPtr, sizeof(DWORD), nullptr) && playerPtr) {
            char playerName[32] = { 0 }; 
            if (ReadProcessMemory(handle, (LPCVOID)(playerPtr + PLAYER_NAME), playerName, sizeof(playerName), nullptr)) {
                if (playerName[0] != '\0') { 
                    std::cout << "Player " << playerCount + 1 << ": " << playerName << std::endl;
                    playerCount++;
                }
            }
        }
    }
    std::cout << "\nValid Players: " << std::dec << playerCount << std::endl;




    // Run the features
    while (true) {
        //RunESP(handle, baseAddress, localPlayer);
        RunHealthMod(handle, localPlayer);
        RunAmmoMod(handle, localPlayer);
        //Sleep(100);
    }

    // Cleanup
    ReleaseDC(hwnd, hDC);
    CloseHandle(handle);
    DeleteObject(color);
    return 0;
}