#include "stdafx.h"
#include "ioTimeGateManager.h"
#include "ioTimeGate.h"
#include "../EtcHelpFunc.h"
#include "../Util/IORandom.h"
#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"
#include "../MainServerNode/MainServerNode.h"

ioTimeGateManager *ioTimeGateManager::sg_Instance = NULL;

ioTimeGateManager::ioTimeGateManager()
{
	Init();
}

ioTimeGateManager::~ioTimeGateManager()
{
	Destroy();
}


void ioTimeGateManager::Init()
{
	m_TimeGateInfoMap.clear();
	m_vTimeGateRandom.clear();
	m_vTimeGateReward.clear();
	m_iCoolTime		= 0;
	//m_RandomBoxRandom.Randomize();
	m_RandomBoxRandom.SetRandomSeed( timeGetTime() );
}

void ioTimeGateManager::Destroy()
{
	m_TimeGateInfoMap.clear();
	m_vTimeGateRandom.clear();
	m_vTimeGateReward.clear();
}

ioTimeGateManager& ioTimeGateManager::GetSingleton()
{
	if( !sg_Instance )
		sg_Instance = new ioTimeGateManager;

	return *sg_Instance;
}

void ioTimeGateManager::LoadINI()
{
	m_bINILoading = true;
	Init();
	char szKey[MAX_PATH]="";

	ioINILoader kLoader;
	kLoader.ReloadFile( "config/sp2_timegate_info.ini" );

	kLoader.SetTitle( "common" );
	m_iItemListCount = kLoader.LoadInt( "count", 0 );
	m_iCoolTime = kLoader.LoadInt( "need_cooltime", 0 );

	kLoader.SetTitle( "present_info" );

	m_iPresent_ment = kLoader.LoadInt( "item_ment", 0 );
	m_iPresent_period = kLoader.LoadInt( "item_period", 0 );

	
	int iTemp = 0;
	for( int i = 0; i< m_iItemListCount; i++)
	{
		TimeGateInfo stTimeGateInfo;

		char szKey[MAX_PATH] = "";

		int iCount = 0, iAccRand = 0, iAlarm = 0, iType = 0, iQuantity = 0;
		int iCode = 0, iPeriod = 0;
		
		char szTitle[MAX_PATH] = "";
		wsprintf( szTitle, "item_list%d", i+1 );
		kLoader.SetTitle( szTitle );

		stTimeGateInfo.iList = i+1;
		
		wsprintf( szKey, "item_list%d_count", i+1 );
		stTimeGateInfo.iItemCount	= kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "item_list%d_rand", i+1);
		//stTimeGateInfo.iAccRand		= kLoader.LoadInt( szKey, 0 );

		//float fRate = kLoader.LoadFloat( szKey, 0 );	// 누적 랜덤
		//fRate *= 0.01;	
		//stTimeGateInfo.iAccRand = iTemp + ( TIMEGATE_RANDOM_MAX * fRate );
		//iTemp = stTimeGateInfo.iAccRand;
		stTimeGateInfo.iRand		= kLoader.LoadInt( szKey, 0 );	// 리스트 랜덤

		wsprintf( szKey, "item_list%d_alarm", i+1);
		stTimeGateInfo.iAlarm		= kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "item_list%d_type", i+1);
		stTimeGateInfo.iType		= kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "item_list%d_quantity", i+1);
		stTimeGateInfo.iQuantity	= kLoader.LoadInt( szKey, 0 );


		for( int j = 0; j< stTimeGateInfo.iItemCount; j++)
		{
			char szKey[MAX_PATH] = "";
			wsprintf( szKey, "item_list%d_code%d", i+1, j+1 );
			iCode = kLoader.LoadInt( szKey, 0 );

			wsprintf( szKey, "item_list%d_period%d", i+1, j+1 );
			iPeriod = kLoader.LoadInt( szKey, 0);

			IntOfTwo stVec;
			stVec.value1 = iCode;
			stVec.value2 = iPeriod;

			//stTimeGateInfo.m_RewardListMap.insert( TimeGateRewardListMap::value_type( j, iPeriod ) );
			stTimeGateInfo.m_RewardListMap.insert( TimeGateRewardListMap::value_type( j+1, stVec ) );
		}
		m_TimeGateInfoMap.insert(TimeGateInfoMap::value_type(i+1, stTimeGateInfo));
		//m_vTimeGateRandom.insert( i );
		m_vTimeGateReward.push_back( stTimeGateInfo );
	}
	std::sort( m_vTimeGateReward.begin(), m_vTimeGateReward.end(), TimeGateRewardAscSort() );

	vTimeGateReward_iter iter = m_vTimeGateReward.begin();
	int iSort = 0;
	int iPrev = 0;
	for(iter = m_vTimeGateReward.begin();iter != m_vTimeGateReward.end();iter++)
	{
		TimeGateInfo &stInfo = *iter;
		float fRate = 0.01 * stInfo.iRand;	
		stInfo.iAccRand = iSort + ( TIMEGATE_RANDOM_MAX * fRate );
		iSort = stInfo.iAccRand;
	}
	std::sort( m_vTimeGateReward.begin(), m_vTimeGateReward.end(), TimeGateRewardRandomSort() );
}

void ioTimeGateManager::LoadTimeGate( ioINILoader &rkLoader )
{
	//Init();
	
}

// 랜덤 확률 가져오기
// 여기서 값 가져오면 또 어떤 아이템을 줘야 하는지 랜덤으로 돌려야함
int ioTimeGateManager::GetTimeGateRandomList()
{
	vTimeGateReward_iter iter = m_vTimeGateReward.begin();

	int iCheck = 0;
	int iTempList = 0;
	int iValue = m_RandomBoxRandom.Random( TIMEGATE_RANDOM_MAX );

	printf("TimeGate randomValue : %d\n", iValue );
	for(iter = m_vTimeGateReward.begin();iter != m_vTimeGateReward.end();iter++)
	{
		TimeGateInfo stInfo = *iter;
		TimeGateInfo& rewardInfo = stInfo;
		int nReturn = 0;

		if( iValue == 0 && stInfo.iAccRand == 0 )
		{
			nReturn = GetSameRandomReward( stInfo.iRand, rewardInfo );
			if( nReturn == -1 )
			{
				return stInfo.iList;
			}
			else
			{
				return rewardInfo.iList;
			}
		}

		if( COMPARE( iValue, iCheck, stInfo.iAccRand ) )
		{
			nReturn = GetSameRandomReward( stInfo.iRand, rewardInfo );

			if( nReturn == -1 )
			{
				return stInfo.iList;
			}
			else
			{
				return rewardInfo.iList;
			}

			//return stInfo.iList;
		}
		else
		{
			iCheck = stInfo.iAccRand;
			iTempList = stInfo.iList;
		}
	}
	return iTempList;
}

// 리스트 내에서 랜덤 아이템 구하기
int ioTimeGateManager::GetTimeGateReward( int &iItemList, int &iPresentType, int&iValue1, int& iValue2, bool& bAlarm  )
{
	int iType = 0;
	iType = GetTimeGateRandomList();

	if( iType == 0 )
		iType = GetTimeGateRandomList();

	TimeGateInfoMap::iterator it = m_TimeGateInfoMap.find( iType );

	if( it == m_TimeGateInfoMap.end() )
		return -1;
	else
	{
		TimeGateInfo &kTimeGate = it->second;
		
		if( kTimeGate.iAlarm == 1 )
			bAlarm = true;
		else
			bAlarm = false;
			
		int j =  m_RandomBoxRandom.Random( 1, kTimeGate.iItemCount );
		TimeGateRewardListMap::iterator it = kTimeGate.m_RewardListMap.find( j );
		if( it != kTimeGate.m_RewardListMap.end() )
		{
			IntOfTwo &stInt = it->second;
			iItemList = iType;
			iPresentType = kTimeGate.iType;

			// 용병인 경우는 value2 를 period 값으로 세팅
			if( iPresentType == IT_SOLDIER )
			{
				iValue1 = stInt.value1;
				iValue2 = stInt.value2;	
			}
			// 페소인 경우는 value2 는 무조건 0
			else if( iPresentType == IT_PESO )
			{
				iValue1 = stInt.value1;
				iValue2 = 0;	
			}
			// 그외 아이템은 value2 를 수량에서 세팅한다. 
			else
			{
				iValue1 = stInt.value1;
				iValue2 = kTimeGate.iQuantity;
			}
			printf("TimeGate Reward presentType : %d, value1:%d, value2:%d, rand:%d, listType:%d, accRand:%d\n", iPresentType, iValue1, iValue2, kTimeGate.iRand, iItemList, kTimeGate.iAccRand );
		}
	}
	/*
	UserAccessoryItem::iterator it = m_mUserAccessoryMap.find(dwIndex);

	if( it == m_mUserAccessoryMap.end() )
	return false;	//없는 아이템

	m_mUserAccessoryMap.erase(it);

	*/


	return 0;
	//ioOakBarrelManager::mapAllReward::iterator iter1 = mapRewardAll.find( i );
}

void ioTimeGateManager::SendTimeGatePresent( int iPresentType, int iValue1, int iValue2, User* pUser  )
{
	// 선물 지급
	if( !pUser )
		return;

	CTimeSpan cPresentGapTime( m_iPresent_period, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;

	pUser->AddPresentMemory( g_MainServer.GetSendID(), iPresentType, iValue1, iValue2, 
		0, 0, m_iPresent_ment, kPresentTime, 0 );

	g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), pUser->GetPublicIP(), pUser->GetUserIndex(), iPresentType, iValue1, 
		iValue2, 0, 0, LogDBClient::PST_TIMEGATE, "TimeGate" );

	pUser->SendPresentMemory();

}

// 같은 확률을 가진 아이템 중에서 어떤 아이템 지급 할지 결정
int ioTimeGateManager::GetSameRandomReward( int iRand, TimeGateInfo& rewardInfo )
{
	vTimeGateReward vTempReward;

	vTimeGateReward_iter iter = m_vTimeGateReward.begin();
	for(iter = m_vTimeGateReward.begin();iter != m_vTimeGateReward.end();iter++)
	{
		TimeGateInfo stInfo = *iter;
		if ( stInfo.iRand == iRand )
		{
			vTempReward.push_back( stInfo );
		}
	}

	io_random_shuffle( vTempReward.begin(), vTempReward.end() );
	if( !vTempReward.empty() )
	{
		rewardInfo = vTempReward[0];
		return 0;
	}
	else
	{
		return -1;
	}
}