#include <windows.h>
#include <string>

void PatchInt(uintptr_t address, int value) {
    DWORD oldProtect;
    VirtualProtect(reinterpret_cast<void*>(address), sizeof(int), PAGE_EXECUTE_READWRITE, &oldProtect);
    *reinterpret_cast<int*>(address) = value;
    VirtualProtect(reinterpret_cast<void*>(address), sizeof(int), oldProtect, &oldProtect);
}

DWORD WINAPI MainThread(LPVOID lpParam) {
    HMODULE hModule = static_cast<HMODULE>(lpParam);
    char modulePath[MAX_PATH];
    GetModuleFileNameA(hModule, modulePath, MAX_PATH);

    std::string iniPath = modulePath;
    size_t lastDot = iniPath.find_last_of(".");
    if (lastDot != std::string::npos) {
        iniPath = iniPath.substr(0, lastDot) + ".ini";
    }

    int desiredFPS = GetPrivateProfileIntA("Settings", "FPS", 60, iniPath.c_str());

    Sleep(15000);

    uintptr_t moduleBase = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));

    if (moduleBase) {
        PatchInt(moduleBase + 0x6FC562C, desiredFPS);
        PatchInt(moduleBase + 0x6FC5624, desiredFPS);
    }

    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
    }
    return TRUE;
}