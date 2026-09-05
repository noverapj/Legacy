#include "stdafx.h"
#include "ioBlockPropertyManager.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "User.h"
#include "RoomNodeManager.h"
#include "GuildRoomInfos.h"

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
	ZeroMemory(&m_BlockInfoInMap, sizeof(m_BlockInfoInMap));
	ZeroMemory(&m_TileInfoInMap, sizeof(m_BlockInfoInMap));

	m_dwLastLeaveTime			= 0;
	m_dwConstructingUserIndex	= 0;
	m_bConstructing				= FALSE;
	m_dwRoomIndex				= 0;
	m_dwGuildIndex				= 0;
}

void GuildRoomInfos::Destroy()
{
}

void GuildRoomInfos::DBToData(CQueryResultData *query_data)
{
	__int64 dwItemSerial	= 0;
	DWORD dwItemCode		= 0;
	int iXZIndex			= 0;
	int iYCoordinate		= 0;
	BYTE byDirection		= 0;
	int	iScore				= 0;
	int iCount				= 0;

	while( query_data->IsExist() )
	{	
		PACKET_GUARD_BREAK( query_data->GetValue( dwItemSerial, sizeof(__int64) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( dwItemCode, sizeof(DWORD) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( iXZIndex, sizeof(int) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( iYCoordinate, sizeof(int) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( byDirection, sizeof(BYTE) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( iScore, sizeof(int) ) );

		if( !IsRightLocation(dwItemCode, iXZIndex, iYCoordinate, byDirection) )
		{
			//인벤토리로 이동.
			g_DBClient.OnRetrieveOrDeleteBlock(GetGuildIndex(), GetGuildRoomIndex(), dwItemSerial, GBT_EXCEPTION, "", 0, dwItemCode);
			continue;
		}

		
		if( !IsConstructedBlock(dwItemSerial) )
		{
			ConstructItem(dwItemSerial, dwItemCode, iXZIndex, iYCoordinate, byDirection);
		}
	}
}

void GuildRoomInfos::RenewalLeaveTime()
{
	m_dwLastLeaveTime	= GetTickCount();
}

BOOL GuildRoomInfos::IsDeleteTime()
{
	DWORD dwCurTime		= GetTickCount();
	DWORD dwLifeTime	= g_RoomNodeManager.GetGuildRoomLifeTime();

	if( dwCurTime - m_dwLastLeaveTime > dwLifeTime )		//ini 지정된 값을 읽어오도록 수정할거임.
		return TRUE;

	return FALSE;
}

BOOL GuildRoomInfos::IsConstructing(const DWORD dwUserIndex)
{
	if( m_bConstructing && dwUserIndex == m_dwConstructingUserIndex )
		return FALSE;

	return m_bConstructing;
}

DWORD GuildRoomInfos::GetGuildRoomIndex()
{
	return m_dwRoomIndex;
}

DWORD GuildRoomInfos::GetGuildIndex()
{
	return m_dwGuildIndex;
}

void GuildRoomInfos::SetGuildRoomIndex(DWORD dwVal)
{
	m_dwRoomIndex	= dwVal;
}

void GuildRoomInfos::SetGuildIndex(DWORD dwVal)
{
	m_dwGuildIndex	= dwVal;
}

void GuildRoomInfos::SendBlockInfos(User* pUser)
{
	if( !pUser )
		return;

	if( m_mBlockInfos.size() == 0 )
		return;

	int iSize		= 0;
	int iSendCount	= 0;
	int iCount		= 0;
	bool bEnd		= false;

	 if( m_mBlockInfos.size() % DB_BUILT_BLOCK_ITEM_SELECT_COUNT == 0 )
		 iSendCount	= m_mBlockInfos.size() / DB_BUILT_BLOCK_ITEM_SELECT_COUNT;
	 else
		 iSendCount = m_mBlockInfos.size() / DB_BUILT_BLOCK_ITEM_SELECT_COUNT + 1;

	BLOCKINFOS::iterator it = m_mBlockInfos.begin();;

	for( int i	= 0; i < iSendCount; i++ )
	{
		if( i == iSendCount-1 )
		{
			iSize	= m_mBlockInfos.size() - ( DB_BUILT_BLOCK_ITEM_SELECT_COUNT * (iSendCount - 1) );
			bEnd	= true;
		}
		else
			iSize	= DB_BUILT_BLOCK_ITEM_SELECT_COUNT;

		SP2Packet kPacket(STPK_GUILD_BLOCK_INFOS);
		PACKET_GUARD_VOID( kPacket.Write(bEnd) );
		PACKET_GUARD_VOID( kPacket.Write(iSize) );

		while( it !=  m_mBlockInfos.end() )
		{
			if( iCount >= DB_BUILT_BLOCK_ITEM_SELECT_COUNT )
			{
				pUser->SendMessage(kPacket);
				iCount	= 0;
				break;
			}

			ioBlockDBItem* pInfo	= it->second;
			if( pInfo )
			{
				PACKET_GUARD_VOID( kPacket.Write(pInfo->m_iIndex) );
				PACKET_GUARD_VOID( kPacket.Write(pInfo->m_iItemCode) );
				PACKET_GUARD_VOID( kPacket.Write(pInfo->m_iPivotXZIndex) );
				PACKET_GUARD_VOID( kPacket.Write(pInfo->m_iPivotY) );
				PACKET_GUARD_VOID( kPacket.Write((BYTE)pInfo->m_iDirection) );
			}

			iCount++;
			it++;

			if( it == m_mBlockInfos.end() )
				pUser->SendMessage(kPacket);
		}
	}
}

void GuildRoomInfos::SetConstructingState(BOOL bVal, DWORD dwUserIndex)
{
	m_bConstructing	= bVal;
	m_dwConstructingUserIndex	= dwUserIndex;
}

DWORD GuildRoomInfos::GetConstructingUserIndex()
{
	return m_dwConstructingUserIndex;
}

int	GuildRoomInfos::GetXZIndex(int iX, int iZ)
{
	return (iZ * GUILD_MAP_ARRAY) + iX;
}

BOOL GuildRoomInfos::GetXZIndexAndYCoordinate(const int iCurXZIndex, const int iCurY, const int iMoveX, const int iMoveY, const int iMoveZ, const int iDirection, int& iOutXZ, int& iOutY)
{
	// 중심 정보.
	int iOutX	= 0;
	int iOutZ	= 0;
	int iX		= iCurXZIndex % GUILD_MAP_ARRAY;
	int iZ		= iCurXZIndex / GUILD_MAP_ARRAY;

	GetBlockCoordRotateValue(iMoveX,  iMoveZ, iDirection, iOutX, iOutZ);

	iOutX += iX;
	iOutZ += iZ;
	iOutY = iCurY + iMoveY;

	if( iOutX < 0 || iOutX >= GUILD_MAP_ARRAY )
		return FALSE;

	if( iOutZ < 0 || iOutZ >= GUILD_MAP_ARRAY )
		return FALSE;

	if( iOutY < 0 || iOutY >= GUILD_MAP_Y_ARRAY )
		return FALSE;

	iOutXZ = GetXZIndex(iOutX, iOutZ);
	if( iOutXZ < 0 || iOutXZ >= GUILD_MAP_XZ_ARRAY )
		return FALSE;

	return TRUE;
}

BOOL GuildRoomInfos::IsRightLocation(const DWORD dwItemCode, const int iXZ, const int iY, const int iDirection)
{
	//ItemCode에 따라 블럭, 타일 체크 분기.
	int iItemType	= GetItemType(dwItemCode);
	if( GRT_NONE == iItemType )
		return FALSE;

	static BLOCKCOORDINATEINFO vInfo;
	vInfo.clear();
	
	// 해당 아이템의 설치 될 너비 정보 Get
	g_BlockPropertyMgr.GetCoordinateInfo(dwItemCode, vInfo);

	if( vInfo.size() == 0 )
		return FALSE;

	int iOutXZ	= 0;
	int iOutY	= 0;

	for( int i = 0; i < (int)vInfo.size(); i++ )
	{
		if( !GetXZIndexAndYCoordinate(iXZ, iY, vInfo[i].iX, vInfo[i].iY, vInfo[i].iZ, iDirection, iOutXZ, iOutY) )
			return FALSE;

		switch( iItemType )
		{
		case GRT_BLOCK:
		case GRT_FISHERY:
			{
				if( m_BlockInfoInMap[iOutXZ][iOutY] != 0 )
					return FALSE;
			}
			break;

		case GRT_TILE:
			{
				if( m_TileInfoInMap[iOutXZ][iOutY] != 0 )
					return FALSE;
			}
			break;
		}
		
	}
	// 
	return TRUE;
}

BOOL GuildRoomInfos::AddObjectArrayInMap(const __int64 dwItemIndex, const DWORD dwItemCode, const int iXZ, const int iY, const int iDirection)
{
	static BLOCKCOORDINATEINFO vInfo;
	vInfo.clear();
	
	ioBlockDBItem* pInfo = GetBlockItemInfo(dwItemIndex);
	if( !pInfo )
		return FALSE;

	GuildRoomItemType eType	= GetItemType(dwItemCode);
	if( GRT_NONE == eType )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][guildroom]Invalid guild room item code  : [%d] [%d]", GetGuildIndex(), dwItemCode );
		return FALSE;
	}

	// 설치 가능한 좌표 GET
	g_BlockPropertyMgr.GetCoordinateInfo(dwItemCode, vInfo);
	if( vInfo.size() == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][guildroom]Item coordinate info is empty  : [%d] [%d]", GetGuildIndex(), dwItemCode );
		return FALSE;
	}

	int iOutXZ	= 0;
	int iOutY	= 0;

	//유영재 TEST
	//printf("설치 \n");
	for( int i = 0; i < (int)vInfo.size(); i++ )
	{
		if( !GetXZIndexAndYCoordinate(iXZ, iY, vInfo[i].iX, vInfo[i].iY, vInfo[i].iZ, iDirection, iOutXZ, iOutY) )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][guildroom]Invalid item coordinate info  : [%d] [%d] [%d] [%d] [%d]", GetGuildIndex(), dwItemCode, iXZ, iY, iDirection );
			return FALSE;
		}
		//유영재 TEST
		//printf("(%I64d :%d)%d %d %d \n",dwItemIndex, dwItemCode, iOutXZ, iOutY, iDirection);

		switch( eType )
		{
		case GRT_BLOCK:
		case GRT_FISHERY:
			m_BlockInfoInMap[iOutXZ][iOutY]	= dwItemIndex;
			break;

		case GRT_TILE:
			m_TileInfoInMap[iOutXZ][iOutY]	= dwItemIndex;
			break;
		}

		ArrayIndexInfo stInfo;
		stInfo.iXZ	= iOutXZ;
		stInfo.iY	= iOutY;
		pInfo->vInstalledArrayIndex.push_back(stInfo);
	}

	return TRUE;
}

BOOL GuildRoomInfos::ConstructItem(const __int64 dwItemIndex, const DWORD dwItemCode, const int iXZ, const int iY, const int iDirection)
{
	if( AddBlockItem(dwItemIndex, dwItemCode, iXZ, iY, iDirection) )
	{
		if( !AddObjectArrayInMap(dwItemIndex, dwItemCode, iXZ, iY, iDirection) )
		{
			g_DBClient.OnRetrieveOrDeleteBlock(GetGuildIndex(), GetGuildRoomIndex(), dwItemIndex, GBT_EXCEPTION, "", 0, dwItemCode);
			return FALSE;
		}
		else
			return TRUE;
	}

	return FALSE;
}

void GuildRoomInfos::RetrieveItem(const __int64 dwItemIndex)
{
	ioBlockDBItem* pInfo = GetBlockItemInfo(dwItemIndex);
	if( !pInfo )
		return;

	GuildRoomItemType eType	= GetItemType(pInfo->m_iItemCode);
	if( GRT_NONE == eType )
		return;

	ARRAYINFO& stInfo = pInfo->vInstalledArrayIndex;
	for( int i = 0; i < (int)stInfo.size(); i++ )
	{
		int iXZIndex	= stInfo[i].iXZ;
		int iY			= stInfo[i].iY;

		if( iXZIndex < 0 || iXZIndex >= GUILD_MAP_XZ_ARRAY )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][guildroom]Object array XZIndex is Invalid : [%d]", iXZIndex);
			continue;
		}

		if( iY < 0 || iY >= GUILD_MAP_Y_ARRAY )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][guildroom]Object array YCoordinate is Invalid : [%d]", iY);
			continue;
		}

		if( GRT_BLOCK == eType || GRT_FISHERY == eType )
		{
			if( m_BlockInfoInMap[iXZIndex][iY] != dwItemIndex )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][guildroom]Object array is Invalid");
				continue;
			}

			m_BlockInfoInMap[iXZIndex][iY]	= 0;
		}
		else if( GRT_TILE == eType )
		{
			if( m_TileInfoInMap[iXZIndex][iY] != dwItemIndex )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][guildroom]Object array is Invalid");
				continue;
			}

			m_TileInfoInMap[iXZIndex][iY]	= 0;
		}
	}

	DeleteInstalledBlockInfo(dwItemIndex);
}

GuildRoomItemType GuildRoomInfos::GetItemType(const DWORD dwItemCode)
{
	int iType	= dwItemCode / GUILD_ROOM_ITEM_DELIMITER;
	if( !COMPARE(iType, GRT_BLOCK, GRT_END) )
		return GRT_NONE;

	return (GuildRoomItemType)iType;
}

void GuildRoomInfos::SendInstalledBlockInfo(User* pUser)
{
	if( !pUser )
		return;

	int iSize		= 0;
	int iSendCount	= 0;
	int iCount		= 0;
	bool bEnd		= false;

	 if( m_mBlockInfos.size() % DB_BUILT_BLOCK_ITEM_SELECT_COUNT == 0 )
		 iSendCount	= m_mBlockInfos.size() / DB_BUILT_BLOCK_ITEM_SELECT_COUNT;
	 else
		 iSendCount = m_mBlockInfos.size() / DB_BUILT_BLOCK_ITEM_SELECT_COUNT + 1;

	BLOCKINFOS::iterator it = m_mBlockInfos.begin();;

	for( int i	= 0; i < iSendCount; i++ )
	{
		if( i == iSendCount-1 )
		{
			iSize	= m_mBlockInfos.size() - ( DB_BUILT_BLOCK_ITEM_SELECT_COUNT * (iSendCount - 1) );
			bEnd	= true;
		}
		else
			iSize	= DB_BUILT_BLOCK_ITEM_SELECT_COUNT;

		SP2Packet kPacket(STPK_DEVELOPER_MACRO);
		PACKET_GUARD_VOID( kPacket.Write(DEVELOPER_HOUSING_TEST) );
		PACKET_GUARD_VOID( kPacket.Write(bEnd) );
		PACKET_GUARD_VOID( kPacket.Write(iSize) );

		while( it !=  m_mBlockInfos.end() )
		{
			if( iCount >= DB_BUILT_BLOCK_ITEM_SELECT_COUNT )
			{
				pUser->SendMessage(kPacket);
				iCount	= 0;
				break;
			}

			ioBlockDBItem* pInfo	= it->second;
			if( pInfo )
			{
				ARRAYINFO& stInfo	= pInfo->vInstalledArrayIndex;
				int iArraySize	= stInfo.size();

				PACKET_GUARD_VOID( kPacket.Write(pInfo->m_iIndex) );
				PACKET_GUARD_VOID( kPacket.Write(pInfo->m_iItemCode) );
				PACKET_GUARD_VOID( kPacket.Write(iArraySize) );

				for( int i = 0; i < iArraySize; i++ )
				{
					PACKET_GUARD_VOID( kPacket.Write(stInfo[i].iXZ) );
					PACKET_GUARD_VOID( kPacket.Write(stInfo[i].iY) );
				}
			}

			iCount++;
			it++;

			if( it == m_mBlockInfos.end() )
				pUser->SendMessage(kPacket);
		}
	}
}

BOOL GuildRoomInfos::IsExistItemType(GuildRoomItemType eType)
{
	BLOCKINFOS::iterator it	= m_mBlockInfos.begin();

	for(	; it != m_mBlockInfos.end(); it++ )
	{
		ioBlockDBItem* pInfo	= it->second;
		if( !pInfo )
			continue;

		if( GetItemType(pInfo->m_iItemCode) == eType )
			return TRUE;
	}

	return FALSE;
}