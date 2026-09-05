#include "stdafx.h"

#include "../MainProcess.h"
#include "../DataBase/DBClient.h"
#include "../MainServerNode/MainServerNode.h"
#include "../NodeInfo/ioEtcItemManager.h"

#include "ServerNodeManager.h"
#include "UserNodeManager.h"
#include "BattleRoomManager.h"
#include "TournamentManager.h"
#include "TestNodeManager.h"
template<> TournamentManager* Singleton< TournamentManager >::ms_Singleton = 0;
//////////////////////////////////////////////////////////////////////////
RegularTournamentReward::RegularTournamentReward( const ioHashString &rkTitle )
{
	m_szTitle = rkTitle;
	m_dwTournamentStartDate = 0;
	m_iBattleRewardPeso     = 0;
	m_dwCampWinRewardIndexB	= 0;
	m_dwCampLoseRewardIndexB= 0;
	m_dwCampWinRewardIndexR	= 0;
	m_dwCampLoseRewardIndexR= 0;
	m_dwCampDrawRewardIndex	= 0;
}

RegularTournamentReward::~RegularTournamentReward()
{
	m_RoundRewardIndex.clear();
}

void RegularTournamentReward::LoadReward( ioINILoader &rkLoader )
{
	rkLoader.SetTitle( m_szTitle.c_str() );

	m_dwTournamentStartDate = atol( m_szTitle.c_str() );

	// 경기 보상
	m_iBattleRewardPeso = rkLoader.LoadInt( "battle_reward_peso", 0 );

	// 대회 보상
	m_RoundRewardIndex.clear();
	int iMaxRoundReward = rkLoader.LoadInt( "max_round_reward", 0 );
	for(int i = 0;i < iMaxRoundReward;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_s( szKey, "reward_table%d", i + 1 );

		m_RoundRewardIndex.push_back( rkLoader.LoadInt( szKey, 0 ) );
	}

	m_dwCampWinRewardIndexB	= rkLoader.LoadInt( "camp_blue_win_table",  0 );
	m_dwCampLoseRewardIndexB= rkLoader.LoadInt( "camp_blue_lose_table", 0 );
	m_dwCampWinRewardIndexR	= rkLoader.LoadInt( "camp_red_win_table",  0 );
	m_dwCampLoseRewardIndexR= rkLoader.LoadInt( "camp_red_lose_table", 0 );
	m_dwCampDrawRewardIndex	= rkLoader.LoadInt( "camp_draw_table", 0 );

	LOG.PrintTimeAndLog( 0, "[info][tournament]Regular tournament reward start date : [%d]", m_dwTournamentStartDate );
}

DWORD RegularTournamentReward::GetRoundRewardIndex( int iRound )
{
	if( COMPARE( iRound - 1, 0, (int)m_RoundRewardIndex.size() ) )
	{
		return m_RoundRewardIndex[iRound - 1];
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
TournamentManager::TournamentManager()
{
	m_iCampPesoRewardMent = 0;
	m_iCampPesoRewardPeriod = 0;
	m_iCheerPesoRewardMent = 0;
	m_iCheerPesoRewardPeriod = 0;

	m_iCustomRewardPresentMent = 1;
	m_iCustomRewardPresentPeriod = 14;
}

TournamentManager::~TournamentManager()
{
	ClearRegularReward();
	m_CustomRewardEtcItemPriceMap.clear();
}

void TournamentManager::ClearRegularReward()
{
	RegularTournamentRewardVec::iterator iter = m_RegularRewardList.begin();
	for(;iter != m_RegularRewardList.end();++iter)
	{
		RegularTournamentReward *pReward = *iter;
		SAFEDELETE( pReward );
	}
	m_RegularRewardList.clear();

	RegularRewardTableMap::iterator iCreator = m_RegularRewardTableMap.begin();
	for(;iCreator != m_RegularRewardTableMap.end();++iCreator)
	{
		RegularRewardTable &rkReward = iCreator->second;
		rkReward.m_PresentList.clear();
	}
	m_RegularRewardTableMap.clear();
}

TournamentManager& TournamentManager::GetSingleton()
{
	return Singleton< TournamentManager >::GetSingleton();
}

void TournamentManager::LoadINI()
{
	ClearRegularReward();

	// 정규 리그 기간별 보상
	{
		ioINILoader kLoader( "config/regular_tournament_reward.ini" );
		kLoader.SetTitle( "common" );

		char szKey[MAX_PATH] = "";
		char szBuf[MAX_PATH] = "";
		int iMaxLoad = kLoader.LoadInt( "max_load", 0 );
		for(int i = 0;i < iMaxLoad;i++)
		{
			kLoader.SetTitle( "common" );

			sprintf_s( szKey, "load_date_%d", i + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			RegularTournamentReward *pReward = new RegularTournamentReward( szBuf );

			pReward->LoadReward( kLoader );
			m_RegularRewardList.push_back( pReward );
		}
	}

	// 정규 리그 보상 테이블
	{
		ioINILoader kLoader( "config/regular_tournament_reward_table.ini" );
		kLoader.SetTitle( "common" );

		m_iCampPesoRewardMent = kLoader.LoadInt( "camp_peso_reward_ment", 0 );
		m_iCampPesoRewardPeriod = kLoader.LoadInt( "camp_peso_reward_period", 0 );
		m_iCheerPesoRewardMent = kLoader.LoadInt( "cheer_peso_reward_ment", 0 );
		m_iCheerPesoRewardPeriod = kLoader.LoadInt( "cheer_peso_reward_period", 0 );

		//
		char szKey[MAX_PATH] = "";
		char szBuf[MAX_PATH] = "";
		int iMaxTable = kLoader.LoadInt( "max_table", 0 );
		for(int i = 0;i < iMaxTable;i++)
		{
			RegularRewardTable kReward;
			sprintf_s( szKey, "table%d", i + 1 );
			kLoader.SetTitle( szKey );		

			int iMaxPresent = kLoader.LoadInt( "max_present", 0 );
			for(int j = 0;j < iMaxPresent;j++)
			{
				RewardPresent kPresent;
				sprintf_s( szKey, "present%d_type", j + 1 );
				kPresent.m_iPresentType = kLoader.LoadInt( szKey, 0 );
				sprintf_s( szKey, "present%d_ment", j + 1 );
				kPresent.m_iPresentMent = kLoader.LoadInt( szKey, 0 );
				sprintf_s( szKey, "present%d_period", j + 1 );
				kPresent.m_iPresentPeriod = kLoader.LoadInt( szKey, 0 );
				sprintf_s( szKey, "present%d_value1", j + 1 );
				kPresent.m_iPresentValue1 = kLoader.LoadInt( szKey, 0 );
				sprintf_s( szKey, "present%d_value2", j + 1 );
				kPresent.m_iPresentValue2 = kLoader.LoadInt( szKey, 0 );

				kReward.m_PresentList.push_back( kPresent );
			}
			m_RegularRewardTableMap.insert( RegularRewardTableMap::value_type( i + 1, kReward ) );
		}

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][tournament]Reward table size : [%d]", (int)m_RegularRewardTableMap.size() );
	}

	// 유저 리그 보상 특별 아이템 가격
	{
		m_CustomRewardEtcItemPriceMap.clear();
		ioINILoader kLoader( "config/sp2_custom_tournament_reward.ini" );
		kLoader.SetTitle( "common" );
		
		m_iCustomRewardPresentMent   = kLoader.LoadInt( "present_ment", 1 );
		m_iCustomRewardPresentPeriod = kLoader.LoadInt( "present_period", 14 );

		char szBuf[MAX_PATH];
		int iMaxCount = kLoader.LoadInt( "max", 0 );
		for( int i = 1 ; i <= iMaxCount ; ++i )
		{
			wsprintf( szBuf, "rewarditem%d", i );
			kLoader.SetTitle( szBuf );

			DWORD dwEtcItem = kLoader.LoadInt( "etc_item_type", 0 );
			int   iItemPrice= kLoader.LoadInt( "reward_item_pirce", 0 );
			m_CustomRewardEtcItemPriceMap.insert( CustomRewardEtcItemPriceMap::value_type( dwEtcItem, iItemPrice ) );
		}
	}
}

TournamentManager::Tournament &TournamentManager::GetTournament( DWORD dwTourIndex )
{
	TournamentVec::iterator iter = m_TournamentList.begin();
	for(;iter != m_TournamentList.end();iter++)
	{
		Tournament &rkTournamentData = *iter;
		if( rkTournamentData.m_dwIndex == dwTourIndex )
		{
			return rkTournamentData;
		}
	}

	static Tournament kNoneData;
	return kNoneData;
}

TournamentManager::RoundBattleRoomData &TournamentManager::GetTournamentBattleRoomData( DWORD dwTourIndex, DWORD dwServerIndex )
{
	Tournament &rkTournament = GetTournament( dwTourIndex );

	RoundBattleRoomDataMap::iterator iter = rkTournament.m_BattleRoom.find( dwServerIndex );
	if( iter == rkTournament.m_BattleRoom.end() )
	{
		RoundBattleRoomData kRoomData;
		rkTournament.m_BattleRoom.insert( RoundBattleRoomDataMap::value_type( dwServerIndex, kRoomData ) );
	}

	iter = rkTournament.m_BattleRoom.find( dwServerIndex );
	if( iter == rkTournament.m_BattleRoom.end() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "GetTournamentBattleRoomData : %d - %d BattleRoom Create Fail", dwTourIndex, dwServerIndex );
		static RoundBattleRoomData kNoneData;
		return kNoneData;
	}
	
	return iter->second;
}

RegularTournamentReward *TournamentManager::GetRegularTournamentReward( DWORD dwStartDate )
{
	RegularTournamentRewardVec::iterator iter = m_RegularRewardList.begin();
	for(;iter != m_RegularRewardList.end();iter++)
	{
		RegularTournamentReward *pReward = *iter;
		if( !pReward ) continue;

		if( pReward->GetStartDate() == dwStartDate )
			return pReward;
	}
	return NULL;
}

TournamentManager::RegularRewardTable &TournamentManager::GetRegularRewardTable( DWORD dwTable )
{
	RegularRewardTableMap::iterator iter = m_RegularRewardTableMap.find( dwTable );
	if( iter == m_RegularRewardTableMap.end() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "GetRegularRewardTable : %d Fail", dwTable );

		static RegularRewardTable kNoneData;
		return kNoneData;
	}

	return iter->second;
}

void TournamentManager::ApplyTournament( SP2Packet &rkPacket )
{	

	Tournament kTournament;
	rkPacket >> kTournament.m_dwIndex >> kTournament.m_Type >> kTournament.m_State >> kTournament.m_dwStartDate >> kTournament.m_dwEndDate >> kTournament.m_bDisableTournament;
	LOG.PrintTimeAndLog( 0, "[info][tournament]Apply tournament : [%d] [%d] [%d] [%d]", kTournament.m_dwIndex, (int)kTournament.m_Type, (int)kTournament.m_State, kTournament.m_bDisableTournament );

	if( kTournament.m_Type == TYPE_REGULAR )
	{
		rkPacket >> kTournament.m_PrevChampTeamCamp >> kTournament.m_PrevWinChampName;
		LOG.PrintTimeAndLog(0, "[info][tournament]prev_champ_sync : [%s] [%d]", kTournament.m_PrevWinChampName.c_str(), kTournament.m_PrevChampTeamCamp );
	}

	TournamentVec::iterator iter = m_TournamentList.begin();
	for(;iter != m_TournamentList.end();iter++)
	{
		Tournament &rkTournamentData = *iter;
		if( rkTournamentData.m_dwIndex == kTournament.m_dwIndex )
		{
			rkTournamentData = kTournament;
			return;
		}
	}
	m_TournamentList.push_back( kTournament );
}

void TournamentManager::ApplyTournamentEnd( SP2Packet &rkPacket )
{
	BYTE Type;
	DWORD dwTourIndex;
	rkPacket >> dwTourIndex >> Type;
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "TournamentManager::ApplyTournament End : %d - %d", dwTourIndex, (int)Type );
	
	TournamentVec::iterator iter = m_TournamentList.begin();
	for(;iter != m_TournamentList.end();iter++)
	{
		Tournament &rkTournamentData = *iter;
		if( rkTournamentData.m_dwIndex == dwTourIndex )
		{
			m_TournamentList.erase( iter );        // 삭제
			break;
		}
	}
	g_UserNodeManager.AllUserTournamentTeamDelete( dwTourIndex );
}

void TournamentManager::ApplyTournamentRoundStart( SP2Packet &rkPacket )
{
	BYTE State, MaxPlayer;
	DWORD dwTourIndex;
	int iPlayMode, iCreateBattleRoomCount;
	rkPacket >> dwTourIndex >> State >> iPlayMode >> MaxPlayer >> iCreateBattleRoomCount;

	{
		// 생성한 전투방을 메인서버에 전송
		SP2Packet kPacket( MSTPK_TOURNAMENT_ROUND_CREATE_BATTLEROOM );
		kPacket << dwTourIndex << iCreateBattleRoomCount;
		for(int i = 0;i < iCreateBattleRoomCount;i++)
		{
			DWORD dwBlueIndex, dwRedIndex;
			SHORT BluePosition, RedPosition;
			rkPacket >> dwBlueIndex >> BluePosition >> dwRedIndex >> RedPosition;

			BattleRoomNode *pBattleRoom = g_BattleRoomManager.CreateNewBattleRoom();
			if( pBattleRoom == NULL )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Tournament Battle Room Create Failed :  %d - %d - %d - %d", dwTourIndex, iCreateBattleRoomCount, dwBlueIndex, dwRedIndex );
				kPacket << (SHORT)0 << 0;
			}
			else
			{
				pBattleRoom->SetBattleEventType( BET_TOURNAMENT_BATTLE );
				pBattleRoom->SetMaxPlayer( MaxPlayer, MaxPlayer, MAX_BATTLE_OBSERVER );
				pBattleRoom->SetDefaultMode( iPlayMode );
				pBattleRoom->SetTournamentBattle( dwTourIndex, dwBlueIndex, ( State - STATE_TOURNAMENT ) + 1, BluePosition, dwRedIndex, ( State - STATE_TOURNAMENT ) + 1, RedPosition );

				kPacket << BluePosition << pBattleRoom->GetIndex(); 

				if( dwTourIndex == TOURNAMENT_REGULAR_INDEX )
					LOG.PrintTimeAndLog( 0, "[대회로그] %s 방 생성 완료 : %d - %d - %d", __FUNCTION__, pBattleRoom->GetIndex(), dwBlueIndex, dwRedIndex );
			}
		}
		g_MainServer.SendMessage( kPacket );
	}

	if( dwTourIndex == TOURNAMENT_REGULAR_INDEX )
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "[대회로그]  ApplyTournamentRoundStart 2: %d - %d - %d - %d", dwTourIndex, (int)State, iPlayMode, iCreateBattleRoomCount );
}

void TournamentManager::ApplyTournamentBattleRoomInvite( SP2Packet &rkPacket )
{
	DWORD dwTourIndex, dwServerIndex;
	rkPacket >> dwTourIndex >> dwServerIndex;

	RoundBattleRoomData &rkRoomData = GetTournamentBattleRoomData( dwTourIndex, dwServerIndex );
	rkRoomData.m_RoomList.clear();

	if( dwTourIndex == TOURNAMENT_REGULAR_INDEX )
		LOG.PrintTimeAndLog( 0, "[대회로그] %s 대회팀 초대 발송 Recv(서버 : %d)", __FUNCTION__, dwServerIndex );

	int iMaxBattleRoom;
	rkPacket >> iMaxBattleRoom;
	for(int i = 0; i < iMaxBattleRoom; i++ )
	{
		RoundData kRoundData;
		rkPacket >> kRoundData.m_dwBattleRoomIndex >> kRoundData.m_dwBlueIndex >> kRoundData.m_szBlueName >> kRoundData.m_BlueCamp
			                                       >> kRoundData.m_dwRedIndex >> kRoundData.m_szRedName >> kRoundData.m_RedCamp;
		kRoundData.m_dwInviteTimer = TIMEGETTIME();

		rkRoomData.m_RoomList.push_back( kRoundData );

		if( dwTourIndex == TOURNAMENT_REGULAR_INDEX )
		{
			LOG.PrintTimeAndLog( 0, "[대회로그] %s 대회팀 초대 발송 : 배틀룸 : %d, 팀정보 : 블루 : %d(%s), 레드 : %d(%s)", __FUNCTION__, 
				kRoundData.m_dwBattleRoomIndex, kRoundData.m_dwBlueIndex, kRoundData.m_szBlueName.c_str(), kRoundData.m_dwRedIndex, kRoundData.m_szRedName.c_str() );
		}
		
		// 유저리스트에 동일팀이 있으면 전송
		g_UserNodeManager.UserTournamentBattleInvite( kRoundData.m_dwBattleRoomIndex, kRoundData.m_dwBlueIndex, kRoundData.m_dwRedIndex );

		
	}

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ApplyTournamentBattleRoomInvite 2: %d - %d - %d", dwTourIndex, dwServerIndex, iMaxBattleRoom );
}

void TournamentManager::ApplyTournamentBattleTeamChange( SP2Packet &rkPacket )
{
	BYTE CampPos;
	ioHashString szTeamName;
	DWORD dwTourIndex, dwBattleRoomIndex, dwDropTeamIndex, dwTeamIndex;
	rkPacket >> dwTourIndex >> dwBattleRoomIndex >> dwDropTeamIndex >> dwTeamIndex >> szTeamName >> CampPos;

	Tournament &rkTournament = GetTournament( dwTourIndex );

	RoundBattleRoomDataMap::iterator iter = rkTournament.m_BattleRoom.begin();
	for(;iter != rkTournament.m_BattleRoom.end();iter++)
	{
		RoundBattleRoomData &rkBattleRoomList = iter->second;	

		RoundDataVec::iterator iter2 = rkBattleRoomList.m_RoomList.begin();
		for(;iter2 != rkBattleRoomList.m_RoomList.end();iter2++)
		{
			RoundData &rkRoundData = *iter2;
			if( rkRoundData.m_dwBattleRoomIndex == dwBattleRoomIndex )
			{
				rkRoundData.m_dwInviteTimer = TIMEGETTIME();
				if( rkRoundData.m_dwBlueIndex == dwDropTeamIndex )
				{
					rkRoundData.m_dwBlueIndex = dwTeamIndex;
					rkRoundData.m_szBlueName  = szTeamName;
					rkRoundData.m_BlueCamp    = CampPos;
				}
				else if( rkRoundData.m_dwRedIndex == dwDropTeamIndex )
				{
					rkRoundData.m_dwRedIndex = dwTeamIndex;
					rkRoundData.m_szRedName  = szTeamName;
					rkRoundData.m_RedCamp    = CampPos;
				}

				// 동일 서버면 유저 입장 전송
				if( (DWORD)iter->first == g_ServerNodeManager.GetServerIndex() )
				{
					BattleRoomNode *pBattleRoom = g_BattleRoomManager.GetBattleRoomNode( dwBattleRoomIndex );
					if( pBattleRoom )
					{
						pBattleRoom->SetTournamentTeamChange( dwTeamIndex );

						//
						// 유저리스트에 동일팀이 있으면 전송
						g_UserNodeManager.UserTournamentBattleInvite( rkRoundData.m_dwBattleRoomIndex, dwTeamIndex, rkRoundData.m_dwBlueIndex, rkRoundData.m_dwRedIndex );
					}
					else
					{
						//
						LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ApplyTournamentBattleTeamChange : Battle Room Node None : %d", dwBattleRoomIndex );
					}
				}
				return;
			}
		}
	}
}

void TournamentManager::ApplyTournamentPrevChampSync( SP2Packet &rkPacket )
{
	BYTE ChampCampPos;
	DWORD dwTourIdx;
	ioHashString szChampTeamName;

	rkPacket >> dwTourIdx;

	LOG.PrintTimeAndLog(0, "%s", __FUNCTION__ );

	TournamentManager::Tournament& Tour = GetTournament( dwTourIdx );
	if( Tour.m_Type == TYPE_REGULAR )
	{
		rkPacket >> ChampCampPos;
		rkPacket >> szChampTeamName;

		Tour.m_PrevChampTeamCamp = ChampCampPos;
		Tour.m_PrevWinChampName  = szChampTeamName;

		LOG.PrintTimeAndLog(0 ,"%s - prev_champ_info sync success", __FUNCTION__ );
	}
	else
	{
		LOG.PrintTimeAndLog(0 ,"%s - custom tournament try sync for prev_champ_info", __FUNCTION__ );
	}
}

DWORD TournamentManager::GetRegularTournamentIndex()
{
	TournamentVec::iterator iter = m_TournamentList.begin();
	for(;iter != m_TournamentList.end();iter++)
	{
		Tournament &rkTournamentData = *iter;
		if( rkTournamentData.m_Type == TYPE_REGULAR )
			return rkTournamentData.m_dwIndex;
	}
	return 0;
}

int TournamentManager::GetRegularTournamentBattleRewardPeso( DWORD dwTourIndex )
{
	TournamentVec::iterator iter = m_TournamentList.begin();
	for(;iter != m_TournamentList.end();iter++)
	{
		Tournament &rkTournamentData = *iter;
		if( rkTournamentData.m_dwIndex != dwTourIndex ) continue;

		if( rkTournamentData.m_Type == TYPE_REGULAR )
		{
			RegularTournamentReward *pRewardData = GetRegularTournamentReward( rkTournamentData.m_dwStartDate );
			if( pRewardData )
			{
				return pRewardData->GetBattleRewardPeso();
			}			
		}
	}
	return 0;
}

bool TournamentManager::IsRegularTournamentTeamEntryAgreeState()
{
	TournamentVec::iterator iter = m_TournamentList.begin();
	for(;iter != m_TournamentList.end();iter++)
	{
		Tournament &rkTournamentData = *iter;
		if( rkTournamentData.m_Type == TYPE_REGULAR )
		{
			if( rkTournamentData.m_State == STATE_TEAM_APP )
				return true;
		}
	}
	return false;
}

bool TournamentManager::IsTournamentTeamEntryAgreeState( DWORD dwTourIndex )
{
	TournamentVec::iterator iter = m_TournamentList.begin();
	for(;iter != m_TournamentList.end();iter++)
	{
		Tournament &rkTournamentData = *iter;
		if( rkTournamentData.m_dwIndex == dwTourIndex )
		{
			if( rkTournamentData.m_State == STATE_TEAM_APP )
				return true;
		}
	}
	return false;
}

bool TournamentManager::IsRegularTournament( DWORD dwTourIndex )
{
	TournamentVec::iterator iter = m_TournamentList.begin();
	for(;iter != m_TournamentList.end();iter++)
	{
		Tournament &rkTournamentData = *iter;
		if( rkTournamentData.m_dwIndex == dwTourIndex )
		{
			if( rkTournamentData.m_Type == TYPE_REGULAR )
				return true;
		}
	}
	return false;
}

void TournamentManager::SendTournamentRoomList( User *pUser, DWORD dwTourIndex, int iCurPage, int iMaxCount )
{
	if( pUser == NULL ) return;

	Tournament &rkTournament = GetTournament( dwTourIndex );
	if( rkTournament.m_dwIndex != dwTourIndex ) return;

	RoundDataVec kRoomList;
	RoundBattleRoomDataMap::iterator iter = rkTournament.m_BattleRoom.begin();
	for(;iter != rkTournament.m_BattleRoom.end();iter++)
	{
		RoundBattleRoomData &rkBattleRoomList = iter->second;	

		RoundDataVec::iterator iter2 = rkBattleRoomList.m_RoomList.begin();
		for(;iter2 != rkBattleRoomList.m_RoomList.end();iter2++)
		{
			RoundData &rkRoomData = *iter2;
			if( g_BattleRoomManager.GetGlobalBattleRoomNode( rkRoomData.m_dwBattleRoomIndex ) )
				kRoomList.push_back( rkRoomData );
		}
	}
	
	int iCurrentRound = max( -1, ( rkTournament.m_State - STATE_TOURNAMENT ) - 1 );

	int iMaxList = kRoomList.size();
	if( iMaxList == 0 )
	{
		SP2Packet kPacket( STPK_TOURNAMENT_ROOM_LIST );
		//kPacket << 0 << 0 << iCurrentRound << 0;
		PACKET_GUARD_VOID( kPacket.Write( 0 ) );
		PACKET_GUARD_VOID( kPacket.Write( 0 ) );
		PACKET_GUARD_VOID( kPacket.Write( iCurrentRound ) );
		PACKET_GUARD_VOID( kPacket.Write( 0 ) );
		pUser->SendMessage( kPacket );
		return;
	}

	int iStartPos = iCurPage * iMaxCount;
	int iCurSize  = max( 0, min( iMaxList - iStartPos, iMaxCount ) );

	SP2Packet kPacket( STPK_TOURNAMENT_ROOM_LIST );
	kPacket << iCurPage << ( ( iMaxList - 1 ) / iMaxCount ) << iCurrentRound << iCurSize;
	for(int i = iStartPos; i < iStartPos + iCurSize;i++)
	{
		if( i < iMaxList )
		{
			RoundData &rkRoomData = kRoomList[i];
			//kPacket << rkRoomData.m_dwBattleRoomIndex << rkRoomData.m_dwBlueIndex << rkRoomData.m_szBlueName << rkRoomData.m_BlueCamp << rkRoomData.m_dwRedIndex << rkRoomData.m_szRedName << rkRoomData.m_RedCamp;
			PACKET_GUARD_VOID( kPacket.Write( rkRoomData.m_dwBattleRoomIndex ) );
			PACKET_GUARD_VOID( kPacket.Write( rkRoomData.m_dwBlueIndex ) );
			PACKET_GUARD_VOID( kPacket.Write( rkRoomData.m_szBlueName ) );
			PACKET_GUARD_VOID( kPacket.Write( rkRoomData.m_BlueCamp ) );
			PACKET_GUARD_VOID( kPacket.Write( rkRoomData.m_dwRedIndex ) );
			PACKET_GUARD_VOID( kPacket.Write( rkRoomData.m_szRedName ) );
			PACKET_GUARD_VOID( kPacket.Write( rkRoomData.m_RedCamp ) );
		}		
		else    //예외
		{
			//kPacket << 0 << 0 << "" << 0 << 0 << "" << 0;
			PACKET_GUARD_VOID( kPacket.Write(0) );
			PACKET_GUARD_VOID( kPacket.Write(0) );
			PACKET_GUARD_VOID( kPacket.Write("") );
			PACKET_GUARD_VOID( kPacket.Write(0) );
			PACKET_GUARD_VOID( kPacket.Write(0) );
			PACKET_GUARD_VOID( kPacket.Write("") );
			PACKET_GUARD_VOID( kPacket.Write(0) );

		}
	}
	pUser->SendMessage( kPacket );
	kRoomList.clear();
}

void TournamentManager::CheckTournamentBattleInvite( User *pUser, DWORD dwTourIndex, DWORD dwTeamIndex )
{
	if( pUser == NULL ) return;

	Tournament &rkTournament = GetTournament( dwTourIndex );
	if( rkTournament.m_dwIndex != dwTourIndex ) return;

	// 입장 체크하여 초대 발송
	RoundBattleRoomDataMap::iterator iter = rkTournament.m_BattleRoom.begin();
	for(;iter != rkTournament.m_BattleRoom.end();iter++)
	{
		RoundBattleRoomData &rkBattleRoomList = iter->second;	

		RoundDataVec::iterator iter2 = rkBattleRoomList.m_RoomList.begin();
		for(;iter2 != rkBattleRoomList.m_RoomList.end();iter2++)
		{
			RoundData &rkRoundData = *iter2;
			
			if( rkRoundData.m_dwBlueIndex == dwTeamIndex || 
				rkRoundData.m_dwRedIndex == dwTeamIndex )
			{

				DWORD dwGapTime = TIMEGETTIME() - rkRoundData.m_dwInviteTimer;
				if( dwGapTime >= TournamentRoundData::INVITE_DELAY_TIME ) continue;

				//
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "CheckTournamentBattleInvite %s : %d - %d - %d", pUser->GetPublicID().c_str(), 
									rkRoundData.m_dwBattleRoomIndex, rkRoundData.m_dwBlueIndex, rkRoundData.m_dwRedIndex );
				
				// 유저리스트에 동일팀이 있으면 전송
				SP2Packet kPacket( STPK_TOURNAMENT_BATTLE_INVITE );
				PACKET_GUARD_VOID( kPacket.Write( rkRoundData.m_dwBattleRoomIndex) );
				PACKET_GUARD_VOID( kPacket.Write( rkRoundData.m_dwBlueIndex) );
				PACKET_GUARD_VOID( kPacket.Write( rkRoundData.m_dwRedIndex) );
				PACKET_GUARD_VOID( kPacket.Write( ( TournamentRoundData::INVITE_DELAY_TIME - dwGapTime ) ) );
				pUser->SendMessage( kPacket );
			}
		}
	}
}

void TournamentManager::InsertRegularTournamentReward( User *pUser, DWORD dwTableIndex, DWORD dwStartDate, BYTE TourPos, int iMyCampPos, int iWinCampPos, int iLadderBonusPeso, int iLadderRank, int iLadderPoint )
{
	if( pUser == NULL ) return;
	if( dwTableIndex == 0 ) return;

	RegularTournamentReward *pRewardData = GetRegularTournamentReward( dwStartDate );
	if( pRewardData )
	{
		// 
		bool bPresentSend = false;

		// 대회 라운드 보상 : 선물 
		if( TourPos > 0 )       // 0이면 예선 탈락
		{
			RegularRewardTable kReward = GetRegularRewardTable( pRewardData->GetRoundRewardIndex( TourPos ) );
			if( !kReward.m_PresentList.empty() )
			{
				RewardPresentVec::iterator iter = kReward.m_PresentList.begin();
				for(;iter != kReward.m_PresentList.end();iter++)
				{
					RewardPresent &rkPresent = *iter;
					
					// 선물 Insert
					CTimeSpan cPresentGapTime( rkPresent.m_iPresentPeriod, 0, 0, 0 );
					CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;

					pUser->AddPresentMemory( g_MainServer.GetSendID(), rkPresent.m_iPresentType, rkPresent.m_iPresentValue1, rkPresent.m_iPresentValue2, 0, 0, rkPresent.m_iPresentMent, kPresentTime, 0 );

					char szNote[MAX_PATH]="";
					StringCbPrintf( szNote, sizeof( szNote ) , "LeagueReward:%d-%d", dwStartDate, (int)TourPos );
					g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), rkPresent.m_iPresentType, rkPresent.m_iPresentValue1, rkPresent.m_iPresentValue2, 0, 0, LogDBClient::PST_RECIEVE, szNote );					
				}
				bPresentSend = true;
			}
		}

		// 진영 보상
		if( iMyCampPos != CAMP_NONE )
		{
			// 진영 보상 : 페소
			if( iLadderBonusPeso > 0 )
			{
				CTimeSpan cPresentGapTime( m_iCampPesoRewardPeriod, 0, 0, 0 );
				CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;

				pUser->AddPresentMemory( g_MainServer.GetSendID(), PRESENT_PESO, iLadderBonusPeso, 0, 0, 0, m_iCampPesoRewardMent, kPresentTime, 0 );

				char szNote[MAX_PATH]="";
				StringCbPrintf( szNote, sizeof( szNote ) , "LeagueCampPesoReward:%d-%d", dwStartDate, iLadderBonusPeso );
				g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), PRESENT_PESO, iLadderBonusPeso, 0, 0, 0, LogDBClient::PST_RECIEVE, szNote );					

				bPresentSend = true;
			}

			// 진영 보상 : 선물
			{
				// 승리한 진영에 따라서 선물 지급
				DWORD dwRewardIndex = 0;
				if( iWinCampPos == CAMP_NONE ) //무승부
				{
					dwRewardIndex = pRewardData->GetCampDrawRewardIndex();
				}
				else if( iMyCampPos == iWinCampPos )  // 승리
				{
					if( iMyCampPos == CAMP_BLUE )
						dwRewardIndex = pRewardData->GetCampWinRewardIndexB();
					else
						dwRewardIndex = pRewardData->GetCampWinRewardIndexR();
				}
				else // 패배
				{
					if( iMyCampPos == CAMP_BLUE )
						dwRewardIndex = pRewardData->GetCampLoseRewardIndexB();
					else
						dwRewardIndex = pRewardData->GetCampLoseRewardIndexR();
				}

				if( dwRewardIndex != 0 )
				{
					RegularRewardTable kReward = GetRegularRewardTable( dwRewardIndex );
					if( !kReward.m_PresentList.empty() )
					{
						RewardPresentVec::iterator iter = kReward.m_PresentList.begin();
						for(;iter != kReward.m_PresentList.end();iter++)
						{
							RewardPresent &rkPresent = *iter;

							// 선물 Insert
							CTimeSpan cPresentGapTime( rkPresent.m_iPresentPeriod, 0, 0, 0 );
							CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;

							pUser->AddPresentMemory( g_MainServer.GetSendID(), rkPresent.m_iPresentType, rkPresent.m_iPresentValue1, rkPresent.m_iPresentValue2, 0, 0, rkPresent.m_iPresentMent, kPresentTime, 0 );

							char szNote[MAX_PATH]="";
							StringCbPrintf( szNote, sizeof( szNote ) , "LeagueCampRewardPresent:%d-%d", dwStartDate, dwRewardIndex );
							g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), rkPresent.m_iPresentType, rkPresent.m_iPresentValue1, rkPresent.m_iPresentValue2, 0, 0, LogDBClient::PST_RECIEVE, szNote );					
						}
						bPresentSend = true;
					}
				}
			}
		}

		if( bPresentSend )
		{
			pUser->SendPresentMemory();
		}

		//보상정보는 응원보상을를 받은 후에 발송
		//SP2Packet kPacket( STPK_TOURNAMENT_REWARD_DATA );
		//kPacket << dwStartDate << TourPos << iMyCampPos << iWinCampPos << iLadderBonusPeso << iLadderRank << iLadderPoint;
		//pUser->SendMessage( kPacket );

	}
	else
	{
		// 보상 없음
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "InsertRegularTournamentReward None Reward: %s - %d - %d - %d - %d - %d - %d", pUser->GetPublicID().c_str(), dwTableIndex, dwStartDate, (int)TourPos, iMyCampPos, iWinCampPos, iLadderBonusPeso );
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "InsertRegularTournamentReward : %s - %d - %d - %d - %d - %d - %d - %d - %d", pUser->GetPublicID().c_str(), dwTableIndex, dwStartDate, (int)TourPos, iMyCampPos, iWinCampPos, iLadderBonusPeso, iLadderRank, iLadderPoint );

	// 보상 테이블 삭제
	g_DBClient.OnDeleteTournamentReward( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), dwTableIndex );
}

void TournamentManager::InsertRegularTournamentCheerReward( User *pUser, DWORD dwTourTableIdx, DWORD dWCheerTableIdx, DWORD dwStartDate, BYTE TourPos, int iMyCampPos, int iWinCampPos, int iLadderBonusPeso, int iLadderRank, int iLadderPoint, DWORD dwCheerPeso )
{
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( 0, "[warning][tournament]User does not exist" );
		return;
	}

	if( dwTourTableIdx == 0 && dWCheerTableIdx == 0 )
	{
		LOG.PrintTimeAndLog( 0, "[info][tournament]Cheer reward does not exist : [%lu]", pUser->GetUserIndex() );
		return;
	}

	if( 0 < dwCheerPeso )
	{
		CTimeSpan cPresentGapTime( m_iCheerPesoRewardPeriod, 0, 0, 0 );
		CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;

		pUser->AddPresentMemory( g_MainServer.GetSendID(), PRESENT_PESO, dwCheerPeso, 0, 0, 0, m_iCheerPesoRewardMent, kPresentTime, 0 );
		pUser->SendPresentMemory();
		char szNote[MAX_PATH]="";
		StringCbPrintf( szNote, sizeof( szNote ) , "LeagueCheerPesoReward:%d-%d", dwCheerPeso );

		g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), PRESENT_PESO, dwCheerPeso, 0, 0, 0, LogDBClient::PST_RECIEVE, szNote );
	}

	BYTE PrevChampCamp;
	ioHashString PrevChampName;
	GetRegularTournamentPrevChampInfo( PrevChampCamp, PrevChampName );
	if( iWinCampPos == CAMP_NONE )
		iWinCampPos = PrevChampCamp;

	//유저 전송
	SP2Packet kPacket( STPK_TOURNAMENT_REWARD_DATA );
	PACKET_GUARD_VOID( kPacket.Write(dwStartDate) );
	PACKET_GUARD_VOID( kPacket.Write(TourPos) );
	PACKET_GUARD_VOID( kPacket.Write(iMyCampPos) );
	PACKET_GUARD_VOID( kPacket.Write(iWinCampPos) );
	PACKET_GUARD_VOID( kPacket.Write(iLadderBonusPeso) );
	PACKET_GUARD_VOID( kPacket.Write(iLadderRank) );
	PACKET_GUARD_VOID( kPacket.Write(iLadderPoint) );
	PACKET_GUARD_VOID( kPacket.Write(dwCheerPeso) );
	PACKET_GUARD_VOID( kPacket.Write(PrevChampName) );
	pUser->SendMessage( kPacket );

	
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][tournament]Reward info : [%lu] [%d] [%s] [%d] [%d] [%d] [%d] [%d] [%d] [%d]", pUser->GetUserIndex(), dwCheerPeso, PrevChampName.c_str(), dwStartDate, (int)TourPos, iMyCampPos, iWinCampPos, iLadderBonusPeso, iLadderRank, iLadderPoint );
	g_DBClient.OnDeleteTournamentCheerReward( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), dWCheerTableIdx );
}

bool TournamentManager::_InsertCustomRewardPresent( User *pUser, const ioHashString &rkNickName, DWORD dwEtcItem, int iCurrentRound )
{
	if( pUser == NULL ) return false;
	if( dwEtcItem == 0 ) return false;
	
	bool bSendPrizeItem = false;
	if( COMPARE( dwEtcItem, ioEtcItem::EIT_ETC_PRIZE_ITEM1, ioEtcItem::EIT_ETC_PRIZE_ITEM200 + 1 ) )
	{
		ioEtcItemPrizeItem *pPrizeItem = static_cast< ioEtcItemPrizeItem * >( g_EtcItemMgr.FindEtcItem( dwEtcItem ) );
		if( pPrizeItem )
		{
			bSendPrizeItem = pPrizeItem->_OnPrizeSend( pUser );
		}
	}

	if( !bSendPrizeItem )
	{ 
		// 예외로 받지 못하는 경우 즉시 선물로 지급
		CTimeSpan cPresentGapTime( m_iCustomRewardPresentPeriod, 0, 0, 0 );
		CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;

		pUser->AddPresentMemory( rkNickName, PRESENT_ETC_ITEM, dwEtcItem, 1, 0, 0, m_iCustomRewardPresentMent, kPresentTime, 0 );

		char szNote[MAX_PATH]="";
		StringCbPrintf( szNote, sizeof( szNote ) , "LeagueCustomReward - %d", iCurrentRound );
		g_LogDBClient.OnInsertPresent( 0, rkNickName, g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), PRESENT_ETC_ITEM, dwEtcItem, 1, 0, 0, LogDBClient::PST_RECIEVE, szNote );	
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "_InsertCustomRewardPresent : None Prize Item Direct Present : %s - %d - %d", pUser->GetPublicID().c_str(), dwEtcItem, iCurrentRound );
	}
	return true;
}

void TournamentManager::InsertCustomTournamentReward( User *pUser, DWORD dwTableIndex, const ioHashString &rkNickName, const ioHashString &rkTourName, int iCurrentRound, short MaxRound, DWORD dwReward1, DWORD dwReward2, DWORD dwReward3, DWORD dwReward4 )
{
	if( pUser == NULL ) return;
	if( dwTableIndex == 0 ) return;

	bool bPresentSend = false;
	if( _InsertCustomRewardPresent( pUser, rkNickName, dwReward1, iCurrentRound ) )
		bPresentSend = true;
	if( _InsertCustomRewardPresent( pUser, rkNickName, dwReward2, iCurrentRound ) )
		bPresentSend = true;
	if( _InsertCustomRewardPresent( pUser, rkNickName, dwReward3, iCurrentRound ) )
		bPresentSend = true;
	if( _InsertCustomRewardPresent( pUser, rkNickName, dwReward4, iCurrentRound ) )
		bPresentSend = true;
	if( bPresentSend )
	{
		pUser->SendPresentMemory();
	}

	g_DBClient.OnDeleteTournamentCustomReward( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), dwTableIndex );
	
	// 유저 전송
	SP2Packet kPacket( STPK_TOURNAMENT_CUSTOM_REWARD_DATA );
	PACKET_GUARD_VOID( kPacket.Write(rkTourName) );
	PACKET_GUARD_VOID( kPacket.Write(iCurrentRound) );
	PACKET_GUARD_VOID( kPacket.Write(MaxRound) );
	PACKET_GUARD_VOID( kPacket.Write(dwReward1) ); 
	PACKET_GUARD_VOID( kPacket.Write(dwReward2) );
	PACKET_GUARD_VOID( kPacket.Write(dwReward3) );
	PACKET_GUARD_VOID( kPacket.Write(dwReward4) );
	pUser->SendMessage( kPacket );


	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "InsertCustomTournamentReward : %s - %s - %d - %d - %d - %d - %d - %d - %d", pUser->GetPublicID().c_str(), rkTourName.c_str(), dwTableIndex, iCurrentRound, (int)MaxRound, dwReward1, dwReward2, dwReward3, dwReward4 );
}

int TournamentManager::GetCustomRewardEtcItemPrice( DWORD dwEtcItem )
{
	CustomRewardEtcItemPriceMap::iterator iter = m_CustomRewardEtcItemPriceMap.find( dwEtcItem );
	if( iter != m_CustomRewardEtcItemPriceMap.end() )
		return iter->second;
	return -1;           // 없는 아이템
}

BYTE TournamentManager::GetTournamentState( DWORD dwTourIdx )
{
	TournamentManager::Tournament& Tour = GetTournament( dwTourIdx );
	return Tour.m_State;
}

DWORD TournamentManager::GetTournamentStartDate( DWORD dwTourIdx )
{
	TournamentManager::Tournament& Tour = GetTournament( dwTourIdx );
	return Tour.m_dwStartDate;
}

void TournamentManager::GetRegularTournamentPrevChampInfo( BYTE& PrevChampCamp, ioHashString& PrevChampName )
{
	TournamentVec::iterator iter = m_TournamentList.begin();
	for( ; iter != m_TournamentList.end(); ++iter )
	{
		Tournament &rkTournamentData = *iter;
		if( rkTournamentData.m_Type == TYPE_REGULAR )
		{
			PrevChampCamp = rkTournamentData.m_PrevChampTeamCamp;
			PrevChampName = rkTournamentData.m_PrevWinChampName;
			return;
		}			
	}
}