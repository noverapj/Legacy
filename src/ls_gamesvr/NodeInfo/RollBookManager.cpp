#include "stdafx.h"
#include "../MainProcess.h"
#include "RollBookManager.h"
#include "../EtcHelpFunc.h"
#include "ioUserPresent.h"
#include "../DataBase/LogDBClient.h"
#include "../MainServerNode/MainServerNode.h"

template<> RollBookManager* Singleton< RollBookManager >::ms_Singleton = 0;

RollBookManager::RollBookManager()
{
	Init();
}

RollBookManager::~RollBookManager()
{
	Destroy();
}

RollBookManager& RollBookManager::GetSingleton()
{
	return Singleton< RollBookManager >::GetSingleton();
}

void RollBookManager::Init()
{
	m_iNewbieJudgmentDay		= 0;
	m_iReturnUserJudgementDay	= 0;
	m_iRenewalHour				= 0;

	m_iNewbieStartTable			= 0;
	m_iReturnUserStartTable		= 0;
	m_iDefaultUserStartTable	= 0;

	m_iNewbieResetTerm			= 0;
	m_iReturnUserResetTerm		= 0;
	m_iDefaultUserResetTerm		= 0;

	m_mRollBookTable.clear();
	m_mRewardTable.clear();
	//m_mNextTable.clear();
	//m_mResetStartTable.clear();
}

void RollBookManager::Destroy()
{
}

void RollBookManager::LoadInI(BOOL bReload)
{
	ioINILoader kLoader;
	if( bReload )
	{
		Init();
		kLoader.ReloadFile("config/sp2_roll_book.ini");
	}
	else
	{
		kLoader.LoadFile( "config/sp2_roll_book.ini" );
	}

	char szKey[MAX_PATH]="";
	
	
	kLoader.SetTitle("common");
	int iMaxTable				= kLoader.LoadInt("max_table", 0);
	
	m_iRenewalHour				= kLoader.LoadInt("renewal_hour", 5);
	
	m_iNewbieJudgmentDay		= kLoader.LoadInt("newbie_judgment_day", 0);
	m_iReturnUserJudgementDay	= kLoader.LoadInt("return_judgment_day", 0);

	m_iNewbieStartTable			= kLoader.LoadInt("newbie_start_table", 0);
	m_iReturnUserStartTable		= kLoader.LoadInt("return_start_table", 0);
	m_iDefaultUserStartTable	= kLoader.LoadInt("default_start_table", 0);

	m_iNewbieResetTerm			= kLoader.LoadInt("newbie_reset_term", 0);
	m_iReturnUserResetTerm		= kLoader.LoadInt("return_reset_term", 0);
	m_iDefaultUserResetTerm		= kLoader.LoadInt("default_reset_term", 0);

	for( int i = 0; i < iMaxTable; i++ )
	{

		sprintf_s( szKey, "table%d", i + 1 );
		kLoader.SetTitle(szKey);

		int iTableIndex		 = kLoader.LoadInt("table_index", 0);
		if( 0 == iTableIndex )
			continue;

		RollBookInfo stRollBookInfo;
		
		stRollBookInfo.iRollBookType		 = kLoader.LoadInt("active_type", 0);
		stRollBookInfo.iNextRollBook		 = kLoader.LoadInt("next_table", 0);
		stRollBookInfo.iResetStartRollBook   = kLoader.LoadInt("reset_start_table", 0);

		RewardVector vRewardInfo;
		int iReward	= 0;

		while( TRUE )
		{
			RewardInfo stRewardInfo;
			char szValues[MAX_PATH] = "";
			
			StringCbPrintf( szKey, sizeof( szKey ), "reward%d_values", iReward+1 );
			kLoader.LoadString(szKey, "", szValues, MAX_PATH );
			IntVec vValues;
			vValues.clear();
			Help::TokenizeToINT(szValues, ".", vValues);

			if( vValues.empty() )
				break;

			for( int j = 0; j < (int)vValues.size(); j++ )
			{
				if( 0 == j )
					stRewardInfo.iType = vValues[j];
				else if( 1 == j )
					stRewardInfo.iValue1 = vValues[j];
				else
					stRewardInfo.iValue2 = vValues[j];
			}

			StringCbPrintf( szKey, sizeof( szKey ), "reward%d_period", iReward+1 );
			stRewardInfo.iPresentPeriod = kLoader.LoadInt(szKey, 0);

			StringCbPrintf( szKey, sizeof( szKey ), "reward%d_ment", iReward+1 );
			stRewardInfo.iMent = kLoader.LoadInt(szKey, 0);
			
			vRewardInfo.push_back(stRewardInfo);
			iReward++;
		}

		m_mRewardTable.insert(make_pair(iTableIndex, vRewardInfo));
		m_mRollBookTable.insert(make_pair(iTableIndex, stRollBookInfo));
		//m_mNextTable.insert(make_pair(iTableIndex, iNextTable));
		//m_mResetStartTable.insert(make_pair(iTableIndex, iResetStartTable));
	}
}

int RollBookManager::GetNextStartTable(int iCurTable, BOOL bReset)
{
	RollBookTable::iterator it = m_mRollBookTable.find(iCurTable);
	if( it == m_mRollBookTable.end() )
		return 0;

	if( bReset )
	{
		return ((RollBookInfo)it->second).iResetStartRollBook;
	}

	
	return ((RollBookInfo)it->second).iNextRollBook;
}

int RollBookManager::GetFirstStartTable(int iUserType)
{
	switch( iUserType )
	{
	case RBT_NEWBIE:
		return m_iNewbieStartTable;
		
	case RBT_RETURN:
		return m_iReturnUserStartTable;

	case RBT_DEFAULT:
		return m_iDefaultUserStartTable;
	}

	return 0;
}

int RollBookManager::GetResetTerm(int iUserType)
{
	switch( iUserType )
	{
	case RBT_NEWBIE:
		return m_iNewbieResetTerm;

	case RBT_RETURN:
		return m_iReturnUserResetTerm;

	case RBT_DEFAULT:
		return m_iDefaultUserResetTerm;
	}

	return 0;
}

int RollBookManager::GetRollBookType(int iTable)
{
	RollBookTable::iterator it = m_mRollBookTable.find(iTable);
	if( it == m_mRollBookTable.end() )
		return RBT_NONE;

	return ((RollBookInfo)it->second).iRollBookType;
}

BOOL RollBookManager::IsReset(CTime& cCompareTime, int iCurTable)
{
	int iRollBookType = GetRollBookType(iCurTable);
	if( RBT_NONE == iRollBookType )
		return FALSE;

	int iResetTerm = GetResetTerm(iRollBookType);

	if( iResetTerm <= 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][roolbookmgr] user reset term is error : [%d,%d]", iRollBookType, iResetTerm );
		return FALSE;
	}

	if( !Help::IsAvailableDate(cCompareTime.GetYear() - 2000, cCompareTime.GetMonth(), cCompareTime.GetDay()) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][roolbookmgr] user last login data is error" );
		return FALSE;
	}

	CTime cCurTime = CTime::GetCurrentTime();
	CTime cCheckTime(cCompareTime.GetYear(), cCompareTime.GetMonth(), cCompareTime.GetDay(), 0, 0 ,0);
	CTimeSpan cLoginGap(iResetTerm, 0, 0, 0);

	cCheckTime += cLoginGap;

	if( cCheckTime <= cCurTime )
		return TRUE;

	return FALSE;
}

BOOL RollBookManager::IsNewbie(CTime& cJoinTime)
{
	CTime cCurTime = CTime::GetCurrentTime();
	CTime cCompare( cCurTime.GetYear(), cCurTime.GetMonth(), cCurTime.GetDay(), 0, 0, 0);

	CTime cUserTime( cJoinTime.GetYear(), cJoinTime.GetMonth(), cJoinTime.GetDay(), 0, 0, 0);
	CTimeSpan cNewbieGap(m_iNewbieJudgmentDay, 0, 0, 0);

	cUserTime += cNewbieGap;

	if( cUserTime >= cCompare )
		return TRUE;

	return FALSE;
}
	
BOOL RollBookManager::IsReturnUser(CTime& cCompareTime)
{
	CTime cCurTime = CTime::GetCurrentTime();
	//CTime cCompare( cCurTime.GetYear(), cCurTime.GetMonth(), cCurTime.GetDay(), 0, 0, 0);

	CTime cUserTime( cCompareTime.GetYear(), cCompareTime.GetMonth(), cCompareTime.GetDay(), 0, 0, 0);
	CTimeSpan cReturnGap(m_iReturnUserJudgementDay, 0, 0, 0);

	cUserTime += cReturnGap;

	if( cCurTime >= cUserTime )
		return TRUE;

	return FALSE;
}

int RollBookManager::JudgmentUserRollBookType(CTime& cJoinTime, CTime& cLogoutTime)
{
	if( !Help::IsAvailableDate(cJoinTime.GetYear() - 2000, cJoinTime.GetMonth(), cJoinTime.GetDay()) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][roolbookmgr] user join date is invalid" );
		return RBT_NONE;
	}

	if( !Help::IsAvailableDate(cLogoutTime.GetYear() - 2000, cLogoutTime.GetMonth(), cLogoutTime.GetDay()) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][roolbookmgr] user log out date is invalid" );
		return RBT_NONE;
	}

	//신규 유저인지 비교
	if( IsNewbie(cJoinTime) )
		return RBT_NEWBIE;

	//복귀 유저인지 체크
	if( IsReturnUser(cLogoutTime) )
		return RBT_RETURN;

	return RBT_DEFAULT;
}

BOOL RollBookManager::SendReward(User* pUser, int iTable, int iNumber)
{
	if( !pUser )
		return FALSE;
	
	RewardInfo stReward;
	GetRewardInfo(iTable, iNumber, stReward);

	if( 0 == stReward.iType )
		return FALSE;

	CTimeSpan cPresentGapTime( stReward.iPresentPeriod, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;

	pUser->AddPresentMemory( g_MainServer.GetSendID(), stReward.iType, stReward.iValue1, stReward.iValue2, 0, 0, stReward.iMent, kPresentTime, ioUserPresent::PRESENT_STATE_NORMAL );
	pUser->SendPresentMemory();

	char szNote[MAX_PATH]="";
	StringCbPrintf( szNote, sizeof( szNote ) , "roll book reward" );
	g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), stReward.iType, stReward.iValue1, stReward.iValue2, 0, 0, LogDBClient::PST_RECIEVE, szNote );
	
	return TRUE;
}

int RollBookManager::GetNextRollBookLine(int iTable, int iCurLine)
{
	AllRewardTable::iterator it = m_mRewardTable.find(iTable);
	if( it == m_mRewardTable.end() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][roolbookmgr] don't exist table : [%d]", iTable );
		return -1;
	}

	int iSize = it->second.size();
	if( iCurLine + 1 <= iSize )
		return iCurLine + 1;

	//다음 테이블로 진행
	return 0;
}

void RollBookManager::GetRewardInfo(int iTable, int iNumber, RewardInfo& stRewardInfo)
{
	AllRewardTable::iterator it = m_mRewardTable.find(iTable);

	if( it == m_mRewardTable.end() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][roolbookmgr] don't exist table index : [%d]", iTable );
		return;
	}

	RewardVector vTableRewardVec = it->second;

	if( !COMPARE(iNumber, 1, (int)vTableRewardVec.size() + 1 ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][roolbookmgr] iNumber is invalid : [%d, %d]", iTable, iNumber );
		return;
	}

	RewardInfo stReward = vTableRewardVec[iNumber-1];

	stRewardInfo = stReward;
}