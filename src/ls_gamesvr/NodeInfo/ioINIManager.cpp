#include "stdafx.h"

//#include "../Window.h"
#include "../MainProcess.h"
#include "../EtcHelpFunc.h"

#include "ioINIManager.h"
#include <strsafe.h>

template<> ioModeINIManager* Singleton< ioModeINIManager >::ms_Singleton = 0;
ioModeINIManager::ioModeINIManager()
{
}

ioModeINIManager::~ioModeINIManager()
{
	INILoaderMap::iterator iter = m_INILoaderMap.begin();
	for(;iter != m_INILoaderMap.end();iter++)
	{
		SAFEDELETE( iter->second );
	}
	m_INILoaderMap.clear();
}

ioModeINIManager& ioModeINIManager::GetSingleton()
{
	return Singleton< ioModeINIManager >::GetSingleton();
}

void ioModeINIManager::InsertINI( const ioHashString &rkFileName )
{
	INILoaderMap::iterator iter = m_INILoaderMap.find( rkFileName );
	if( iter != m_INILoaderMap.end() )	
	{
		ioINILoader *pLoader = iter->second;

		pLoader->SetFileName( rkFileName.c_str() );
		pLoader->ReloadFile( rkFileName.c_str() );
		return;
	}
	m_INILoaderMap.insert( INILoaderMap::value_type( rkFileName, new ioINILoader( rkFileName.c_str() ) ) );	
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][mode]Insert mode info : [%s]", rkFileName.c_str() );
}

void ioModeINIManager::LoadINIData( const ioHashString &rkFileName )
{
	ioINILoader kLoader( rkFileName.c_str() );
	kLoader.SetTitle( "Common" );
	int iMaxINI = kLoader.LoadInt( "MaxINI", 0 );
	for(int i = 0;i < iMaxINI;i++)
	{
		char szKey[MAX_PATH] = "";
		char szBuf[MAX_PATH] = "";
		sprintf_s( szKey, "FileName%d", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		InsertINI( szBuf );
	}
}

void ioModeINIManager::ReloadINIData()
{
	INILoaderMap::iterator iter = m_INILoaderMap.begin();
	for(;iter != m_INILoaderMap.end();iter++)
	{
		const ioHashString &rkFileName = iter->first;

		ioINILoader *pLoader = iter->second;
		pLoader->SetFileName( rkFileName.c_str() );
		if( pLoader->ReadBool( "info", "Change", false ) )
		{
			//pLoader->SaveBool( "info", "change", false );

			pLoader->ReloadFile( rkFileName.c_str() );
		}
	}
}

ioINILoader &ioModeINIManager::GetINI( const ioHashString &rkFileName )
{
	INILoaderMap::iterator iter = m_INILoaderMap.find( rkFileName );
	if( iter != m_INILoaderMap.end() )	
	{
		return *iter->second;
	}

	// 없으면 넣는다.
	InsertINI( rkFileName );
	return GetINI( rkFileName );
}