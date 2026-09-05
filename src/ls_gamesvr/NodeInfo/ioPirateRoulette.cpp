
#include "stdafx.h"
#include "ioPirateRoulette.h"

#include "../EtcHelpFunc.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "ioPirateRouletteManager.h"
#include "../DataBase/LogDBClient.h"
#include "../MainProcess.h"
#include "UserNodeManager.h"
#include "../MainServerNode/MainServerNode.h"

ioPirateRoulette::ioPirateRoulette() : m_bSendPresentState( FALSE )
{
	Init();
	m_bIsFromDB = false;
}

ioPirateRoulette::~ioPirateRoulette()
{
	Destroy();
}

void ioPirateRoulette::Init()
{
	ZeroMemory( m_RouletteBoard, sizeof( m_RouletteBoard ) );
	ZeroMemory( m_present, sizeof(m_present));
//	ZeroMemory( m_Bonus, sizeof(m_Bonus));
	m_iHP = ROULETTE_HP_MAX;
	m_bChangeRoulette		= false;
	m_bSendPresentState		= false;
	m_bChangePresent		= false;
	m_bIsFromDB				= false;
}

void ioPirateRoulette::Destroy()
{
	ZeroMemory( m_RouletteBoard, sizeof( m_RouletteBoard ) );	
	ZeroMemory( m_present, sizeof(m_present));
//	ZeroMemory( m_Bonus, sizeof(m_Bonus));
	m_iHP = ROULETTE_HP_MAX;
}

void ioPirateRoulette::Initialize( User *pUser )
{
	SetUser( pUser );
	Init();
	for( int i = 0; i < ROULETTE_PRESENT_MAX; ++i )
	{
		m_present[i] = 1;
	}
}

bool ioPirateRoulette::DBtoNewIndex( DWORD dwIndex )
{
	return true;
}

void ioPirateRoulette::DBtoData( CQueryResultData *query_data )
{
}

void ioPirateRoulette::DBtoData_RouletteBoard( CQueryResultData *query_data )
{
	int iArrayIdx = 0;
	if (query_data->IsExist())
	{
		query_data->GetValue(m_iHP, sizeof(int));
		for( iArrayIdx = 0; iArrayIdx < ROULETTE_BOARD_MAX; ++iArrayIdx )
		{
			query_data->GetValue( m_RouletteBoard[iArrayIdx], sizeof(BYTE));
		}
		
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[INFO] Roulette Number Save 1~20 : [%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d]", m_RouletteBoard[0], m_RouletteBoard[1], m_RouletteBoard[2], m_RouletteBoard[3], m_RouletteBoard[4], m_RouletteBoard[5], m_RouletteBoard[6], m_RouletteBoard[7], m_RouletteBoard[8], m_RouletteBoard[9], m_RouletteBoard[10], m_RouletteBoard[11], m_RouletteBoard[12], m_RouletteBoard[13], m_RouletteBoard[14], m_RouletteBoard[15], m_RouletteBoard[16], m_RouletteBoard[17], m_RouletteBoard[18], m_RouletteBoard[19]);
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[INFO] Roulette Number Save 21~40: [%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d]", m_RouletteBoard[20], m_RouletteBoard[21], m_RouletteBoard[22], m_RouletteBoard[23], m_RouletteBoard[24], m_RouletteBoard[25], m_RouletteBoard[26], m_RouletteBoard[27], m_RouletteBoard[28], m_RouletteBoard[29], m_RouletteBoard[30], m_RouletteBoard[31], m_RouletteBoard[32], m_RouletteBoard[33], m_RouletteBoard[34], m_RouletteBoard[35], m_RouletteBoard[36], m_RouletteBoard[37], m_RouletteBoard[38], m_RouletteBoard[39]);

		// 없는 상황에 대한 예외처리 부족.. HP 값으로 예외처리.. OTL
		if( m_iHP < 0 )
		{
			m_iHP = 0;
			return;
		}
		else
		{
			SetFromDB(true);

		}
	}
}

void ioPirateRoulette::DBtoData_Present( CQueryResultData *query_data )
{
	int iArrayIdx = 0;
	if( query_data->IsExist())
	{
		for ( iArrayIdx = 0; iArrayIdx < ROULETTE_PRESENT_MAX; ++iArrayIdx )
		{
			query_data->GetValue( m_present[iArrayIdx], sizeof(BYTE));
		}
	}
}

void ioPirateRoulette::SaveData()
{
	// Get : User
	User* pUser = GetUser();
	if( pUser == NULL )
		return;

	// Check : Change Number
	if( GetChangeRoulette() == TRUE )
	{
		BYTE ArrayNumber[ROULETTE_BOARD_MAX] = { 0, };
		GetRouletteBoardData( ArrayNumber );

		g_DBClient.OnUpdatePirateRouletteNumber( pUser->GetUserDBAgentID(),
										pUser->GetAgentThreadID(),
										pUser->GetUserIndex(),
										m_iHP, ArrayNumber );

		SetChangeRoulette( FALSE );
//		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[INFO] Roulette Number Save : HP : %d", m_iHP);
//		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[INFO] Roulette Number Save 1~20 : [%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d]", m_RouletteBoard[0], m_RouletteBoard[1], m_RouletteBoard[2], m_RouletteBoard[3], m_RouletteBoard[4], m_RouletteBoard[5], m_RouletteBoard[6], m_RouletteBoard[7], m_RouletteBoard[8], m_RouletteBoard[9], m_RouletteBoard[10], m_RouletteBoard[11], m_RouletteBoard[12], m_RouletteBoard[13], m_RouletteBoard[14], m_RouletteBoard[15], m_RouletteBoard[16], m_RouletteBoard[17], m_RouletteBoard[18], m_RouletteBoard[19]);
//		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[INFO] Roulette Number Save 21~40: [%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d]", m_RouletteBoard[20], m_RouletteBoard[21], m_RouletteBoard[22], m_RouletteBoard[23], m_RouletteBoard[24], m_RouletteBoard[25], m_RouletteBoard[26], m_RouletteBoard[27], m_RouletteBoard[28], m_RouletteBoard[29], m_RouletteBoard[30], m_RouletteBoard[31], m_RouletteBoard[32], m_RouletteBoard[33], m_RouletteBoard[34], m_RouletteBoard[35], m_RouletteBoard[36], m_RouletteBoard[37], m_RouletteBoard[38], m_RouletteBoard[39]);
//		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[INFO] Roulette Number Save 1~20 : [%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d]", ArrayNumber[0], ArrayNumber[1], ArrayNumber[2], ArrayNumber[3], ArrayNumber[4], ArrayNumber[5], ArrayNumber[6], ArrayNumber[7], ArrayNumber[8], ArrayNumber[9], ArrayNumber[10], ArrayNumber[11], ArrayNumber[12], ArrayNumber[13], ArrayNumber[14], ArrayNumber[15], ArrayNumber[16], ArrayNumber[17], ArrayNumber[18], ArrayNumber[19]);
//		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[INFO] Roulette Number Save 21~40: [%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d]", ArrayNumber[20], ArrayNumber[21], ArrayNumber[22], ArrayNumber[23], ArrayNumber[24], ArrayNumber[25], ArrayNumber[26], ArrayNumber[27], ArrayNumber[28], ArrayNumber[29], ArrayNumber[30], ArrayNumber[31], ArrayNumber[32], ArrayNumber[33], ArrayNumber[34], ArrayNumber[35], ArrayNumber[36], ArrayNumber[37], ArrayNumber[38], ArrayNumber[39]);
	}

	// Check : Reward
	if( GetChangePresent() == TRUE )
	{
		BYTE ArrayPresent[ROULETTE_PRESENT_MAX] = {0,};
		GetPresentData( ArrayPresent );
		g_DBClient.OnUpdatePirateRoulettePresent( pUser->GetUserDBAgentID(),
												pUser->GetAgentThreadID(),
												pUser->GetUserIndex(),
												ArrayPresent );
		SetChangePresent( FALSE );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[INFO] Roulette Reward Save : HP : %d", m_iHP);
	}
}

void ioPirateRoulette::FillMoveData( SP2Packet &rkPacket )
{
	// 숫자
	rkPacket << m_iHP;
	for( int i = 0 ; i < ROULETTE_BOARD_MAX ; ++i )
	{
		rkPacket << m_RouletteBoard[i];
	}

	for( int j = 0; j < ROULETTE_PRESENT_MAX; ++j )
	{
		rkPacket << m_present[j];
	}
}

void ioPirateRoulette::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode /*= false*/ )
{
	// 숫자
	rkPacket >> m_iHP;
	for( int i = 0 ; i < ROULETTE_BOARD_MAX ; ++i )
	{
		rkPacket >> m_RouletteBoard[i];
	}

	for( int j = 0; j < ROULETTE_PRESENT_MAX; ++j )
	{
		rkPacket >> m_present[j];
	}
	SetFromDB(true);
}

void ioPirateRoulette::UseSword(int iRouletteBoardPosition, int iSwordType, SP2Packet& rkPacket)
{
	if( iRouletteBoardPosition < 0 || iRouletteBoardPosition >= ROULETTE_BOARD_MAX )
	{
		rkPacket << m_iHP << 0 << 0 << (ROULETTE_BOARD_MAX + 1);
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[ERROR] Roulette index over Board Max, index : %d", iRouletteBoardPosition );
		return;
	}

	if( m_RouletteBoard[iRouletteBoardPosition] != 0 )
	{
		rkPacket << m_iHP << 0 << 0 << (ROULETTE_BOARD_MAX + 1);
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[Error] Requested Already Used Position [%d]", iRouletteBoardPosition );
		return;	
	}

	if( m_iHP == 0 )
	{
		rkPacket << m_iHP << 0 << (ROULETTE_BOARD_MAX + 1);
	}

	int iDamage = 0;
	int iCriticalType = 0;
	CheckCriticalType( iSwordType, iCriticalType, iDamage );
	if( m_iHP - iDamage <= 0 )
	{
		m_iHP = 0;
	}
	else
	{
		m_iHP -= iDamage;
	}

	m_RouletteBoard[iRouletteBoardPosition] = iSwordType;
	rkPacket << m_iHP << iCriticalType << iSwordType << iRouletteBoardPosition;

	// Give Reward.
	stRouletteRewardData rewardData;
	rewardData = g_PirateRouletteMgr.GetRouletteRewardInfoByHP( m_iHP );
	if( rewardData.type )
	{
		int iRewardIndex = rewardData.index - 1;
		if( m_present[iRewardIndex] == 0 )
		{
			stRouletteRewardData missedReward;
			for( int rewardIndex = 0; rewardIndex < iRewardIndex; ++rewardIndex)
			{
				if( m_present[ rewardIndex ] == 0 )
				{
					missedReward = g_PirateRouletteMgr.GetRouletteRewardInfo( rewardIndex + 1 );
					SendPresent(missedReward);
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[Info] Send Missed Reward. type[%d],[%d][%d]", missedReward.type, missedReward.value1, missedReward.value2 );
					m_present[rewardIndex] = 1;
				}
			}
			// Send Present
			SendPresent( rewardData );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[Info] Send Reward. type[%d],[%d][%d]", rewardData.type, rewardData.value1, rewardData.value2 );
			// Set Flag

			m_present[ iRewardIndex ] = 1;

			// 선물 변경 정보
			SetChangePresent( TRUE );
		}

	}

	if( m_iHP == 0 )
	{
		int UseCount = 0;

		for( int i = 0; i < ROULETTE_BOARD_MAX; ++i )
		{
			if( m_RouletteBoard[i] )
			{
				UseCount++;
			}
		}

		stRouletteRewardData bonusData = g_PirateRouletteMgr.GetBonusRewardPresentInfoByCount(UseCount);
		if( bonusData.type )
		{
			SendBonus( bonusData );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[Info] Send Reward. type[%d],[%d][%d]", rewardData.type, rewardData.value1, rewardData.value2 );

			if( UseCount <= g_PirateRouletteMgr.GetNoticeSwordUseCount() )
			{
				// 전체 유저에게 공지
				if( GetUser() != NULL )
				{
					SP2Packet kPacket( SUPK_SERVER_ALARM_MENT );
					kPacket << UDP_SERVER_ALARM_OAK_PRESENT << GetUser()->GetPublicID() << UseCount << bonusData.type << bonusData.value1 << bonusData.value2;
					g_UserNodeManager.SendAllServerAlarmMent( kPacket );
				}
			}

		}
	}

	SetChangeRoulette(TRUE);
	SaveData();
}

void ioPirateRoulette::InitAll()
{
	// Member Init
	Init();
	SetFromDB(true);
	//InitRouletteBoard();
}

void ioPirateRoulette::GetRouletteBoardData( BYTE *pArray)
{
	for( int i = 0 ; i < ROULETTE_BOARD_MAX ; ++i )
	{
		pArray[i] = static_cast<BYTE>(m_RouletteBoard[i]);
	}
//	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[INFO] Roulette Number Save 1~20 : [%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d]", m_RouletteBoard[0], m_RouletteBoard[1], m_RouletteBoard[2], m_RouletteBoard[3], m_RouletteBoard[4], m_RouletteBoard[5], m_RouletteBoard[6], m_RouletteBoard[7], m_RouletteBoard[8], m_RouletteBoard[9], m_RouletteBoard[10], m_RouletteBoard[11], m_RouletteBoard[12], m_RouletteBoard[13], m_RouletteBoard[14], m_RouletteBoard[15], m_RouletteBoard[16], m_RouletteBoard[17], m_RouletteBoard[18], m_RouletteBoard[19]);
//	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[INFO] Roulette Number Save 21~40: [%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d]", m_RouletteBoard[20], m_RouletteBoard[21], m_RouletteBoard[22], m_RouletteBoard[23], m_RouletteBoard[24], m_RouletteBoard[25], m_RouletteBoard[26], m_RouletteBoard[27], m_RouletteBoard[28], m_RouletteBoard[29], m_RouletteBoard[30], m_RouletteBoard[31], m_RouletteBoard[32], m_RouletteBoard[33], m_RouletteBoard[34], m_RouletteBoard[35], m_RouletteBoard[36], m_RouletteBoard[37], m_RouletteBoard[38], m_RouletteBoard[39]);
}

void ioPirateRoulette::GetRouletteBoardData( SP2Packet &rkPacket )
{
	rkPacket << m_iHP;
	rkPacket << ROULETTE_BOARD_MAX;
	for( int i = 0; i < ROULETTE_BOARD_MAX; ++i )
	{
		rkPacket << m_RouletteBoard[i];
	}
//	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[INFO] Roulette Number Save 1~20 : [%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d]", m_RouletteBoard[0], m_RouletteBoard[1], m_RouletteBoard[2], m_RouletteBoard[3], m_RouletteBoard[4], m_RouletteBoard[5], m_RouletteBoard[6], m_RouletteBoard[7], m_RouletteBoard[8], m_RouletteBoard[9], m_RouletteBoard[10], m_RouletteBoard[11], m_RouletteBoard[12], m_RouletteBoard[13], m_RouletteBoard[14], m_RouletteBoard[15], m_RouletteBoard[16], m_RouletteBoard[17], m_RouletteBoard[18], m_RouletteBoard[19]);
//	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[INFO] Roulette Number Save 21~40: [%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d]", m_RouletteBoard[20], m_RouletteBoard[21], m_RouletteBoard[22], m_RouletteBoard[23], m_RouletteBoard[24], m_RouletteBoard[25], m_RouletteBoard[26], m_RouletteBoard[27], m_RouletteBoard[28], m_RouletteBoard[29], m_RouletteBoard[30], m_RouletteBoard[31], m_RouletteBoard[32], m_RouletteBoard[33], m_RouletteBoard[34], m_RouletteBoard[35], m_RouletteBoard[36], m_RouletteBoard[37], m_RouletteBoard[38], m_RouletteBoard[39]);
}

void ioPirateRoulette::GetPresentData( BYTE* pArray )
{
	for( int i = 0; i < ROULETTE_PRESENT_MAX; ++i )
	{
		pArray[i] =  static_cast<BYTE>(m_present[i]);
	}
}

void ioPirateRoulette::CheckCriticalType(int iSwordType, int& iCritical, int& iDamage)
{
	int iCriticalType = 0;
	stRouletteSwordData swordData = g_PirateRouletteMgr.GetSwordInfo(iSwordType);

	int iRandomTotal = swordData.iSwordDamage1_rand + swordData.iSwordDamage2_rand + swordData.iSwordDamage3_rand;
	int iSelect = rand()%iRandomTotal;

	if( iSelect <= swordData.iSwordDamage1_rand )
	{
		// Damage1
		iDamage = swordData.iSwordDamage1;
		iCritical = 0;
	}
	else if( iSelect <= ( swordData.iSwordDamage1_rand + swordData.iSwordDamage2_rand ) )
	{
		// Damage2
		iDamage = swordData.iSwordDamage2;
		iCritical = 1;
	}
	else
	{
		// Damage3
		iDamage = swordData.iSwordDamage3;
		iCritical = 2;
	}
}


//void ioPirateRoulette::CheckPresent( const int index, bool isEnd )
//{
//	//
//	if( index >= ROULETTE_PRESENT_MAX || index < 0)
//	{
//		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[ERROR] Rullet presentInfo.index over Rullet Board Max, index : %d", m_present[ index ] );
//		return;
//	}
//
//	// Check : 이미 선물 지급되었는지..
//	if( m_present[ index ] > 0 )
//		return;
//
//	// Declare)
//	stRouletteRewardData	presentInfo;
//
//	// index로 실제 선물 정보를 얻어와야함.
//	// 선물
//	presentInfo = g_PirateRouletteMgr.GetRouletteRewardInfo( m_present[index] );
//
//	// Check
//	if( presentInfo.index == 0 )
//	{
//		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[ERROR] Pirate Roulette presentInfo.index == 0, index : %d", m_present[index] );
//		return;
//	}
//
//	// Send Present
//	SendPresent( presentInfo );
//
//	// Set Flag
//	m_present[ index ] = 1;
//
//	// 선물 변경 정보
//	SetChangePresent( TRUE );
//}

void ioPirateRoulette::SendPresent( stRouletteRewardData& presentInfo )
{
	// Get User
	User* pUser = GetUser();
	if( pUser == NULL )
		return;

	// Check : 이미 선물 지급되었는지..
	if( m_present[presentInfo.index] > 0 )
		return;

	// 선물지급.
	CTimeSpan cPresentGapTime( g_PirateRouletteMgr.GetPeriod(), 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;

	// To Memory
	pUser->AddPresentMemory( g_MainServer.GetSendID(), presentInfo.type, presentInfo.value1,
		presentInfo.value2, 0, 0, g_PirateRouletteMgr.GetMent(), kPresentTime, g_PirateRouletteMgr.GetState() );
			
	// LogDB
	g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), presentInfo.type,
		presentInfo.value1, presentInfo.value2, 0, 0, LogDBClient::PST_RECIEVE, "Roulette" );

	// Set Flag
//	SetSendPresentState( TRUE );

	pUser->SendPresentMemory();

	// 다시 FALSE로..
//	SetSendPresentState( FALSE );
}

void ioPirateRoulette::SendBonus( stRouletteRewardData& bonusInfo )
{
	// Get User
	User* pUser = GetUser();
	if( pUser == NULL )
		return;

	// 선물지급.
	CTimeSpan cPresentGapTime( g_PirateRouletteMgr.GetPeriod(), 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;

	// To Memory
	pUser->AddPresentMemory( g_MainServer.GetSendID(), bonusInfo.type, bonusInfo.value1,
		bonusInfo.value2, 0, 0, g_PirateRouletteMgr.GetMent(), kPresentTime, g_PirateRouletteMgr.GetState() );

	// LogDB
	g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), bonusInfo.type,
		bonusInfo.value1, bonusInfo.value2, 0, 0, LogDBClient::PST_RECIEVE, "Roulette" );

	// Set Flag
	//	SetSendPresentState( TRUE );

	pUser->SendPresentMemory();

	// 다시 FALSE로..
	//	SetSendPresentState( FALSE );
}
//
//void ioPirateRoulette::ResetRouletteAndGetPresent(bool isGiveBonus)
//{
//	int UseCount = 0;
//
//	for( int i = 0; i < ROULETTE_BOARD_MAX; ++i )
//	{
//		if( m_RouletteBoard[i] )
//		{
//			UseCount++;
//		}
//	}
//
//	stRouletteRewardData bonusData = g_PirateRouletteMgr.GetBonusRewardPresentInfoByCount(UseCount);
//	if( bonusData.type )
//	{
//		SendPresent( bonusData );
//		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[Info] Send Reward. type[%d],[%d][%d]", bonusData.type, bonusData.value1, bonusData.value2 );
//	}
//
////	OnRouletteReset( GetUser() );
//}


void ioPirateRoulette::OnRouletteStart( User* pUser )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog(0, "%s- pUser == NULL !!", __FUNCTION__ );
		return;
	}

	int UseCount = 0;

	for( int i = 0; i < ROULETTE_BOARD_MAX; ++i )
	{
		if( m_RouletteBoard[i] )
		{
			UseCount++;
		}
	}

	// Give Reward.
	stRouletteRewardData rewardData;
	rewardData = g_PirateRouletteMgr.GetRouletteRewardInfoByHP( m_iHP );
	if( rewardData.type )
	{
		int iRewardIndex = rewardData.index - 1;
		if( m_present[iRewardIndex] == 0 )
		{
			stRouletteRewardData missedReward;
			for( int rewardIndex = 0; rewardIndex < iRewardIndex; ++rewardIndex)
			{
				if( m_present[ rewardIndex ] == 0 )
				{
					missedReward = g_PirateRouletteMgr.GetRouletteRewardInfo( rewardIndex + 1 );
					SendPresent(missedReward);
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[Info] Send Missed Reward. type[%d],[%d][%d]", missedReward.type, missedReward.value1, missedReward.value2 );
					m_present[rewardIndex] = 1;
				}
			}
			// Send Present
			SendPresent( rewardData );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[Info] Send Reward. type[%d],[%d][%d]", rewardData.type, rewardData.value1, rewardData.value2 );
			// Set Flag

			m_present[ iRewardIndex ] = 1;

			// 선물 변경 정보
			SetChangePresent( TRUE );
		}

	}

	InitAll();

	BYTE ArrayNumber[ROULETTE_BOARD_MAX] = { 0, };
	BYTE ArrayPresent[ROULETTE_PRESENT_MAX] = {0,};
	GetRouletteBoardData( ArrayNumber );
	g_DBClient.OnUpdatePirateRouletteNumber( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex(), ROULETTE_HP_MAX, ArrayNumber );
	g_DBClient.OnUpdatePirateRoulettePresent( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex(), ArrayPresent );
//	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[INFO] Roulette Number Save 1~20 : [%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d]", ArrayNumber[0], ArrayNumber[1], ArrayNumber[2], ArrayNumber[3], ArrayNumber[4], ArrayNumber[5], ArrayNumber[6], ArrayNumber[7], ArrayNumber[8], ArrayNumber[9], ArrayNumber[10], ArrayNumber[11], ArrayNumber[12], ArrayNumber[13], ArrayNumber[14], ArrayNumber[15], ArrayNumber[16], ArrayNumber[17], ArrayNumber[18], ArrayNumber[19]);
//	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[INFO] Roulette Number Save 21~40: [%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d],[%d][%d][%d][%d][%d]", ArrayNumber[20], ArrayNumber[21], ArrayNumber[22], ArrayNumber[23], ArrayNumber[24], ArrayNumber[25], ArrayNumber[26], ArrayNumber[27], ArrayNumber[28], ArrayNumber[29], ArrayNumber[30], ArrayNumber[31], ArrayNumber[32], ArrayNumber[33], ArrayNumber[34], ArrayNumber[35], ArrayNumber[36], ArrayNumber[37], ArrayNumber[38], ArrayNumber[39]);

	SP2Packet kPacket( STPK_OAK_RESET );
	PACKET_GUARD_VOID( kPacket.Write( 0 ) );
	pUser->SendMessage( kPacket );
}


//void ioPirateRoulette::OnRouletteInitialize( User* pUser )
//{
//	if( !pUser )
//	{
//		LOG.PrintTimeAndLog(0, "%s- pUser == NULL !!", __FUNCTION__ );
//		return;
//	}
//	
//	Init();
////	InitRouletteBoard();
//	
//	SP2Packet kPacket( STPK_OAK_INFO_REQUEST );
//	PACKET_GUARD_VOID( kPacket.Write( GetHP() ) );
//	PACKET_GUARD_VOID( ROULETTE_BOARD_MAX );
//	FillRouletteData(kPacket);
//	pUser->SendMessage( kPacket );
//}

void ioPirateRoulette::OnRouletteReset( User* pUser )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog(0, "%s- pUser == NULL !!", __FUNCTION__ );
		return;
	}

	InitAll();

	SP2Packet kPacket(  STPK_OAK_RESET );
	PACKET_GUARD_VOID( kPacket.Write( 0 ) );
	pUser->SendMessage( kPacket );
}

void ioPirateRoulette::FillRouletteData( SP2Packet& rkPacket )
{
	for( int i = 0 ; i < ROULETTE_BOARD_MAX; ++i )
	{
		PACKET_GUARD_VOID( rkPacket.Write( m_RouletteBoard[i] ) );
	}
}