#include <windows.h>
#include <stdio.h>

#include "PluginEngine_API.h"

int main(void)
{
    HMODULE hDll = LoadLibraryW(L"PluginEngine.dll");
    if (!hDll)
    {
        DWORD err = GetLastError();
        fprintf(stderr, "LoadLibrary(PluginEngine.dll) failed: %lu\n", err);
        if (err == 193) /* ERROR_BAD_EXE_FORMAT */
            fprintf(stderr, "DLL architecture does not match this process. Build both PluginEngine and PluginEngineTestHost for x64.\n");
        return 1;
    }

    auto pGetVersion = (unsigned int (__cdecl*)(void))GetProcAddress(hDll, "PluginEngine_GetVersion");
    auto pInitialize = (int (__cdecl*)(void))GetProcAddress(hDll, "PluginEngine_Initialize");
    auto pShutdown = (void (__cdecl*)(void))GetProcAddress(hDll, "PluginEngine_Shutdown");
    auto pGetThirdPartyCheck = (int (__cdecl*)(void))GetProcAddress(hDll, "PluginEngine_GetThirdPartyCheck");

    if (!pGetVersion || !pInitialize || !pShutdown || !pGetThirdPartyCheck)
    {
        fprintf(stderr, "GetProcAddress failed\n");
        FreeLibrary(hDll);
        return 1;
    }

    unsigned int ver = pGetVersion();
    printf("PluginEngine version: %u.%u.%u (0x%08X)\n",
           (ver >> 16) & 0xFFu,
           (ver >> 8) & 0xFFu,
           ver & 0xFFu,
           ver);

    if (pInitialize() != 0)
    {
        fprintf(stderr, "PluginEngine_Initialize failed\n");
        FreeLibrary(hDll);
        return 1;
    }

    int thirdParty = pGetThirdPartyCheck();
    if (thirdParty != 1)
    {
        fprintf(stderr, "OpenCV (third_party) check failed: got %d, expected 1\n", thirdParty);
        FreeLibrary(hDll);
        return 1;
    }
    printf("OpenCV (third_party) check: OK\n");

    pShutdown();
    FreeLibrary(hDll);
    printf("OK\n");
    return 0;
}
