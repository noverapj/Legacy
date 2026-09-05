// io3DEngine.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include "ioResourceLoader.h"
#include "ioThreadTaskManager.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved )
{
	if( ul_reason_for_call == DLL_PROCESS_DETACH )
	{
		ioThreadTaskManager::ReleaseInstance();
		ioResourceLoader::ReleaseInstance();
	}
	

    return TRUE;
}

