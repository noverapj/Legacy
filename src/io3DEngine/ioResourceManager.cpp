
#include "stdafx.h"
#include "ioResource.h"
#include "ioResourceManager.h"

ioResourceManager::ioResourceManager() : m_StaticSwitch( SATICS_OFF )
{
	m_DefaultErasePolicy = EP_AUTOMATIC;
	ZeroMemory( m_szStartDir, MAX_PATH );
}

ioResourceManager::~ioResourceManager()
{
}

void ioResourceManager::AddMem( const ioHashString& KeyName, DWORD dwMem )
{
	if( m_StaticSwitch == SATICS_ON )
	{
		m_MemoryMap.insert( MemoryMap::value_type( KeyName, dwMem ) );
	}
}

void ioResourceManager::RemoveMem( const ioHashString& KeyName )
{
	if( m_StaticSwitch == SATICS_ON )
	{
		MemoryMap::iterator iter = m_MemoryMap.find( KeyName );
		if( iter != m_MemoryMap.end() )
		{
			m_MemoryMap.erase( iter );
		}
	}
}

void ioResourceManager::GetMemStatics( DWORD& dwCount, DWORD& dwSize )
{
	dwCount	= 0;
	dwSize	= 0;

	for( MemoryMap::iterator iter = m_MemoryMap.begin(); iter != m_MemoryMap.end(); ++iter )
	{
		dwSize	+= iter->second;
		dwCount++;
	}
}

void ioResourceManager::PrepareLoading( const ioHashString &name, bool bThread /* = false  */ )
{
	if( name.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "ioResourceManager::PrepareLoading - Resource Name Empty" );
		return;
	}

	if( GetByName( name ) != NULL ) return;

	// 미리 메모리에만 준비 시킨다.
	ioResource *pRes = CreateImpl( name, bThread );
	if( pRes )
	{
		RegisterImpl( pRes );       
	}
}

ioResource* ioResourceManager::Create( const ioHashString &name, bool bThread )
{
	if( name.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "ioResourceManager::Create - Resource Name Empty" );
		return NULL;
	}

	ioResource *pRes = GetByName( name );
	if( pRes )
	{
		pRes->AddRef();
		return pRes;
	}

	pRes = CreateImpl( name, bThread );
	if( pRes )
	{
		RegisterImpl( pRes );

		pRes->AddRef();
		pRes->SetErasePolicy( m_DefaultErasePolicy );
		return pRes;
	}

	return NULL;
}

ioResource* ioResourceManager::GetByName( const ioHashString& name )
{
	ResourceMap::iterator	iter = m_Resource.find( name );
	if( iter != m_Resource.end() )
		return iter->second;

	return NULL;
}

void ioResourceManager::Destroy( ioResource *pRes )
{
	if( NULL == pRes )		return;
	if( pRes->Release() > 0 )		return;
	if( pRes->GetErasePolicy() != EP_AUTOMATIC )	return;

	if( pRes->IsLoadDone() )
	{
		m_Resource.erase( pRes->GetName() );
		RemoveMem( pRes->GetName() );
		delete pRes;
	}
}

void ioResourceManager::ForceDestroy( ioResource *pRes )
{
	if( NULL == pRes )		return;
	if( pRes->IsLoadDone() )
	{
		m_Resource.erase( pRes->GetName() );
		delete pRes;
	}
}

void ioResourceManager::DestroyAll()
{
	ResourceMap::iterator iter;
	for( iter=m_Resource.begin() ; iter!=m_Resource.end() ; ++iter )
	{
		ioResource *pRes = iter->second;
		if (pRes->PeekRef() > 0 && pRes->GetErasePolicy() != EP_NO_ERASE )
		{
			LOG.PrintTimeAndLog( 0, "%s Not Released(%d)", pRes->GetName().c_str(),
														   pRes->PeekRef() );
		}

		delete pRes;
	}

	m_Resource.clear();
}

void ioResourceManager::DestroyNoneRefResource( ErasePolicy ePolicy )
{
	ResourceMap::iterator iter=m_Resource.begin();
	while( iter!=m_Resource.end() )
	{
		ioResource *pRes = iter->second;

		if( !pRes->IsLoadDone() )
		{
			++iter;
		}
		else
		{
			if( pRes->PeekRef() <= 0 && pRes->GetErasePolicy() == ePolicy )
			{
				iter = m_Resource.erase( iter );
				delete pRes;
			}
			else
			{
				++iter;
			}
		}
	}
}

void ioResourceManager::PrintAllResources()
{
	ResourceMap::iterator iter;
	for( iter=m_Resource.begin() ; iter!=m_Resource.end() ; ++iter )
	{
		ioResource *pRes = iter->second;
		if( pRes )
		{
			LOG.PrintTimeAndLog( 0, "%s(%d)", pRes->GetName().c_str(), pRes->PeekRef() );
		}
	}
}

void ioResourceManager::NotifyLoadDone( ioStream *pStream, void *pParam, bool bLoaded )
{
}

void ioResourceManager::RegisterImpl( ioResource *pRes )
{
	std::pair< std::map< ioHashString, ioResource* >::iterator, bool> ret;
	if( pRes )
	{
		//뭔가 냄새가 난다... 윤태철
		ret = m_Resource.insert( std::pair<ioHashString, ioResource*>( pRes->GetName(), pRes ) );
		if( ret.second == false )
			LOG.PrintTimeAndLog( 0, "ioResourceManager Error -> resource already existed ( %d )", pRes->GetName().c_str() );
		//m_Resource.insert( ResourceMap::value_type( pRes->GetName(), pRes ) );
	}
}

void ioResourceManager::SetStartDir( const char *szPath )
{
	if( szPath )
		strcpy( m_szStartDir, szPath );
	else
		memset( m_szStartDir, 0, sizeof(m_szStartDir) );
}

void ioResourceManager::SetDefaultErasePolicy( ErasePolicy ePolicy )
{
	m_DefaultErasePolicy = ePolicy;
}
