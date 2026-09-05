
#include "stdafx.h"

#include "ioVictoriesMgr.h"

template<> ioVictoriesMgr* Singleton< ioVictoriesMgr >::ms_Singleton = 0;

ioVictoriesMgr::ioVictoriesMgr()
{
	Initialize();
}

ioVictoriesMgr::~ioVictoriesMgr()
{
}

ioVictoriesMgr& ioVictoriesMgr::GetSingleton()
{
	return Singleton< ioVictoriesMgr >::GetSingleton();
}

ioVictoriesMgr::VictoriesData &ioVictoriesMgr::FindList( const ioHashString &szUserID )
{
	int iSize = m_VictoriesData.size();
	for(int i = 0;i < iSize;i++)
	{
		VictoriesData &kData = m_VictoriesData[i];
		if( kData.m_szUserID == szUserID )
			return kData;
	}

	static VictoriesData kNullData;
	return kNullData;
}

void ioVictoriesMgr::Initialize()
{
	m_VictoriesData.clear();
}

void ioVictoriesMgr::EnterUser( const ioHashString &szUserID )
{
	VictoriesData &kData = FindList( szUserID );
	if( kData.m_szUserID != szUserID )
	{
		VictoriesData kVictoriesData;
		kVictoriesData.m_szUserID = szUserID;
		m_VictoriesData.push_back( kVictoriesData );
	}
}

void ioVictoriesMgr::LeaveUser( const ioHashString &szUserID )
{
	int iSize = m_VictoriesData.size();
	for(int i = 0;i < iSize;i++)
	{
		VictoriesData &kData = m_VictoriesData[i];
		if( kData.m_szUserID == szUserID )
		{
			m_VictoriesData.erase( m_VictoriesData.begin() + i );
			return;
		}
	}
}

int ioVictoriesMgr::GetVictories( const ioHashString &szUserID )
{
	VictoriesData &kData = FindList( szUserID );
	if( kData.m_szUserID == szUserID )
		return kData.m_iVictories;
	return 0;
}

void ioVictoriesMgr::SetVictories( const ioHashString &szUserID, bool bWinTeam )
{
	VictoriesData &kData = FindList( szUserID );
	if( kData.m_szUserID == szUserID )
	{
		if( bWinTeam )
			kData.m_iVictories++;
		else
			kData.m_iVictories = 0;
	}
	else
		LOG.PrintTimeAndLog( 0, "SetVictories Fun One: None User Name :%s", szUserID.c_str() );
}

void ioVictoriesMgr::SetVictories( const ioHashString &szUserID, int iVictories )
{
	VictoriesData &kData = FindList( szUserID );
	if( kData.m_szUserID == szUserID )
	{
		kData.m_iVictories = iVictories;
	}
	else
		LOG.PrintTimeAndLog( 0, "SetVictories Fun Two: None User Name :%s", szUserID.c_str() );
}