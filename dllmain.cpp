#include <windows.h>
#include <string>

void Patch(uintptr_t a, int v) {
    DWORD p;
    VirtualProtect((void*)a, 4, 0x40, &p);
    *(int*)a = v;
    VirtualProtect((void*)a, 4, p, &p);
}

DWORD WINAPI Init(LPVOID p) {
    char path[260];
    GetModuleFileNameA((HMODULE)p, path, 260);
    std::string ini = path;
    ini = ini.substr(0, ini.find_last_of('.')) + ".ini";

    int fps = GetPrivateProfileIntA("Settings", "FPS", 60, ini.c_str());
    uintptr_t base = (uintptr_t)GetModuleHandleA(NULL);

    while (*(int*)(base + 0x6FC562C) == 0) {
        SwitchToThread();
    }

    Patch(base + 0x6FC562C, fps);
    Patch(base + 0x6FC5624, fps);

    FreeLibraryAndExitThread((HMODULE)p, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE h, DWORD r, LPVOID res) {
    if (r == 1) {
        DisableThreadLibraryCalls(h);
        CreateThread(0, 0, Init, h, 0, 0);
    }
    return 1;
}