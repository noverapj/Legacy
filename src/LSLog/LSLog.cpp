// LSLog.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "TlHelp32.h"

#define nInitStart 0x01321B76
#define nInitComplete 0x01321D81
#define n23 0x02023535
#define n23JMP n23 + 0xF1
#define n24 n23JMP + 0x62
#define n24JMP n24 + 0x225

static DWORD GetProcID(LPCTSTR module)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	while (hRes)
	{
		if (lstrcmp(pEntry.szExeFile, module) == 0) {
			return pEntry.th32ProcessID;
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);
	return 0;
}

static bool TerminateProcessByName(LPCTSTR module)
{
	DWORD myProcID = GetProcID(module);
	if (myProcID)
	{
		PROCESSENTRY32 pe;

		memset(&pe, 0, sizeof(PROCESSENTRY32));
		pe.dwSize = sizeof(PROCESSENTRY32);

		HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if (::Process32First(hSnap, &pe))
		{
			BOOL bCONtinue = TRUE;

			// kill child processes
			while (bCONtinue)
			{
				// ONly kill child processes
				if (pe.th32ParentProcessID == myProcID)
				{
					HANDLE hChildProc = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);

					if (hChildProc)
					{
						::TerminateProcess(hChildProc, 1);
						::CloseHandle(hChildProc);
					}
				}

				bCONtinue = ::Process32Next(hSnap, &pe);
			}

			// kill the main process
			HANDLE hProc = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, myProcID);

			if (hProc)
			{
				::TerminateProcess(hProc, 1);
				::CloseHandle(hProc);
				return true;
			}
		}
	}
	return false;
}

static void* DetourFunction(BYTE* src, DWORD dst, const int len)
{
	BYTE* jmp = (BYTE*)malloc(len + 5);
	DWORD dwBack;
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &dwBack);
	memcpy(jmp, src, len);
	jmp += len;
	jmp[0] = 0xE9;
	*(DWORD*)(jmp + 1) = (DWORD)(src + len - jmp) - 5;
	src[0] = 0xE9;
	*(DWORD*)(src + 1) = (DWORD)(dst - (DWORD)src) - 5;
	for (int i = 5; i < len; i++)  src[i] = 0x90;
	VirtualProtect(src, len, dwBack, &dwBack);
	return (jmp - len);
}


static void NProtectBypass()
{
  
    while (1)
    {
        if (TerminateProcessByName("GameGuard.des")) {
            DetourFunction((PBYTE)nInitStart, (DWORD)nInitComplete, 5);
            DetourFunction((PBYTE)n23, (DWORD)n23JMP, 5);
            DetourFunction((PBYTE)n24, (DWORD)n24JMP, 5);
        }

        Sleep(20);
    }
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);

        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)NProtectBypass, NULL, NULL, NULL);
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}