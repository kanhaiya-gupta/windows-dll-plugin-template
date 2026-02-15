#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#include "MyDll1_API.h"

int main(void)
{
    HMODULE hDll = LoadLibraryW(L"MyDll1.dll");
    if (!hDll)
    {
        DWORD err = GetLastError();
        fprintf(stderr, "LoadLibrary(MyDll1.dll) failed: %lu\n", err);
        if (err == 193) /* ERROR_BAD_EXE_FORMAT */
            fprintf(stderr, "DLL architecture does not match this process. Build both MyDll1 and MyDll1TestHost for the same platform (e.g. x64 or Win32).\n");
        return 1;
    }

    auto pGetVersion = (unsigned int (__cdecl*)(void))GetProcAddress(hDll, "MyDll1_GetVersion");
    auto pInitialize = (int (__cdecl*)(void))GetProcAddress(hDll, "MyDll1_Initialize");
    auto pShutdown = (void (__cdecl*)(void))GetProcAddress(hDll, "MyDll1_Shutdown");
    auto pGetThirdPartyCheck = (int (__cdecl*)(void))GetProcAddress(hDll, "MyDll1_GetThirdPartyCheck");

    if (!pGetVersion || !pInitialize || !pShutdown || !pGetThirdPartyCheck)
    {
        fprintf(stderr, "GetProcAddress failed\n");
        FreeLibrary(hDll);
        return 1;
    }

    unsigned int ver = pGetVersion();
    printf("MyDll1 version: %u.%u.%u (0x%08X)\n",
           (ver >> 16) & 0xFFu,
           (ver >> 8) & 0xFFu,
           ver & 0xFFu,
           ver);

    if (pInitialize() != 0)
    {
        fprintf(stderr, "MyDll1_Initialize failed\n");
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
