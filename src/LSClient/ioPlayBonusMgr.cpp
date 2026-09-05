
#include "stdafx.h"

#include "ioPlayBonusMgr.h"

template<> ioPlayBonusMgr* Singleton< ioPlayBonusMgr >::ms_Singleton = 0;

ioPlayBonusMgr::ioPlayBonusMgr()
{
	Initialize();
}

ioPlayBonusMgr::~ioPlayBonusMgr()
{
}

ioPlayBonusMgr& ioPlayBonusMgr::GetSingleton()
{
	return Singleton< ioPlayBonusMgr >::GetSingleton();
}

void ioPlayBonusMgr::Initialize()
{
	m_PlayBonusMap.clear();
}

void ioPlayBonusMgr::EnterUser( const ioHashString &szUserID )
{
	PlayBonusMap::iterator iter = m_PlayBonusMap.find( szUserID );
	if( iter != m_PlayBonusMap.end() )
		return;  //이미 있는 유저
	
	PlayBonusData PlayBonus;
	PlayBonus.Init();
	m_PlayBonusMap.insert( PlayBonusMap::value_type( szUserID, PlayBonus ) );
}

void ioPlayBonusMgr::LeaveUser( const ioHashString &szUserID )
{
	PlayBonusMap::iterator iter = m_PlayBonusMap.find( szUserID );
	if( iter != m_PlayBonusMap.end() )
	{
		PlayBonusData &kPlayBonus = iter->second;
		kPlayBonus.Init();
	}
}

void ioPlayBonusMgr::SetUserPlayBonus( const ioHashString &rkName, bool &rkPCRoomBonus, bool &rkGuildBonus, bool &rkFriendBonus )
{
	PlayBonusMap::iterator iter = m_PlayBonusMap.find( rkName );
	if( iter != m_PlayBonusMap.end() )
	{
		PlayBonusData &kPlayBonus = iter->second;
		kPlayBonus.m_bPCRoomBonus = rkPCRoomBonus;
		kPlayBonus.m_bGuildBonus  = rkGuildBonus;
		kPlayBonus.m_bFriendBonus = rkFriendBonus;

		// 개발자 & 매니저 & 마스터는 친구 보너스 표시 X
		if( g_App.IsAdminID( rkName.c_str() ) )
			kPlayBonus.m_bFriendBonus = false;
	}
	//else
		//LOG.PrintTimeAndLog( 0, "Not PlayUserBonus : Set : %s", rkName.c_str() );
}

void ioPlayBonusMgr::GetUserPlayBonus( const ioHashString &rkName, bool &rkPCRoomBonus, bool &rkGuildBonus, bool &rkFriendBonus )
{
	PlayBonusMap::iterator iter = m_PlayBonusMap.find( rkName );
	if( iter != m_PlayBonusMap.end() )
	{
		PlayBonusData &kPlayBonus = iter->second;
		rkPCRoomBonus = kPlayBonus.m_bPCRoomBonus;
		rkGuildBonus  = kPlayBonus.m_bGuildBonus;
		rkFriendBonus = kPlayBonus.m_bFriendBonus;
	}
	//else
		//LOG.PrintTimeAndLog( 0, "Not PlayUserBonus : Get : %s", rkName.c_str() );
}