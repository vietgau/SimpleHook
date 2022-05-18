// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "hook.h"

HMODULE hModule;

BOOL APIENTRY DllMain( HMODULE hDllModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
         DisableThreadLibraryCalls(hDllModule);
         hModule = GetModuleHandle(0);
         if (!Hooks())
             FreeLibrary(hModule);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

