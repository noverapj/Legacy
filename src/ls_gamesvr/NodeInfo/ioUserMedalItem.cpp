#include "stdafx.h"

//#include "../Window.h"
#include "../MainProcess.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"
#include "../Util/cSerialize.h"
#include "../EtcHelpFunc.h"

#include "User.h"
#include "Room.h"
#include "ioEtcItemManager.h"

#include ".\iousermedalitem.h"
#include <strsafe.h>
#include "ioMedalItemInfoManager.h"

ioUserMedalItem::ioUserMedalItem()
{
	Initialize( NULL );
}

ioUserMedalItem::~ioUserMedalItem()
{
	m_vMedalItemList.clear();
}

void ioUserMedalItem::Initialize( User *pUser )
{
	m_pUser = pUser;

	m_vMedalItemList.clear();
}

void ioUserMedalItem::InsertDBMedalItem( MEDALITEMDB &kMedalItemDB, int iLogType, int iLimitTime )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserMedalItem::InsertDBEtcItem() User NULL!!"); 
		return;
	}

	cSerialize v_FT;
	v_FT.Write( m_pUser->GetUserIndex() );
	for(int i = 0;i < MAX_SLOT;i++)
	{
		GetQueryArgument(kMedalItemDB.m_kMedalItem[i], v_FT );
	}
	
	g_DBClient.OnInsertMedalItemData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex(), v_FT, iLogType, iLimitTime );
}

bool ioUserMedalItem::DBtoNewIndex( DWORD dwIndex )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserMedalItem::DBtoNewIndex() User NULL!!"); 
		return false;
	}

	{	// 빈 인덱스 없으면 패스
		bool bEmptyIndex = false;
		vMEDALITEMDB::iterator iter, iEnd;
		iEnd = m_vMedalItemList.end();
		for(iter = m_vMedalItemList.begin();iter != iEnd;iter++)
		{
			MEDALITEMDB &kMedalItemDB = *iter;
			if( kMedalItemDB.m_dwIndex == NEW_INDEX )
			{
				bEmptyIndex = true;
				break;
			}
		}

		if( !bEmptyIndex )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserMedalItem::DBtoNewIndex() None Empty Index : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  ); 
			return false;
		}
	}

	{	// 이미 보유하고 있는 인덱스라면 다시 가져온다.
		vMEDALITEMDB::iterator iter, iEnd;
		iEnd = m_vMedalItemList.end();
		for(iter = m_vMedalItemList.begin();iter != iEnd;iter++)
		{
			MEDALITEMDB &kMedalItemDB = *iter;
			if( kMedalItemDB.m_dwIndex == dwIndex )
			{
				g_DBClient.OnSelectMedalItemIndex( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex(), 0, 0 );
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserMedalItem::DBtoNewIndex() Already Index : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  );
				return false;
			}
		}
	}

	{   // 빈 인덱스에 받은 인덱스 적용
		vMEDALITEMDB::iterator iter, iEnd;
		iEnd = m_vMedalItemList.end();
		for(iter = m_vMedalItemList.begin();iter != iEnd;iter++)
		{
			MEDALITEMDB &kMedalItemDB = *iter;
			if( kMedalItemDB.m_dwIndex == NEW_INDEX )
			{
				kMedalItemDB.m_dwIndex = dwIndex;
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ioUserMedalItem::DBtoNewIndex() Add : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  );
				return true;
			}
		}
	}

	return false;
}

void ioUserMedalItem::DBtoData( CQueryResultData *query_data )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserMedalItem::DBtoData() User NULL!!"); 
		return;
	}

	Initialize( m_pUser );

	LOOP_GUARD();
	while( query_data->IsExist() )
	{		
		MEDALITEMDB kMedalItemDB;
		PACKET_GUARD_BREAK( query_data->GetValue( kMedalItemDB.m_dwIndex, sizeof(int) ) );
		for(int i = 0;i < MAX_SLOT;i++)
		{
			DBTIMESTAMP dts;

			PACKET_GUARD_BREAK( query_data->GetValue( kMedalItemDB.m_kMedalItem[i].m_iItemType, sizeof(int) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kMedalItemDB.m_kMedalItem[i].m_iEquipClass, sizeof(int) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kMedalItemDB.m_kMedalItem[i].m_iPeriodType, sizeof(int) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( (char*)&dts, sizeof(DBTIMESTAMP) ) );

			CTime kLimitTime(Help::GetSafeValueForCTimeConstructor(dts.year,dts.month,dts.day, dts.hour,dts.minute,dts.second));
			kMedalItemDB.m_kMedalItem[i].SetDate( kLimitTime.GetYear(), kLimitTime.GetMonth(), kLimitTime.GetDay(), kLimitTime.GetHour(), kLimitTime.GetMinute() );
		} 
		m_vMedalItemList.push_back( kMedalItemDB );
	}	
	LOOP_GUARD_CLEAR();

	if( m_vMedalItemList.empty() ) return;

	g_CriticalError.CheckMedalTableCount( m_pUser->GetPublicID(), m_vMedalItemList.size() );

	int iItemSize = 0;
	vMEDALITEMDB::iterator iter, iEnd;
	iEnd = m_vMedalItemList.end();
	for(iter = m_vMedalItemList.begin();iter != iEnd;iter++)
	{
		MEDALITEMDB &kMedalItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kMedalItemDB.m_kMedalItem[i].m_iItemType > 0 )
			{
				iItemSize++;

				// 착용한 용병이 없다면 해제
				int iEquipClass = kMedalItemDB.m_kMedalItem[i].m_iEquipClass;
				if( iEquipClass > 0 )
				{
					if( !m_pUser->IsCharClassType( iEquipClass ) )
					{
						int iReleaseCnt = ReleaseEquipMedal( iEquipClass );
						LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Release Equip Medal ( None ClassType ) :%d)%s : %d:%d", __FUNCTION__, m_pUser->GetUserIndex(), m_pUser->GetPublicID().c_str(), iEquipClass, iReleaseCnt );
					}
				}
				//
			}
		}
	}


	// 장착갯수가 틀리면 해제
	int iCharSize = m_pUser->GetCharCount();
	for(int i = 0;i < iCharSize; i++)
	{
		ioCharacter *rkChar = m_pUser->GetCharacter( i );    
		if( rkChar == NULL )
			continue;

		const CHARACTER &rCharinfo = rkChar->GetCharInfo();

		int iExMedalSlotNum = 0;
		ioUserExpandMedalSlot *pExMedalSlot = m_pUser->GetUserExpandMedalSlot();
		if( pExMedalSlot )
		{
			iExMedalSlotNum = pExMedalSlot->GetExpandMedalSlotNum( rCharinfo.m_class_type );
		}

		int iMedalSlotNum = g_MedalItemMgr.GetSlotNum( m_pUser->GetClassLevelByType( rCharinfo.m_class_type,true ) );
		if( iMedalSlotNum + iExMedalSlotNum < GetEquipNum( rCharinfo.m_class_type ) )
		{
			int iReleaseCnt = ReleaseEquipMedal( rCharinfo.m_class_type );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Release Equip Medal ( Medal Equip Num Error ) :%d)%s : %d:%d", __FUNCTION__, m_pUser->GetUserIndex(), m_pUser->GetPublicID().c_str(), rCharinfo.m_class_type, iReleaseCnt );
		}
	}
	//

	SP2Packet kPacket( STPK_USER_MEDALITEM_DATA );
	PACKET_GUARD_VOID( kPacket.Write(iItemSize) );
	{
		iEnd = m_vMedalItemList.end();
		for(iter = m_vMedalItemList.begin();iter != iEnd;iter++)
		{
			MEDALITEMDB &kMedalItemDB = *iter;
			for(int i = 0;i < MAX_SLOT;i++)
			{
				if( kMedalItemDB.m_kMedalItem[i].m_iItemType == 0 )
					continue;

				PACKET_GUARD_VOID( kPacket.Write(kMedalItemDB.m_kMedalItem[i].m_iItemType) );
				PACKET_GUARD_VOID( kPacket.Write(kMedalItemDB.m_kMedalItem[i].m_iEquipClass) );
				PACKET_GUARD_VOID( kPacket.Write(kMedalItemDB.m_kMedalItem[i].m_iPeriodType) );
				PACKET_GUARD_VOID( kPacket.Write(kMedalItemDB.m_kMedalItem[i].m_iLimitDate) );
				PACKET_GUARD_VOID( kPacket.Write(kMedalItemDB.m_kMedalItem[i].m_iLimitTime) );
			}
		}
	}	
	m_pUser->SendMessage( kPacket );
}

void ioUserMedalItem::SaveData()
{
	if( m_vMedalItemList.empty() )
		return;

	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserMedalItem::SaveData() User NULL!!"); 
		return;
	}


	const int iTestCount = 500;
	int iLoopCnt = 0;
	DWORD dwLastIndex = 0;

	cSerialize v_FT;

	vMEDALITEMDB::iterator iter, iEnd;
	iEnd = m_vMedalItemList.end();
	for(iter = m_vMedalItemList.begin();iter != iEnd;iter++)
	{
		if( iLoopCnt++ > iTestCount )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SaveMedalItem Loop Error - (%s - %d)", m_pUser->GetPublicID().c_str(), dwLastIndex );
			break;
		}

		MEDALITEMDB &kMedalItemDB = *iter;
		if( kMedalItemDB.m_bChange )
		{
			v_FT.Reset();
			v_FT.Write( kMedalItemDB.m_dwIndex );
			for(int i = 0;i < MAX_SLOT;i++)
			{
				GetQueryArgument( kMedalItemDB.m_kMedalItem[i], v_FT );
			}

			if( kMedalItemDB.m_dwIndex == NEW_INDEX )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SaveMedalItem(%s:%d) None Index", m_pUser->GetPublicID().c_str(), kMedalItemDB.m_dwIndex );
				//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s", szQueryArgument );
			}
			else
			{
				g_DBClient.OnUpdateMedalItemData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetPublicID(), kMedalItemDB.m_dwIndex, v_FT );
				kMedalItemDB.m_bChange = false;
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "SaveMedalItem(%s:%d)", m_pUser->GetPublicID().c_str(), kMedalItemDB.m_dwIndex );
				dwLastIndex = kMedalItemDB.m_dwIndex;
			}
		}		
	}
}

bool ioUserMedalItem::AddMedalItem( IN const MEDALITEMSLOT &rkNewSlot, IN int iLogType, IN int iLimitTime, OUT DWORD &rdwIndex, OUT int &riArray )
{
	if( rkNewSlot.m_iItemType <= 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserMedalItem::AddMedalItem Code is Zero" );
		return false;
	}

	MEDALITEMSLOT kTempSlot;
	//if( GetMedalItem( rkNewSlot.m_iItemType, kTempSlot ) ) // 중복 검사
		//return false;

	vMEDALITEMDB::iterator iter, iEnd;
	iEnd = m_vMedalItemList.end();

	// update blank
	for(iter = m_vMedalItemList.begin();iter != iEnd;iter++)
	{
		MEDALITEMDB &kMedalItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kMedalItemDB.m_kMedalItem[i].m_iItemType == 0 )
			{
				kMedalItemDB.m_kMedalItem[i] = rkNewSlot;
				kMedalItemDB.m_bChange    = true;

				rdwIndex = kMedalItemDB.m_dwIndex;
				riArray  = i;
				return true;
			}
		}
	}

	// New insert
	MEDALITEMDB kMedalItemDB;
	kMedalItemDB.m_dwIndex        = NEW_INDEX;
	kMedalItemDB.m_kMedalItem[0]  = rkNewSlot;
	m_vMedalItemList.push_back( kMedalItemDB );

	InsertDBMedalItem( kMedalItemDB, iLogType, iLimitTime );

	if( m_pUser )
	{
		g_CriticalError.CheckMedalTableCount( m_pUser->GetPublicID(), m_vMedalItemList.size() );
	}
	return true;
}

void ioUserMedalItem::FillMoveData( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID( rkPacket.Write((int)m_vMedalItemList.size()) );

	vMEDALITEMDB::iterator iter, iEnd;
	iEnd = m_vMedalItemList.end();
	for(iter = m_vMedalItemList.begin();iter != iEnd;iter++)
	{
		MEDALITEMDB &kMedalItemDB = *iter;

		PACKET_GUARD_VOID( rkPacket.Write(kMedalItemDB.m_dwIndex) );
		PACKET_GUARD_VOID( rkPacket.Write(kMedalItemDB.m_bChange) );

		for(int i = 0;i < MAX_SLOT;i++)
		{
			PACKET_GUARD_VOID( rkPacket.Write(kMedalItemDB.m_kMedalItem[i].m_iItemType) );
			PACKET_GUARD_VOID( rkPacket.Write(kMedalItemDB.m_kMedalItem[i].m_iEquipClass) );
			PACKET_GUARD_VOID( rkPacket.Write(kMedalItemDB.m_kMedalItem[i].m_iPeriodType) );
			PACKET_GUARD_VOID( rkPacket.Write(kMedalItemDB.m_kMedalItem[i].m_iLimitDate) );
			PACKET_GUARD_VOID( rkPacket.Write(kMedalItemDB.m_kMedalItem[i].m_iLimitTime) );
		}
	}
}

void ioUserMedalItem::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode )
{
	int iSize = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iSize) );

	for(int i = 0;i < iSize;i++)
	{
		MEDALITEMDB kMedalItemDB;
		rkPacket >> kMedalItemDB.m_dwIndex >> kMedalItemDB.m_bChange;
		for(int j = 0;j < MAX_SLOT;j++)
		{
			PACKET_GUARD_VOID( rkPacket.Read(kMedalItemDB.m_kMedalItem[j].m_iItemType) );
			PACKET_GUARD_VOID( rkPacket.Read(kMedalItemDB.m_kMedalItem[j].m_iEquipClass) );
			PACKET_GUARD_VOID( rkPacket.Read(kMedalItemDB.m_kMedalItem[j].m_iPeriodType) );
			PACKET_GUARD_VOID( rkPacket.Read(kMedalItemDB.m_kMedalItem[j].m_iLimitDate) );
			PACKET_GUARD_VOID( rkPacket.Read(kMedalItemDB.m_kMedalItem[j].m_iLimitTime) );
		}

		if( kMedalItemDB.m_dwIndex == NEW_INDEX && !bDummyNode )
		{
			g_DBClient.OnSelectMedalItemIndex( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex(), 0, 0 );
		}

		m_vMedalItemList.push_back( kMedalItemDB );
	}

	if( m_pUser )
	{
		g_CriticalError.CheckMedalTableCount( m_pUser->GetPublicID(), m_vMedalItemList.size() );
	}
}

bool ioUserMedalItem::GetMedalItem( IN int iItemType, OUT MEDALITEMSLOT &rkMedalItem )
{
	// 초기화
	rkMedalItem.Init();

	for(vMEDALITEMDB::iterator iter = m_vMedalItemList.begin(); iter != m_vMedalItemList.end(); ++iter)
	{
		MEDALITEMDB &kMedalItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( kMedalItemDB.m_kMedalItem[i].m_iItemType == iItemType )
			{
				rkMedalItem = kMedalItemDB.m_kMedalItem[i];
				return true;
			}
		}
	}
	return false;
}

bool ioUserMedalItem::GetRowMedalItem( IN DWORD dwIndex, OUT MEDALITEMSLOT kMedalItem[MAX_SLOT] )
{
	bool bReturn = false;
	for(vMEDALITEMDB::iterator iter = m_vMedalItemList.begin(); iter != m_vMedalItemList.end(); ++iter)
	{
		MEDALITEMDB &kMedalItemDB = *iter;
		if( kMedalItemDB.m_dwIndex != dwIndex )
			continue;

		for (int i = 0; i < MAX_SLOT ; i++)
		{
			kMedalItem[i] = kMedalItemDB.m_kMedalItem[i];
			bReturn = true;
		}

		if( bReturn )
			break;
	}

	return bReturn;
}

void ioUserMedalItem::SetMedalItem( const MEDALITEMSLOT &rkMedalItem )
{
	for(vMEDALITEMDB::iterator iter = m_vMedalItemList.begin(); iter != m_vMedalItemList.end(); ++iter)
	{
		MEDALITEMDB &kMedalItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( kMedalItemDB.m_kMedalItem[i].m_iItemType == rkMedalItem.m_iItemType )
			{
				kMedalItemDB.m_kMedalItem[i] = rkMedalItem;
				kMedalItemDB.m_bChange = true;
				return;
			}
		}
	}
}

bool ioUserMedalItem::DeleteMedalItem( int iItemType )
{
	for(vMEDALITEMDB::iterator iter = m_vMedalItemList.begin(); iter != m_vMedalItemList.end(); ++iter)
	{
		MEDALITEMDB &kMedalItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( kMedalItemDB.GetEquipMedalType(i) == iItemType )
			{
				if( m_pUser )
				{
					char szItemIndex[MAX_PATH]="";
					StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", kMedalItemDB.m_dwIndex, i+1 ); // db field는 1부터 이므로 +1
					g_LogDBClient.OnInsertMedalItem( m_pUser, kMedalItemDB.m_kMedalItem[i].m_iItemType, kMedalItemDB.m_kMedalItem[i].m_iPeriodType, szItemIndex, LogDBClient::MT_DEL );
				}

				kMedalItemDB.m_kMedalItem[i].Init();
				kMedalItemDB.m_bChange = true;
				return true;
			}
		}
	}

	return false;
}

bool ioUserMedalItem::DeleteMedalItem( const int iItemType, const int iListArrayIndex )
{
	if( iListArrayIndex >= (int)m_vMedalItemList.size() )
		return false;

	MEDALITEMDB &kMedalItemDB = m_vMedalItemList[iListArrayIndex];

	for( int i=0; i<MAX_SLOT; i++ )
	{
		if( (kMedalItemDB.GetEquipMedalType(i) == iItemType) && (kMedalItemDB.GetEquipClass(i) == 0) )
		{
			if( m_pUser )
			{
				char szItemIndex[MAX_PATH]="";
				StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", kMedalItemDB.m_dwIndex, i+1 ); // db field는 1부터 이므로 +1
				g_LogDBClient.OnInsertMedalItem( m_pUser, kMedalItemDB.m_kMedalItem[i].m_iItemType, kMedalItemDB.m_kMedalItem[i].m_iPeriodType, szItemIndex, LogDBClient::MT_DEL );
			}

			kMedalItemDB.m_kMedalItem[i].Init();
			kMedalItemDB.m_bChange = true;
			return true;
		}
	}

	return false;
}

void ioUserMedalItem::DeleteMedalItemPassedDate( OUT IntVec &rvTypeVec )
{
	CTime kCurTime = CTime::GetCurrentTime();

	for(vMEDALITEMDB::iterator iter = m_vMedalItemList.begin(); iter != m_vMedalItemList.end(); ++iter)
	{
		MEDALITEMDB &kMedalItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( kMedalItemDB.m_kMedalItem[i].m_iItemType == 0 )
				continue;

			if( (*iter).m_kMedalItem[i].m_iPeriodType == PT_MORTMAIN )		// 무제한은 무시
				continue;

			CTime kLimitTime( Help::GetSafeValueForCTimeConstructor( kMedalItemDB.m_kMedalItem[i].GetYear(), kMedalItemDB.m_kMedalItem[i].GetMonth(), kMedalItemDB.m_kMedalItem[i].GetDay(), kMedalItemDB.m_kMedalItem[i].GetHour(), kMedalItemDB.m_kMedalItem[i].GetMinute(), 0 ) );
			CTimeSpan kRemainTime = kLimitTime - kCurTime;

			if( kRemainTime.GetTotalMinutes() > 0 )
				continue;

			if( m_pUser )
			{
				char szItemIndex[MAX_PATH]="";
				StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", kMedalItemDB.m_dwIndex, i+1 ); // db field는 1부터 이므로 +1
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "DeleteMedalItemPassedDate / ID:%d(%s) / %d / %d:%d / INDEX:%d-%d", m_pUser->GetUserIndex(), m_pUser->GetPublicID().c_str(), kMedalItemDB.m_kMedalItem[i].m_iItemType, kMedalItemDB.m_kMedalItem[i].m_iLimitDate, kMedalItemDB.m_kMedalItem[i].m_iLimitTime, kMedalItemDB.m_dwIndex, i+1 ); 
			}

			rvTypeVec.push_back( kMedalItemDB.m_kMedalItem[i].m_iItemType );

			(*iter).m_kMedalItem[i].Init();
			(*iter).m_bChange = true;
		}
	}
}

bool ioUserMedalItem::GetMedalItemIndex( IN int iItemType, OUT DWORD &rdwIndex, OUT int &iFieldCnt )
{
	// 초기화
	rdwIndex  = 0;
	iFieldCnt = 0;

	for(vMEDALITEMDB::iterator iter = m_vMedalItemList.begin(); iter != m_vMedalItemList.end(); ++iter)
	{
		MEDALITEMDB &kMedalItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( kMedalItemDB.m_kMedalItem[i].m_iItemType == iItemType )
			{
				rdwIndex = kMedalItemDB.m_dwIndex;
				iFieldCnt= i+1; // +1 : 필드는 1부터 시작 하므로 
				return true;
			}
		}
	}
	return false;
}

void ioUserMedalItem::GetQueryArgument( IN MEDALITEMSLOT &rkMedalItem, cSerialize& v_FT )
{
	SYSTEMTIME sysTime;
	rkMedalItem.GetDate( sysTime );

	v_FT.Write(rkMedalItem.m_iItemType);
	v_FT.Write(rkMedalItem.m_iEquipClass);
	v_FT.Write(rkMedalItem.m_iPeriodType);
	v_FT.Write((uint8*)(&sysTime), sizeof(SYSTEMTIME), TRUE);
}

int ioUserMedalItem::GetEquipNum( int iClassType )
{
	int iEquipNum = 0;
	for(vMEDALITEMDB::iterator iter = m_vMedalItemList.begin(); iter != m_vMedalItemList.end(); ++iter)
	{
		MEDALITEMDB &kMedalItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( kMedalItemDB.m_kMedalItem[i].m_iEquipClass == iClassType )
			{
				iEquipNum++;
			}
		}
	}

	return iEquipNum;
}

void ioUserMedalItem::FillEquipClass( IN int iClassType, IN int iMaxSlotNum, OUT SP2Packet &rkPacket )
{
	int iEquipNum = 0;
	for(vMEDALITEMDB::iterator iter = m_vMedalItemList.begin(); iter != m_vMedalItemList.end(); ++iter)
	{
		MEDALITEMDB &kMedalItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( kMedalItemDB.m_kMedalItem[i].m_iEquipClass == iClassType )
			{
				PACKET_GUARD_VOID( rkPacket.Write(kMedalItemDB.m_kMedalItem[i].m_iItemType) );
				iEquipNum++;
				if( iEquipNum >= iMaxSlotNum )
					return;
			}
		}
	}

	// 빈값 셋팅
	for (int i = iEquipNum; i < iMaxSlotNum ; i++)
	{
		PACKET_GUARD_VOID( rkPacket.Write(0) );
	}
}

bool ioUserMedalItem::GetMedalItemTypeVec( OUT IntVec &rvItemTypeVec, IN int iClassType )
{
	rvItemTypeVec.clear();
	bool bExist = false;
	for(vMEDALITEMDB::iterator iter = m_vMedalItemList.begin(); iter != m_vMedalItemList.end(); ++iter)
	{
		MEDALITEMDB &kMedalItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( kMedalItemDB.m_kMedalItem[i].m_iEquipClass == iClassType )
			{
				rvItemTypeVec.push_back( kMedalItemDB.m_kMedalItem[i].m_iItemType );
				bExist = true;
			}
		}
	}

	return bExist;
}

int ioUserMedalItem::ReleaseEquipMedal( int iClassType )
{
	int iReleaseCnt = 0;
	for(vMEDALITEMDB::iterator iter = m_vMedalItemList.begin(); iter != m_vMedalItemList.end(); ++iter)
	{
		MEDALITEMDB &kMedalItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( kMedalItemDB.m_kMedalItem[i].m_iEquipClass == iClassType )
			{
				kMedalItemDB.m_kMedalItem[i].m_iEquipClass = 0;
				kMedalItemDB.m_bChange = true;
				iReleaseCnt++;
			}
		}
	}

	return iReleaseCnt;
}

bool ioUserMedalItem::IsEquipMedalByChar( const int iClassType, const int iItemType )
{
	for( int i=0; i<(int)m_vMedalItemList.size(); i++ )
	{
		MEDALITEMDB &kMedalItemDB = m_vMedalItemList[i];

		for( int j=0; j<MAX_SLOT; j++)
		{
			if( (kMedalItemDB.GetEquipClass(j) ==  iClassType) && ( kMedalItemDB.GetEquipMedalType(j) == iItemType  ) )
				return true;
		}

	}

	return false;
}

bool ioUserMedalItem::GetMedalItem( const int iClassType, const int iItemType, const  bool bEquip, MEDALITEMSLOT &rkMedalItem )
{
	if( bEquip )
	{
		if( !GetNotEquipMedalItem( iItemType, rkMedalItem ) )
			return false;
	}
	else
	{
		if( !GetEquipMedalItem( iClassType, iItemType, rkMedalItem ) )
			return false;
	}

	return true;
}

bool ioUserMedalItem::GetNotEquipMedalItem( const int iItemType, MEDALITEMSLOT &rkMedalItem )
{
	// 초기화
	rkMedalItem.Init();

	for(int i=0; i<(int)m_vMedalItemList.size(); i++)
	{
		MEDALITEMDB &kMedalItemDB = m_vMedalItemList[i];
		for (int j = 0; j < MAX_SLOT ; j++)
		{
			if( (kMedalItemDB.GetEquipMedalType(j) == iItemType) && (kMedalItemDB.GetEquipClass(j) == 0) ) // equipClass가 0이면 미장착.
			{
				rkMedalItem = kMedalItemDB.m_kMedalItem[j];
				return true;
			}
		}
	}

	return false;
}

bool ioUserMedalItem::GetNotEquipMedalItem( const int iItemType, MEDALITEMSLOT &rkMedalItem, int &iListArrayIndex )
{
	// 초기화
	rkMedalItem.Init();

	for(int i=0; i<(int)m_vMedalItemList.size(); i++)
	{
		MEDALITEMDB &kMedalItemDB = m_vMedalItemList[i];
		for (int j = 0; j < MAX_SLOT ; j++)
		{
			if( (kMedalItemDB.GetEquipMedalType(j) == iItemType) && (kMedalItemDB.GetEquipClass(j) == 0) ) // equipClass가 0이면 미장착.
			{
				rkMedalItem = kMedalItemDB.m_kMedalItem[j];
				iListArrayIndex = i;
				return true;
			}
		}
	}

	return false;
}

bool ioUserMedalItem::GetEquipMedalItem( const int iClassType, const int iItemType, MEDALITEMSLOT &rkMedalItem )
{
	rkMedalItem.Init();

	for(vMEDALITEMDB::iterator iter = m_vMedalItemList.begin(); iter != m_vMedalItemList.end(); ++iter)
	{
		MEDALITEMDB &kMedalItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( (kMedalItemDB.GetEquipMedalType(i) == iItemType) && (kMedalItemDB.GetEquipClass(i) == iClassType) ) // equipClass가 0이면 미장착.
			{
				rkMedalItem = kMedalItemDB.m_kMedalItem[i];
				return true;
			}
		}
	}

	return false;
}

void ioUserMedalItem::SetMedalItemDueToTakeOff( const int iClassType, const int iItemType, MEDALITEMSLOT &rkMedalItem )
{
	for(vMEDALITEMDB::iterator iter = m_vMedalItemList.begin(); iter != m_vMedalItemList.end(); ++iter)
	{
		MEDALITEMDB &kMedalItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( (kMedalItemDB.GetEquipMedalType(i) == rkMedalItem.m_iItemType) && (kMedalItemDB.GetEquipClass(i) == iClassType) )
			{
				kMedalItemDB.m_kMedalItem[i] = rkMedalItem;
				kMedalItemDB.m_bChange = true;
				return;
			}
		}
	}
}

void ioUserMedalItem::SetMedalItemDueToEquip( const int iItemType, MEDALITEMSLOT &rkMedalItem )
{
	for(vMEDALITEMDB::iterator iter = m_vMedalItemList.begin(); iter != m_vMedalItemList.end(); ++iter)
	{
		MEDALITEMDB &kMedalItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( (kMedalItemDB.GetEquipMedalType(i) == rkMedalItem.m_iItemType) && (kMedalItemDB.GetEquipClass(i) == 0) )
			{
				kMedalItemDB.m_kMedalItem[i] = rkMedalItem;
				kMedalItemDB.m_bChange = true;
				return;
			}
		}
	}
}

int ioUserMedalItem::GetNotEquipMedalItemCount( const int iItemType )
{
	int iCount = 0;

	for(int i=0; i<(int)m_vMedalItemList.size(); i++)
	{
		MEDALITEMDB &kMedalItemDB = m_vMedalItemList[i];
		for (int j = 0; j < MAX_SLOT ; j++)
		{
			if( (kMedalItemDB.GetEquipMedalType(j) == iItemType) && (kMedalItemDB.GetEquipClass(j) == 0) ) // equipClass가 0이면 미장착.
			{
				iCount++;
			}
		}
	}

	return iCount;
}