#include "stdafx.h"
#include "PersonalHQInven.h"
#include "User.h"
#include "../MainProcess.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"

PersonalHQInven::PersonalHQInven()
{
}

PersonalHQInven::~PersonalHQInven()
{
}

void PersonalHQInven::Init(User* pUser)
{
	m_mUserInvenInfo.clear();
	m_pUser	= pUser;
}

void PersonalHQInven::Destroy()
{
	m_mUserInvenInfo.clear();
	m_pUser	= NULL;
}

void PersonalHQInven::DBtoData( CQueryResultData *query_data )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "PersonalHQInven::DBtoData() User NULL!!"); 
		return;
	}

	LOOP_GUARD();
	while( query_data->IsExist() )
	{	
		DWORD dwItemCode	= 0;
		int iCount			= 0;

		PACKET_GUARD_BREAK( query_data->GetValue( dwItemCode, sizeof(dwItemCode) ) );	//블럭 코드
		PACKET_GUARD_BREAK( query_data->GetValue( iCount, sizeof(iCount) ) );			//소지 수

		m_mUserInvenInfo.insert( make_pair(dwItemCode, iCount) );
	}
	LOOP_GUARD_CLEAR();

	SendInvenInfo();
}

void PersonalHQInven::SendInvenInfo()
{
	if( !m_pUser )
		return;

	int iSize	= m_mUserInvenInfo.size();

	SP2Packet kPacket(STPK_PERSONAL_HQ_INVEN_DATA);
	PACKET_GUARD_VOID( kPacket.Write(iSize) );

	INVENINFO::iterator it	= m_mUserInvenInfo.begin();

	for( ; it != m_mUserInvenInfo.end() ; it++ )
	{
		DWORD dwCode	= it->first;
		int iCount		= it->second;

		PACKET_GUARD_VOID( kPacket.Write(dwCode) );
		PACKET_GUARD_VOID( kPacket.Write(iCount) );
	}

	m_pUser->SendMessage(kPacket);
}

void PersonalHQInven::FillMoveData( SP2Packet &rkPacket )
{
	int iSize	= m_mUserInvenInfo.size();
	PACKET_GUARD_VOID( rkPacket.Write(iSize) );

	INVENINFO::iterator it = m_mUserInvenInfo.begin();

	for( ; it != m_mUserInvenInfo.end() ; it++ )
	{
		DWORD dwItemCode	= it->first;
		int	  iItemCount	= it->second;

		PACKET_GUARD_VOID( rkPacket.Write(dwItemCode) );
		PACKET_GUARD_VOID( rkPacket.Write(iItemCount) );
	}
}

void PersonalHQInven::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode )
{
	int iSize			= 0;
	int iCount			= 0;
	DWORD dwItemCode	= 0;
	
	PACKET_GUARD_VOID( rkPacket.Read(iSize) );

	for( int i = 0; i < iSize; i++ )
	{
		PACKET_GUARD_VOID( rkPacket.Read(dwItemCode) );
		PACKET_GUARD_VOID( rkPacket.Read(iCount) );

		m_mUserInvenInfo.insert( make_pair(dwItemCode, iCount) );
	}
}

int PersonalHQInven::AddBlockCount(const DWORD dwItemCode, const int iCount)
{
	INVENINFO::iterator it	= m_mUserInvenInfo.find(dwItemCode);

	if( it == m_mUserInvenInfo.end() )
	{
		m_mUserInvenInfo.insert( make_pair(dwItemCode, iCount) );
		return iCount;
	}
	else
		it->second += iCount;

	return it->second;
}

int PersonalHQInven::DecreaseBlockCount(const DWORD dwItemCode, const int iCount)
{
	INVENINFO::iterator it	= m_mUserInvenInfo.find(dwItemCode);
	
	if( it == m_mUserInvenInfo.end() )
		return -1;

	int iRemainCount = it->second - iCount;

	if( iRemainCount < 0 )
		return -1;

	it->second -= iCount;

	return it->second;
}

void PersonalHQInven::FillAllBlockInfo(SP2Packet &rkPacket)
{
	PACKET_GUARD_VOID( rkPacket.Write((int)m_mUserInvenInfo.size()) );
	INVENINFO::iterator it = m_mUserInvenInfo.begin();

	for( ; it != m_mUserInvenInfo.end() ; it++ )
	{
		DWORD dwItemCode	= it->first;
		int	  iItemCount	= it->second;

		PACKET_GUARD_VOID( rkPacket.Write(dwItemCode) );
		PACKET_GUARD_VOID( rkPacket.Write(iItemCount) );
	}
}

int PersonalHQInven::GetItemCount(const DWORD dwItemCode)
{
	INVENINFO::iterator it	= m_mUserInvenInfo.find(dwItemCode);
	
	if( it == m_mUserInvenInfo.end() )
		return 0;

	return it->second;
}