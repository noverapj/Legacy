#include "stdafx.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "User.h"
#include "RoomNodeManager.h"
#include "ioBlockPropertyManager.h"
#include "PersonalHomeInfo.h"

PersonalHomeInfo::PersonalHomeInfo()
{
	Init();
}

PersonalHomeInfo::~PersonalHomeInfo()
{
	Destroy();
}

void PersonalHomeInfo::Init()
{
	ZeroMemory(&m_BlockInfoInMap, sizeof(m_BlockInfoInMap));
	ZeroMemory(&m_TileInfoInMap, sizeof(m_BlockInfoInMap));

	m_dwRoomIndex				= 0;
	m_dwMasterIndex				= 0;
}

void PersonalHomeInfo::Destroy()
{
}

void PersonalHomeInfo::DBToData(CQueryResultData *query_data)
{
	__int64 dwItemSerial	= 0;
	DWORD dwItemCode		= 0;
	int iXZIndex			= 0;
	int iYCoordinate		= 0;
	BYTE byDirection		= 0;
	int	iScore				= 0;
	int iCount				= 0;

	DWORD dwThreadID		= 0;
	DWORD dwAgentID			= 0;

	PACKET_GUARD_VOID( query_data->GetValue( dwThreadID, sizeof(dwThreadID) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwAgentID, sizeof(dwAgentID) ) );

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
			g_DBClient.OnPersonalHQRetrieveBlock(dwAgentID, dwThreadID, GetRoomIndex(), dwItemSerial, GBT_EXCEPTION, "", GetMasterIndex(), dwItemCode);
			continue;
		}

		
		if( !IsConstructedBlock(dwItemSerial) )
		{
			ConstructItem(dwItemSerial, dwItemCode, iXZIndex, iYCoordinate, byDirection);
		}
	}
}

DWORD PersonalHomeInfo::GetRoomIndex()
{
	return m_dwRoomIndex;
}

DWORD PersonalHomeInfo::GetMasterIndex()
{
	return m_dwMasterIndex;
}

void PersonalHomeInfo::SetRoomIndex(DWORD dwVal)
{
	m_dwRoomIndex	= dwVal;
}

void PersonalHomeInfo::SetMasterIndex(DWORD dwVal)
{
	m_dwMasterIndex	= dwVal;
}

void PersonalHomeInfo::SendBlockInfos(User* pUser)
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

		SP2Packet kPacket(STPK_PERSONAL_HQ_BLOCKS_INFO);
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

int	PersonalHomeInfo::GetXZIndex(int iX, int iZ)
{
	return (iZ * HOME_MAP_ARRAY) + iX;
}

BOOL PersonalHomeInfo::GetXZIndexAndYCoordinate(const int iCurXZIndex, const int iCurY, const int iMoveX, const int iMoveY, const int iMoveZ, const int iDirection, int& iOutXZ, int& iOutY)
{
	// 중심 정보.
	int iOutX	= 0;
	int iOutZ	= 0;
	int iX		= iCurXZIndex % HOME_MAP_ARRAY;
	int iZ		= iCurXZIndex / HOME_MAP_ARRAY;

	GetBlockCoordRotateValue(iMoveX,  iMoveZ, iDirection, iOutX, iOutZ);

	iOutX += iX;
	iOutZ += iZ;
	iOutY = iCurY + iMoveY;

	if( iOutX < 0 || iOutX >= HOME_MAP_ARRAY )
		return FALSE;

	if( iOutZ < 0 || iOutZ >= HOME_MAP_ARRAY )
		return FALSE;

	if( iOutY < 0 || iOutY >= HOME_MAP_Y_ARRAY )
		return FALSE;

	iOutXZ = GetXZIndex(iOutX, iOutZ);
	if( iOutXZ < 0 || iOutXZ >= HOME_MAP_XZ_ARRAY )
		return FALSE;

	return TRUE;
}

BOOL PersonalHomeInfo::IsRightLocation(const DWORD dwItemCode, const int iXZ, const int iY, const int iDirection)
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
		case HMT_BLOCK:
		//case GRT_FISHERY:
			{
				if( m_BlockInfoInMap[iOutXZ][iOutY] != 0 )
					return FALSE;
			}
			break;

		case HMT_TILE:
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

BOOL PersonalHomeInfo::AddObjectArrayInMap(const __int64 dwItemIndex, const DWORD dwItemCode, const int iXZ, const int iY, const int iDirection)
{
	static BLOCKCOORDINATEINFO vInfo;
	vInfo.clear();
	
	ioBlockDBItem* pInfo = GetBlockItemInfo(dwItemIndex);
	if( !pInfo )
		return FALSE;

	HomeModeItemType eType	= GetItemType(dwItemCode);
	if( GRT_NONE == eType )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][homemode]Invalid room item code  : [%d] [%d]", GetMasterIndex(), dwItemCode );
		return FALSE;
	}

	// 설치 가능한 좌표 GET
	g_BlockPropertyMgr.GetCoordinateInfo(dwItemCode, vInfo);
	if( vInfo.size() == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][homemode]Item coordinate info is empty  : [%d] [%d]", GetMasterIndex(), dwItemCode );
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
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][homemode]Invalid item coordinate info  : [%d] [%d] [%d] [%d] [%d]", GetMasterIndex(), dwItemCode, iXZ, iY, iDirection );
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

BOOL PersonalHomeInfo::ConstructItem(const __int64 dwItemIndex, const DWORD dwItemCode, const int iXZ, const int iY, const int iDirection)
{
	if( AddBlockItem(dwItemIndex, dwItemCode, iXZ, iY, iDirection) )
	{
		if( !AddObjectArrayInMap(dwItemIndex, dwItemCode, iXZ, iY, iDirection) )
		{
			User *pUser = g_UserNodeManager.GetUserNode( GetMasterIndex() );
			if( !pUser )
				return FALSE;

			g_DBClient.OnPersonalHQRetrieveBlock(pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), GetRoomIndex(), dwItemIndex, GBT_EXCEPTION, "", GetMasterIndex(), dwItemCode);

			return FALSE;
		}
		else
			return TRUE;
	}

	return FALSE;
}

void PersonalHomeInfo::RetrieveItem(const __int64 dwItemIndex)
{
	ioBlockDBItem* pInfo = GetBlockItemInfo(dwItemIndex);
	if( !pInfo )
		return;

	HomeModeItemType eType	= GetItemType(pInfo->m_iItemCode);
	if( GRT_NONE == eType )
		return;

	ARRAYINFO& stInfo = pInfo->vInstalledArrayIndex;
	for( int i = 0; i < (int)stInfo.size(); i++ )
	{
		int iXZIndex	= stInfo[i].iXZ;
		int iY			= stInfo[i].iY;

		if( iXZIndex < 0 || iXZIndex >= HOME_MAP_XZ_ARRAY )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][homemode]Object array XZIndex is Invalid : [%d]", iXZIndex);
			continue;
		}

		if( iY < 0 || iY >= HOME_MAP_Y_ARRAY )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][homemode]Object array YCoordinate is Invalid : [%d]", iY);
			continue;
		}

		if( HMT_BLOCK == eType || GRT_FISHERY == eType )
		{
			if( m_BlockInfoInMap[iXZIndex][iY] != dwItemIndex )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][homemode]Object array is Invalid");
				continue;
			}

			m_BlockInfoInMap[iXZIndex][iY]	= 0;
		}
		else if( HMT_TILE == eType )
		{
			if( m_TileInfoInMap[iXZIndex][iY] != dwItemIndex )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][homemode]Object array is Invalid");
				continue;
			}

			m_TileInfoInMap[iXZIndex][iY]	= 0;
		}
	}

	DeleteInstalledBlockInfo(dwItemIndex);
}

HomeModeItemType PersonalHomeInfo::GetItemType(const DWORD dwItemCode)
{
	int iType	= dwItemCode / HOME_MODE_ITEM_DELIMITER;
	if( !COMPARE(iType, HMT_BLOCK, HMT_END) )
		return HMT_NONE;

	return (HomeModeItemType)iType;
}

void PersonalHomeInfo::SendInstalledBlockInfo(User* pUser)
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

BOOL PersonalHomeInfo::IsExistItemType(HomeModeItemType eType)
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