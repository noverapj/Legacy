// LSLog.cpp : Defines the entry point for the DLL application.
//
// Game log library (CLog) with an integrated nProtect GameGuard bypass:
//  - kills GameGuard.des (and its children) while the client runs
//  - patches the client's GameGuard init via signature scanning, so client
//    updates no longer require re-derived hardcoded offsets
//
// Patch flow (on the thread spawned from DllMain): locate the client's
// GameGuard init via signature scanning and patch it as soon as Themida
// finishes unpacking, then kill GameGuard.des forever. Landing the patch
// before GameGuard.des even spawns removes the race that triggers nProtect
// Error 1; afterwards the already-patched client cannot observe the death.
//
// Safety rules before any byte is patched:
//   - every signature matches EXACTLY once across all executable regions
//   - the expected opcode is present at the computed site
//   - JE jump targets are forward and inside the module (targets are read
//     from the instructions' own rel32 displacement, so block-size changes
//     between client builds are followed automatically)
//
// Known-good sites for the currently supported build (KR2021, reference only):
//   init  : 0x01321B76 -> 0x01321D81
//   JE23  : 0x02023535 -> 0x02023626
//   JE24  : 0x02023688 -> 0x020238AD

#include "stdafx.h"
#include "TlHelp32.h"

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

// ============================================================================
// Signature scanner
// ============================================================================

static const char* g_szSigInitStart =
	"EB 0E 8B 15 ?? ?? ?? ?? 68 ?? ?? ?? ?? 6A 00 52 FF D7 "
	"A1 ?? ?? ?? ?? 83 C4 0C 68 ?? ?? ?? ?? 6A 00 50 FF D7";          // +0x12 -> mov eax,[g]
static const char* g_szSigInitComplete =
	"C2 10 00 33 F6 E9 ?? ?? ?? ?? A1 ?? ?? ?? ?? 68 ?? ?? ?? ?? "
	"6A 00 50 FF 15 ?? ?? ?? ??";                                      // +0x0A -> mov eax,[g]
static const char* g_szSigJE23 =
	"3D ?? ?? ?? ?? 0F 84 ?? ?? ?? ?? 68 03 01 00 00 8D 85";            // +0x05 -> JE rel32
static const char* g_szSigJE24 =
	"8B F9 8B 47 10 85 C0 0F 84 ?? ?? ?? ?? 80 7F 0C 00 0F 85";        // +0x07 -> JE rel32

// parse "EB 0E 8B 15 ?? ..." into bytes+mask; returns length or -1
static int ParsePattern(const char* szPattern, BYTE* pBytes, bool* pMask, int nMaxLen)
{
	int n = 0;
	const char* p = szPattern;
	while (*p && n < nMaxLen)
	{
		while (*p == ' ')
			p++;
		if (!*p)
			break;

		if (p[0] == '?' && p[1] == '?')
		{
			pBytes[n] = 0;
			pMask[n] = false;
			n++;
			p += 2;
			continue;
		}

		int v = 0;
		for (int k = 0; k < 2; k++)
		{
			char c = *p++;
			v <<= 4;
			if (c >= '0' && c <= '9')         v |= c - '0';
			else if (c >= 'A' && c <= 'F')    v |= c - 'A' + 10;
			else if (c >= 'a' && c <= 'f')    v |= c - 'a' + 10;
			else return -1;
		}
		pBytes[n] = (BYTE)v;
		pMask[n] = true;
		n++;
	}
	return n;
}

struct SCAN_REGION
{
	BYTE*  pBase;
	SIZE_T nSize;
};

// scan one region; returns the number of matches found (sets the first match VA)
static int ScanRegion(const BYTE* pRegion, SIZE_T nSize,
                      const BYTE* pBytes, const bool* pMask, int nLen,
                      DWORD* pdwMatchVA, DWORD dwModuleVA)
{
	int nMatches = 0;
	const BYTE pFirst = pBytes[0];

	for (SIZE_T i = 0; i + nLen <= nSize; i++)
	{
		if (pRegion[i] != pFirst)
			continue;

		bool bMatch = true;
		for (int j = 1; j < nLen; j++)
		{
			if (pMask[j] && pRegion[i + j] != pBytes[j])
			{
				bMatch = false;
				break;
			}
		}
		if (bMatch)
		{
			nMatches++;
			if (pdwMatchVA)
				*pdwMatchVA = dwModuleVA + (DWORD)i;
		}
	}
	return nMatches;
}

static bool ScanAndPatch()
{
	HMODULE hBase = GetModuleHandleA(NULL);

	// enumerate committed executable regions of the host module
	SCAN_REGION arrRegions[512];
	int nRegions = 0;

	MEMORY_BASIC_INFORMATION mbi;
	BYTE* pAddr = (BYTE*)hBase;
	while (VirtualQuery(pAddr, &mbi, sizeof(mbi)) == sizeof(mbi) && nRegions < 512)
	{
		if (mbi.AllocationBase != hBase)
			break;   // walked past the host module
		if (mbi.State == MEM_COMMIT &&
			(mbi.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) &&
			!(mbi.Protect & PAGE_GUARD))
		{
			arrRegions[nRegions].pBase = (BYTE*)mbi.BaseAddress;
			arrRegions[nRegions].nSize = mbi.RegionSize;
			nRegions++;
		}
		pAddr = (BYTE*)mbi.BaseAddress + mbi.RegionSize;
	}

	if (nRegions == 0)
		return false;

	// find each site: signature must match EXACTLY once across all regions
	struct SITE
	{
		const char* szName;
		const char* szPattern;
		int        nDelta;     // match -> site
		DWORD      dwVA;
	};
	SITE arrSites[4] =
	{
		{ "nInitStart",    g_szSigInitStart,    0x12, 0 },
		{ "nInitComplete", g_szSigInitComplete, 0x0A, 0 },
		{ "JE23",          g_szSigJE23,         0x05, 0 },
		{ "JE24",          g_szSigJE24,         0x07, 0 },
	};

	BYTE  aBytes[128];
	bool  aMask[128];

	for (int s = 0; s < 4; s++)
	{
		int nLen = ParsePattern(arrSites[s].szPattern, aBytes, aMask, 128);
		if (nLen <= 0)
			return false;

		int nTotal = 0;
		DWORD dwFound = 0;
		for (int r = 0; r < nRegions; r++)
		{
			DWORD dwHit = 0;
			int n = ScanRegion(arrRegions[r].pBase, arrRegions[r].nSize,
			                   aBytes, aMask, nLen, &dwHit,
			                   (DWORD)(DWORD_PTR)arrRegions[r].pBase);
			if (n > 0 && nTotal == 0)
				dwFound = dwHit;
			nTotal += n;
		}

		// 0 matches = still unpacking (normal while Themida is busy);
		// anything other than exactly 1 = refuse to patch
		if (nTotal != 1)
			return false;

		arrSites[s].dwVA = dwFound + arrSites[s].nDelta;
	}

	DWORD dwInitStart    = arrSites[0].dwVA;
	DWORD dwInitComplete = arrSites[1].dwVA;
	DWORD dwJE23         = arrSites[2].dwVA;
	DWORD dwJE24         = arrSites[3].dwVA;

	// opcode sanity checks
	if (*(BYTE*)dwInitStart != 0xA1 || *(BYTE*)dwInitComplete != 0xA1)
		return false;
	if (*(BYTE*)dwJE23 != 0x0F || *((BYTE*)dwJE23 + 1) != 0x84 ||
	    *(BYTE*)dwJE24 != 0x0F || *((BYTE*)dwJE24 + 1) != 0x84)
		return false;

	// jump targets come from the JE instructions themselves:
	// target = JE addr + 6 + rel32
	DWORD dw23Target = dwJE23 + 6 + *(int*)(dwJE23 + 2);
	DWORD dw24Target = dwJE24 + 6 + *(int*)(dwJE24 + 2);

	DWORD dwModuleEnd = (DWORD)(DWORD_PTR)arrRegions[0].pBase;
	for (int r = 0; r < nRegions; r++)
	{
		DWORD dwEnd = (DWORD)(DWORD_PTR)arrRegions[r].pBase + (DWORD)arrRegions[r].nSize;
		if (dwEnd > dwModuleEnd)
			dwModuleEnd = dwEnd;
	}

	if (dw23Target <= dwJE23 || dw23Target >= dwModuleEnd ||
	    dw24Target <= dwJE24 || dw24Target >= dwModuleEnd)
		return false;

	DetourFunction((PBYTE)dwInitStart, dwInitComplete, 5);
	DetourFunction((PBYTE)dwJE23, dw23Target, 5);
	DetourFunction((PBYTE)dwJE24, dw24Target, 5);

	return true;
}

static void NProtectBypass()
{
	// patch as early as possible (before GameGuard even spawns): the scan
	// fails quietly while Themida is still unpacking and succeeds the moment
	// unpacking is done - typically several seconds BEFORE GameGuard.des
	// is created
	bool bPatched = false;
	int  nScanAttempts = 0;

	while (!bPatched)
	{
		if (ScanAndPatch())
		{
			bPatched = true;
			break;
		}

		nScanAttempts++;
		if (nScanAttempts >= 3000)   // ~5 minutes at 100 ms
			break;                   // unsupported build - fall through to the GameGuard killer

		Sleep(100);
	}

	// GameGuard killer - forever. The client is already patched (or the
	// scan gave up), so killing GameGuard at leisure cannot be observed.
	while (1)
	{
		TerminateProcessByName("GameGuard.des");
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
