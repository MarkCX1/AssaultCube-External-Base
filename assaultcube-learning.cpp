#include "Header.h"
#include "Player.h"
#include <iostream>
#include <Windows.h>
#include <ctime>
#include <set>
#include <string>

void ClearConsole() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize, dwWritten;
    COORD coord = { 0, 0 };

    // console buffer 
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    FillConsoleOutputCharacter(hConsole, ' ', dwConSize, coord, &dwWritten);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coord, &dwWritten);
    SetConsoleCursorPosition(hConsole, coord);
}

void PrintInitialInfo(HWND hwnd, DWORD pID, DWORD baseAddress, DWORD localPlayer, DWORD entityList, DWORD viewMatrix) {
    std::cout << "Window found!" << std::endl;
    std::cout << "Process ID: " << pID << std::endl;
    std::cout << "Base address: 0x" << std::hex << baseAddress << std::endl;
    std::cout << "Local player address: 0x" << std::hex << localPlayer << std::endl;
    std::cout << "Entity list address: 0x" << std::hex << entityList << std::endl;
    std::cout << "View matrix address: 0x" << std::hex << viewMatrix << std::endl;
    std::cout << std::endl;
}

// player name and health struct
struct PlayerInfo {
    std::string name;
    int health;

    // Define comparison operator for std::set
    bool operator<(const PlayerInfo& other) const {
        if (name != other.name) return name < other.name;
        return health < other.health;
    }
};

void PrintPlayerList(HANDLE handle, DWORD baseAddress, DWORD entityList) {
    // Use a set to track unique player name and health combinations
    std::set<PlayerInfo> uniquePlayers;
    int playerCount = 0;

    std::cout << "Scanning for players in lobby..." << std::endl;

    // Iterate through the entity list with a reasonable upper limit
    for (int i = 0; i < 32; ++i) { // Max 32 players for AssaultCube
        DWORD playerPtr;
        if (!ReadProcessMemory(handle, (LPCVOID)(entityList + (i * 4)), &playerPtr, sizeof(DWORD), nullptr) || !playerPtr) {
            continue; // Skip invalid or null pointers
        }

        PlayerData data;
        if (ReadPlayerData(handle, playerPtr, &data)) {
            // Check for printable ASCII
            bool isValid = true;
            for (int j = 0; j < strlen(data.name); ++j) {
                if (data.name[j] < 32 || data.name[j] > 126) {
                    isValid = false;
                    break;
                }
            }

            if (isValid && data.name[0] != '\0') {
                // Create PlayerInfo struct for this player
                PlayerInfo player = { std::string(data.name), data.health };

                // Insert into set to ignore duplicates
                if (uniquePlayers.insert(player).second) {
                    // Print the player
                    std::cout << "Player " << std::dec << playerCount + 1 << ": " << data.name << " (Health: ";
                    if (data.health <= 0) {
                        std::cout << "DEAD";
                    }
                    else {
                        std::cout << std::dec << data.health;
                    }
                    std::cout << ")" << std::endl;
                    playerCount++;
                }
            }
        }
    }

    std::cout << "\nTotal Players in Lobby: " << std::dec << playerCount << std::endl;
}

int main() {
    // Find the window
    HWND hwnd = FindWindowA(NULL, "AssaultCube");
    if (!hwnd) {
        std::cout << "Window not found!" << std::endl;
        return 1;
    }

    // Get process ID
    DWORD pID;
    GetWindowThreadProcessId(hwnd, &pID);
    if (!pID) {
        std::cout << "Process ID not found!" << std::endl;
        return 1;
    }

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
        CloseHandle(handle);
        return 1;
    }

    // Get LOCAL_PLAYER address
    DWORD localPlayer;
    if (!ReadProcessMemory(handle, (LPCVOID)(baseAddress + LOCAL_ENTITY), &localPlayer, sizeof(DWORD), nullptr)) {
        std::cout << "Local player not found!" << std::endl;
        CloseHandle(handle);
        return 1;
    }

    // Get ENTITY_LIST address
    DWORD entityList;
    if (!ReadProcessMemory(handle, (LPCVOID)(baseAddress + ENTITY_LIST), &entityList, sizeof(DWORD), nullptr)) {
        std::cout << "Entity list not found!" << std::endl;
        CloseHandle(handle);
        return 1;
    }

    // Get VIEW_MATRIX address
    DWORD viewMatrix;
    if (!ReadProcessMemory(handle, (LPCVOID)(baseAddress + VIEW_MATRIX), &viewMatrix, sizeof(DWORD), nullptr)) {
        std::cout << "View matrix not found!" << std::endl;
        CloseHandle(handle);
        return 1;
    }

    // Print initial info once
    PrintInitialInfo(hwnd, pID, baseAddress, localPlayer, entityList, viewMatrix);

    // Run features and refresh player list every 10 seconds
    ULONGLONG lastRefresh = GetTickCount64();
    const ULONGLONG refreshInterval = 1000;
    while (true) {
        // Run health and ammo mods
        RunHealthMod(handle, localPlayer);
        RunAmmoMod(handle, localPlayer);

        ULONGLONG currentTime = GetTickCount64();
        if (currentTime - lastRefresh >= refreshInterval) {
            // Clear console and move cursor to top
            ClearConsole();

            // Reprint the initial info offsets, etc.
            PrintInitialInfo(hwnd, pID, baseAddress, localPlayer, entityList, viewMatrix);

            // Reprint player list
            PrintPlayerList(handle, baseAddress, entityList);
            lastRefresh = currentTime;
        }

        Sleep(100);
    }

    // Cleanup
    CloseHandle(handle);
    return 0;
}