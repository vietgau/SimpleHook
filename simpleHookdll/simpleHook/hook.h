#pragma once
#include "framework.h"

BOOL WINAPI Hooks();
__declspec(dllexport) __declspec(noinline) VOID test(VOID);
typedef DWORD(WINAPI* Func_GetCurrentProcessId)(VOID);
VOID Hook(char* functionName, void* newFunction, LPVOID* savedAddress);



