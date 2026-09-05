#include "stdafx.h"
#include "../DataBase/DBClient.h"
#include "../QueryData/QueryResultData.h"
#include "ServerGuildInvenInfo.h"

template<> ServerGuildInvenInfo* Singleton< ServerGuildInvenInfo >::ms_Singleton = 0;

ServerGuildInvenInfo::ServerGuildInvenInfo()
{
	Init();
}

ServerGuildInvenInfo::~ServerGuildInvenInfo()
{
	Destroy();
}

ServerGuildInvenInfo& ServerGuildInvenInfo::GetSingleton()
{
	return Singleton< ServerGuildInvenInfo >::GetSingleton();
}

void ServerGuildInvenInfo::Init()
{
	m_mServerGuildInvenInfo.clear();
}

void ServerGuildInvenInfo::Destroy()
{
	m_mServerGuildInvenInfo.clear();
}

void ServerGuildInvenInfo::SQLCheckInvenData(const DWORD dwGuildIndex, const DWORD dwUserIndex, const int iRequestType)
{
	//DB업데이트 버전 요청.
	g_DBClient.OnSelectGuildInvenVersion(dwUserIndex, dwGuildIndex, iRequestType);
}

void ServerGuildInvenInfo::RequestGuildInvenData(const DWORD dwGuildIndex, const DWORD dwUserIndex, const int iRequestType)
{
	SQLCheckInvenData(dwGuildIndex, dwUserIndex, iRequestType);
}

BOOL ServerGuildInvenInfo::IsPrevData(const DWORD dwGuildIndex, const __int64 i64Ver)
{
	SERVERGUILDINVENINFO::iterator it = m_mServerGuildInvenInfo.find(dwGuildIndex);
	if( it == m_mServerGuildInvenInfo.end() )
		return TRUE;

	GuildInvenInfo& stInfo = it->second;
	if( stInfo.i64UpdateVer < i64Ver )
		return TRUE;

	return FALSE;
}

BOOL ServerGuildInvenInfo::FillGuildInvenData(const DWORD dwGuildIndex, SP2Packet& kPacket)
{
	SERVERGUILDINVENINFO::iterator it = m_mServerGuildInvenInfo.find(dwGuildIndex);
	if( it == m_mServerGuildInvenInfo.end() )
		return FALSE;

	GuildInvenInfo& stInfo = it->second;

	GUILDINVENINFO& mInven	= stInfo.mInvenInfo;

	int iSize = mInven.size();

	PACKET_GUARD_BOOL( kPacket.Write(iSize) );

	GUILDINVENINFO::iterator it2 = mInven.begin();

	while( it2 != mInven.end() )
	{
		DWORD dwItemCode = it2->first;
		int iCount	= it2->second;

		PACKET_GUARD_BOOL( kPacket.Write(dwItemCode) );
		PACKET_GUARD_BOOL( kPacket.Write(iCount) );

		it2++;
	}
	
	return TRUE;
}

void ServerGuildInvenInfo::UpdateInvenVer(const DWORD dwGuildIndex, const __int64 i64Ver)
{
	SERVERGUILDINVENINFO::iterator it = m_mServerGuildInvenInfo.find(dwGuildIndex);
	if( it == m_mServerGuildInvenInfo.end() )
		return;

	GuildInvenInfo& stInfo = it->second;
	stInfo.i64UpdateVer = i64Ver;
}

void ServerGuildInvenInfo::InsertInvenData(const DWORD dwGuildIndex, CQueryResultData* query_data)
{
	SERVERGUILDINVENINFO::iterator it = m_mServerGuildInvenInfo.find(dwGuildIndex);
	if( it == m_mServerGuildInvenInfo.end() )
	{
		GuildInvenInfo stInfo;
		it = m_mServerGuildInvenInfo.insert( m_mServerGuildInvenInfo.begin(), std::make_pair(dwGuildIndex, stInfo) );
	}

	GuildInvenInfo& stInfo = it->second;
	stInfo.mInvenInfo.clear();

	//가져온다.
	while( query_data->IsExist() )
	{
		DWORD dwItemCode	= 0;
		int iCount			= 0;

		PACKET_GUARD_VOID( query_data->GetValue( dwItemCode, sizeof(DWORD) ) );
		PACKET_GUARD_VOID( query_data->GetValue( iCount, sizeof(int) ) );

		stInfo.mInvenInfo.insert( std::make_pair(dwItemCode, iCount) );
	}
}

int ServerGuildInvenInfo::GetItemCount(const DWORD dwGuildIndex, const DWORD dwItemCode)
{
	SERVERGUILDINVENINFO::iterator it	= m_mServerGuildInvenInfo.find(dwGuildIndex);
	if( it == m_mServerGuildInvenInfo.end() )
		return 0;

	GuildInvenInfo& stInfo	= it->second;

	GUILDINVENINFO::iterator itItem	= stInfo.mInvenInfo.find(dwItemCode);
	if( itItem == stInfo.mInvenInfo.end() )
		return 0;

	return itItem->second;
}