// IATHook.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"
#include "framework.h"
#include "hook.h"



static LPVOID savedAddr; // the saved address of the original function hooked.

/*
* new function to hook
*/
DWORD WINAPI hook_GetCurrentProcessId(VOID) {
	printf("hooked \n");
	MessageBoxA(NULL, "hooked", NULL, 0);
	Func_GetCurrentProcessId orignalGetCurrentProcessId = (Func_GetCurrentProcessId)savedAddr;
	return orignalGetCurrentProcessId();
}
/*
* @hook function
* mở CurrentProcess
* lấy ID function cần hook
* change protection
* hook
* unchange protection
*/
VOID Hook(char* functionName, void* newFunction, LPVOID* savedAddress) {

	MODULEINFO modInfo;
	DWORD accessProtect = NULL;
	HMODULE hMod = GetModuleHandle(0);
	GetModuleInformation(GetCurrentProcess(), hMod, &modInfo, sizeof(MODULEINFO));
	LPBYTE lpAddress = (LPBYTE)modInfo.lpBaseOfDll;
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)lpAddress;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(lpAddress + pDosHeader->e_lfanew);
	PIMAGE_OPTIONAL_HEADER pOptionHeader = (PIMAGE_OPTIONAL_HEADER) & (pNTHeader->OptionalHeader);
	PIMAGE_IMPORT_DESCRIPTOR pImportDir = PIMAGE_IMPORT_DESCRIPTOR(lpAddress + pOptionHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	while (pImportDir->OriginalFirstThunk) {
		PIMAGE_THUNK_DATA pFirstThunk = (PIMAGE_THUNK_DATA)(lpAddress + pImportDir->FirstThunk);
		PIMAGE_THUNK_DATA pOriginalFirstThunk = (PIMAGE_THUNK_DATA)(lpAddress + pImportDir->OriginalFirstThunk);
		while (pOriginalFirstThunk->u1.AddressOfData != 0) {
			PIMAGE_IMPORT_BY_NAME pName = (PIMAGE_IMPORT_BY_NAME)(lpAddress + pOriginalFirstThunk->u1.AddressOfData);

			if (strcmp(functionName, (char*)pName->Name) == 0) {
				if (savedAddress)
					*savedAddress = (LPVOID)pFirstThunk->u1.Function;

				printf("Found %s in %s\n", (char*)pName->Name, (char*)(pImportDir->Name + lpAddress));
				//change protection
				VirtualProtect(&pFirstThunk->u1.Function, sizeof(LPVOID), PAGE_EXECUTE_READWRITE, &accessProtect);
				pFirstThunk->u1.Function = (uintptr_t)newFunction;
				//unchange protection
				VirtualProtect(&pFirstThunk->u1.Function, sizeof(LPVOID), accessProtect, &accessProtect);

			}
			pOriginalFirstThunk++;
			pFirstThunk++;
		}
		pImportDir++;
	}

}
/*@main*/
BOOL WINAPI Hooks()
{
	char* functionName = (char*)"GetCurrentProcessId";
	Hook(functionName, hook_GetCurrentProcessId, &savedAddr);
	return TRUE;
}
__declspec(dllexport) __declspec(noinline) VOID test(VOID) { return; }