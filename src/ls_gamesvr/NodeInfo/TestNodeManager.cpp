#include "stdafx.h"
#include "TestNodeManager.h"
#include "ServerNodeManager.h"
#include "TournamentManager.h"
#include "UserNodeManager.h"


#include "../MainProcess.h"

#include "../EtcHelpFunc.h"
#include "../DataBase/DBClient.h"
#include "../QueryData/QueryResultData.h"
#include "../MainServerNode/MainServerNode.h"

template<> ioTestNodeManager* Singleton< ioTestNodeManager >::ms_Singleton = 0;

ioTestNodeManager::ioTestNodeManager( bool bTestServer )
{
	m_bLogPrint   = false;
	m_bTestServer = false;
}

ioTestNodeManager::~ioTestNodeManager()
{
	DestroyNode();
}

ioTestNodeManager& ioTestNodeManager::GetSingleton()
{
	return Singleton< ioTestNodeManager >::GetSingleton();
}

int ioTestNodeManager::IsLogPrint()
{
	if( m_bLogPrint )
		return  LOG_DEBUG_LEVEL;

	return LOG_TEST_LEVEL;
}

void ioTestNodeManager::LoadINI()
{
	DestroyNode();

	ioINILoader kLoader( "config/sp2_test_node.ini" );
	kLoader.SetTitle( "common" );
	
	m_bLogPrint   = kLoader.LoadBool( "log_print", false );	
	if( !m_bTestServer )
	{
		return;
	}	
	
	int iMaxNode  = kLoader.LoadInt( "max_node", 0 );

	char szKey[MAX_PATH] = "";
	for( int i = 0; i < iMaxNode; ++i )
	{		
		sprintf_s( szKey, "Node%d", i+1 );
		kLoader.SetTitle( szKey );
		
		TestNode* pNode = NULL;
		BOTTYPE Type = (BOTTYPE)kLoader.LoadInt( "type", 0 );
		int iSubType = kLoader.LoadInt( "sub_type", 0 );

		switch ( Type )
		{
		case BOT_USER:
			pNode = CreateUserBotTemplete( kLoader, iSubType );
			break;
		default:
			{
				LOG.PrintTimeAndLog( IsLogPrint(), "%s Load Fail : UnKown Type %d", __FUNCTION__, (int)Type );
				continue;
			}
		}

		if( pNode && pNode->LoadINI( kLoader ) )
		{
			pNode->SetType( Type );
			pNode->SetSubType( iSubType );
			pNode->OnCreate();
			m_TestNodeMap.insert( TestNodeMap::value_type( pNode->GetGUID(), pNode ) );
			LOG.PrintTimeAndLog( IsLogPrint() ,"%s - insert Ok : %s(%d, %d, %d)", __FUNCTION__, pNode->GetGUID().c_str(), (int)Type, iSubType, i );
		}
		else
		{
			LOG.PrintTimeAndLog(0, "%s Load Fail : INI FAIL( %d, %d, %d)", __FUNCTION__, (int)Type, iSubType, i );
		}
	}	
}

UserBot* ioTestNodeManager::CreateUserBotTemplete( ioINILoader &rkLoader, int iSubType )
{
	UserBot* pNode = NULL;
	switch( iSubType )
	{	
	case BOT_TURNAMENT:
		pNode = new TournamentBot;
		break;
	}

	return pNode;
}

void ioTestNodeManager::ServerDownAllSave()
{
}

void ioTestNodeManager::DestroyNode()
{
	if( m_TestNodeMap.empty() )
		return;

	TestNodeMap::iterator iter = m_TestNodeMap.begin();
	for( ; iter != m_TestNodeMap.end(); ++iter )
	{
		TestNode* pNode = iter->second;
		if( pNode )
			pNode->OnDestroy();

		SAFEDELETE(pNode);
	}
}

TestNode* ioTestNodeManager::GetNode( const ioHashString& szGUID )
{
	TestNodeMap::iterator iter = m_TestNodeMap.find( szGUID );
	if( iter == m_TestNodeMap.end() )
		return NULL;

	return iter->second;
}

void ioTestNodeManager::GetNodeList( TestNodeVec& DestTourVec )
{
	TestNodeMap::iterator iter = m_TestNodeMap.begin();
	for( ; iter != m_TestNodeMap.end(); ++iter )
	{
		TestNode* pNode = iter->second;
		if( pNode )			
			DestTourVec.push_back( pNode );		
	}
}

void ioTestNodeManager::OnUpdate()
{
	TestNodeMap::iterator iter = m_TestNodeMap.begin();
	for( ; iter != m_TestNodeMap.end(); ++iter )
	{
		TestNode* pNode = iter->second;
		if( pNode )
		{
			pNode->OnUpdate();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

TestNode::TestNode()
{
	char szTempGUID[USER_GUID_NUM_PLUS_ONE]="";
	Help::GetGUID(szTempGUID, sizeof(szTempGUID) );
	m_szGUID = szTempGUID;
}

TestNode::~TestNode()
{
}

bool TestNode::LoadINI( ioINILoader &rkLoader )
{
	return true;
}

bool TestNode::OnUpdate()
{
	return true;
}

void TestNode::OnCreate()
{

}

void TestNode::OnDestroy()
{

}

////////////////////////////////////////////////////////////////////////////////////////////
//
// 테스트용 가상유저 노드, 유저가 접속하지 않았을때 유저 정보를 취득
// 유저 정보를 기준으로 테스트를 하고자 할때 사용
//
UserBot::UserBot()
{
	m_dwUserID		= 0;
	m_dwDBAgentID	= 0;
	m_State			= ULS_NONE;
	m_iDBCampPos	= CAMP_NONE;

	m_bUserConnect = false;
}

UserBot::~UserBot()
{

}

bool UserBot::LoadINI( ioINILoader &rkLoader )
{
	TestNode::LoadINI( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString( "private_id", "", szBuf, MAX_PATH );
	m_szPrivateID = szBuf;

	if( m_szPrivateID.IsEmpty() )
		return false;

	m_dwUserID	  = 0;
	m_dwDBAgentID = Help::GetUserDBAgentID( m_szPrivateID );

	return true;
}

UserBot* UserBot::ToUserBot( TestNode *pNode )
{
	if( !pNode || pNode->GetType() != BOT_USER )
		return NULL;

	return static_cast< UserBot* > ( pNode );
}

bool UserBot::IsActive()
{
	switch( m_State )
	{
	case ULS_LOADDB_DONE:
		return true;
	}

	return false;
}

void UserBot::OnCreate()
{

}

void UserBot::OnDestroy()
{

}

bool UserBot::OnUpdate()
{
	if( !g_DBClient.IsDBAgentActive( GetDBAgentID() ) )
		return false;
	
	switch( m_State )
	{
	case ULS_NONE:
		{
			LOG.PrintTimeAndLog( g_TestNodeMgr.IsLogPrint(), "%s, %s", __FUNCTION__, GetGUID().c_str() );
			g_DBClient.OnSelectUserData( GetDBAgentID(), GetAgentThreadID(), GetGUID(), GetPrivateID() );
			SetState( ULS_LOADDB );			
		}
		return false;

	case ULS_LOADDB:
		return false;

	case ULS_LOADDB_FAIL:
		return false;
	}

	//유저가 서버에 존재하면 프로세스를 수행 하지 않음
	if( g_UserNodeManager.GetUserNode( GetUserIndex() ) != NULL )
	{
		m_bUserConnect = true;
		return false;
	}
	m_bUserConnect = false;
	return true;
}

void UserBot::ApplyDBUserData( CQueryResultData *query_data )
{
	//SELECT
	int  user_idx = 0;
	char szDBID[ID_NUM_PLUS_ONE] = "";
	char nick_name[ID_NUM_PLUS_ONE] = "";
	query_data->GetValue(user_idx,sizeof(int));
	query_data->GetValue(szDBID,ID_NUM_PLUS_ONE);
	query_data->GetValue(nick_name,ID_NUM_PLUS_ONE);

	// 대문자가 들어오는 경우를 막기 위한 임시 방지 코드	JCLEE 140507
	CString strDbID;
	strDbID = szDBID;
	strDbID.MakeLower();

	CString strPrivateID;
	strPrivateID = GetPrivateID().c_str();
	strPrivateID.MakeLower();
		
	if( strPrivateID != strDbID )
	{
		LOG.PrintTimeAndLog( g_TestNodeMgr.IsLogPrint(), "private_id error : %s", GetGUID().c_str() );
		SetState( ULS_LOADDB_FAIL );
		return;
	}
	else if( user_idx <= 0 )
	{
		LOG.PrintTimeAndLog( g_TestNodeMgr.IsLogPrint(), "user_idx error : %s", GetGUID().c_str() );
		SetState( ULS_LOADDB_FAIL );
		return;
	}

	m_dwUserID = user_idx;

	__int64 iMoney;
	int iUserState, iConnectCnt, iGreade, iGradeExp, iEventType, iRank;
	short iEntryType;

	//최종 접속 시간
	DBTIMESTAMP dts;

	query_data->GetValue( iUserState,sizeof(int) );
	query_data->GetValue( iMoney,sizeof(__int64) );
	query_data->GetValue( iConnectCnt,sizeof(int) );
	query_data->GetValue( (char*)&dts, sizeof(DBTIMESTAMP) );
	query_data->GetValue( iGreade,sizeof(int) );	
	query_data->GetValue( iGradeExp,sizeof(int) );	
	query_data->GetValue( iEventType ,sizeof(int) );
	query_data->GetValue( iRank,sizeof(int) );
	query_data->GetValue( iEntryType,sizeof(short) ); // db smallint	
	query_data->GetValue( (char*)&dts, sizeof(DBTIMESTAMP) );	
	query_data->GetValue( m_iDBCampPos,sizeof(int) );

	SetState( ULS_LOADDB_DONE );

	LOG.PrintTimeAndLog( g_TestNodeMgr.IsLogPrint(), "%s OK : %s", __FUNCTION__, GetGUID().c_str() );
	
}

////////////////////////////////////////////////////////////////////////////////////////////
//
// 대회테스트용 노드, 유저가 접속하지 않았을때 유저를 대신하여 진영 및 대회 팀 생성, 응원하기 참여
//

TournamentBot::TournamentBot()
{
	m_dwTeamIndex  = 0;

	m_iLadderPoint = 0;
	m_CampPos	   = 0;

	m_TourBotType  = TBT_NONE;
	m_State		   = TS_NONE;
	m_CheerState   = TCS_NONE;
	
	m_dwJoinCheckTime  = 0;
	m_dwCheerCheckTime = 0;
	m_dwCurrCheckTime  = 0;

	m_dwTryCount = 0;
	m_dwPrevTournamentStartDate = 0;

	m_dwCheerTeam = 0;
}

TournamentBot::~TournamentBot()
{

}

bool TournamentBot::LoadINI( ioINILoader &rkLoader )
{
	if( !UserBot::LoadINI( rkLoader ) )
		return false;
		
	m_iLadderPoint	= rkLoader.LoadInt( "ladder_point", 0 );
	m_CampPos		= rkLoader.LoadInt( "camp", 0 );
	
	char szBuf[MAX_PATH];
	rkLoader.LoadString( "team_name", "", szBuf, MAX_PATH );
	m_szTeamName = szBuf;

	m_dwJoinCheckTime  = rkLoader.LoadInt( "join_check_time", 300000 );
	m_dwCheerCheckTime = rkLoader.LoadInt( "cheer_check_time", 300000 );
	m_dwCheerTeam	   = rkLoader.LoadInt( "cheer_check_time", 300000 );

	m_TourBotType	  = (TOURNAMENTBOTTYPE)rkLoader.LoadInt( "tournament_bot_type", 0 );
	if( m_TourBotType == TBT_NONE )
		return false;

	if( m_szTeamName.IsEmpty() )
		return false;
	
	return true;
}

TournamentBot* TournamentBot::ToTournamentBot( TestNode *pNode )
{
	UserBot* pUser = ToUserBot( pNode );
	if( !pUser || pUser->GetSubType() != BOT_TURNAMENT )
		return NULL;

	return static_cast< TournamentBot* > ( pNode );
}

TournamentBot* TournamentBot::GetTournamentBot( DWORD dwUserIdx )
{
	TestNodeVec BotVec;
	g_TestNodeMgr.GetNodeList( BotVec );
	TestNodeVec::iterator iter = BotVec.begin();
	for( ; iter != BotVec.end(); ++iter )
	{
		TournamentBot* pNode = ToTournamentBot( *iter );
		if( pNode && pNode->GetUserIndex() == dwUserIdx && !pNode->IsUserConnect() )
		{
			return pNode;
		}
	}

	return NULL;
}


void TournamentBot::OnCreate()
{

}

void TournamentBot::OnDestroy()
{

}

bool TournamentBot::OnUpdate()
{
	//유저 정보가 로드될때까지 대기
	if( !UserBot::OnUpdate() )
		return false;

	DWORD dwTourIndex = g_TournamentManager.GetRegularTournamentIndex();

	//정규대회정보가 생성될때까지 대기
	if( dwTourIndex <= 0 )
		return false;
	
	switch( m_TourBotType )
	{
	case TBT_TOURNAMENT:
		ProcessTournamentJoinState( dwTourIndex );
		break;
	case TBT_CHEER:
		ProcessCheerTryState( dwTourIndex );
		break;
	}
	
	return true;
}

void TournamentBot::ProcessTournamentJoinState( DWORD dwTourIdx )
{
	switch( m_State )
	{
	case TS_NONE:
		{
			CampJoinTry( dwTourIdx );
		}
		break;
	case TS_JOIN:
		{
			TournamentJoinTry( dwTourIdx );
		}
		break;
	case TS_TEAM_GET:
		{
			//정상적인 경우라면 해당 스테이트에서 스테이트 값이 변하지 않음
			//변할 경우 DB에서 결과값이 날라오지 않아 강제 변환되는 상황
			TournamentJoinCheck( dwTourIdx );
		}
		break;
	case TS_TEAM_GET_FAIL:
		{
			//실패시 다시 시도(가입기간에만..)
			TournamentJoinCheck( dwTourIdx );
		}
		break;
	case TS_TEAM_GET_OK:
		{
			//대기하기
			TournamentReStartCheck( dwTourIdx );
		}
		break;
	case TS_NOT_EXIST_TEAM:
		{
			//모집기간일 경우 여기로는 오지 않음
			//다음대회가 올때까지 대기
			TournamentReStartCheck( dwTourIdx );
		}
	}
}

void TournamentBot::CampJoinTry( DWORD dwTourIdx )
{
	//모집기간이 아니면 진영가입 X
	if( g_TournamentManager.GetTournamentState( dwTourIdx ) == TournamentManager::STATE_TEAM_APP )
	{
		//진영 정보 업데이트
		if( GetDBCampPos() == CAMP_NONE )
		{
			g_DBClient.OnUpdateUserCampPosition( GetDBAgentID(), GetAgentThreadID(), GetUserIndex(), m_CampPos );
			SetDBCampPos( m_CampPos );
		}
	}
	m_State = TS_JOIN;
}

void TournamentBot::TournamentJoinTry( DWORD dwTourIdx )
{
	//모집기간이면
	if( g_TournamentManager.GetTournamentState( dwTourIdx ) == TournamentManager::STATE_TEAM_APP )
	{
		//대회팀 만들기
		g_DBClient.OnInsertTournamentTeamCreate( GetDBAgentID(), GetAgentThreadID(), GetUserIndex(), dwTourIdx, m_szTeamName, 1, m_iLadderPoint, m_CampPos );
		m_State = TS_TEAM_GET;
		m_dwCurrCheckTime = TIMEGETTIME();

		LOG.PrintTimeAndLog( g_TestNodeMgr.IsLogPrint(), "%s Tournament Join Try : %d", __FUNCTION__, m_dwTryCount );
	}
	else
	{
		//진영에 가입 되있다면 대회에 참여 중일수도 있음으로 대회 참가 여부를 판단
		if( GetDBCampPos() != CAMP_NONE && m_dwTeamIndex == 0 )
		{
			g_DBClient.OnSelectTournamentTeamList( GetDBAgentID(),GetAgentThreadID(), GetGUID(), GetUserIndex(), MAX_INT_VALUE, TOURNAMENT_TEAM_MAX_LOAD );
			LOG.PrintTimeAndLog( g_TestNodeMgr.IsLogPrint(), "%s my tournament team index select count : %d", __FUNCTION__, m_dwTryCount );
		}
		m_State = TS_TEAM_GET;
	}
}

void TournamentBot::TournamentJoinCheck( DWORD dwTourIdx )
{
	if( m_dwJoinCheckTime < TIMEGETTIME() - m_dwCurrCheckTime )
	{	
		if( g_TournamentManager.GetTournamentState( dwTourIdx ) != TournamentManager::STATE_TEAM_APP )
		{
			//가입기간이 아니면
			m_State			  = TS_NOT_EXIST_TEAM;
			m_dwCurrCheckTime = 0;
			m_dwTryCount	  = 0;
		}
		else
		{
			//가입기간이면 다시 시도
			m_State			  = TS_NONE;
			m_dwCurrCheckTime = TIMEGETTIME();
			m_dwTryCount++;
		}
	}
}

void TournamentBot::TournamentReStartCheck( DWORD dwTourIdx )
{	
	if( m_dwPrevTournamentStartDate == 0 )
		m_dwPrevTournamentStartDate = g_TournamentManager.GetTournamentStartDate( dwTourIdx );

	if( 0 < m_dwPrevTournamentStartDate && m_dwPrevTournamentStartDate != g_TournamentManager.GetTournamentStartDate( dwTourIdx ) )
	{
		m_State						= TS_NONE;
		m_CheerState				= TCS_NONE;
		m_dwCurrCheckTime			= 0;
		m_dwTryCount				= 0;
		m_dwPrevTournamentStartDate = 0;
	}
}

void TournamentBot::ApplyTeamCreateOk( DWORD dwTeamIndex )
{
	m_dwTeamIndex = dwTeamIndex;
	m_State		  = TS_TEAM_GET_OK;
	m_dwTryCount  = 0;

	LOG.PrintTimeAndLog( g_TestNodeMgr.IsLogPrint(), "%s Team Crreate OK : %d, %s", __FUNCTION__, m_dwTeamIndex, m_szTeamName.c_str() );
}

void TournamentBot::ApplyTeamCreateFail()
{
	m_State		  = TS_TEAM_GET_FAIL;
	m_dwTeamIndex = 0;

	LOG.PrintTimeAndLog( g_TestNodeMgr.IsLogPrint(), "%s Team Crreate FAIL : %d, %s", __FUNCTION__, m_dwTeamIndex, m_szTeamName.c_str() );
}

void TournamentBot::ApplyUserTeamIndex( CQueryResultData *query_data )
{
	LOOP_GUARD();

	int iCount = 0;
	DWORD dwTeamIndex = 0;
	while( query_data->IsExist() )
	{
		iCount++;
		DWORD dwTourIndex, dwOwnerIndex;
		char szTeamName[TOURNAMENT_TEAM_NAME_NUM_PLUS_ONE] = "";

		query_data->GetValue( dwTeamIndex, sizeof(DWORD) );
		query_data->GetValue( dwTourIndex, sizeof(DWORD) );
		query_data->GetValue( szTeamName, TOURNAMENT_TEAM_NAME_NUM_PLUS_ONE );
		query_data->GetValue( dwOwnerIndex, sizeof(DWORD) );

		if( dwTourIndex == 0 )		
			continue;		

		if( dwTourIndex != g_TournamentManager.GetRegularTournamentIndex() )
			continue;

		if( GetUserIndex() == dwOwnerIndex && strcmp( m_szTeamName.c_str(), szTeamName ) == 0 )
		{
			LOOP_GUARD_CLEAR();
			LOG.PrintTimeAndLog( g_TestNodeMgr.IsLogPrint(), "%s - my team index find ok", __FUNCTION__ );
			m_dwTeamIndex = dwTeamIndex;
			m_State	= TS_TEAM_GET_OK;
			return;
		}
	}
	LOOP_GUARD_CLEAR();

	if( iCount == 0 )
	{
		m_State	= TS_TEAM_GET_FAIL;
		LOG.PrintTimeAndLog( g_TestNodeMgr.IsLogPrint(), "%s - my team index find fail", __FUNCTION__  );
		return;
	}

	if( iCount >= TOURNAMENT_TEAM_MAX_LOAD )
	{
		g_DBClient.OnSelectTournamentTeamList( GetDBAgentID(),GetAgentThreadID(), GetGUID(), GetUserIndex(), dwTeamIndex, TOURNAMENT_TEAM_MAX_LOAD );
	}

}

void TournamentBot::ProcessCheerTryState( DWORD dwTourIdx )
{
	if( g_TournamentManager.GetTournamentState( dwTourIdx ) != TournamentManager::STATE_TEAM_DELAY )
		return;
	
	switch( m_CheerState )
	{
	case TCS_NONE:
		{
			TournamentCheerNone( dwTourIdx );
		}
		break;
	case TCS_CHEER_TRY:
		{
			TournamentCheerTry( dwTourIdx );
		}
		break;
	case TCS_RESULT_GET:
		{
			//결과값 대기
		}
		break;
	case TCS_CHEER_DECISION_OK:
		{
			TournamentReStartCheck( dwTourIdx );
		}
		break;
	case TCS_CHEER_DECISION_FAIL:
		{
			TournamentReStartCheck( dwTourIdx );
		}
		break;
	}
}

void TournamentBot::TournamentCheerNone( DWORD dwTourIdx )
{
	if( m_dwCurrCheckTime == 0 )
		m_dwCurrCheckTime = TIMEGETTIME();

	if( m_dwCheerCheckTime < TIMEGETTIME() - m_dwCurrCheckTime )
	{				
		m_CheerState = TCS_CHEER_TRY;
	}
}

void TournamentBot::TournamentCheerTry( DWORD dwTourIdx )
{
	//g_DBClient.OnInsertTournamentCheerDecision( GetDBAgentID(), GetAgentThreadID(), GetUserIndex(), m_dwTeamIndex, dwTeamIdx );
	m_CheerState = TCS_RESULT_GET;
}

void TournamentBot::ApplyCheerDecisionOK( SP2Packet &rkPacket )
{
	DWORD dwTourIdx, dwTeamIdx;

	rkPacket >> dwTourIdx;
	rkPacket >> dwTeamIdx;

	m_CheerState = TCS_CHEER_DECISION_OK;
	LOG.PrintTimeAndLog( g_TestNodeMgr.IsLogPrint(), "%s", __FUNCTION__  );
}

void TournamentBot::ApplyCheerDecisionFail( int iResult )
{
	m_CheerState = TCS_CHEER_DECISION_FAIL;

	switch( iResult )
	{
	case TOURNAMENT_CHEER_DECISION_ALREADY:
		{
			LOG.PrintTimeAndLog( g_TestNodeMgr.IsLogPrint(), "%s - alredy cheer team", __FUNCTION__ );
		}
		break;
	case TOURNAMENT_CHEER_DECISION_CLOSE:
		{
			LOG.PrintTimeAndLog( g_TestNodeMgr.IsLogPrint(), "%s - tournament close", __FUNCTION__ );
		}
		break;
	case TOURNAMENT_CHEER_DECISION_NOT_REGULAR:
		{
			LOG.PrintTimeAndLog( g_TestNodeMgr.IsLogPrint(), "%s - not regular tournament", __FUNCTION__ );
		}
		break;
	case TOURNAMENT_CHEER_DECISION_NONE_TEAM:
		{
			LOG.PrintTimeAndLog( g_TestNodeMgr.IsLogPrint(), "%s - not find tournament team", __FUNCTION__ );
		}
		break;
	default:
		{
			LOG.PrintTimeAndLog( g_TestNodeMgr.IsLogPrint(), "%s", __FUNCTION__ );
		}
		break;		
	}	
}