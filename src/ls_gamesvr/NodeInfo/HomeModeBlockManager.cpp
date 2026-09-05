#include "stdafx.h"
#include "PersonalHomeInfo.h"
#include "../DataBase/DBClient.h"
#include "User.h"
#include "HomeModeBlockManager.h"

template<> HomeModeBlockManager* Singleton< HomeModeBlockManager >::ms_Singleton = 0;

HomeModeBlockManager::HomeModeBlockManager()
{
	Init();
}

HomeModeBlockManager::~HomeModeBlockManager()
{
	Destroy();
}

void HomeModeBlockManager::Init()
{
	m_iLimitedInstallCount	= 0;

	m_mHomeBlockInfo.clear();
	m_vDefaultConstructInfo.clear();
}

void HomeModeBlockManager::LoadIni()
{
	m_vDefaultConstructInfo.clear();
	
	char szKey[MAX_PATH] = "";
	ioINILoader kLoader;

	kLoader.ReloadFile( "config/sp2_personal_room_block.ini" );

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

void HomeModeBlockManager::ConstructDefaultBlock(const DWORD dwUserIndex, const DWORD dwAgentID, const DWORD dwThreadID)
{
	int iSize = m_vDefaultConstructInfo.size();

	for( int i = 0; i < iSize; i++ )
	{
		g_DBClient.OnDefaultConstructPersonalBlock(dwAgentID, dwThreadID, dwUserIndex, m_vDefaultConstructInfo[i].dwItemCode, m_vDefaultConstructInfo[i].iXZ, m_vDefaultConstructInfo[i].iY, m_vDefaultConstructInfo[i].iDirection);
	}
}

void HomeModeBlockManager::Destroy()
{
	HOMEBLOCKINFO::iterator it = m_mHomeBlockInfo.begin();

	for(	; it != m_mHomeBlockInfo.end(); it++ )
	{
		PersonalHomeInfo* pInfo	= it->second;
		if( pInfo )
			delete pInfo;
	}

	m_mHomeBlockInfo.clear();
	m_vDefaultConstructInfo.clear();
}

HomeModeBlockManager& HomeModeBlockManager::GetSingleton()
{
	return Singleton< HomeModeBlockManager >::GetSingleton();
}

PersonalHomeInfo* HomeModeBlockManager::GetPersonalRoomInfo(const DWORD dwUserIndex)
{
	HOMEBLOCKINFO::iterator it = m_mHomeBlockInfo.find(dwUserIndex);

	if( it == m_mHomeBlockInfo.end() )
	{
		//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][guildroom]None exist guild room : [%d]", dwGuildIndex );
		return NULL;
	}

	return it->second;
}

BOOL HomeModeBlockManager::AddBlockItem(const DWORD dwUserIndex,  const __int64 iBlockIndex, const int iItemCode, const int iXZIndex, const int iY, const int iDirection)
{
	PersonalHomeInfo* pInfo	= GetPersonalRoomInfo(dwUserIndex);
	if( pInfo )
	{
		if( !pInfo->ConstructItem(iBlockIndex, iItemCode, iXZIndex, iY, iDirection) )
			return FALSE;

		return TRUE;
	}

	return FALSE;
}

BOOL HomeModeBlockManager::CreatePersonalRoomInfo(const DWORD dwRoomIndex, const DWORD dwUserIndex, CQueryResultData *query_data)
{
	PersonalHomeInfo* pInfo	= GetPersonalRoomInfo(dwUserIndex);
	if( !pInfo )
	{
		pInfo	= new PersonalHomeInfo;
		pInfo->SetRoomIndex(dwRoomIndex);
		pInfo->SetMasterIndex(dwUserIndex);

		m_mHomeBlockInfo.insert( std::make_pair(dwUserIndex, pInfo) );
	}

	//DB에 길드 블럭 정보 호출.
	pInfo->DBToData(query_data);

	return TRUE;
}

BOOL HomeModeBlockManager::DeleteBlockItem(const DWORD dwUserIndex,  const __int64 iBlockIndex)
{
	PersonalHomeInfo* pInfo	= GetPersonalRoomInfo(dwUserIndex);
	if( !pInfo )
		return FALSE;

	//작업 필요.
	pInfo->RetrieveItem(iBlockIndex);

	return TRUE;
}

void HomeModeBlockManager::DeletePersonalRoom(const DWORD dwUserIndex)
{
	HOMEBLOCKINFO::iterator it = m_mHomeBlockInfo.find(dwUserIndex);

	if( it != m_mHomeBlockInfo.end() )
	{
		PersonalHomeInfo* pInfo	= it->second;

		if( pInfo )
			delete pInfo;

		m_mHomeBlockInfo.erase(it);
	}
}

BOOL HomeModeBlockManager::SendBlockInfo(const DWORD dwMasterIndex, User* pUser)
{
	if( !pUser )
		 return FALSE;

	HOMEBLOCKINFO::iterator it	= m_mHomeBlockInfo.find(dwMasterIndex);
	if( it == m_mHomeBlockInfo.end() )
		return FALSE;

	PersonalHomeInfo* pInfo = it->second;
	if( !pInfo )
		return FALSE;

	pInfo->SendBlockInfos(pUser);
	return TRUE;
}

BOOL HomeModeBlockManager::IsConstructedBlockAtRoom(const DWORD dwUserIndex,  const __int64 dwBlockIndex)
{
	PersonalHomeInfo* pInfo = GetPersonalRoomInfo(dwUserIndex);
	if( !pInfo )
		return FALSE;

	return pInfo->IsConstructedBlock(dwBlockIndex);
}

void HomeModeBlockManager::DeleteAllRoomInfo()
{
	HOMEBLOCKINFO::iterator it = m_mHomeBlockInfo.begin();

	while( it != m_mHomeBlockInfo.end() )
	{
		PersonalHomeInfo* pInfo	= it->second;

		if( !pInfo )
		{
			it++;
			continue;
		}

		delete pInfo;

		it = m_mHomeBlockInfo.erase(it);
	}
}

BOOL HomeModeBlockManager::IsRightPosition(const DWORD dwUserIndex, const int iItemcode, const int iXZIndex, const int iY, const int iDirection)
{
	PersonalHomeInfo* pInfo = GetPersonalRoomInfo(dwUserIndex);
	if( !pInfo )
		return FALSE;

	return pInfo->IsRightLocation(iItemcode, iXZIndex, iY, iDirection);
}

BOOL HomeModeBlockManager::IsExistPersonalHQ(const DWORD dwUserIndex)
{
	PersonalHomeInfo* pInfo = GetPersonalRoomInfo(dwUserIndex);
	if( !pInfo )
		return FALSE;

	return TRUE;
}

DWORD HomeModeBlockManager::GetPersonalHQIndex(const DWORD dwUserIndex)
{
	PersonalHomeInfo* pInfo = GetPersonalRoomInfo(dwUserIndex);
	if( !pInfo )
		return 0;

	return pInfo->GetRoomIndex();
}
