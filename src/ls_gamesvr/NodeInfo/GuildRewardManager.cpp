#include "stdafx.h"
#include "GuildRewardManager.h"
#include "User.h"
#include "ioUserPresent.h"
#include "../DataBase/LogDBClient.h"
#include "../EtcHelpFunc.h"
#include "../MainProcess.h"
#include "../MainServerNode/MainServerNode.h"


template<> GuildRewardManager* Singleton< GuildRewardManager >::ms_Singleton = 0;

GuildRewardManager::GuildRewardManager()
{
	Init();
}

GuildRewardManager::~GuildRewardManager()
{
	Destroy();
}

GuildRewardManager& GuildRewardManager::GetSingleton()
{
	return Singleton< GuildRewardManager >::GetSingleton();
}

void GuildRewardManager::Init()
{
	m_vAttendanceRewardList.clear();
	m_vRankRewardList.clear();

	m_dwActiveCampDate		= 0;
}

void GuildRewardManager::Destroy()
{
}

void GuildRewardManager::LoadINI()
{
	char szKey[MAX_PATH]="";

	ioINILoader kLoader( "config/sp2_guild_reward.ini" );

	kLoader.SetTitle("common");

	m_iRenewalHour	= kLoader.LoadInt("renewal_hour", 5);

	kLoader.SetTitle( "rank_reward" );

	for( int i = 0; i < 20; i++ )
	{
		RewardInfo stReward;
		char szValues[MAX_PATH] = "";

		StringCbPrintf( szKey, sizeof( szKey ), "reward%d_rank", i+1 );
		stReward.iDelimiter = kLoader.LoadInt( szKey, 0 );
		if( 0 == stReward.iDelimiter )
			break;

		StringCbPrintf( szKey, sizeof( szKey ), "reward%d_values", i+1 );
		kLoader.LoadString(szKey, "", szValues, MAX_PATH );
		IntVec vValues;
		vValues.clear();
		Help::TokenizeToINT(szValues, ".", vValues);
		for( int j = 0; j < (int)vValues.size(); j++ )
		{
			if( 0 == j )
				stReward.iType = vValues[j];
			else if( 1 == j )
				stReward.iValue1 = vValues[j];
			else
				stReward.iValue2 = vValues[j];
		}

		StringCbPrintf( szKey, sizeof( szKey ), "reward%d_period", i+1 );
		stReward.iPresentPeriod = kLoader.LoadInt(szKey, 0);

		StringCbPrintf( szKey, sizeof( szKey ), "reward%d_ment", i+1 );
		stReward.iMent = kLoader.LoadInt(szKey, 0);

		m_vRankRewardList.push_back(stReward);
	}
	
	kLoader.SetTitle( "attendance_reward" );

	for( int i = 0; i < 100; i++ )
	{
		RewardInfo stReward;
		char szValues[MAX_PATH] = "";

		StringCbPrintf( szKey, sizeof( szKey ), "reward%d_attendance", i+1 );
		stReward.iDelimiter = kLoader.LoadInt( szKey, 0 );
		if( 0 == stReward.iDelimiter )
			break;

		StringCbPrintf( szKey, sizeof( szKey ), "reward%d_values", i+1 );
		kLoader.LoadString(szKey, "", szValues, MAX_PATH );
		IntVec vValues;
		vValues.clear();
		Help::TokenizeToINT(szValues, ".", vValues);
		for( int j = 0; j < (int)vValues.size(); j++ )
		{
			if( 0 == j )
				stReward.iType = vValues[j];
			else if( 1 == j )
				stReward.iValue1 = vValues[j];
			else
				stReward.iValue2 = vValues[j];
		}

		StringCbPrintf( szKey, sizeof( szKey ), "reward%d_period", i+1 );
		stReward.iPresentPeriod = kLoader.LoadInt(szKey, 0);

		StringCbPrintf( szKey, sizeof( szKey ), "reward%d_ment", i+1 );
		stReward.iMent = kLoader.LoadInt(szKey, 0);

		m_vAttendanceRewardList.push_back(stReward);
	}
}

void GuildRewardManager::GetAttendReward(int iAttendCount, RewardInfo& stReward)
{
	int iSize = m_vAttendanceRewardList.size();
	for( int i = 0; i < iSize; i++ )
	{
		if( iAttendCount <= m_vAttendanceRewardList[i].iDelimiter )
		{
			stReward = m_vAttendanceRewardList[i];
			break;
		}
	}
}

void GuildRewardManager::GetRankReward(int iRank, RewardInfo& stReward)
{
	int iSize = m_vRankRewardList.size();
	for( int i = 0; i < iSize; i++ )
	{
		if( iRank == m_vRankRewardList[i].iDelimiter )
		{
			stReward = m_vRankRewardList[i];
			break;
		}
	}
}

DWORD GuildRewardManager::ConvertCampDateToDwordDate( DWORD dwDate)
{
	DWORD dwYear = dwDate / 1000000;
	DWORD dwMonth= ( dwDate % 1000000 ) / 10000;
	DWORD dwDay  = ( dwDate % 10000 ) / 100;
	DWORD dwHour = dwDate % 100;

	CTime cActiveDate( dwYear, dwMonth, dwDay, dwHour, 0, 0 );
	return cActiveDate.GetTime();
}

void GuildRewardManager::SetActiveCampDate(DWORD dwDate)
{
	if( 0 == dwDate )
		return;

	DWORD dwActiveDate = ConvertCampDateToDwordDate(dwDate);

	if( m_dwActiveCampDate != dwActiveDate )
		m_dwActiveCampDate = dwActiveDate;
}

BOOL GuildRewardManager::SendAttendReward(User *pUser, int iAttendCount)
{
	if( !pUser )
		return FALSE;

	RewardInfo stReward;
	GetAttendReward(iAttendCount, stReward);
	if( 0 == stReward.iType )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][guildreward]Invalid attend count : [%d] [%d]", pUser->GetUserIndex(), iAttendCount );
		return FALSE;
	}

	CTimeSpan cPresentGapTime( stReward.iPresentPeriod, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;

	pUser->AddPresentMemory( g_MainServer.GetSendID(), stReward.iType, stReward.iValue1, stReward.iValue2, 0, 0, stReward.iMent, kPresentTime, ioUserPresent::PRESENT_STATE_NORMAL );
	pUser->SendPresentMemory();

	char szNote[MAX_PATH]="";
	StringCbPrintf( szNote, sizeof( szNote ) , "guild attend reward" );
	g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), stReward.iType, stReward.iValue1, stReward.iValue2, 0, 0, LogDBClient::PST_RECIEVE, szNote );

	SP2Packet kPacket(STPK_RECV_GUILDATTEND_REWARD);
	PACKET_GUARD_BOOL( kPacket.Write(GUILD_ATTEND_REWARD_OK) );
	PACKET_GUARD_BOOL( kPacket.Write((BYTE)iAttendCount) );
	PACKET_GUARD_BOOL( kPacket.Write(stReward.iType) );
	PACKET_GUARD_BOOL( kPacket.Write(stReward.iValue1) );
	PACKET_GUARD_BOOL( kPacket.Write(stReward.iValue2) );
	pUser->SendMessage(kPacket);

	return TRUE;
}

BOOL GuildRewardManager::SendRankReward(User *pUser, int iRank)
{
	if( !pUser )
		return FALSE;

	RewardInfo stReward;
	GetRankReward(iRank, stReward);
	if( 0 == stReward.iType )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][guildreward] invalid rank : [%d] [%d]", pUser->GetUserIndex(), iRank );
		return FALSE;
	}

	CTimeSpan cPresentGapTime( stReward.iPresentPeriod, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;

	pUser->AddPresentMemory( g_MainServer.GetSendID(), stReward.iType, stReward.iValue1, stReward.iValue2, 0, 0, stReward.iMent, kPresentTime, ioUserPresent::PRESENT_STATE_NORMAL );
	pUser->SendPresentMemory();

	char szNote[MAX_PATH]="";
	StringCbPrintf( szNote, sizeof( szNote ) , "guild rank reward" );
	g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), stReward.iType, stReward.iValue1, stReward.iValue2, 0, 0, LogDBClient::PST_RECIEVE, szNote );

	SP2Packet kPacket(STPK_RECV_GUILDRANK_REWARD);
	PACKET_GUARD_BOOL( kPacket.Write(GUILD_RANK_REWARD_OK) );
	PACKET_GUARD_BOOL( kPacket.Write(stReward.iType) );
	PACKET_GUARD_BOOL( kPacket.Write(stReward.iValue1) );
	PACKET_GUARD_BOOL( kPacket.Write(stReward.iValue2) );
	pUser->SendMessage(kPacket);

	return TRUE;
}

BOOL GuildRewardManager::IsRecvGuildRank(int iGuildRank)
{
	int iSize = m_vRankRewardList.size();

	for( int i = 0; i < iSize; i++ )
	{
		if( iGuildRank == m_vRankRewardList[i].iDelimiter )
			return TRUE;
	}

	return FALSE;
}


DWORD GuildRewardManager::RecvDateConvertDBTIMESTAMPToDWORD(DBTIMESTAMP& RecvDate)
{
	if( !Help::IsAvailableDate(RecvDate.year - 2000, RecvDate.month, RecvDate.day) )
		return 0;

	CTime cRecvDate(RecvDate.year, RecvDate.month, RecvDate.day, RecvDate.hour, RecvDate.minute, 0);

	return cRecvDate.GetTime();
}