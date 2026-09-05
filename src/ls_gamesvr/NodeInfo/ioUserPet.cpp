#include "stdafx.h"

#include "../MainProcess.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"
#include "../Util/cSerialize.h"
#include "../EtcHelpFunc.h"

#include "User.h"
#include "Room.h"
#include "ioUserPet.h"
#include "ioEtcItem.h"
#include <strsafe.h>
#include "ioPetInfoManager.h"

ioUserPet::ioUserPet()
{
	Initialize( NULL );
}

ioUserPet::~ioUserPet()
{
	m_vPetSlotList.clear();
}

void ioUserPet::Initialize( User *pUser )
{
	m_pUser = pUser;
	m_iEquipPetListIdx = NONE_EQUIP;
	m_vPetSlotList.reserve( MAX_SLOT );
	m_vPetSlotList.clear();
}

bool ioUserPet::DBtoNewIndex( DWORD dwIndex )
{
	return true;
}

//TEST
void ioUserPet::TestInput()
{
	PETSLOT rkPet;
	rkPet.m_iIndex = 1;
	rkPet.m_iPetCode = 1;
	rkPet.m_iPetRank = 3;
	m_vPetSlotList.push_back( rkPet );


	rkPet.m_iIndex = 2;
	rkPet.m_iPetCode = 1;
	rkPet.m_iPetRank = 4;
	m_vPetSlotList.push_back( rkPet );

	rkPet.m_iIndex = 3;
	rkPet.m_iPetCode = 2;
	rkPet.m_iPetRank = 4;
	m_vPetSlotList.push_back( rkPet );

}

void ioUserPet::DBtoData( CQueryResultData *query_data )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserPet::DBtoData() User NULL!!"); 
		return;
	}

	Initialize( m_pUser );

	LOOP_GUARD();
	while( query_data->IsExist() )
	{	
		//펫 정보 가져와서 저장.
		PETSLOT pet_data;
		BYTE petRank;
		PACKET_GUARD_BREAK( query_data->GetValue( pet_data.m_iIndex, sizeof(int) ) );		//펫 인덱스
		PACKET_GUARD_BREAK( query_data->GetValue( pet_data.m_iPetCode, sizeof(int) ) );		//펫 코드
		PACKET_GUARD_BREAK( query_data->GetValue( petRank, sizeof(BYTE) ) );					//펫 랭크
		PACKET_GUARD_BREAK( query_data->GetValue( pet_data.m_iCurLevel, sizeof(int) ) );		//펫 레벨
		PACKET_GUARD_BREAK( query_data->GetValue( pet_data.m_iCurExp, sizeof(int) ) );		//펫 경험치
		PACKET_GUARD_BREAK( query_data->GetValue( pet_data.m_bEquip, sizeof(BYTE) ) );		//펫 장착여부
#if defined( DIVIDE_PET ) 
		PACKET_GUARD_BREAK( query_data->GetValue( pet_data.m_iClassType, sizeof(int) ) );	//펫 착용 영웅
#endif
		pet_data.m_iPetRank = petRank;
		
		g_PetInfoMgr.SetMaxExp( pet_data );

		m_vPetSlotList.push_back( pet_data );

		if( pet_data.m_bEquip )
			m_iEquipPetListIdx = m_vPetSlotList.size()-1;
	}
	LOOP_GUARD_CLEAR();

	//가지고 있는 펫이 없다면 return
	if( m_vPetSlotList.empty() ) return;

	int iPetCount = m_vPetSlotList.size();

	SP2Packet kPacket( STPK_USER_PET_DATA );
	PACKET_GUARD_VOID ( kPacket.Write( iPetCount ) );

	for(int i = 0; i < iPetCount; i++ )
	{
		PACKET_GUARD_VOID( kPacket.Write( m_vPetSlotList[i].m_iPetCode ) );
		PACKET_GUARD_VOID( kPacket.Write( m_vPetSlotList[i].m_iPetRank ) );
		PACKET_GUARD_VOID( kPacket.Write( m_vPetSlotList[i].m_iIndex ) );
		PACKET_GUARD_VOID( kPacket.Write( m_vPetSlotList[i].m_iCurLevel ) );
		PACKET_GUARD_VOID( kPacket.Write( m_vPetSlotList[i].m_iCurExp ) );
		PACKET_GUARD_VOID( kPacket.Write( m_vPetSlotList[i].m_bEquip ) );
#if defined( DIVIDE_PET ) 
		PACKET_GUARD_VOID( kPacket.Write( m_vPetSlotList[i].m_iClassType ) );
#endif
	}

	m_pUser->SendMessage( kPacket );
}

void ioUserPet::SaveData()
{
	if( m_vPetSlotList.empty() )
		return;

	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserPet::SaveData() User NULL!!"); 
		return;
	}

#ifndef DIVIDE_PET
	CheckEquipIndexError(); //용병 별 중복 저장이 가능
#endif

	int iPetCount = m_vPetSlotList.size();

	if( iPetCount == 0 )
		return;

	cSerialize v_FT;
	BYTE rank; //DB테이블에선 랭크를 1바이트로 저장.

	for( int i = 0; i < iPetCount ; i++ )
	{
		if( m_vPetSlotList[i].m_bChange )
		{
			v_FT.Reset();
			rank = m_vPetSlotList[i].m_iPetRank;

			v_FT.Write( m_pUser->GetUserIndex() );
			v_FT.Write( m_vPetSlotList[i].m_iIndex );
			v_FT.Write( rank );
			v_FT.Write( m_vPetSlotList[i].m_iCurLevel );
			v_FT.Write( m_vPetSlotList[i].m_iCurExp );
			v_FT.Write( (BYTE)m_vPetSlotList[i].m_bEquip );
#if defined( DIVIDE_PET ) 
			v_FT.Write( m_vPetSlotList[i].m_iClassType );
#endif
			g_DBClient.OnUpdatePetData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetPublicID(), m_vPetSlotList[i].m_iIndex, v_FT );
			m_vPetSlotList[i].m_bChange = false;
		}
	}
}

bool ioUserPet::AddData( PETSLOT &rkNewSlot, CQueryData &query_data )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserPet::AddData() User NULL!!"); 
		return false;
	}

	if( rkNewSlot.m_iPetCode == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserPet::AddData Code is Zero" );
		return false;
	}

	cSerialize v_FT;
	BYTE rankData = rkNewSlot.m_iPetRank;

	v_FT.Reset();
	v_FT.Write( m_pUser->GetUserIndex() );
	v_FT.Write( rkNewSlot.m_iPetCode );
	v_FT.Write( rankData );
	v_FT.Write( rkNewSlot.m_iCurLevel );
#if defined( DIVIDE_PET ) 
	v_FT.Write( 0 ); //착용 용병
#endif
	if( !g_DBClient.OnInsertPetData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetPublicID() ,v_FT, query_data ) )
		return false;

	return true;
}

void ioUserPet::AddPet( const int& iNewIndex, const int& iPetCode, const int& iPetRank, const int& iPetLevel )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserPet::AddPet() User NULL!!"); 
		return;
	}

	PETSLOT rkPetSlot;
	rkPetSlot.m_iIndex = iNewIndex;
	rkPetSlot.m_iPetCode = iPetCode;
	rkPetSlot.m_iPetRank = iPetRank;
	rkPetSlot.m_iCurLevel = iPetLevel;

	int iVecIndex = CheckEmptyVecIndex();

	if( iVecIndex == 0 )
		m_vPetSlotList.push_back( rkPetSlot );
	else
		m_vPetSlotList[iVecIndex] = rkPetSlot;	
}

bool ioUserPet::DeleteData( const int& iPetIndex )
{
	PETSLOT rkPetSlot;
	int iPetVecIndex;

	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserPet::DeleteData() User NULL!!"); 
		return false;
	}

	if( !GetPetInfo( iPetIndex, iPetVecIndex ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserPet::DeleteData() None PetIndex Have : %s!!",m_pUser->GetPublicID().c_str() ); 
		return false;
	}

	cSerialize v_FT;

	v_FT.Reset();
	v_FT.Write( m_pUser->GetUserIndex() );
	v_FT.Write( m_vPetSlotList[iPetVecIndex].m_iIndex );

	//삭제 DB쿼리 호출
	if( !g_DBClient.OnDeletePetData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetPublicID(), m_vPetSlotList[iPetVecIndex].m_iIndex, v_FT ) )
		return false;

	g_LogDBClient.OnInsertPetLog( m_pUser, m_vPetSlotList[iPetVecIndex].m_iIndex, m_vPetSlotList[iPetVecIndex].m_iPetCode,m_vPetSlotList[iPetVecIndex].m_iPetRank,
		m_vPetSlotList[iPetVecIndex].m_iCurLevel, m_vPetSlotList[iPetVecIndex].m_iCurExp, 0, LogDBClient::PDT_DELETE );

	//삭제된 벡터 정보 초기화
	if( m_vPetSlotList[iPetVecIndex].m_bEquip )
	{
		m_iEquipPetListIdx = NONE_EQUIP;
	}

	m_vPetSlotList[iPetVecIndex].Init();
	return true;
}

void ioUserPet::FillMoveData( SP2Packet &rkPacket )
{
	int iPetCount = m_vPetSlotList.size();
	PACKET_GUARD_VOID( rkPacket.Write( iPetCount ) );
	PACKET_GUARD_VOID( rkPacket.Write( m_iEquipPetListIdx ) );
	for( int i = 0; i < iPetCount; i++ )
	{
		PACKET_GUARD_BREAK( rkPacket.Write( m_vPetSlotList[i].m_iIndex ) );
		PACKET_GUARD_BREAK( rkPacket.Write( m_vPetSlotList[i].m_iPetCode ) );
		PACKET_GUARD_BREAK( rkPacket.Write( m_vPetSlotList[i].m_iPetRank ) );
		PACKET_GUARD_BREAK( rkPacket.Write( m_vPetSlotList[i].m_iCurLevel ) );
		PACKET_GUARD_BREAK( rkPacket.Write( m_vPetSlotList[i].m_iMaxExp ) );
		PACKET_GUARD_BREAK( rkPacket.Write( m_vPetSlotList[i].m_iCurExp ) );
		PACKET_GUARD_BREAK( rkPacket.Write( m_vPetSlotList[i].m_bEquip ) );
#if defined( DIVIDE_PET ) 
		PACKET_GUARD_BREAK( rkPacket.Write( m_vPetSlotList[i].m_iClassType ) );
#endif
		PACKET_GUARD_BREAK( rkPacket.Write( m_vPetSlotList[i].m_bChange ) );
	}
}

void ioUserPet::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode )
{
	int iSize;
	PACKET_GUARD_VOID( rkPacket.Read( iSize ) );
	PACKET_GUARD_VOID( rkPacket.Read( m_iEquipPetListIdx ) );

	for( int i = 0; i < iSize; i++ )
	{
		PETSLOT kPetSlot;
		PACKET_GUARD_VOID( rkPacket.Read( kPetSlot.m_iIndex ) );
		PACKET_GUARD_VOID( rkPacket.Read( kPetSlot.m_iPetCode ) );
		PACKET_GUARD_VOID( rkPacket.Read( kPetSlot.m_iPetRank ) );
		PACKET_GUARD_VOID( rkPacket.Read( kPetSlot.m_iCurLevel ) );
		PACKET_GUARD_VOID( rkPacket.Read( kPetSlot.m_iMaxExp ) );
		PACKET_GUARD_VOID( rkPacket.Read( kPetSlot.m_iCurExp ) );
		PACKET_GUARD_VOID( rkPacket.Read( kPetSlot.m_bEquip ) );
#if defined( DIVIDE_PET ) 
		PACKET_GUARD_VOID( rkPacket.Read( kPetSlot.m_iClassType ) );
#endif
		PACKET_GUARD_VOID( rkPacket.Read( kPetSlot.m_bChange ) );

		m_vPetSlotList.push_back( kPetSlot );
	}
}

bool ioUserPet::GetPetInfo( const int& iPetIndex, PETSLOT& rkPet )
{	
	if( m_vPetSlotList.empty() )
		return false;

	int iPetCount = m_vPetSlotList.size();

	for( int i = 0; i < iPetCount; i++ )
	{
		if( m_vPetSlotList[i].m_iIndex == iPetIndex )
		{
			rkPet = m_vPetSlotList[i];
			return true;
		}
	}
	return false;

}

bool ioUserPet::GetPetInfo( const int& iPetIndex, PETSLOT& rkPet, int& iListIndx )
{
	if( m_vPetSlotList.empty() )
		return false;

	int iPetCount = m_vPetSlotList.size();
	for( int i = 0; i < iPetCount; i++ )
	{
		if( m_vPetSlotList[i].m_iIndex == iPetIndex )
		{
			rkPet = m_vPetSlotList[i];
			iListIndx = i;
			return true;
		}
	}
	return false;
}

bool ioUserPet::GetPetInfo( const int& iPetIndex, int& iListIndx )
{
	if( m_vPetSlotList.empty() )
		return false;

	int iPetCount = m_vPetSlotList.size();
	for( int i = 0; i < iPetCount; i++ )
	{
		if( m_vPetSlotList[i].m_iIndex == iPetIndex )
		{
			iListIndx = i;
			return true;
		}
	}
	return false;
}

int  ioUserPet::GetPetVecIndex( const int& iPetIndex )
{
	int iPetCount = m_vPetSlotList.size();

	for( int i = 0; i < iPetCount; i++ )
	{
		if( m_vPetSlotList[i].m_iIndex == iPetIndex )
			return i;
	}

	return -1;
}

int ioUserPet::GetPetCode( const int iPetIndex )
{
	int iPetCount = m_vPetSlotList.size();
	for( int i = 0; i < iPetCount; i++ )
	{
		if( m_vPetSlotList[i].m_iIndex == iPetIndex )
		{
			return m_vPetSlotList[i].m_iPetCode;
		}
	}
	return 0;
}

bool ioUserPet::ClearPetEquip( const int& iPetIndex  )
{
	if( m_iEquipPetListIdx == NONE_EQUIP )
	{
		int index = CheckEquipPetVecIndex();
		if( index == -1 )
			return false;

		m_iEquipPetListIdx = index;
	}

	if( m_vPetSlotList[m_iEquipPetListIdx].m_iIndex != iPetIndex )
	{
		int iIndex = GetPetVecIndex( iPetIndex );
		if( iIndex == -1 )
			return false;

		m_iEquipPetListIdx = iIndex;
		CheckEquipIndexError();
	}

	m_vPetSlotList[m_iEquipPetListIdx].m_bEquip = false;
#if defined( DIVIDE_PET ) 
	m_vPetSlotList[m_iEquipPetListIdx].m_iClassType = 0;
#endif
	m_vPetSlotList[m_iEquipPetListIdx].m_bChange = true;

	m_iEquipPetListIdx = NONE_EQUIP;

	return true;
}

bool ioUserPet::EquipPet( const int &iPetIndex, PETSLOT& rkPet )
{
	if(  !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserPet::EquipPet() User NULL!!"); 
		return false;
	}

	int iListIndx = 0;

	if( !GetPetInfo( iPetIndex, rkPet, iListIndx ) )
		return false;
#if defined( DIVIDE_PET ) 
	for( auto PetSlot = m_vPetSlotList.begin(), EndSlot = m_vPetSlotList.end() ; PetSlot != EndSlot ; ++PetSlot ) //기존 장착중인 펫 해제
	{
		if((PetSlot->m_iClassType == iClassType && PetSlot->m_iIndex != iPetIndex ) || //동일 용병이 펫 변경하는 경우
			( PetSlot->m_iClassType != iClassType && PetSlot->m_bEquip && PetSlot->m_iIndex == iPetIndex )) //다른 용병이 착용 중인 펫을 착용하는 경우 
		{
			PetSlot->m_bEquip = false;
			PetSlot->m_iClassType = 0;
			continue;
		}
	}
#else
	if( m_iEquipPetListIdx != NONE_EQUIP ) //장착 중인 펫이 있을 경우
	{
		m_vPetSlotList[m_iEquipPetListIdx].m_bEquip = false;
		m_vPetSlotList[m_iEquipPetListIdx].m_bChange = true;
	}
#endif
	m_iEquipPetListIdx = iListIndx;
	m_vPetSlotList[m_iEquipPetListIdx].m_bEquip = true;
#if defined( DIVIDE_PET ) 
	m_vPetSlotList[m_iEquipPetListIdx].m_iClassType = iClassType;
#endif
	m_vPetSlotList[m_iEquipPetListIdx].m_bChange = true;

	rkPet = m_vPetSlotList[m_iEquipPetListIdx];

	return true;
}

int ioUserPet::PetNurture( User* pUser, const int& iPetIndex, const int& iEtcItemCode, PETSLOT& rkPetInfo )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserPet::PetNurture() Error : User is Null"); 
		return 2; 
	}

	if( !COMPARE( iEtcItemCode, ioEtcItem::EIT_ETC_PET_FEED_01, ioEtcItem::EIT_ETC_PET_FEED_10+1 ) && iEtcItemCode != ioEtcItem::EIT_ETC_ADDICTIVE_PIECE )
		return 3;

	if( !GetPetInfo( iPetIndex, rkPetInfo ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserPet::PetNurture() Error : Pet is None ID:%s PetIndex:%d", pUser->GetPublicID().c_str(), iPetIndex ); 
		return 4;
	}

	ioUserEtcItem *pUserEtcItem = pUser->GetUserEtcItem();
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserPet::PetNurture() Error : pUserEtcItem is None ID:%s", pUser->GetPublicID().c_str() ); 
		return 5;
	}

	int iDeleteCount = 1;
	if( iEtcItemCode == ioEtcItem::EIT_ETC_ADDICTIVE_PIECE )
		iDeleteCount = g_PetInfoMgr.GetPetEatAdditiveCount();

	ioUserEtcItem::ETCITEMSLOT kSlot;
	pUserEtcItem->GetEtcItem( iEtcItemCode, kSlot );
	if( kSlot.m_iType <= 0 || kSlot.m_iValue1 < iDeleteCount )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserPet::PetNurture() Error : Don't have value ID:%s EtcItemCode:%d ItemCount:%d", pUser->GetPublicID().c_str(), iEtcItemCode, kSlot.m_iValue1 ); 
		return 6;
	}

	if( rkPetInfo.m_iMaxExp == 0 )
	{
		if( !g_PetInfoMgr.SetMaxExp( rkPetInfo ) )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserPet::PetNurture() Error : Rank Const Null"); 
			return 7;
		}
	}

	int iPetPrevLevel = rkPetInfo.m_iCurLevel;
	kSlot.AddUse( -iDeleteCount );
	if( kSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}
	pUser->DoAdditiveMission(iDeleteCount, AMT_COMPOUND);

	SP2Packet kReturn( STPK_ETCITEM_USE );
	PACKET_GUARD_bool( kReturn.Write( ETCITEM_USE_OK ) );
	PACKET_GUARD_bool( kReturn.Write( kSlot.m_iType ) );
	PACKET_GUARD_bool( kReturn.Write( kSlot.m_iValue1 ) );
	PACKET_GUARD_bool( kReturn.Write( kSlot.m_iValue2 ) );
	pUser->SendMessage( kReturn );

	if( !g_PetInfoMgr.AddExp( pUser, rkPetInfo, iEtcItemCode ) )
		return 8;

	//육성 로그
	g_LogDBClient.OnInsertPetLog( pUser, rkPetInfo.m_iIndex, rkPetInfo.m_iPetCode, rkPetInfo.m_iPetRank, rkPetInfo.m_iCurLevel, rkPetInfo.m_iCurExp, iEtcItemCode, LogDBClient::PDT_NURTURE );

	//레벨업
	if( iPetPrevLevel != rkPetInfo.m_iCurLevel )
	{
		g_LogDBClient.OnInsertPetLog( pUser, rkPetInfo.m_iIndex, rkPetInfo.m_iPetCode, rkPetInfo.m_iPetRank, iPetPrevLevel, rkPetInfo.m_iCurExp, 0, LogDBClient::PDT_LEVELUP, rkPetInfo.m_iCurLevel );

		//장착 중인것이 레벨업 했을 경우.
		if( m_iEquipPetListIdx != NONE_EQUIP && m_vPetSlotList[ m_iEquipPetListIdx ].m_iIndex == rkPetInfo.m_iIndex )
			return 1;
	}
	return 0;
}

bool ioUserPet::GetEquipPetInfo( PETSLOT &rkNewSlot )
{
	rkNewSlot.Init();
#if defined( DIVIDE_PET ) 
	for( int i = 0 ; i < m_vPetSlotList.size() ; ++i )
	{
		if ( m_vPetSlotList[i].m_bEquip && m_vPetSlotList[i].m_iClassType == iClassType )
		{
			rkNewSlot = m_vPetSlotList[i];
			return true;
		}
	}
#else
	if( m_iEquipPetListIdx == NONE_EQUIP )
		return false;

	rkNewSlot = m_vPetSlotList[m_iEquipPetListIdx];
	return true;
#endif
	return false;
}

int ioUserPet::CheckEquipPetVecIndex()
{
	int iPetCount = m_vPetSlotList.size();
	for( int i=0; i < iPetCount; i++ )
	{
		if( m_vPetSlotList[i].m_bEquip )
			return i;
	}

	return -1; 
}

void ioUserPet::CheckEquipIndexError()
{
	int iPetCount = m_vPetSlotList.size();

	for( int i=0; i < iPetCount; i++ )
	{
		if( m_vPetSlotList[i].m_bEquip )
		{
			if( i != m_iEquipPetListIdx )
			{
				m_vPetSlotList[i].m_bEquip = false;
#if defined( DIVIDE_PET ) 
				m_vPetSlotList[i].m_iClassType = 0;
#endif
				m_vPetSlotList[i].m_bChange = true;
			}
		}
	}
}

int ioUserPet::CheckEmptyVecIndex()
{
	int iPetCount = m_vPetSlotList.size();

	for( int i=0; i < iPetCount; i++ )
	{
		if( m_vPetSlotList[i].m_iIndex == 0 )
			return i;
	}

	return 0;
}

bool ioUserPet::CheckPetHavePossible()
{
	int iPetCount = m_vPetSlotList.size();

	for( int i=0; i < iPetCount; i++ )
	{
		if( m_vPetSlotList[i].m_iIndex == 0 )
			return true;
	}

	if( iPetCount >= g_PetInfoMgr.GetMaxPetCount() )
		return false;

	return true;
}

void ioUserPet::SetPetData( const PETSLOT &rkPetSlot )
{
	int iPetCount = m_vPetSlotList.size();

	for( int i=0; i < iPetCount; i++ )
	{
		if( m_vPetSlotList[i].m_iIndex == rkPetSlot.m_iIndex )
		{
			m_vPetSlotList[i] = rkPetSlot;
			m_vPetSlotList[i].m_bChange = true;
			return;
		}
	}
}