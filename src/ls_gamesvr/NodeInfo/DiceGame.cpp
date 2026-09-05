
#include "stdafx.h"
//#include "ioOakBarrel.h"
#include "DiceGame.h"

#include "../EtcHelpFunc.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "DiceGameManager.h"
#include "../DataBase/LogDBClient.h"
#include "../MainProcess.h"
#include "../Util/IORandom.h"
#include "UserNodeManager.h"

DiceGame::DiceGame()
{
	Init();
}

DiceGame::~DiceGame()
{
	Destroy();
}

void DiceGame::Init()
{
	m_bIsProgressDice = false;
	m_iPosition = 0;

	for( int i = 0; i < DICE_GAME_TRACE_DB; ++i )
		m_iTrace[i]	= 0;

	m_byBoradIndex = 0;

	for( int i = 0; i < DICE_GAME_REWARD_DB; ++i )
		m_iRewardIndex[i]	= 0;

	for( int i = 0; i < DICE_GAME_SLOT_COUNT; ++i )
		m_UserTrace[i]	= NON_STEP;

	m_iDiceCountUsed = 0;
}

void DiceGame::Destroy()
{
	m_bIsProgressDice = false;
	m_iPosition = 0;

	for( int i = 0; i < DICE_GAME_TRACE_DB; ++i )
		m_iTrace[i]	= 0;

	m_byBoradIndex = 0;

	for( int i = 0; i < DICE_GAME_REWARD_DB; ++i )
		m_iRewardIndex[i]	= 0;

	for( int i = 0; i < DICE_GAME_SLOT_COUNT; ++i )
		m_UserTrace[i]	= NON_STEP;

	m_iDiceCountUsed = 0;
}

void DiceGame::Initialize( User *pUser )
{
	SetUser( pUser );
	Init();
}

bool DiceGame::DBtoNewIndex( DWORD dwIndex )
{
	return true;
}
void DiceGame::DBtoData( CQueryResultData *query_data )
{
}
void DiceGame::FillMoveData( SP2Packet &rkPacket )
{
}
void DiceGame::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode /*= false*/ )
{
}
///////////////////////////////////////
void DiceGame::SaveData()
{
	User* pUser = GetUser();
	if( pUser == NULL )
		return;

	int ArTempTrace[DICE_GAME_TRACE_DB] = {0,};
	for(int i=0; i<DICE_GAME_SLOT_COUNT; i++)
	{
		int step = i / 16;
		int slot = i % 16; // 0,1,2,3,4,5,6,7 ........,
		double temp22 =  (pow((double)2,(double)(15-slot)));
		ArTempTrace[step] = ArTempTrace[step] + ( m_UserTrace[i] * (int)temp22 );

		if( ArTempTrace[step] == 1)
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[DICE_GAME] SaveData step[%d] temp[%d],m_UserTrace[%d] , [%d]  ",step, ArTempTrace[step],m_UserTrace[i], i);

	}

	this->AllSetDiceGameTrace(ArTempTrace);

	int ArrayTraceInfo[DICE_GAME_TRACE_DB] = {0,};
	this->AllGetDiceGameTrace(ArrayTraceInfo);

	int ArrayRewardIndex[DICE_GAME_REWARD_DB] = {0,};
	this->AllGetRewardIndex(ArrayRewardIndex);

	g_DBClient.OnUpdateDiceGameData( pUser->GetUserDBAgentID()
		, pUser->GetAgentThreadID()
		, pUser->GetUserIndex()
		, this->GetDiceGamePosition()		//		int		m_iPosition;
		, ArrayTraceInfo					//		int		m_iTrace[DICE_GAME_TRACE_DB];
		, this->GetBoradIndex()				//		BYTE	m_byBoradIndex;
		, ArrayRewardIndex					//		int		m_iRewardIndex[DICE_GAME_REWARD_DB];	
		);	
}

void DiceGame::SetDiceGameData( int iPosition, int *pArrayTrace, BYTE byBoradIndex, int *pArrayRewardIndex)
{
	User* pUser = GetUser();
	if( pUser == NULL )
		return;

	// 진행 단계 저장
	SetDiceGamePosition(iPosition);
	for( int i=0; i< DICE_GAME_TRACE_DB ; i++)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[DICE_GAME] SetDiceGameData iTrace[%d] = %d", i,pArrayTrace[i]);
		SetDiceGameTrace(i, pArrayTrace[i]);
	}
	SetBoradIndex(byBoradIndex);
	AllSetRewardIndex(pArrayRewardIndex);	
}

int DiceGame::GetDiceGameTrace( int iStep )
{
	return m_iTrace[iStep];
}

void DiceGame::AllGetDiceGameTrace( int *pArray )
{
	for( int i = 0 ; i < DICE_GAME_TRACE_DB ; ++i )
		pArray[i] = static_cast<int>(m_iTrace[i]);
}
void DiceGame::AllSetDiceGameTrace( int *pArray )
{
	for( int i = 0 ; i < DICE_GAME_TRACE_DB ; ++i )
	{
		//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[DICE_GAME] AllSetDiceGameTrace BEFORE >> m_iTrace[%d][%d]",i,m_iTrace[i]);
		m_iTrace[i] = static_cast<int>(pArray[i]);
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[DICE_GAME] AllSetDiceGameTrace AFTER! >> m_iTrace[%d][%d]",i,m_iTrace[i]);
	}
}
void DiceGame::SetDiceGameTrace( int iStep, int iTrace )
{
	m_iTrace[iStep] = iTrace;	
	SetDetailDiceGameTrace(iStep,iTrace);
}
void DiceGame::SetDetailDiceGameTrace( int iStep, int iTrace )
{
	int iTemp = iStep * 16;
	int iTempNumber = iTrace;

	for(int i = 15; i>=0; i-- )
	{
		m_UserTrace[ iTemp + i] = iTempNumber%2;
		iTempNumber = iTempNumber/2;
	}
	
	User* pTempUser = GetUser();
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[DICE_GAME] ============SetDetailDiceGameTrace =========== CLient[%s]", pTempUser->GetPublicID().c_str());
	for( int i = 0 ; i< DICE_GAME_SLOT_COUNT ; i+=16)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[DICE_GAME] [%02d>>%02d]====[%d][%d][%d][%d] [%d][%d][%d][%d] [%d][%d][%d][%d] [%d][%d][%d][%d]====",i,i+15,
			m_UserTrace[i+0],m_UserTrace[i+1],m_UserTrace[i+2],m_UserTrace[i+3],m_UserTrace[i+4],m_UserTrace[i+5],m_UserTrace[i+6],m_UserTrace[i+7],
			m_UserTrace[i+8],m_UserTrace[i+9],m_UserTrace[i+10],m_UserTrace[i+11],m_UserTrace[i+12],m_UserTrace[i+13],m_UserTrace[i+14],m_UserTrace[i+15]);
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[DICE_GAME] ============SetDetailDiceGameTrace =========== CLient[%s]", pTempUser->GetPublicID().c_str());
}
void DiceGame::InitDiceGameTrace()
{
	for( int i = 0 ; i < DICE_GAME_TRACE_DB ; ++i )
		m_iTrace[i] = 0;
}
void DiceGame::AllGetRewardIndex( int *pArray )
{		
	for( int i = 0 ; i < DICE_GAME_REWARD_DB ; ++i )
		pArray[i] = static_cast<int>(m_iRewardIndex[i]);
}
void DiceGame::AllSetRewardIndex( int *pArray )
{
	for( int i = 0 ; i < DICE_GAME_REWARD_DB ; ++i )
	{
		m_iRewardIndex[i] = static_cast<int>(pArray[i]);		
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[DICE_GAME] @@@@  AllSetRewardIndex m_iRewardIndex[%d] = %d", i,m_iRewardIndex[i]);
	}
}

void DiceGame::SetRewardIndex( int iGroupNum, int iIndex )
{
	m_iRewardIndex[iGroupNum] = iIndex;
}

int DiceGame::GetRewardIndex( int iGroupNum)
{
	return m_iRewardIndex[iGroupNum];
}


void DiceGame::InitRewardIndex()
{
	for( int i = 0 ; i < DICE_GAME_REWARD_DB ; ++i )
		m_iRewardIndex[0] = 0;
}

int DiceGame::GetNumberOfTraceCount()
{
	int iCount = 0;
	for( int i = 0; i < DICE_GAME_SLOT_COUNT; i++ )
		iCount = iCount + m_UserTrace[i];

	return iCount;
}

void DiceGame::SendDiceGameData( User *pUser)
{
	SP2Packet kPacket( STPK_DICE_GAME_GET_INFO );	
	PACKET_GUARD_VOID( kPacket.Write(DICE_GAME_GET_INFO_LIST) );
	PACKET_GUARD_VOID( kPacket.Write( m_iPosition ) );
	PACKET_GUARD_VOID( kPacket.Write( GetNumberOfTraceCount() ) );
	for( int i = 0; i < DICE_GAME_SLOT_COUNT; i++ )
	{
		if(m_UserTrace[i] == ON_STEPED)
			PACKET_GUARD_VOID( kPacket.Write(i));
	}
	PACKET_GUARD_VOID( kPacket.Write( m_byBoradIndex ) );
	for( int i = 0; i < DICE_GAME_REWARD_DB; i++ )
		PACKET_GUARD_VOID( kPacket.Write( m_iRewardIndex[i] ) );

	pUser->SendMessage( kPacket );
}

void DiceGame::SendDiceGameData_Re( User *pUser)
{
	SP2Packet kPacket( STPK_DICE_GAME_RESTART_GET_INFO );	
	PACKET_GUARD_VOID( kPacket.Write(DICE_GAME_GET_INFO_LIST) );
	PACKET_GUARD_VOID( kPacket.Write( m_iPosition ) );
	PACKET_GUARD_VOID( kPacket.Write( GetNumberOfTraceCount() ) );
	for( int i = 0; i < DICE_GAME_SLOT_COUNT; i++ )
	{
		if(m_UserTrace[i] == ON_STEPED)
			PACKET_GUARD_VOID( kPacket.Write(i));
	}
	PACKET_GUARD_VOID( kPacket.Write( m_byBoradIndex ) );
	for( int i = 0; i < DICE_GAME_REWARD_DB; i++ )
		PACKET_GUARD_VOID( kPacket.Write( m_iRewardIndex[i] ) );

	pUser->SendMessage( kPacket );
}

void DiceGame::DiceGameDataFill( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID( rkPacket.Write(GetProgressStateDice()));
	PACKET_GUARD_VOID( rkPacket.Write( m_iPosition ) );
	for( int i = 0; i < DICE_GAME_TRACE_DB; i++ )
		PACKET_GUARD_VOID( rkPacket.Write( m_iTrace[i] ) );
	PACKET_GUARD_VOID( rkPacket.Write( m_byBoradIndex ) );
	for( int i = 0; i < DICE_GAME_REWARD_DB; i++ )
		PACKET_GUARD_VOID( rkPacket.Write( m_iRewardIndex[i] ) );

#ifdef DICE_GAME_BY_BCKIM_DEBUG	
	User* pTEmpUser = GetUser();
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[DICE_GAME]DiceGameDataFill IDX[%s][%d][%d][%d]", pTEmpUser->GetPublicID().c_str(),GetProgressStateDice(),m_iPosition,m_byBoradIndex);
#endif	// DICE_GAME_BY_BCKIM_DEBUG
}
//////////////////////////////
