
/*************************************************************************************************/
// [TAG] XTrap4ClientExt1.h | 2009.01.16 | X-TRAP Library For XTrap4ClientExt1
// ************************************************************************************************
// Copyright (c) 2005 - 2008 WiseLogic. All Rights Reserved
/*************************************************************************************************/

#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// Library Definition macro
///////////////////////////////////////////////////////////////////////////////////////////////////

#define XTRAP_CE1_MEMORY_CODE					0x00000001
#define XTRAP_CE1_MEMORY_RDATA					0x00000002
#define XTRAP_CE1_MEMORY_ALL					(XTRAP_CE1_MEMORY_CODE|XTRAP_CE1_MEMORY_RDATA)
#define XTRAP_CE1_MEMORY_DEFAULT				0x00000001

#define XTrap_CE1_StaticMemoryDefenseInit		XTrap_CE1_Func0_Init
#define XTrap_CE1_StaticMemoryDefenseOn			XTrap_CE1_Func1_Memory
#define XTrap_CE1_StaticMemoryDefenseOff		XTrap_CE1_Func2_Memory

#define XTrap_CE1_DllMemoryDefenseInit			XTrap_CE1_Func0_Init
#define XTrap_CE1_DllMemoryDefenseOn			XTrap_CE1_Func3_Module
#define XTrap_CE1_DllMemoryDefenseOff			XTrap_CE1_Func4_Module

#define	XTrap_CE1_SmallMemoryDefenseInit		XTrap_CE1_Func10_Init
#define XTrap_CE1_SmallMemoryDefenseOn			XTrap_CE1_Func11_Protect
#define XTrap_CE1_SmallMemoryDefenseOff			XTrap_CE1_Func12_Protect

#define XTrap_CE1_Func5_Init					XTrap_Extra1_Export_InitializeBillingSystem
#define XTrap_CE1_Func6_Payment					XTrap_Extra1_Export_OnBillingSystem
#define XTrap_CE1_Func7_Payment					XTrap_Extra1_Export_OffBillingSystem

///////////////////////////////////////////////////////////////////////////////////////////////////
// Library Declaration Function
///////////////////////////////////////////////////////////////////////////////////////////////////

DWORD XTrap_CE1_Func0_Init();
DWORD XTrap_CE1_Func1_Memory(LPVOID lpBaseAddr, SIZE_T ulSize);
DWORD XTrap_CE1_Func2_Memory(LPVOID lpBaseAddr, SIZE_T ulSize);

DWORD XTrap_CE1_Func3_Module(LPVOID lpBaseAddr, DWORD dwMethod);
DWORD XTrap_CE1_Func4_Module(LPVOID lpBaseAddr, DWORD dwMethod);

DWORD XTrap_CE1_Func10_Init();
DWORD XTrap_CE1_Func11_Protect(LPVOID lpBaseAddr, SIZE_T ulSize);
DWORD XTrap_CE1_Func12_Protect(LPVOID lpBaseAddr, SIZE_T ulSize);

DWORD XTrap_Extra1_Export_InitializeBillingSystem();
DWORD XTrap_Extra1_Export_OnBillingSystem();
DWORD XTrap_Extra1_Export_OffBillingSystem();
