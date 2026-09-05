#include "../stdafx.h"
#include "GuildRoomInfos.h"

template<> GuildRoomInfos* Singleton< GuildRoomInfos >::ms_Singleton = 0;

GuildRoomInfos::GuildRoomInfos()
{
	Init();
}

GuildRoomInfos::~GuildRoomInfos()
{
	Destroy();
}

void GuildRoomInfos::Init()
{
	m_mGuildRoomInfos.clear();
	m_bActive	= FALSE;
	m_iReqCount	= 0;
}

void GuildRoomInfos::Destroy()
{
	m_mGuildRoomInfos.clear();
}

GuildRoomInfos& GuildRoomInfos::GetSingleton()
{
	return Singleton< GuildRoomInfos >::GetSingleton();
}

BOOL GuildRoomInfos::IsExistGuildRoom(DWORD dwGuildIndex)
{
	GUILDROOMINFOS::iterator it = m_mGuildRoomInfos.find(dwGuildIndex);
	if( it == m_mGuildRoomInfos.end() )
		return FALSE;

	return TRUE;
}

BOOL GuildRoomInfos::IsPrepare(DWORD dwGuildIndex)
{
	GUILDROOMINFOS::iterator it = m_mGuildRoomInfos.find(dwGuildIndex);
	if( it == m_mGuildRoomInfos.end() )
		return TRUE;

	if( it->second != 0 )
		return FALSE;

	return TRUE;
}

BOOL GuildRoomInfos::AddGuildRoomInfo(DWORD dwGuildIndex)
{
	GUILDROOMINFOS::iterator it = m_mGuildRoomInfos.find(dwGuildIndex);
	if( it != m_mGuildRoomInfos.end() )
		return FALSE;

	m_mGuildRoomInfos.insert( make_pair(dwGuildIndex, 0) );
	return TRUE;
}

DWORD GuildRoomInfos::GetGuildRoomIndex(DWORD dwGuildIndex)
{
	GUILDROOMINFOS::iterator it = m_mGuildRoomInfos.find(dwGuildIndex);
	if( it == m_mGuildRoomInfos.end() )
		return 0;

	return it->second;
}

int GuildRoomInfos::GetRoomResultByGuildIndex(DWORD dwGuildIndex)
{
	if( !IsActive() )
		return GUILD_ROOM_WAIT;

	if( IsExistGuildRoom(dwGuildIndex) )
	{
		if( IsPrepare(dwGuildIndex) )
			return GUILD_ROOM_WAIT;
	}
	else
	{
		AddGuildRoomInfo(dwGuildIndex);
		return GUILD_ROOM_CREATE;
	}

	return GUILD_ROOM_INFO;
}

void GuildRoomInfos::DeleteGuildRoomInfo(DWORD dwGuildIndex, DWORD dwRoomIndex)
{
	GUILDROOMINFOS::iterator it = m_mGuildRoomInfos.find(dwGuildIndex);
	if( it == m_mGuildRoomInfos.end() )
		return;

	if( it->second == dwRoomIndex )
		m_mGuildRoomInfos.erase(it);
	else
	{
		if( it->second == 0 )
			m_mGuildRoomInfos.erase(it);
	}
}

BOOL GuildRoomInfos::UpdateGuildRoomIndex(DWORD dwGuildIndex, DWORD dwRoomIndex)
{
	GUILDROOMINFOS::iterator it = m_mGuildRoomInfos.find(dwGuildIndex);
	if( it == m_mGuildRoomInfos.end() )
		return FALSE;

	if( it->second != 0 )
		return FALSE;

	it->second = dwRoomIndex;
	return TRUE;
}

void GuildRoomInfos::AddRequestCount()
{
	//InterlockedIncrement(&m_iReqCount);
	m_iReqCount++;
}

void GuildRoomInfos::DecreaseRequestCount()
{
	//InterlockedDecrement(&m_iReqCount);
	m_iReqCount--;
	if( m_iReqCount <= 0 )
	{
		m_iReqCount	= 0;
		m_bActive	= TRUE;
	}
}