#include "stdafx.h"

#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"
#include "../QueryData/QueryResultData.h"

#include "../Util/cSerialize.h"
#include "../EtcHelpFunc.h"

#include "User.h"
#include "Room.h"
#include "ioEtcItemManager.h"

#include "ioCustomMedal.h"
#include "CustomMedal.h"
#include "ioMedalItemInfoManager.h"

ioCustomMedal::ioCustomMedal()
{
	//Initialize( NULL );
}

ioCustomMedal::~ioCustomMedal()
{
	Destroy();
}

CustomMedal* ioCustomMedal::CreateCustomMedal()
{
	CustomMedal* pInfo	= new CustomMedal;
	return pInfo;
}

void ioCustomMedal::Initialize( User* pUser )
{
	m_vCustomMedalItemList.clear();
	m_pUser = pUser;
}

void ioCustomMedal::Destroy()
{
	int iSize	= m_vCustomMedalItemList.size();
	for( int i = 0; i < iSize; i++ )
	{
		CustomMedal* pTemp = NULL;
		pTemp = m_vCustomMedalItemList[i];
		
		if( pTemp) 
			delete pTemp;
	}
	m_vCustomMedalItemList.clear();

	m_pUser = NULL;
}

void ioCustomMedal::DBtoData( CQueryResultData *query_data )
{
	if( !m_pUser )
		return;

	while( query_data->IsExist() )
	{
		CustomMedal* pInfo = CreateCustomMedal();
		if( pInfo == NULL )
		{
			return;
		}

		DBTIMESTAMP dts;

		PACKET_GUARD_BREAK( query_data->GetValue( pInfo->m_iItemIndex, sizeof(int) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( pInfo->m_iItemCode, sizeof(int) ) );

		for( int i = 0; i< CustomMedal::MEDAL_STAT; i++)
		{
			PACKET_GUARD_BREAK( query_data->GetValue( pInfo->m_iGrowth[i], sizeof(int) ) );
		}
		
		PACKET_GUARD_BREAK( query_data->GetValue( pInfo->m_iEquipClass, sizeof(int) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( pInfo->m_iPeriodType, sizeof(int) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( (char*)&dts, sizeof(DBTIMESTAMP) ) );

		CTime kLimitTime(Help::GetSafeValueForCTimeConstructor(dts.year,dts.month,dts.day, dts.hour,dts.minute,dts.second));
		pInfo->SetDate( kLimitTime.GetYear(), kLimitTime.GetMonth(), kLimitTime.GetDay(), kLimitTime.GetHour(), kLimitTime.GetMinute() );

		if( pInfo->m_iItemCode <= 0 )
			continue;

		m_vCustomMedalItemList.push_back( pInfo );
	}

	int iSize	= m_vCustomMedalItemList.size();

	SP2Packet kPacket(STPK_USER_CUSTOM_MEDALITEM_DATA);
	PACKET_GUARD_VOID( kPacket.Write(iSize) );
	
	for( int i = 0; i < iSize; i++ )
	{
		CustomMedal* pInfo = m_vCustomMedalItemList[i];
		if( pInfo )
		{
			PACKET_GUARD_VOID( kPacket.Write(pInfo->m_iItemCode) );
			PACKET_GUARD_VOID( kPacket.Write(pInfo->m_iItemIndex) );
			PACKET_GUARD_VOID( kPacket.Write(pInfo->m_iEquipClass) );
			PACKET_GUARD_VOID( kPacket.Write(pInfo->m_iPeriodType) );
			PACKET_GUARD_VOID( kPacket.Write(pInfo->m_iLimitDate) );
			PACKET_GUARD_VOID( kPacket.Write(pInfo->m_iLimitTime) );
			SendCustomMedalData( pInfo, kPacket );
			/*LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][customMedal]ioCustomMedal::DBtoData userIndex:%d, CustomCount:%d, Code:%d, Index:%d, class:%d, periodType:%d, limitDate:%d, limitTime:%d", 
				m_pUser->GetUserIndex(), iSize, pInfo->m_iItemCode, pInfo->m_iItemIndex, pInfo->m_iEquipClass, pInfo->m_iPeriodType, pInfo->m_iLimitDate, pInfo->m_iLimitTime );*/
		}
	}
	m_pUser->SendMessage(kPacket);
}

void ioCustomMedal::SaveData()
{

}

void ioCustomMedal::FillMoveData( SP2Packet &rkPacket )
{
	int iSize = 0;
	iSize = m_vCustomMedalItemList.size();

	PACKET_GUARD_VOID(rkPacket.Write(iSize));
	
	for(int i = 0;i < iSize; i++)
	{
		CustomMedal* pTemp = m_vCustomMedalItemList[i];
		pTemp->FillMoveData(rkPacket);
	}
}

void ioCustomMedal::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode )
{
	int iSize = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iSize) );

	for( int i = 0; i< iSize; i++ )
	{
		CustomMedal* pTemp = CreateCustomMedal();
		if( pTemp )
		{
			pTemp->ApplyMoveData(rkPacket);
			m_vCustomMedalItemList.push_back(pTemp);
		}
	}
}

//
int ioCustomMedal::GetRandomPoint( int iItemType, CustomMedal* pInfo )
{
	if ( pInfo )
	{
		// 초기화
		for( int i = 0; i< CustomMedal::MEDAL_STAT; i++ )
		{
			pInfo->m_iGrowth[i] = 0;
		}


		int iCustomItem = 0, iMaxPoint = 0, iMinPoint = 0, iStatCount = 0;
		int iMedalType = 1;

		iCustomItem = g_MedalItemMgr.GetSubMedalType( iItemType );

		iMinPoint = g_MedalItemMgr.GetMinStatPoint( iItemType );
		iMaxPoint = g_MedalItemMgr.GetMaxStatPoint( iItemType );
		iStatCount = g_MedalItemMgr.GetSelectStat( iItemType );
		int iPoint = g_MedalItemMgr.GetTotalPoint( iItemType );
		

		int iRandom = 0;
		int iTotalRandom = 0; //총 랜덤 합산값
		int iRandomDiv = 0, iTotalSub = 0;
		iRandomDiv = iStatCount / 2;

		int iStatValue[10] = {0};
		for( int i = 0; i<iStatCount; i++ )
		{
			iStatValue[i] = m_iPointRandom.Random( iMinPoint, iMaxPoint );
			iTotalRandom += iStatValue[i];
		}

		// 랜덤합산 값 == 포인트 값
		if( iTotalRandom == iPoint )
		{
			int iCount = 0;
			for( int i = 0; i< CustomMedal::MEDAL_STAT; i++ )
			{
				if( pInfo->m_bGrowth[i] == 1 )
				{
					pInfo->m_iGrowth[i] = iStatValue[iCount];
					iCount ++;
				}

			}
			return iRandom;
		}
		// 랜덤합산 값 > 포인트값
		if( iTotalRandom > iPoint )
		{
			iTotalSub = iTotalRandom - iPoint;
			int iSub = 0;
			bool bTurnEnd = false;

			// 가장 작은 값보다 큰 수에서 1만큼씩 줄여서 포인트값 맞춤
			while( iSub < iTotalSub )
			{
				// 무한 루프 방지
				if( bTurnEnd && iSub == 0)
					break;

				for( int i = 0; i<iStatCount; i++ )
				{
					if( iSub == iTotalSub )
						break;

					if ( iStatValue[i] > iMinPoint )
					{
						iStatValue[i] = iStatValue[i] - 1;
						++iSub;
					}
				}
				bTurnEnd = true;
			}
		}
		if( iTotalRandom < iPoint )
		{
			iTotalSub =  iPoint - iTotalRandom;
			int iSub = 0;
			bool bTurnEnd = false;

			// 가장 큰 값보다 작은 수에서 1만큼씩 줄여서 포인트값 맞춤
			while( iSub < iTotalSub )
			{
				// 무한 루프 방지
				if( bTurnEnd && iSub == 0)
					break;

				for( int i = 0; i<iStatCount; i++ )
				{
					if( iSub == iTotalSub )
						break;

					if ( iStatValue[i] < iMaxPoint )
					{
						iStatValue[i] = iStatValue[i] + 1;
						++iSub;
					}
				}
				bTurnEnd = true;
			}
		}

		int iCount = 0;
		for( int i = 0; i< CustomMedal::MEDAL_STAT; i++ )
		{
			if( pInfo->m_bGrowth[i] == 1 )
			{
				pInfo->m_iGrowth[i] = iStatValue[iCount];
				iCount ++;
			}

		}
		return iRandom;
	}
	return -1;
}

CustomMedal* ioCustomMedal::SetCustomMedal( int iPresentIndex, int iSlotIndex, int iItemCode, int iItemValue, SP2Packet &rkRecvPacket )
{
	// 기존에 있는 데이터 라면
	int iSize = m_vCustomMedalItemList.size();
	for(int i = 0;i < iSize;i++)
	{
		CustomMedal* pTemp = m_vCustomMedalItemList[i];
		if( pTemp->m_iPresentIndex == iPresentIndex && pTemp->m_iPresentSlotIndex== iSlotIndex && pTemp->m_iItemCode == iItemCode )
		{
			for( int i = 0; i< CustomMedal::MEDAL_STAT; i++)
			{
				rkRecvPacket >> pTemp->m_bGrowth[i];			// 재선택된 스탯 저장
			}

			/*LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][customMedal]ioCustomMedal::SetCustomMedal exist userIndex: %d, ItemCode: %d, presentIndex:%d, slotInde : %d, %d:%d:%d:%d:%d:%d:%d:%d", 
				m_pUser->GetUserIndex(), iItemCode, pTemp->m_iPresentIndex, pTemp->m_iPresentSlotIndex
			,pTemp->m_bGrowth[0]
			,pTemp->m_bGrowth[1]
			,pTemp->m_bGrowth[2]
			,pTemp->m_bGrowth[3]
			,pTemp->m_bGrowth[4]
			,pTemp->m_bGrowth[5]
			,pTemp->m_bGrowth[6]
			,pTemp->m_bGrowth[7]

			);*/

			return pTemp;


		}
	}
	
	// 신규 생성
	CustomMedal* pInfo = CreateCustomMedal();
	if( pInfo == NULL )
	{
		return NULL;
	}
	else
	{
		pInfo->m_iPresentIndex = iPresentIndex;
		pInfo->m_iPresentSlotIndex = iSlotIndex;
		pInfo->m_iItemCode = iItemCode;
		for( int i = 0; i< CustomMedal::MEDAL_STAT; i++)
		{
			rkRecvPacket >> pInfo->m_bGrowth[i];
		}

		if( iItemValue >= 0 )
		{
			CTime kLimiteTime = CTime::GetCurrentTime();
			CTimeSpan kAddTime( 0, iItemValue, 0, 0 );
			kLimiteTime += kAddTime;
			pInfo->SetDate( kLimiteTime.GetYear(), kLimiteTime.GetMonth(), kLimiteTime.GetDay(), kLimiteTime.GetHour(), kLimiteTime.GetMinute() );

			if( iItemValue == 0 )
			{
				pInfo->m_iPeriodType = ioUserMedalItem::PT_MORTMAIN;
			}
			else
			{
				pInfo->m_iPeriodType = ioUserMedalItem::PT_TIME;
			}
		}


	}
	/*LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][customMedal]ioCustomMedal::SetCustomMedal new userIndex: %d, ItemCode: %d, presentIndex:%d, slotIndex:%d, period:%d, %d:%d:%d:%d:%d:%d:%d:%d", 
		m_pUser->GetUserIndex(), iItemCode, pInfo->m_iPresentIndex, pInfo->m_iPresentSlotIndex, pInfo->m_iPeriodType
		,pInfo->m_bGrowth[0]
		,pInfo->m_bGrowth[1]
		,pInfo->m_bGrowth[2]
		,pInfo->m_bGrowth[3]
		,pInfo->m_bGrowth[4]
		,pInfo->m_bGrowth[5]
		,pInfo->m_bGrowth[6]
		,pInfo->m_bGrowth[7]

	);*/
	m_vCustomMedalItemList.push_back( pInfo );
	return pInfo;
}

void ioCustomMedal::SendCustomMedalData( CustomMedal* pInfo, SP2Packet& rkPacket )
{
	for( int i = 0; i< CustomMedal::MEDAL_STAT; i++)
	{
		PACKET_GUARD_VOID(rkPacket.Write(pInfo->m_iGrowth[i] ));
	}
}

CustomMedal* ioCustomMedal::GetCustomMedalPresentIndex( int iIndex, int iSlotIndex, int iItemCode )
{
	CustomMedal* pTemp = NULL;
	int iSize = m_vCustomMedalItemList.size();
	for(int i = 0;i < iSize;i++)
	{
		pTemp  = m_vCustomMedalItemList[i];
		if( pTemp->m_iPresentIndex == iIndex && pTemp->m_iPresentSlotIndex == iSlotIndex && pTemp->m_iItemCode == iItemCode )
		{		
			return pTemp;
		}
	}
	return NULL;
}

void ioCustomMedal::GetCustomMedalEquipClass( int iClass, IntOfStatVec &kCustomMedal )
{
	CustomMedal* pTemp = NULL;
	int iSize = m_vCustomMedalItemList.size();
	for(int i = 0;i < iSize;i++)
	{
		pTemp  = m_vCustomMedalItemList[i];
		if( pTemp->m_iEquipClass == iClass )
		{		
			IntOfStat stInfo;
			stInfo.index	= pTemp->m_iItemIndex;
			stInfo.itemCode = pTemp->m_iItemCode;
			stInfo.stat1	= pTemp->m_iGrowth[0];
			stInfo.stat2	= pTemp->m_iGrowth[1];
			stInfo.stat3	= pTemp->m_iGrowth[2];
			stInfo.stat4	= pTemp->m_iGrowth[3];
			stInfo.stat5	= pTemp->m_iGrowth[4];
			stInfo.stat6	= pTemp->m_iGrowth[5];
			stInfo.stat7	= pTemp->m_iGrowth[6];
			stInfo.stat8	= pTemp->m_iGrowth[7];

			kCustomMedal.push_back( stInfo );
		}
	}
}

void ioCustomMedal::FillEquipAllCustomMedal( int iClass, OUT SP2Packet &rkPacket )
{
	CustomMedal* pTemp = NULL;
	int iSize = m_vCustomMedalItemList.size();
	for(int i = 0; i < iSize;i++)
	{
		pTemp  = m_vCustomMedalItemList[i];
		if( pTemp->m_iEquipClass == iClass && pTemp->m_iItemIndex != 0 && pTemp->m_iItemCode != 0 )
		{		
			rkPacket << pTemp->m_iItemCode;
			rkPacket << pTemp->m_iItemIndex;
			for( int i = 0; i<CustomMedal::MEDAL_STAT; i++)
			{
				rkPacket << pTemp->m_iGrowth[i];
			}
		}
	}
}

void ioCustomMedal::FillEquipCustomMedal( int iClass, int iMaxSlotNum, OUT SP2Packet &rkPacket )
{
	int iEquipNum = 0;
	CustomMedal* pTemp = NULL;
	int iCustomMedalSize = 0;

	iCustomMedalSize = m_vCustomMedalItemList.size(); 
	int iSize = iMaxSlotNum;

	for(int i = 0; i < iCustomMedalSize; i++)
	{
		pTemp  = m_vCustomMedalItemList[i];

		if( pTemp == NULL )
		{
			break;
		}
		if( pTemp->m_iEquipClass == iClass && pTemp->m_iItemIndex != 0 && pTemp->m_iItemCode != 0 )
		{		
			PACKET_GUARD_VOID( rkPacket.Write(pTemp->m_iItemCode) );
			PACKET_GUARD_VOID( rkPacket.Write(pTemp->m_iItemIndex) );
			for( int j = 0; j<CustomMedal::MEDAL_STAT; j++)
			{
				PACKET_GUARD_VOID( rkPacket.Write(pTemp->m_iGrowth[j]) );
			}
			iEquipNum++;
			if( iEquipNum >= iMaxSlotNum )
				return;
		}
		pTemp = NULL;
		
	}

	// 빈값 셋팅
	for (int i = iEquipNum; i < iMaxSlotNum ; i++)
	{
		PACKET_GUARD_VOID( rkPacket.Write(0) );
		PACKET_GUARD_VOID( rkPacket.Write(0) );
	}
}

int ioCustomMedal::GetEquipCustomMedalNum( int iClass )
{
	int iCount = 0;
	CustomMedal* pTemp = NULL;
	int iSize = m_vCustomMedalItemList.size();
	for(int i = 0; i < iSize;i++)
	{
		pTemp  = m_vCustomMedalItemList[i];
		if( pTemp->m_iEquipClass == iClass && pTemp->m_iItemIndex != 0 && pTemp->m_iItemCode != 0 )
		{		
			iCount++;	
		}
	}
	return iCount;
}

CustomMedal* ioCustomMedal::GetCustomMedal( int iIndex, int iItemCode )
{
	CustomMedal* pTemp = NULL;
	int iSize = m_vCustomMedalItemList.size();
	for(int i = 0;i < iSize;i++)
	{
		pTemp  = m_vCustomMedalItemList[i];
		if( pTemp->m_iItemIndex == iIndex && pTemp->m_iItemCode == iItemCode )
		{		
			return pTemp;
		}
	}
	return NULL;
}

void ioCustomMedal::DeleteCustomMedal( int iItemIndex, int iItemCode )
{
	CustomMedal* pTemp = NULL;

	CustomMedalIter it	= m_vCustomMedalItemList.begin();
	while( it != m_vCustomMedalItemList.end() )
	{
		pTemp  = *it;
		if( pTemp )
		{
			if( pTemp->m_iItemIndex == iItemIndex && pTemp->m_iItemCode == iItemCode )
			{	
				SYSTEMTIME sysTime;
				pTemp->GetDate( sysTime );

				g_LogDBClient.OnInsertCustomMedalInfo(m_pUser, pTemp->m_iItemIndex, pTemp->m_iItemCode 
				, pTemp->m_iGrowth[0]
				, pTemp->m_iGrowth[1]
				, pTemp->m_iGrowth[2]
				, pTemp->m_iGrowth[3]
				, pTemp->m_iGrowth[4]
				, pTemp->m_iGrowth[5]
				, pTemp->m_iGrowth[6]
				, pTemp->m_iGrowth[7]
				, pTemp->m_iPeriodType
				, sysTime
				, LogDBClient::MT_DEL  );

				delete pTemp;
				it = m_vCustomMedalItemList.erase(it);

				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][customMedal]ioCustomMedal::DeleteCustomMedal ItemIndex: %d, ItemCode: %d", 
					iItemIndex, iItemCode );

			}
			else
			{
				it++;
			}
		}
		else
		{
			it++;
		}
	}
}

int ioCustomMedal::ReleaseEquipcustomMedal( int iClassType )
{
	bool bEquip = false;
	int iReleaseCnt = 0;
	CustomMedal* pTemp = NULL;
	int iSize = m_vCustomMedalItemList.size();
	for(int i = 0;i < iSize;i++)
	{
		pTemp  = m_vCustomMedalItemList[i];
		if( pTemp->m_iEquipClass == iClassType )
		{
			pTemp->m_iEquipClass = 0;
			iReleaseCnt++;

			g_DBClient.OnUpdateCustomMedalState( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), pTemp->m_iItemCode, pTemp->m_iItemIndex, pTemp->m_iEquipClass, bEquip );
		}
	}

	return iReleaseCnt;
}

void ioCustomMedal::SendPresentCustomMedalToClient( CQueryResultData *query_data )
{
	if( !query_data )
		return;

	int iItemCode = 0, iPresentIndex = 0, iPresentSlotIndex = 0, iItemIndex = 0;
	PACKET_GUARD_VOID( query_data->GetValue( iItemCode , sizeof(iItemCode ) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iPresentIndex, sizeof(iPresentIndex) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iPresentSlotIndex, sizeof(iPresentSlotIndex) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iItemIndex, sizeof(iItemIndex) ) );

	CustomMedal* pInfo = NULL;
	pInfo = GetCustomMedalPresentIndex( iPresentIndex, iPresentSlotIndex, iItemCode );
	
	
	if( pInfo ) 
	{
		pInfo->m_iItemIndex = iItemIndex;
		SP2Packet kPacket(STPK_CUSTOM_MEDAL_PRESENT);
		
		PACKET_GUARD_VOID( kPacket.Write(pInfo->m_iItemCode) );
		PACKET_GUARD_VOID( kPacket.Write(pInfo->m_iItemIndex) );
		PACKET_GUARD_VOID( kPacket.Write(pInfo->m_iEquipClass) );
		PACKET_GUARD_VOID( kPacket.Write(pInfo->m_iPeriodType) );
		PACKET_GUARD_VOID( kPacket.Write(pInfo->m_iLimitDate) );
		PACKET_GUARD_VOID( kPacket.Write(pInfo->m_iLimitTime) );
		for( int i = 0; i< CustomMedal::MEDAL_STAT; i++)
		{
			PACKET_GUARD_VOID( kPacket.Write(pInfo->m_iGrowth[i]) );
		}
		m_pUser->SendMessage(kPacket);		

		pInfo->m_iPresentIndex = 0;
		pInfo->m_iPresentSlotIndex = 0;
	}
	
}

void ioCustomMedal::OnSellCustomMedal( SP2Packet &rkPacket )
{
	int iItemCode = 0, iItemIndex = 0;
	
	PACKET_GUARD_VOID( rkPacket.Read(iItemCode) );
	PACKET_GUARD_VOID( rkPacket.Read(iItemIndex) );
	
	__int64 iPeso = (__int64)g_MedalItemMgr.GetSellPeso( iItemCode );
	m_pUser->AddMoney( iPeso );


	g_DBClient.OnDeleteCustomMedal( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), iItemIndex );

	DeleteCustomMedal( iItemIndex, iItemCode);

	SP2Packet kPacket( STPK_CUSTOM_MEDALITEM_SELL );
	PACKET_GUARD_VOID( kPacket.Write((int)MEDALITEM_SELL_OK) );
	PACKET_GUARD_VOID( kPacket.Write(iItemCode) );
	PACKET_GUARD_VOID( kPacket.Write(iItemIndex) );
	PACKET_GUARD_VOID( kPacket.Write(iPeso) );
	PACKET_GUARD_VOID( kPacket.Write(m_pUser->GetMoney()) );
	m_pUser->SendMessage( kPacket );

	
	/*LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][customMedal]ioCustomMedal::OnSellCustomMedal UserIndex:%d, ItemIndex: %d, ItemCode: %d", 
		m_pUser->GetUserIndex(), iItemIndex, iItemCode );*/
}

CustomMedal* ioCustomMedal::OnChangeCustomMedalState( SP2Packet &rkPacket, int& iRoomClass, bool& bState )
{
	int iEqupClass = 0, iItemCode = 0, iItemIndex = 0;
	bool bEquip = false;
	
	PACKET_GUARD_bool( rkPacket.Read(iEqupClass) );		//용병번호
	PACKET_GUARD_bool( rkPacket.Read(bEquip) );			//장착 여부 : ture : 장착, false : 해제
	PACKET_GUARD_bool( rkPacket.Read(iItemCode) );
	PACKET_GUARD_bool( rkPacket.Read(iItemIndex) );
	
	CustomMedal* pInfo = NULL;
	pInfo = GetCustomMedal( iItemIndex, iItemCode );
	iRoomClass = iEqupClass;

	if( pInfo )
	{
		// 해제 인 경우 이전에 착용하고 있던 정보 저장
		if( bEquip == true )
		{
			pInfo->m_iEquipClass = iEqupClass;		
			bState = true;
		}
		else
		{
			pInfo->m_iEquipClass = 0;
			bState = false; 
		}

		g_DBClient.OnUpdateCustomMedalState( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), iItemCode, iItemIndex, pInfo->m_iEquipClass, bEquip );

		

		SP2Packet kPacket( STPK_CUSTOM_MEDALITEM_CHANGE );
		PACKET_GUARD_bool( kPacket.Write((int)MEDALITEM_CHANGE_OK) );
		PACKET_GUARD_bool( kPacket.Write(m_pUser->GetPublicID()) );
		PACKET_GUARD_bool( kPacket.Write(iItemCode) );
		PACKET_GUARD_bool( kPacket.Write(iItemIndex) );
		PACKET_GUARD_bool( kPacket.Write(iEqupClass) );
		PACKET_GUARD_bool( kPacket.Write(bEquip) );			//용병번호 
		for( int i = 0; i< CustomMedal::MEDAL_STAT; i++)
		{
			PACKET_GUARD_bool( kPacket.Write(pInfo->m_iGrowth[i]) );
		}
		m_pUser->SendMessage( kPacket );

		/*LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][customMedal]ioCustomMedal::OnChangeCustomMedalState UserIndex:%d, ItemCode: %d, ItemIdex:%d, EquipClass:%d, bEquip : %d", 
			m_pUser->GetUserIndex(), iItemCode, iItemIndex, iEqupClass, bEquip );*/
		
	}
	else
	{
		SP2Packet kPacket( STPK_CUSTOM_MEDALITEM_CHANGE );
		PACKET_GUARD_bool( kPacket.Write((int)MEDALITEM_CHANGE_FAIL) );
		PACKET_GUARD_bool( kPacket.Write(m_pUser->GetPublicID()) );
		PACKET_GUARD_bool( kPacket.Write(iItemCode) );
		PACKET_GUARD_bool( kPacket.Write(iItemIndex) );
		m_pUser->SendMessage( kPacket );

	}
	return pInfo;
	
}

bool ioCustomMedal::IsCustomMedal( int iItemType )
{
	int iMedalType = 0;
	iMedalType = g_MedalItemMgr.GetSubMedalType( iItemType );

	if( iMedalType == CUSTOM_TYPE )
	{
		return true;
	}
	return false;
}
//
//
//CustomMedal* ioCustomMedal::GetCustomMedal( int iIndex, int iSlotIndex, int iItemCode )
//{
//	CustomMedal* pTemp = NULL;
//	int iSize = m_vCustomMedalItemList.size();
//	for(int i = 0;i < iSize;i++)
//	{
//		pTemp  = m_vCustomMedalItemList[i];
//		if( pTemp->m_iIndex == iIndex && pTemp->m_iSlotIndex == iSlotIndex && pTemp->m_iItemType == iItemCode )
//		{		
//			return pTemp;
//		}
//	}
//}

