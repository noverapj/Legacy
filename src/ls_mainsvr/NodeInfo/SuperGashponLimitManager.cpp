#include "../stdafx.h"

#include "../EtcHelpFunc.h"
#include "ServerNodeManager.h"

#include "SuperGashponLimitManager.h"

template<> SuperGashponLimitManager* Singleton< SuperGashponLimitManager >::ms_Singleton = 0;

SuperGashponLimitManager::SuperGashponLimitManager()
{
}

SuperGashponLimitManager::~SuperGashponLimitManager()
{
	SaveLimit();	
}

SuperGashponLimitManager& SuperGashponLimitManager::GetSingleton()
{
	return Singleton< SuperGashponLimitManager >::GetSingleton();
}

void SuperGashponLimitManager::InitLimit()
{
	ioINILoader kLoader( "config/sp2_super_gashpon_limit.ini" );

	char szBuffer[MAX_PATH];
	kLoader.SetTitle( "common" );
	int iMaxLimit = kLoader.LoadInt( "MaxLimit", 0 );

	for( int iLimit = 1; iLimit <= iMaxLimit; ++iLimit )
	{
		sprintf_s( szBuffer, "Limit%d", iLimit );
		kLoader.SetTitle( szBuffer );

		SuperGashponLimit Limit;
		Limit.m_dwEtcItemCode = kLoader.LoadInt( "EtcItemType", 0 );		
		Limit.m_dwLimit		  = kLoader.LoadInt( "Limit", 0 );

		m_vSuperGashponLimit.push_back( Limit );
	}
}

void SuperGashponLimitManager::SaveLimit()
{
	if( m_vSuperGashponLimit.empty() ) return;

	ioINILoader kLoader( "config/sp2_super_gashpon_limit.ini" );
	kLoader.SaveInt( "common", "MaxLimit", (int)m_vSuperGashponLimit.size() );

	char szTitle[MAX_PATH];	
	vSuperGashponLimit::iterator iter = m_vSuperGashponLimit.begin();
	for( int iPresent = 1; iter != m_vSuperGashponLimit.end(); ++iter, ++iPresent )
	{		
		SuperGashponLimit& Limit = *iter;

		sprintf_s( szTitle, "Limit%d", iPresent );
		kLoader.SaveInt( szTitle, "EtcItemType", Limit.m_dwEtcItemCode );		
		kLoader.SaveInt( szTitle, "Limit", Limit.m_dwLimit );
	}

	m_vSuperGashponLimit.clear();
}

void SuperGashponLimitManager::ServerDownAllSave()
{
	SaveLimit();
}

SuperGashponLimitManager::SuperGashponLimit& SuperGashponLimitManager::GetLimitData( DWORD dwEtcItemCode )
{
	vSuperGashponLimit::iterator iter = m_vSuperGashponLimit.begin();
	for( ; iter != m_vSuperGashponLimit.end(); ++iter )
	{
		SuperGashponLimit& Limit = *iter;
		if( Limit.m_dwEtcItemCode == dwEtcItemCode )
			return Limit;
	}

	SuperGashponLimit Limit;
	Limit.m_dwEtcItemCode  = dwEtcItemCode;	
	m_vSuperGashponLimit.push_back( Limit );

	return m_vSuperGashponLimit.back();
}

bool SuperGashponLimitManager::IncraseLimit( DWORD dwEtcItemCode, DWORD dwMaxLimit )
{
	SuperGashponLimit& rkLimit = GetLimitData( dwEtcItemCode );
	if( rkLimit.m_dwLimit < dwMaxLimit )
	{
		rkLimit.m_dwLimit++;
		return true;
	}
	return false;
}

void SuperGashponLimitManager::DecraseLimit( DWORD dwEtcItemCode )
{
	SuperGashponLimit& rkLimit = GetLimitData( dwEtcItemCode );

	if( rkLimit.m_dwLimit == 0 )
		return;

	--rkLimit.m_dwLimit;
}

DWORD SuperGashponLimitManager::GetLimit( DWORD dwEtcItemCode )
{
	SuperGashponLimit& rkLimit = GetLimitData( dwEtcItemCode );
	return rkLimit.m_dwLimit;
}

void SuperGashponLimitManager::LimitReset( DWORD dwEtcItemCode )
{
	SuperGashponLimit& rkLimit = GetLimitData( dwEtcItemCode );
	rkLimit.m_dwLimit = 0;
}