#include "stdafx.h"
#include "ioUserSpirit.h"
#include "ioEtcItem.h"
#include "ioSpiritManager.h"
#include "ioAlchemicMgr.h"
#include "ioUserEtcItem.h"
#include "ioUserPresent.h"
#include "Room.h"
#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"
#include "../QueryData/QueryResultData.h"
#include "ioMyLevelMgr.h"
#include "../MainServerNode/MainServerNode.h"


ioUserSpirit::ioUserSpirit()
{
	Initialize( NULL );
}

ioUserSpirit::~ioUserSpirit()
{
	Initialize( NULL );
}

void ioUserSpirit::Initialize( User *pUser )
{
	m_pUser = pUser;

	m_vSpiritInfo.clear();

	m_SpiritRandom.SetRandomSeed( timeGetTime() );
}

bool ioUserSpirit::DBtoNewIndex( DWORD dwIndex )
{
	return true;
}

void ioUserSpirit::DBtoData( CQueryResultData *query_data )
{
	if( !m_pUser )
		return;

	int iDBSelectCount = query_data->GetResultCount();

	int inLastTableIndex = 0;
	while( query_data->IsExist() )
	{
		UserSpiritInfo info;
		info.Init();

		PACKET_GUARD_BREAK( query_data->GetValue( inLastTableIndex, sizeof(inLastTableIndex) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( info.item_code, sizeof(info.item_code) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( info.quantity, sizeof(info.quantity) ) );

		if( info.item_code <= 0 )
			continue;

		m_vSpiritInfo.push_back( info );
	}

	if( iDBSelectCount < DB_SPIRIT_SELECT_COUNT )
	{
		SendUserSpiritData();
	}
	else
	{
		g_DBClient.OnSelectUserSpiritData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), inLastTableIndex, m_pUser->GetUserIndex(), DB_SPIRIT_SELECT_COUNT );
	}
}

void ioUserSpirit::SaveData()
{
	if( !m_pUser )
		return;

	int iCnt = m_vSpiritInfo.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( m_vSpiritInfo[i].state == SPIRIT_STATE_NEW )
		{
			g_DBClient.OnInsertUserSpiritData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), m_vSpiritInfo[i].item_code, m_vSpiritInfo[i].quantity );
			m_vSpiritInfo[i].state = SPIRIT_STATE_NONE;
		}
		else if( m_vSpiritInfo[i].state == SPIRIT_STATE_CHANGED )
		{
			g_DBClient.OnUpdateUserSpiritData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), m_vSpiritInfo[i].item_code, m_vSpiritInfo[i].quantity );
			m_vSpiritInfo[i].state = SPIRIT_STATE_NONE;
		}
	}
}

void ioUserSpirit::FillMoveData( SP2Packet &rkPacket )
{
	int iCnt = m_vSpiritInfo.size();
	PACKET_GUARD_VOID( rkPacket.Write(iCnt) );
	for( int i=0; i<iCnt; ++i )
	{
		PACKET_GUARD_VOID( rkPacket.Write(m_vSpiritInfo[i].item_code) );
		PACKET_GUARD_VOID( rkPacket.Write(m_vSpiritInfo[i].quantity) );
		PACKET_GUARD_VOID( rkPacket.Write(m_vSpiritInfo[i].state) );
	}
}

void ioUserSpirit::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode /* = false */ )
{
	int iCnt = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iCnt) );
	for( int i=0; i<iCnt; ++i )
	{
		UserSpiritInfo info;
		info.Init();
		PACKET_GUARD_VOID( rkPacket.Read(info.item_code) );
		PACKET_GUARD_VOID( rkPacket.Read(info.quantity) );
		PACKET_GUARD_VOID( rkPacket.Write(info.state) );
		m_vSpiritInfo.push_back( info );
	}
}

void ioUserSpirit::SendUserSpiritData()
{
	if( !m_pUser )
		return;

	SP2Packet kPacket(STPK_SPIRIT_DATA);
	
	int iCnt = m_vSpiritInfo.size();
	PACKET_GUARD_VOID( kPacket.Write(iCnt) );
	for( int i=0; i<iCnt; ++i )
	{
		PACKET_GUARD_VOID( kPacket.Write(m_vSpiritInfo[i].item_code) );
		PACKET_GUARD_VOID( kPacket.Write(m_vSpiritInfo[i].quantity) );
	}
	m_pUser->SendMessage( kPacket );
}

int ioUserSpirit::GetSpiritQuantity( int iItemCode )
{
	for each( UserSpiritInfo info in m_vSpiritInfo )
	{
		if( info.item_code == iItemCode )
			return info.quantity;
	}

	return 0;
}

void ioUserSpirit::IncreaseSpirit( int iItemCode, int iQuantity )
{
	if( iQuantity <= 0 )
		return;

	auto find_spirit = [iItemCode]( UserSpiritInfo info )->bool { return info.item_code == iItemCode; };
	vUserSpiritInfo::iterator iter = std::find_if( m_vSpiritInfo.begin(), m_vSpiritInfo.end(), find_spirit );
	if( iter != m_vSpiritInfo.end() )
	{
		(*iter).quantity += iQuantity;
		(*iter).state = SPIRIT_STATE_CHANGED;
	}
	else
	{
		UserSpiritInfo info;
		info.item_code = iItemCode;
		info.quantity = iQuantity;
		info.state = SPIRIT_STATE_NEW;
		m_vSpiritInfo.push_back( info );
	}
}

void ioUserSpirit::DecreaseSpirit( int iItemCode, int iQuantity )
{
	if( iQuantity <= 0 )
		return;

	auto find_spirit = [iItemCode]( UserSpiritInfo info )->bool { return info.item_code == iItemCode; };
	vUserSpiritInfo::iterator iter = std::find_if( m_vSpiritInfo.begin(), m_vSpiritInfo.end(), find_spirit );
	if( iter != m_vSpiritInfo.end() )
	{
		(*iter).quantity -= iQuantity;
		(*iter).state = SPIRIT_STATE_CHANGED;
	}
}

void ioUserSpirit::OnComposeSpirit( SP2Packet &rkPacket )
{
	int iSpiritCode;
	int iSpiritQuantity;
	int iSpecialSpiritCode;
	int iSpecialSpiritQuantity;
	PACKET_GUARD_VOID( rkPacket.Read(iSpiritCode) );
	PACKET_GUARD_VOID( rkPacket.Read(iSpiritQuantity) );
	PACKET_GUARD_VOID( rkPacket.Read(iSpecialSpiritCode) );
	PACKET_GUARD_VOID( rkPacket.Read(iSpecialSpiritQuantity) );

	int iResult = ComposeSpirit( iSpiritCode, iSpiritQuantity, iSpecialSpiritCode, iSpecialSpiritQuantity );

	if( m_pUser )
	{
		if( iResult == SPIRIT_RESULT_SUCCESS )
		{
			SP2Packet kPacket(STPK_SPIRIT_COMPOSE);
			PACKET_GUARD_VOID( kPacket.Write(iResult) );
			PACKET_GUARD_VOID( kPacket.Write(iSpiritCode) );
			PACKET_GUARD_VOID( kPacket.Write(GetSpiritQuantity(iSpiritCode)) );
			PACKET_GUARD_VOID( kPacket.Write(iSpecialSpiritCode) );
			PACKET_GUARD_VOID( kPacket.Write(GetSpiritQuantity(iSpecialSpiritCode)) );
			m_pUser->SendMessage( kPacket );

			g_LogDBClient.OnInsertComposeSpirit( m_pUser, iSpiritCode, iSpiritCode, iSpiritQuantity, iSpecialSpiritCode, iSpecialSpiritQuantity );
		}
		else
		{
			SP2Packet kPacket(STPK_SPIRIT_COMPOSE);
			PACKET_GUARD_VOID( kPacket.Write(iResult) );
			m_pUser->SendMessage( kPacket );
		}
	}
}

int ioUserSpirit::ComposeSpirit( int iSpiritCode, int iSpiritQuantity, int iSpecialSpiritCode, int iSpecialSpiritQuantity )
{
	if( !m_pUser )
		return SPIRIT_RESULT_EXCEPTION;

	iSpiritQuantity = min( iSpiritQuantity, g_SpiritManager.GetComposeNeedMaxSpirit(iSpiritCode) );
	if( iSpiritQuantity == -1 )
		return SPIRIT_RESULT_EXCEPTION;

#ifdef __OHTG_SOLDIER_TAKE_CHANGE__
	bool bLive = false;
	int iExtendCharArray = -1;

	// 용병 체크
	for( int i=0; i<m_pUser->GetCharCount(); ++i )
	{
		ioCharacter *pChar = m_pUser->GetCharacter( i );
		if( !pChar )
			continue;

		if( !pChar->HasExerciseStyle( EXERCISE_NONE ) )
			continue;

		const CHARACTER &rkCharInfo = pChar->GetCharInfo();
		if( rkCharInfo.m_class_type == iSpiritCode )
		{
			if( rkCharInfo.m_ePeriodType == CPT_MORTMAIN )
			{
				bLive = true;
			}

			iExtendCharArray = i;
			break;
		}
	}
#else //__OHTG_SOLDIER_TAKE_CHANGE__
	// 용병 체크
	for( int i=0; i<m_pUser->GetCharCount(); ++i )
	{
		ioCharacter *pChar = m_pUser->GetCharacter( i );
		if( !pChar )
			continue;

		if( !pChar->HasExerciseStyle( EXERCISE_NONE ) )
			continue;

		const CHARACTER &rkCharInfo = pChar->GetCharInfo();
		if( rkCharInfo.m_class_type == iSpiritCode )
		{
			if( rkCharInfo.m_ePeriodType == CPT_MORTMAIN )
				return SPIRIT_RESULT_ALREADY_SOLDIER;

			break;
		}
	}
#endif //__OHTG_SOLDIER_TAKE_CHANGE__

	// 사용할 영혼 개수 체크
	if( iSpiritQuantity < g_SpiritManager.GetComposeNeedMinSpirit(iSpiritCode) )
	{
		return SPIRIT_RESULT_NOT_ENOUGH_SPIRIT;
	}
	if( GetSpiritQuantity(iSpiritCode) < g_SpiritManager.GetComposeNeedMinSpirit(iSpiritCode) )
	{
		return SPIRIT_RESULT_NOT_ENOUGH_SPIRIT;
	}
	if( iSpiritQuantity > GetSpiritQuantity(iSpiritCode) )
	{
		return SPIRIT_RESULT_NOT_ENOUGH_SPIRIT;
	}
	// 사용할 만능조각 개수 체크
	if( iSpiritQuantity < g_SpiritManager.GetComposeNeedMaxSpirit(iSpiritCode) )
	{
		int iMinimumNeedSpirit = g_SpiritManager.GetComposeNeedMaxSpirit(iSpiritCode) - iSpiritQuantity;
		if( iSpecialSpiritQuantity < iMinimumNeedSpirit )
		{
			return SPIRIT_RESULT_NOT_ENOUGH_SPECIAL_SPIRIT;
		}

		if( iSpecialSpiritQuantity > GetSpiritQuantity(iSpecialSpiritCode) )
		{
			return SPIRIT_RESULT_NOT_ENOUGH_SPECIAL_SPIRIT;
		}
	}

#ifdef __OHTG_SOLDIER_TAKE_CHANGE__
	DecreaseSpirit(iSpiritCode, iSpiritQuantity );
	DecreaseSpirit(iSpecialSpiritCode, iSpecialSpiritQuantity );

	if(false == m_pUser->SendSoldierTake(iSpiritCode, 0))
	{
		CTimeSpan cPresentGapTime( g_SpiritManager.GetComposePresentPeriod(), 0, 0, 0 );
		CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;

		m_pUser->AddPresentMemory( g_MainServer->GetSendID(), PRESENT_SOLDIER, iSpiritCode, 0, 0, 0, g_SpiritManager.GetComposePresentMent(), kPresentTime, ioUserPresent::PRESENT_STATE_NORMAL );
		m_pUser->SendPresentMemory();
	}
#else //__OHTG_SOLDIER_TAKE_CHANGE__
	DecreaseSpirit(iSpiritCode, iSpiritQuantity );
	DecreaseSpirit(iSpecialSpiritCode, iSpecialSpiritQuantity );

	CTimeSpan cPresentGapTime( g_SpiritManager.GetComposePresentPeriod(), 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;

	m_pUser->AddPresentMemory( g_MainServer.GetSendID(), PRESENT_SOLDIER, iSpiritCode, 0, 0, 0, g_SpiritManager.GetComposePresentMent(), kPresentTime, ioUserPresent::PRESENT_STATE_NORMAL );
	m_pUser->SendPresentMemory();
#endif //__OHTG_SOLDIER_TAKE_CHANGE__


	return SPIRIT_RESULT_SUCCESS;
}

void ioUserSpirit::OnDecomposeSpirit( SP2Packet &rkPacket )
{
	int iClassType = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iClassType) );

	int iSpiritCode = 0;
	if( g_SpiritManager.GetDecomposeType(iClassType) == ioSpiritManager::DECOMPOSE_SPECIAL_SPIRIT )
	{
		iSpiritCode = ioSpiritManager::SST_SPECIAL_SPIRIT;
	}
	else
	{
		iSpiritCode = iClassType;
	}

	int iPreQuantity = GetSpiritQuantity(iSpiritCode);
	bool bCritical = false;
	int iResult = DecomposeSpirit( iClassType, bCritical );
	int iSoulStoneCnt = g_AlchemicMgr.GetSouleStoneGainCnt();
#ifdef SRC_ID
	int iItemCnt = g_SpiritManager.GetItemGainCntByMercenaryRank(iClassType);		// 지금은 소울스톤 갯수
#endif
	if( m_pUser )
	{
		if( iResult == SPIRIT_RESULT_SUCCESS )
		{
#ifdef __OHTG_SOLDIER_TAKE_CHANGE__
			int iAddSoldierExp = g_LevelMgr.GetSoldierAddExp();
			if( m_pUser->IsClassTypeExerciseStyle( iClassType, EXERCISE_RENTAL ) == false )
				m_pUser->AddClassExp( iClassType, iAddSoldierExp );

			m_pUser->GradeNClassUPBonus();
#endif //__OHTG_SOLDIER_TAKE_CHANGE__

			int iQuantity = GetSpiritQuantity(iSpiritCode) - iPreQuantity;
			iQuantity = max( 0, iQuantity );
			SP2Packet kPacket( STPK_SPIRIT_DECOMPOSE );
			PACKET_GUARD_VOID( kPacket.Write(iResult) );
			PACKET_GUARD_VOID( kPacket.Write(iClassType) );
			PACKET_GUARD_VOID( kPacket.Write(bCritical) );
			PACKET_GUARD_VOID( kPacket.Write(iSpiritCode) );
#ifdef SRC_ID
			PACKET_GUARD_VOID( kPacket.Write( 0 ) );
			PACKET_GUARD_VOID( kPacket.Write(iQuantity) );
			PACKET_GUARD_VOID( kPacket.Write(iItemCnt) );
#ifdef __OHTG_SOLDIER_TAKE_CHANGE__
			PACKET_GUARD_VOID( kPacket.Write(iAddSoldierExp) );
#endif //__OHTG_SOLDIER_TAKE_CHANGE__
			m_pUser->SendMessage( kPacket );

			if( iItemCnt > 0 )
			{
				ioUserEtcItem *pEtcItem = m_pUser->GetUserEtcItem();
				if( pEtcItem )
				{
					pEtcItem->GainSpendTypeEtcItem( g_SpiritManager.GetItemTypeByMercenaryRank(iClassType),iItemCnt );
					g_LogDBClient.OnInsertDisassemble( m_pUser, LogDBClient::DST_SOLDIER, iClassType, g_SpiritManager.GetItemTypeByMercenaryRank(iClassType), iItemCnt );
				}
			}

			g_LogDBClient.OnInsertDecomposeSpirit( m_pUser, iClassType, iSpiritCode, iQuantity, iItemCnt, (BYTE)bCritical );
#else
			PACKET_GUARD_VOID( kPacket.Write(GetSpiritQuantity(iSpiritCode)) );
			PACKET_GUARD_VOID( kPacket.Write(iQuantity) );
			PACKET_GUARD_VOID( kPacket.Write(iSoulStoneCnt) );
#ifdef __OHTG_SOLDIER_TAKE_CHANGE__
			PACKET_GUARD_VOID( kPacket.Write(iAddSoldierExp) );
#endif //__OHTG_SOLDIER_TAKE_CHANGE__
			m_pUser->SendMessage( kPacket );

			if( iSoulStoneCnt > 0 )
			{
				ioUserEtcItem *pEtcItem = m_pUser->GetUserEtcItem();
				if( pEtcItem )
				{
					pEtcItem->GainSpendTypeEtcItem( ioEtcItem::EIT_ETC_SOUL_STONE,iSoulStoneCnt );
					g_LogDBClient.OnInsertDisassemble( m_pUser, LogDBClient::DST_SOLDIER, iClassType, ioEtcItem::EIT_ETC_SOUL_STONE, iSoulStoneCnt );
				}
			}

			g_LogDBClient.OnInsertDecomposeSpirit( m_pUser, iClassType, iSpiritCode, iQuantity, iSoulStoneCnt, (BYTE)bCritical );
#endif
		}
		else
		{
			SP2Packet kPacket( STPK_SPIRIT_DECOMPOSE );
			PACKET_GUARD_VOID( kPacket.Write(iResult) );
			m_pUser->SendMessage( kPacket );
		}
	}
}

int ioUserSpirit::DecomposeSpirit( int iClassType, bool &bCritical )
{
	if( !m_pUser )
		return SPIRIT_RESULT_EXCEPTION;

	// 용병 개수 체크
	if( m_pUser->GetBuyCharCount() <= 1 )
	{
		return SPIRIT_RESULT_NO_MORE_DECOMPOSE_SOLDIER;
	}

	int iCharArray = -1;
	for( int i=0; i<m_pUser->GetCharCount(); ++i )
	{
		if( m_pUser->GetCharClassType(i) == iClassType )
		{
			iCharArray = i;
			break;
		}
	}

	// 체험용병, 보유여부, 용병기간 체크
	ioCharacter *pChar = m_pUser->GetCharacter( iCharArray );
	if( pChar )
	{
		if( !pChar->HasExerciseStyle(EXERCISE_NONE) )
		{
			return SPIRIT_RESULT_HAS_NOT_SOLDIER;
		}

		const CHARACTER &rkCharInfo = pChar->GetCharInfo();
		if( rkCharInfo.m_ePeriodType != CPT_MORTMAIN )
		{
			return SPIRIT_RESULT_NO_MORTMAIN_SOLDIER;
		}
	}
	else
	{
		return SPIRIT_RESULT_HAS_NOT_SOLDIER;
	}

	Room *pRoom = m_pUser->GetMyRoom();
	if( pRoom )
	{
		if( iCharArray == m_pUser->GetSelectChar() )
		{
			// 전투중 사용 용병 체크
			return SPIRIT_RESULT_SELECTED_SOLDIER;
		}
		else
		{
			// R용병 체크
			int iSoldierType = m_pUser->GetSpecialSoldierType(iClassType);
			if( iSoldierType != SST_END )
			{
				if( SST_RSOLDIER == iSoldierType )
				{
					//해당 방 유저에게 R용병 분해 했다구 통지.
					SP2Packet kPacket(STPK_RSOLDIER_STATUS);
					kPacket << RSOLDIER_DISASSEMBLE;
					kPacket << m_pUser->GetPublicID();

					if( pRoom )
						pRoom->RoomSendPacketTcp(kPacket);
					else
						m_pUser->SendMessage(kPacket);
				}
				else
				{
					SP2Packet kPacket(STPK_SOLDIER_SET_STATUS);
					kPacket << RSOLDIER_DISASSEMBLE;
					kPacket << m_pUser->GetPublicID();
					kPacket << iClassType;

					if( pRoom )
						pRoom->RoomSendPacketTcp(kPacket);
					else
						m_pUser->SendMessage(kPacket);
				}
			}
		}
	}

	m_pUser->_OnCharDelete( iCharArray );

	int iValue = m_SpiritRandom.Random( SPIRIT_RANDOM_MAX );
	int iQuantity = 0;
	if( COMPARE( iValue, 0, g_SpiritManager.GetDecomposeCriticalValue(iClassType) ) )
	{
		bCritical = true;
		iQuantity = g_SpiritManager.GetDecomposeCriticalQuantity(iClassType);
	}
	else
	{
		bCritical = false;
		iQuantity = g_SpiritManager.GetDecomposeQuantity(iClassType);
	}

#ifndef SRC_ID		// 인니 용병 분해 시 용병 정기 지급되지 않게 수정
	if( g_SpiritManager.GetDecomposeType(iClassType) == ioSpiritManager::DECOMPOSE_SPECIAL_SPIRIT )
	{
		IncreaseSpirit( (int)ioSpiritManager::SST_SPECIAL_SPIRIT, iQuantity );
	}
	else
	{
		IncreaseSpirit( iClassType, iQuantity );
	}
#endif

	return SPIRIT_RESULT_SUCCESS;
}

void ioUserSpirit::OnPresentDecomposeSpirit( int iClassType )
{
	int iSpiritCode = 0;

	if( g_SpiritManager.GetDecomposeType(iClassType) == ioSpiritManager::DECOMPOSE_SPECIAL_SPIRIT )
	{
		iSpiritCode = ioSpiritManager::SST_SPECIAL_SPIRIT;
	}
	else
	{
		iSpiritCode = iClassType;
	}

	int iPreQuantity = GetSpiritQuantity(iSpiritCode);
	bool bCritical = false;
	int iItemCnt = -1;
	int iSoulStoneCnt = g_AlchemicMgr.GetSouleStoneGainCnt();
#ifdef SRC_ID
	int iItemCnt = g_SpiritManager.GetItemGainCntByMercenaryRank(iClassType);		// 지금은 소울스톤 갯수
#endif

#ifndef SRC_ID
	int iValue = m_SpiritRandom.Random( SPIRIT_RANDOM_MAX );
	int iQuantity = 0;
	if( COMPARE( iValue, 0, g_SpiritManager.GetDecomposeCriticalValue(iClassType) ) )
	{
		bCritical = true;
		iQuantity = g_SpiritManager.GetDecomposeCriticalQuantity(iClassType);
	}
	else
	{
		bCritical = false;
		iQuantity = g_SpiritManager.GetDecomposeQuantity(iClassType);
	}

	if( g_SpiritManager.GetDecomposeType(iClassType) == ioSpiritManager::DECOMPOSE_SPECIAL_SPIRIT )
	{
		IncreaseSpirit( (int)ioSpiritManager::SST_SPECIAL_SPIRIT, iQuantity );
	}
	else
	{
		IncreaseSpirit( iClassType, iQuantity );
	}
#endif

	if( m_pUser )
	{
#ifdef __OHTG_SOLDIER_TAKE_CHANGE__
		int iAddSoldierExp = g_LevelMgr.GetSoldierAddExp();
		if( m_pUser->IsClassTypeExerciseStyle( iClassType, EXERCISE_RENTAL ) == false )
			m_pUser->AddClassExp( iClassType, iAddSoldierExp );

		m_pUser->GradeNClassUPBonus();
#endif //__OHTG_SOLDIER_TAKE_CHANGE__

		int iQuantity = GetSpiritQuantity(iSpiritCode) - iPreQuantity;
		iQuantity = max( 0, iQuantity );
		SP2Packet kPacket( STPK_SPIRIT_DECOMPOSE );
		PACKET_GUARD_VOID( kPacket.Write( (int)SPIRIT_RESULT_PRESENT_SUCCESS ) );
		PACKET_GUARD_VOID( kPacket.Write(iClassType) );
		PACKET_GUARD_VOID( kPacket.Write(bCritical) );
		PACKET_GUARD_VOID( kPacket.Write(iSpiritCode) );
#ifdef SRC_ID
		PACKET_GUARD_VOID( kPacket.Write(GetSpiritQuantity(iSpiritCode)) );
		PACKET_GUARD_VOID( kPacket.Write(iQuantity) );
		PACKET_GUARD_VOID( kPacket.Write(iItemCnt) );
#ifdef __OHTG_SOLDIER_TAKE_CHANGE__
		PACKET_GUARD_VOID( kPacket.Write(iAddSoldierExp) );
#endif //__OHTG_SOLDIER_TAKE_CHANGE__
		m_pUser->SendMessage( kPacket );

		if( iItemCnt > 0 )
		{
			ioUserEtcItem *pEtcItem = m_pUser->GetUserEtcItem();
			if( pEtcItem )
			{
				pEtcItem->GainSpendTypeEtcItem( g_SpiritManager.GetItemTypeByMercenaryRank(iClassType), iItemCnt );
				g_LogDBClient.OnInsertDisassemble( m_pUser, LogDBClient::DST_SOLDIER, iClassType, g_SpiritManager.GetItemTypeByMercenaryRank(iClassType), iItemCnt );
			}
		}

		g_LogDBClient.OnInsertDecomposeSpirit( m_pUser, iClassType, iSpiritCode, iQuantity, iItemCnt, (BYTE)bCritical );
#else
			PACKET_GUARD_VOID( kPacket.Write(GetSpiritQuantity(iSpiritCode)) );
			PACKET_GUARD_VOID( kPacket.Write(iQuantity) );
			PACKET_GUARD_VOID( kPacket.Write(iSoulStoneCnt) );
#ifdef __OHTG_SOLDIER_TAKE_CHANGE__
			PACKET_GUARD_VOID( kPacket.Write(iAddSoldierExp) );
#endif //__OHTG_SOLDIER_TAKE_CHANGE__
			m_pUser->SendMessage( kPacket );

			if( iSoulStoneCnt > 0 )
			{
				ioUserEtcItem *pEtcItem = m_pUser->GetUserEtcItem();
				if( pEtcItem )
				{
					pEtcItem->GainSpendTypeEtcItem( ioEtcItem::EIT_ETC_SOUL_STONE,iSoulStoneCnt );
					g_LogDBClient.OnInsertDisassemble( m_pUser, LogDBClient::DST_SOLDIER, iClassType, ioEtcItem::EIT_ETC_SOUL_STONE, iSoulStoneCnt );
				}
			}

			g_LogDBClient.OnInsertDecomposeSpirit( m_pUser, iClassType, iSpiritCode, iQuantity, iSoulStoneCnt, (BYTE)bCritical );
#endif
	}
}

void ioUserSpirit::OnConversionSpirit( SP2Packet &rkPacket )
{
	int iConsumeItemCode = 0;
	int iConsumeItemQuantity = 0;
	int iCreateItemCode = 0;
	
	PACKET_GUARD_VOID( rkPacket.Read(iConsumeItemCode) );
	PACKET_GUARD_VOID( rkPacket.Read(iConsumeItemQuantity) );
	PACKET_GUARD_VOID( rkPacket.Read(iCreateItemCode) );

	int iPreQuantity = GetSpiritQuantity(iCreateItemCode);

	bool bCritical = false;
	int iResult = ConversionSpirit( iConsumeItemCode, iConsumeItemQuantity, iCreateItemCode, bCritical );

	if( m_pUser )
	{
		if( iResult == SPIRIT_RESULT_SUCCESS )
		{
			int iQuantity = GetSpiritQuantity(iCreateItemCode) - iPreQuantity;
			SP2Packet kPacket(STPK_SPIRIT_CONVERSION);
			PACKET_GUARD_VOID( kPacket.Write(iResult) );
			PACKET_GUARD_VOID( kPacket.Write(bCritical) );
			PACKET_GUARD_VOID( kPacket.Write(iConsumeItemCode) );
			PACKET_GUARD_VOID( kPacket.Write(GetSpiritQuantity(iConsumeItemCode)) );
			PACKET_GUARD_VOID( kPacket.Write(iCreateItemCode) );
			PACKET_GUARD_VOID( kPacket.Write(GetSpiritQuantity(iCreateItemCode)) );
			PACKET_GUARD_VOID( kPacket.Write(iQuantity) );
			m_pUser->SendMessage( kPacket );

			g_LogDBClient.OnInsertConversionSpirit( m_pUser, iConsumeItemCode, iConsumeItemQuantity, iCreateItemCode, iQuantity, (BYTE)bCritical );
		}
		else
		{
			SP2Packet kPacket(STPK_SPIRIT_CONVERSION);
			PACKET_GUARD_VOID( kPacket.Write(iResult) );
			m_pUser->SendMessage( kPacket );
		}
	}
}

void ioUserSpirit::OnSellSpirit( SP2Packet &rkPacket )
{
	if( !m_pUser )
		return;

	int iSpiritCode;
	PACKET_GUARD_VOID( rkPacket.Read(iSpiritCode) );

	int iCnt = GetSpiritQuantity( iSpiritCode );
	if( iCnt == 0 )
	{
		SP2Packet kPacket( STPK_SPIRIT_SELL );
		PACKET_GUARD_VOID( kPacket.Write((int)SPIRIT_RESULT_EXCEPTION) );
		m_pUser->SendMessage( kPacket );
		return;
	}

	__int64 iPeso = (__int64)g_SpiritManager.GetSellPesoByItemCode(iSpiritCode) * iCnt;
	m_pUser->AddMoney( iPeso );
	DecreaseSpirit( iSpiritCode, iCnt );

	SP2Packet kPacket( STPK_SPIRIT_SELL );
	PACKET_GUARD_VOID( kPacket.Write((int)SPIRIT_RESULT_SUCCESS) );
	PACKET_GUARD_VOID( kPacket.Write(iSpiritCode) );
	PACKET_GUARD_VOID( kPacket.Write(0) );
	PACKET_GUARD_VOID( kPacket.Write(iPeso) );
	PACKET_GUARD_VOID( kPacket.Write(m_pUser->GetMoney()) );
	m_pUser->SendMessage( kPacket );

	g_LogDBClient.OnInsertObtainSpirit( m_pUser, iSpiritCode, 0, iPeso, (BYTE)LogDBClient::SPIRIT_SELL );
}

int ioUserSpirit::ConversionSpirit( int iConsumeSpiritCode, int iConsumeSpiritQuantity, int iCreateSpiritCode, bool &bCritical )
{
	if( !g_SpiritManager.EnableSpirit( iConsumeSpiritCode, iCreateSpiritCode ) )
		return SPIRIT_RESULT_EXCEPTION;

	if( GetSpiritQuantity(iConsumeSpiritCode) >= iConsumeSpiritQuantity )
	{
		int iCreateQuantity = iConsumeSpiritQuantity / g_SpiritManager.GetSpiritConversionInput();
		int iValue = m_SpiritRandom.Random( SPIRIT_RANDOM_MAX );
		int iQuantity = 0;
		if( COMPARE( iValue, 0, g_SpiritManager.GetSpiritConversionCriticalValue() ) )
		{
			bCritical = true;
			iQuantity = g_SpiritManager.GetSpiritConversionCriticalOutput();
		}
		else
		{
			bCritical = false;
			iQuantity = g_SpiritManager.GetSpiritConversionOutput();
		}

		DecreaseSpirit(iConsumeSpiritCode, iConsumeSpiritQuantity );
		IncreaseSpirit(iCreateSpiritCode, iQuantity * iCreateQuantity );

		return SPIRIT_RESULT_SUCCESS;
	}
	else
	{
		return SPIRIT_RESULT_NOT_ENOUGH_SPIRIT;
	}
}
