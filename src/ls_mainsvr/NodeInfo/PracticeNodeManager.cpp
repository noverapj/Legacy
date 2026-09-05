#include "../stdafx.h"

#include "../Network/GameServer.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "../ioProcessChecker.h"
#include "ServerNode.h"
#include "PracticeNode.h"
#include "PracticeNodeManager.h"
#include "ServerNodeManager.h"


CPracticeNodeManager *CPracticeNodeManager::sg_Instance = NULL;

CPracticeNodeManager::CPracticeNodeManager()
{
	m_mPracticeNode.clear();
	m_SPracticeReward.clear();
}


CPracticeNodeManager::~CPracticeNodeManager()
{
	ReleaseMemoryPool();
}


CPracticeNodeManager &CPracticeNodeManager::GetInstance()
{
	if( !sg_Instance )
		sg_Instance = new CPracticeNodeManager;

	return *sg_Instance;
}

void CPracticeNodeManager::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}


void CPracticeNodeManager::ReleaseMemoryPool()
{
	mPracticeNode_iter iter  = m_mPracticeNode.begin();

	for( ; iter != m_mPracticeNode.end(); iter++ )
	{
		CPracticeNode *pNode = iter->second;
		if( !pNode ) continue;

		pNode->Init();
	}

	m_mPracticeNode.clear();
}


void CPracticeNodeManager::LoadINIData()
{
	// LoadINI보다 먼저 로딩되어야하는 INI
	// 정규 리그 세팅 사항
	ioINILoader kLoader( "config/sp2_practice_info.ini" );

	kLoader.SetTitle( "rankdate" );

	m_iRankRewardWeek = kLoader.LoadInt( "rank_reset_week", 0 );
	int RewardTime = kLoader.LoadInt( "rank_reset_time", 0 );
	m_iRankRewardHour = RewardTime/100;
	m_iRankRewardMinute  = RewardTime%100;

	char szBuf[MAX_PATH] = "";
	kLoader.LoadString( "reward_sender_id", "DeveloperK", szBuf, MAX_PATH );
	m_szSenderId = szBuf;

	kLoader.SetTitle( "reward" );

	char szKey[MAX_PATH];
	for(int i = 0;i < PracticeSectionCount;i++)
	{
		SPracticeReward kSPracticeReward;
		kSPracticeReward.Init();

		sprintf_s( szKey, "reward_section%d_start", i + 1 );
		kSPracticeReward.m_iPracticeRewadStart = kLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "reward_section%d_end", i + 1 );
		kSPracticeReward.m_iPracticeRewadEnd = kLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "reward_section%d_ment", i + 1 );
		kSPracticeReward.m_iPracticeRewadment = kLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "reward_section%d_periad", i + 1 );
		kSPracticeReward.m_iPracticeRewadperiad = kLoader.LoadInt( szKey, 0 );

		for(int k = 0;k < PracticePresentCOUNT;k++)
		{
			sprintf_s( szKey, "reward_section%d_type%d", i + 1, k + 1 );
			kSPracticeReward.m_kPresent[k].m_iPracticeRewadType = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, "reward_section%d_code%d", i + 1, k + 1 );
			kSPracticeReward.m_kPresent[k].m_iPracticeRewadCode = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, "reward_section%d_count%d", i + 1, k + 1 );
			kSPracticeReward.m_kPresent[k].m_iPracticeRewadCount = kLoader.LoadInt( szKey, 0 );
		}

		m_SPracticeReward.push_back( kSPracticeReward );

	}
	
	ProcessState(ST_NONE);

	m_bLoading = false;
	LOG.PrintTimeAndLog( 0, "Practice Load INI -> W : %d - H : %d - M : %d", m_iRankRewardWeek, m_iRankRewardHour, m_iRankRewardMinute);
}


bool CPracticeNodeManager::IsExistPracticeNode( int iPracticeIndex)
{
	mPracticeNode_iter findIter	= m_mPracticeNode.find(iPracticeIndex);
	if( findIter == m_mPracticeNode.end() )
		return false;

	return true;
}

CPracticeNode* CPracticeNodeManager::FindPracticeNode( int iPracticeIndex )
{
	mPracticeNode_iter findIter	= m_mPracticeNode.find(iPracticeIndex);
	if( findIter == m_mPracticeNode.end() )
		return NULL;

	return findIter->second;
}

CPracticeNode* CPracticeNodeManager::CreatePracticeNode( int iPracticeIndex )
{
	CPracticeNode* pkTempPractice = FindPracticeNode(iPracticeIndex );
	if(NULL != pkTempPractice)
	{
		return pkTempPractice;
	}

	CPracticeNode* pkPractice = new CPracticeNode();

	pkPractice->SetPracticeIdx(iPracticeIndex);

	m_mPracticeNode.insert( make_pair(iPracticeIndex, pkPractice) );
	
	return pkPractice;
}


void CPracticeNodeManager::SortRankAll(bool bFirst)
{
	mPracticeNode_iter iter  = m_mPracticeNode.begin();

	for( ; iter != m_mPracticeNode.end(); iter++ )
	{
		CPracticeNode *pNode = iter->second;
		if( !pNode ) continue;

		pNode->SortAll();
	}

	if(bFirst)
	{
		m_dwInitTime = TIMEGETTIME()-3000;
		m_bLoading = true;
	}
}

void CPracticeNodeManager::SortRankPracticeIndex( int iPracticeIndex )
{
	mPracticeNode_iter findIter	= m_mPracticeNode.find(iPracticeIndex);
	if( findIter == m_mPracticeNode.end() )
		return ;

	CPracticeNode *pNode = findIter->second;

	pNode->SortAll();
}

void CPracticeNodeManager::ProcessPractice()
{
		// 1분마다 Call
	switch( m_eStateType )
	{
	case ST_NONE:
		{
			if(false == m_bLoading)
			{
				return ;
			}

			if( TIMEGETTIME() - m_dwInitTime > 60000 )     //1Min
			{
				ProcessState(ST_PLAY_PROCEED);
			}
		}
		break;
	case ST_PLAY_PROCEED:
		{
			SYSTEMTIME SystemTime;
            GetLocalTime(&SystemTime);

			if(SystemTime.wDayOfWeek == m_iRankRewardWeek && SystemTime.wHour == m_iRankRewardHour && SystemTime.wMinute == m_iRankRewardMinute)
			{
				m_eStateType = ST_REWARD;
				LOG.PrintTimeAndLog( 0, "Practice StateType: Change %d - Week %d - Hour %d", (int)m_eStateType, SystemTime.wDayOfWeek, SystemTime.wHour);
			}
		}
		break;
	case ST_REWARD:
		{
			//  이곳에서 보상처리를 하고 
			PrecessReward();
		}
		break;
	case ST_STANDBY:
		{
		}
		break;
	case ST_DATA_INIT:
		{
			// 이곳에서 보상 처리가 완료되면 모든 수련장 정보를 초기화한다.
			PrecessDataInit();
		}
		break;
	}
}

void CPracticeNodeManager::ProcessState(State_Type eStateType)
{
		m_eStateType = eStateType;
}


void CPracticeNodeManager::PrecessReward()
{
	mPracticeNode_iter iter  = m_mPracticeNode.begin();

	g_DBClient.OnPractice_Index_RankPresent(m_szSenderId, m_SPracticeReward );


	ProcessState(ST_STANDBY);
}


void CPracticeNodeManager::PrecessDataInit()
{
	m_dwInitTime = TIMEGETTIME();

	// 이곳에서 초기화를 세팅한다.
	mPracticeNode_iter iter  = m_mPracticeNode.begin();

	for( ; iter != m_mPracticeNode.end(); iter++ )
	{
		CPracticeNode *pNode = iter->second;
		if( !pNode ) continue;

		pNode->Init();
	}

	// 여기서 모든 게임서버에 있는 유저의 수련장 정보를 초기화하는 패킷을 보낸다.op
	m_eStateType = ST_NONE;

	SP2Packet	kPacket( MSTPC_PRACTICE_INIT_DATA );
	g_ServerNodeManager.SendMessageAllNode( kPacket );

}

void CPracticeNodeManager::INIList(ServerNode *pSender, DWORD dwUserIndex)
{
	int iSize = m_SPracticeReward.size();
	SP2Packet kPacket( MSTPC_PRACTICE_INI_LIST );
	PACKET_GUARD_VOID( kPacket.Write( dwUserIndex ) );
	PACKET_GUARD_VOID( kPacket.Write( m_iRankRewardWeek ) );
	PACKET_GUARD_VOID( kPacket.Write( m_iRankRewardHour ) );
	PACKET_GUARD_VOID( kPacket.Write( m_iRankRewardMinute ) );
	PACKET_GUARD_VOID( kPacket.Write( iSize ) );
	if(iSize > 0)
	{
		for( int i=0 ; i< iSize; i++ )
		{ 
			SPracticeReward kPractice = m_SPracticeReward[i];
			PACKET_GUARD_VOID( kPacket.Write( kPractice.m_iPracticeRewadStart ) );
			PACKET_GUARD_VOID( kPacket.Write( kPractice.m_iPracticeRewadEnd ) );
			PACKET_GUARD_VOID( kPacket.Write( PracticePresentCOUNT ) );
			for(int k = 0;k < PracticePresentCOUNT;k++)
			{
				PACKET_GUARD_VOID( kPacket.Write( kPractice.m_kPresent[k].m_iPracticeRewadType ) );
				PACKET_GUARD_VOID( kPacket.Write( kPractice.m_kPresent[k].m_iPracticeRewadCode ) );
				PACKET_GUARD_VOID( kPacket.Write( kPractice.m_kPresent[k].m_iPracticeRewadCount ) );
			}
		}
	}

	pSender->SendMessage( kPacket );
}

void CPracticeNodeManager::BlockUserDelete(DWORD dwUserIndex)
{
	mPracticeNode_iter iter  = m_mPracticeNode.begin();

	for( ; iter != m_mPracticeNode.end(); iter++ )
	{
		CPracticeNode *pNode = iter->second;
		if( !pNode ) continue;

		pNode->BlockUserDelete(dwUserIndex);
	}
}

void CPracticeNodeManager::NickNameChange(DWORD dwUserIndex, ioHashString szNickname)
{
	mPracticeNode_iter iter  = m_mPracticeNode.begin();

	for( ; iter != m_mPracticeNode.end(); iter++ )
	{
		CPracticeNode *pNode = iter->second;
		if( !pNode ) continue;

		pNode->NickNameChange(dwUserIndex, szNickname);
	}
}

