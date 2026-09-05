

#include "stdafx.h"

#include "ioMaterial.h"
#include "ioTechnique.h"
#include "ioPass.h"
#include "ioTextureUnitState.h"

#include "ioMaterialModifier.h"
#include "ioMaterialManager.h"

#include "ioStream.h"
#include "ioResourceLoader.h"
#include "ioStringConverter.h"

using namespace std;

template<> ioMaterialManager* Singleton< ioMaterialManager >::ms_Singleton = 0;

ioMaterialManager::ioMaterialManager()
{
	m_pScriptModifier = NULL;
	m_pCloneModifier = NULL;

	ZeroMemory( m_szStartDir, MAX_PATH );
	strcpy( m_szStartDir, "Material" );
	m_iUnNamedCnt = 0;

	char szFileName[_MAX_PATH] = "";
	
	ZeroMemory( szFileName, MAX_PATH );
	GetCurrentDirectory( _MAX_PATH, szFileName);

	strcat( szFileName, "/info/XY.txt" );

	HANDLE hFile = CreateFile( szFileName, 
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL );
	if( hFile != INVALID_HANDLE_VALUE )
		m_bUseNearest = true;
	else
		m_bUseNearest = false;
}

ioMaterialManager::~ioMaterialManager()
{
	MaterialScriptMap::iterator iter;
	for( iter=m_MaterialScriptMap.begin() ; iter!=m_MaterialScriptMap.end() ; ++iter )
	{
		delete iter->second;
	}

	SAFEDELETE( m_pScriptModifier );
	SAFEDELETE( m_pCloneModifier );

	m_MaterialScriptMap.clear();
}

ioMaterial* ioMaterialManager::CreateMaterial()
{
	m_iUnNamedCnt++;

	char szUnNamed[MAX_PATH];
	wsprintf( szUnNamed, "UnNamed_%d", m_iUnNamedCnt );

	return new ioMaterial( szUnNamed );
}

ioMaterial* ioMaterialManager::CreateMaterial( const ioHashString &name )
{
	ioMaterial *pScript = GetMaterial( name );
	if( !pScript )
	{
		ioMaterial *pMtl = new ioMaterial( name );
		m_MaterialScriptMap.insert( MaterialScriptMap::value_type( name, pMtl ) );
		return pMtl;
	}

	LOG.PrintTimeAndLog( 0, "ioMaterialManager - %s Exist Material", name.c_str() );
	return pScript;
}

ioMaterial* ioMaterialManager::CloneMaterial( const ioHashString &name )
{
	const ioMaterial *pScript = GetMaterial( name );
	if( pScript )
	{
		ioMaterial *pMtl = new ioMaterial( name );
		*pMtl = *pScript;

		if( m_pCloneModifier )
		{
			m_pCloneModifier->ModifyMaterial( pMtl );
		}

		return pMtl;
	}

	return NULL;
}

ioMaterial* ioMaterialManager::GetMaterial( const ioHashString &name )
{
	MaterialScriptMap::iterator iter = m_MaterialScriptMap.find( name );
	if( iter != m_MaterialScriptMap.end() )
		return iter->second;

	return NULL;
}

int ioMaterialManager::ParseScript( const ioHashString &szScriptName )
{
	SubMaterialCountMap::iterator iter = m_SubMaterialCountMap.find( szScriptName );
	if( iter != m_SubMaterialCountMap.end() )
		return iter->second;

	char szFullName[MAX_PATH];
	wsprintf( szFullName, "%s/%s.txt", m_szStartDir, szScriptName.c_str() );

	ioTextStream kStream;
	if( g_ResourceLoader.LoadStream( szFullName, &kStream ) )
	{
		int iSubMaterialCount = m_Serializer.ParseScript( kStream, szScriptName.c_str() );
		if( iSubMaterialCount > 0 )
		{
			m_SubMaterialCountMap.insert( SubMaterialCountMap::value_type( szScriptName, iSubMaterialCount ) );
			return iSubMaterialCount;
		}
	}

	LOG.PrintTimeAndLog( 0, "ioMaterialManager::ParseScript - %s Load Failed", szFullName );
	return -1;
}

void ioMaterialManager::SaveScript( std::vector< ioMaterial* > &rkList, const char *szFullPath )
{
	FILE *fp = fopen( szFullPath, "wt" );
	if( !fp )
	{
		LOG.PrintTimeAndLog( 0, "ioMaterialManaager::SaveScript - %s Save Failed", szFullPath );
		return;
	}

	int iMaterialCnt = rkList.size();
	for( int i=0 ; i<iMaterialCnt ; i++ )
	{
		m_Serializer.SaveScript( fp, rkList[i] );
	}

	fclose( fp );
}

void ioMaterialManager::SetStartDir( const char *szPath )
{
	strcpy( m_szStartDir, szPath );
}

const char* ioMaterialManager::GetStartDir() const
{
	return m_szStartDir;
}

void ioMaterialManager::SetLoadScriptModifier( ioMaterialScriptModifier *pModifier )
{
	SAFEDELETE( m_pScriptModifier );
	m_pScriptModifier = pModifier;
}

void ioMaterialManager::SetCloneScriptModifier( ioMaterialScriptModifier *pModifier )
{
	SAFEDELETE( m_pCloneModifier );
	m_pCloneModifier = pModifier;
}

void ioMaterialManager::ApplyLoadScriptModifier( ioMaterial *pMaterial )
{
	if( m_pScriptModifier )
	{
		m_pScriptModifier->ModifyMaterial( pMaterial );
	}
}

void ioMaterialManager::ApplyCloneScriiptModifier( ioMaterial *pMaterial )
{
	if( m_pCloneModifier )
	{
		m_pCloneModifier->ModifyMaterial( pMaterial );
	}
}

void ioMaterialManager::ModifyLoadedScript(ioMaterialScriptModifier *pModifier )
{
	MaterialScriptMap::iterator iter;
	for( iter=m_MaterialScriptMap.begin() ; iter!=m_MaterialScriptMap.end() ; ++iter )
	{
		pModifier->ModifyMaterial( iter->second );
	}
}

ioMaterialManager& ioMaterialManager::GetSingleton()
{
	return Singleton<ioMaterialManager>::GetSingleton();
}
