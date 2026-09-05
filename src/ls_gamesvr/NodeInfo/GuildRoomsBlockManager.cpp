#include "stdafx.h"
#include "GuildRoomInfos.h"
#include "../MainServerNode/MainServerNode.h"
#include "../DataBase/DBClient.h"
#include "User.h"
#include "GuildRoomsBlockManager.h"

template<> GuildRoomsBlockManager* Singleton< GuildRoomsBlockManager >::ms_Singleton = 0;

GuildRoomsBlockManager::GuildRoomsBlockManager()
{
	Init();
}

GuildRoomsBlockManager::~GuildRoomsBlockManager()
{
	Destroy();
}

void GuildRoomsBlockManager::Init()
{
	m_iLimitedInstallCount	= 0;

	m_mGuildBlockInfos.clear();
	m_vDefaultConstructInfo.clear();
}

void GuildRoomsBlockManager::LoadIni()
{
	m_vDefaultConstructInfo.clear();
	
	char szKey[MAX_PATH] = "";
	ioINILoader kLoader;

	kLoader.ReloadFile( "config/sp2_guild_room_block.ini" );

	kLoader.SetTitle( "common" );
	m_iLimitedInstallCount = kLoader.LoadInt("limited_install_cnt", 0);

	kLoader.SetTitle( "default_construct" );

	for( int i	= 0; i < 4000; i++ )
	{
		wsprintf( szKey, "item%d", i+1 );
		DWORD dwItemCode	= kLoader.LoadInt(szKey, 0);

		if( 0 == dwItemCode )
			break;

		if( !g_BlockPropertyMgr.IsValidItemCode(dwItemCode) )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][block]Invalid default block coordinate info : [%d]", dwItemCode );
			continue;
		}

		BlockDefaultInfo stInfo;

		stInfo.dwItemCode = dwItemCode;

		wsprintf( szKey, "XZ%d", i+1 );
		stInfo.iXZ			= kLoader.LoadInt(szKey, 0);

		wsprintf( szKey, "Y%d", i+1 );
		stInfo.iY			= kLoader.LoadInt(szKey, 0);

		wsprintf( szKey, "direction%d", i+1 );
		stInfo.iDirection	= kLoader.LoadInt(szKey, 0);

		m_vDefaultConstructInfo.push_back(stInfo);
	}
	
}

void GuildRoomsBlockManager::ConstructDefaultBlock(const DWORD dwGuildIndex)
{
	int iSize = m_vDefaultConstructInfo.size();

	for( int i = 0; i < iSize; i++ )
	{
		g_DBClient.OnDefaultConstructGuildBlock( dwGuildIndex, m_vDefaultConstructInfo[i].dwItemCode, m_vDefaultConstructInfo[i].iXZ, m_vDefaultConstructInfo[i].iY, m_vDefaultConstructInfo[i].iDirection);
	}
}

void GuildRoomsBlockManager::Destroy()
{
	GUILDBLOCKINFOS::iterator it = m_mGuildBlockInfos.begin();

	for(	; it != m_mGuildBlockInfos.end(); it++ )
	{
		GuildRoomInfos* pInfo	= it->second;
		if( pInfo )
			delete pInfo;
	}

	m_mGuildBlockInfos.clear();
	m_vDefaultConstructInfo.clear();
}

GuildRoomsBlockManager& GuildRoomsBlockManager::GetSingleton()
{
	return Singleton< GuildRoomsBlockManager >::GetSingleton();
}

GuildRoomInfos* GuildRoomsBlockManager::GetGuildRoomInfos(const DWORD dwGuildIndex)
{
	GUILDBLOCKINFOS::iterator it = m_mGuildBlockInfos.find(dwGuildIndex);

	if( it == m_mGuildBlockInfos.end() )
	{
		//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][guildroom]None exist guild room : [%d]", dwGuildIndex );
		return NULL;
	}

	return it->second;
}

BOOL GuildRoomsBlockManager::AddGuildItem(const DWORD dwGuildIndex,  const __int64 iBlockIndex, const int iItemCode, const int iXZIndex, const int iY, const int iDirection)
{
	GuildRoomInfos* pInfo	= GetGuildRoomInfos(dwGuildIndex);
	if( pInfo )
	{
		if( !pInfo->ConstructItem(iBlockIndex, iItemCode, iXZIndex, iY, iDirection) )
			return FALSE;

		return TRUE;
	}

	return FALSE;
}

BOOL GuildRoomsBlockManager::CreateGuildRoomInfos(const DWORD dwGuildIndex, const DWORD dwRoomIndex, CQueryResultData *query_data)
{
	GuildRoomInfos* pInfo	= GetGuildRoomInfos(dwGuildIndex);
	if( !pInfo )
	{
		pInfo	= new GuildRoomInfos;
		pInfo->SetGuildRoomIndex(dwRoomIndex);
		pInfo->SetGuildIndex(dwGuildIndex);
		pInfo->RenewalLeaveTime();

		m_mGuildBlockInfos.insert( std::make_pair(dwGuildIndex, pInfo) );
	}

	//DB에 길드 블럭 정보 호출.
	pInfo->DBToData(query_data);

	return TRUE;
}

BOOL GuildRoomsBlockManager::CreateGuildRoomInfos(const DWORD dwGuildIndex, const DWORD dwRoomIndex)
{
	////TEST 용
	GuildRoomInfos* pInfo	= GetGuildRoomInfos(dwGuildIndex);
	if( pInfo )
		return FALSE;

	pInfo	= new GuildRoomInfos;
	if( !pInfo )
		return FALSE;

	pInfo->SetGuildRoomIndex(dwRoomIndex);
	pInfo->SetGuildIndex(dwGuildIndex);
	pInfo->RenewalLeaveTime();

	m_mGuildBlockInfos.insert( std::make_pair(dwGuildIndex, pInfo) );
	
	return TRUE;
}

BOOL GuildRoomsBlockManager::DeleteGuildItem(const DWORD dwGuildIndex,  const __int64 iBlockIndex)
{
	GuildRoomInfos* pInfo	= GetGuildRoomInfos(dwGuildIndex);
	if( !pInfo )
		return FALSE;

	//작업 필요.
	pInfo->RetrieveItem(iBlockIndex);

	return TRUE;
}

void GuildRoomsBlockManager::DeleteGuildRoom(const DWORD dwGuildIndex)
{
	GUILDBLOCKINFOS::iterator it = m_mGuildBlockInfos.find(dwGuildIndex);
	DWORD dwRoomIndex		= 0;

	if( it != m_mGuildBlockInfos.end() )
	{
		GuildRoomInfos* pInfo	= it->second;
		
		if( pInfo )
			dwRoomIndex = pInfo->GetGuildRoomIndex();
		
		delete pInfo;

		m_mGuildBlockInfos.erase(it);
		//메인서버에 해당 길드 룸 삭제 요청.
	}

	SP2Packet kPacket(MSTPK_DELETE_GUILD_ROOM_INFO);
	PACKET_GUARD_VOID( kPacket.Write(dwGuildIndex) );
	PACKET_GUARD_VOID( kPacket.Write(dwRoomIndex) );
	g_MainServer.SendMessage(kPacket);

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[info][guildroom]Delete guild room : [%d] [%d]", dwGuildIndex, dwRoomIndex);
}

BOOL GuildRoomsBlockManager::IsDeleteGuildRoom(DWORD dwRoomIndex, DWORD& dwGuildIndex)
{
	GUILDBLOCKINFOS::iterator it	= m_mGuildBlockInfos.begin();

	while( it != m_mGuildBlockInfos.end() )
	{
		GuildRoomInfos* pInfo	= it->second;
		if( pInfo )
		{
			if( pInfo->GetGuildRoomIndex() == dwRoomIndex )
			{
				dwGuildIndex = pInfo->GetGuildIndex();
				return pInfo->IsDeleteTime();
			}
		}
		it++;
	}

	return TRUE;
}

BOOL GuildRoomsBlockManager::SendBlockInfos(const DWORD dwGuildIndex, User* pUser)
{
	if( !pUser )
		 return FALSE;

	GUILDBLOCKINFOS::iterator it	= m_mGuildBlockInfos.find(dwGuildIndex);
	if( it == m_mGuildBlockInfos.end() )
		return FALSE;

	GuildRoomInfos* pInfo = it->second;
	if( !pInfo )
		return FALSE;

	pInfo->SendBlockInfos(pUser);
	return TRUE;
}

BOOL GuildRoomsBlockManager::IsConstructedBlockAtGuildRoom(const DWORD dwGuildIndex,  const __int64 dwBlockIndex)
{
	GuildRoomInfos* pInfo = GetGuildRoomInfos(dwGuildIndex);
	if( !pInfo )
		return FALSE;

	return pInfo->IsConstructedBlock(dwBlockIndex);
}

BOOL GuildRoomsBlockManager::IsConstucting(const DWORD dwGuildIndex, const DWORD dwUserIndex)
{
	GuildRoomInfos* pInfo = GetGuildRoomInfos(dwGuildIndex);
	if( !pInfo )
		return TRUE;

	return pInfo->IsConstructing(dwUserIndex);
}

BOOL GuildRoomsBlockManager::SetConstructingState(const DWORD dwGuildIndex, BOOL bVal, DWORD dwUserIndex)
{
	GuildRoomInfos* pInfo = GetGuildRoomInfos(dwGuildIndex);
	if( !pInfo )
		return FALSE;

	pInfo->SetConstructingState(bVal, dwUserIndex);
	return TRUE;
}

void GuildRoomsBlockManager::UpdateGuildRoomLeaveTime(const DWORD dwGuildIndex)
{
	GuildRoomInfos* pInfo = GetGuildRoomInfos(dwGuildIndex);
	if( !pInfo )
		return;

	pInfo->RenewalLeaveTime();
}

void GuildRoomsBlockManager::DeleteAllRoomInfo()
{
	GUILDBLOCKINFOS::iterator it = m_mGuildBlockInfos.begin();

	while( it != m_mGuildBlockInfos.end() )
	{
		GuildRoomInfos* pInfo	= it->second;
		DWORD dwRoomIndex		= 0;
		DWORD dwGuildIndex		= 0;

		if( !pInfo )
		{
			it++;
			continue;
		}

		dwRoomIndex		= pInfo->GetGuildRoomIndex();
		dwGuildIndex	= pInfo->GetGuildIndex();
		
		delete pInfo;

		it = m_mGuildBlockInfos.erase(it);

		//메인서버에 해당 길드 룸 삭제 요청.
		SP2Packet kPacket(MSTPK_DELETE_GUILD_ROOM_INFO);
		PACKET_GUARD_VOID( kPacket.Write(dwGuildIndex) );
		PACKET_GUARD_VOID( kPacket.Write(dwRoomIndex) );
		g_MainServer.SendMessage(kPacket);
	}
}

void GuildRoomsBlockManager::FillAllGuildRoomInfo(SP2Packet &rkPacket)
{
	GUILDBLOCKINFOS::iterator it = m_mGuildBlockInfos.begin();

	PACKET_GUARD_VOID( rkPacket.Write((int)m_mGuildBlockInfos.size()) );
	while( it != m_mGuildBlockInfos.end() )
	{
		GuildRoomInfos* pInfo	= it->second;
		if( pInfo )
		{
			PACKET_GUARD_VOID( rkPacket.Write(pInfo->GetGuildIndex()) );
			PACKET_GUARD_VOID( rkPacket.Write(pInfo->GetGuildRoomIndex()) );
		}
		else
		{
			PACKET_GUARD_VOID( rkPacket.Write(0) );
			PACKET_GUARD_VOID( rkPacket.Write(0) );
		}

		it++;
	}
}

BOOL GuildRoomsBlockManager::IsConstructingUser(const DWORD dwGuildIndex, const DWORD dwUserIndex)
{
	GuildRoomInfos* pInfo = GetGuildRoomInfos(dwGuildIndex);
	if( !pInfo )
		return FALSE;

	if( pInfo->GetConstructingUserIndex() != dwUserIndex )
		return FALSE;

	return TRUE;
}

BOOL GuildRoomsBlockManager::IsRightPosition(const DWORD dwGuildIndex, const int iItemcode, const int iXZIndex, const int iY, const int iDirection)
{
	GuildRoomInfos* pInfo = GetGuildRoomInfos(dwGuildIndex);
	if( !pInfo )
		return FALSE;

	return pInfo->IsRightLocation(iItemcode, iXZIndex, iY, iDirection);
}

void GuildRoomsBlockManager::SendBlockVerifyInfo(const DWORD dwGuildIndex, User* pUser)
{
	GuildRoomInfos* pInfo = GetGuildRoomInfos(dwGuildIndex);
	if( !pInfo )
		return;

	pInfo->SendInstalledBlockInfo(pUser);
}

BOOL GuildRoomsBlockManager::IsExistFisheryBlock(const DWORD dwGuildIndex)
{
	GuildRoomInfos* pInfo = GetGuildRoomInfos(dwGuildIndex);
	if( !pInfo )
		return FALSE;

	return pInfo->IsExistItemType(GRT_FISHERY);
}