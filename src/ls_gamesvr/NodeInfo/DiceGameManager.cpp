#include "stdafx.h"

#include "DiceGameManager.h"
#include "../Util/IORandom.h"

template<> DiceGameManager* Singleton< DiceGameManager >::ms_Singleton = 0;

DiceGameManager::DiceGameManager()
{
	Init();
}

DiceGameManager::~DiceGameManager()
{
	Destroy();
}

void DiceGameManager::Init()
{
	m_mapSnakeLadder_Move_Info.clear();
	m_mapSnakeLadder_Reward_Info.clear();		

	m_bDiceGameOpen	= false;
	m_iState			= 0;
	m_iPeriod			= 0;
	m_dwMent			= 0;
	m_iRatio			= 0;
	/////////////////////

	m_iInvalidityMax	= 0;
	m_iRewardStepMax	= 0;
	m_mapDiceGameReward.clear();

	for( int i = 0; i < OAK_BARREL_HOLE; ++i )
		m_dwRewardRandomMax[i]	= 0;
}

void DiceGameManager::Destroy()
{
	m_bDiceGameOpen	= false;
	m_iState			= 0;
	m_iPeriod			= 0;
	m_dwMent			= 0;
	m_iRatio			= 0;


	//////////////////////////////////////
	m_iInvalidityMax	= 0;
	m_iRewardStepMax	= 0;
	m_mapDiceGameReward.clear();

	for( int i = 0; i < OAK_BARREL_HOLE; ++i )
		m_dwRewardRandomMax[i]	= 0;
}

DiceGameManager& DiceGameManager::GetSingleton()
{
	return Singleton< DiceGameManager >::GetSingleton();
}

BOOL DiceGameManager::LoadINIData( const ioHashString &rkFileName )
{
	m_pSnakeLadderMoveInfoDat	= new SnakeLadders_Move_Info;
	m_pSnakeLadderRewardInfoDat	= new SnakeLadders_Reward_Info;

	if( !m_pSnakeLadderMoveInfoDat || !m_pSnakeLadderRewardInfoDat )
		return false;

	if( !m_pSnakeLadderMoveInfoDat->LoadData(SNAKELADDER_MOVE_INFO) )
		return false;

	if( !m_pSnakeLadderRewardInfoDat->LoadData(SNAKELADDER_REWARD_INFO) )
		return false;

	//실제 데이타 겟
	m_mapSnakeLadder_Move_Info.clear();
	int iTotal = m_pSnakeLadderMoveInfoDat->GetTotal();

	for( int i = 0; i < iTotal; i++ )
	{
		SnakeLadders_Move* pInfo	= m_pSnakeLadderMoveInfoDat->GetAt(i);
		if( pInfo )
		{
			SnakeLadders_Move stMove;			
			stMove.Index			= pInfo->Index;		
			stMove.SnakeGroup		= pInfo->SnakeGroup;
			stMove.SnakeStart		= pInfo->SnakeStart;
			stMove.SnakeEnd			= pInfo->SnakeEnd;

			mapSNAKELADDER_MOVE_INFO::iterator it = m_mapSnakeLadder_Move_Info.find(stMove.SnakeGroup);
			if( it != m_mapSnakeLadder_Move_Info.end() )
				(it->second).push_back(stMove);				// 있으니까 벡터에 
			else
			{
				vecSNAKELADDER_MOVE vVac;
				vVac.push_back(stMove);						// 없으니까 벡터를 새로운 맵에..
				m_mapSnakeLadder_Move_Info.insert(std::make_pair(stMove.SnakeGroup, vVac));
			}
		}
	}

	m_mapSnakeLadder_Reward_Info.clear();	
	iTotal	= m_pSnakeLadderRewardInfoDat->GetTotal();
	for( int i = 0; i < iTotal; i++ )
	{
		SnakeLadders_Reward_data* pInfo	= m_pSnakeLadderRewardInfoDat->GetAt(i);
		if( pInfo )
		{
			SnakeLadders_Reward stReward;
			stReward.Index            =	 pInfo->Index;         
			stReward.RewardGroup      =	 pInfo->RewardGroup;   

			stReward.Item_Info[0].RewardType     =	 pInfo->Reward1Type;   
			stReward.Item_Info[0].RewardValue1    =  pInfo->Reward1Value1; 
			stReward.Item_Info[0].RewardValue2    =	 pInfo->Reward1Value2; 

			stReward.Item_Info[1].RewardType     =	 pInfo->Reward2Type;   
			stReward.Item_Info[1].RewardValue1    =  pInfo->Reward2Value1; 
			stReward.Item_Info[1].RewardValue2    =	 pInfo->Reward2Value2; 

			stReward.Item_Info[2].RewardType     =	 pInfo->Reward3Type;   
			stReward.Item_Info[2].RewardValue1    =  pInfo->Reward3Value1; 
			stReward.Item_Info[2].RewardValue2    =	 pInfo->Reward3Value2; 

			stReward.Item_Info[3].RewardType     =	 pInfo->Reward4Type;   
			stReward.Item_Info[3].RewardValue1    =  pInfo->Reward4Value1; 
			stReward.Item_Info[3].RewardValue2    =	 pInfo->Reward4Value2; 

			stReward.Item_Info[4].RewardType     =	 pInfo->Reward5Type;   
			stReward.Item_Info[4].RewardValue1    =  pInfo->Reward5Value1; 
			stReward.Item_Info[4].RewardValue2    =	 pInfo->Reward5Value2; 

			stReward.Item_Info[5].RewardType     =	 pInfo->Reward6Type;   
			stReward.Item_Info[5].RewardValue1    =  pInfo->Reward6Value1; 
			stReward.Item_Info[5].RewardValue2    =	 pInfo->Reward6Value2; 

			stReward.Item_Info[6].RewardType     =	 pInfo->Reward7Type;   
			stReward.Item_Info[6].RewardValue1    =  pInfo->Reward7Value1; 
			stReward.Item_Info[6].RewardValue2    =	 pInfo->Reward7Value2; 

			stReward.Item_Info[7].RewardType     =	 pInfo->Reward8Type;   
			stReward.Item_Info[7].RewardValue1    =  pInfo->Reward8Value1; 
			stReward.Item_Info[7].RewardValue2    =	 pInfo->Reward8Value2; 

			stReward.Item_Info[8].RewardType     =	 pInfo->Reward9Type;   
			stReward.Item_Info[8].RewardValue1    =  pInfo->Reward9Value1; 
			stReward.Item_Info[8].RewardValue2    =	 pInfo->Reward9Value2; 

			stReward.Item_Info[9].RewardType     =	 pInfo->Reward10Type;   
			stReward.Item_Info[9].RewardValue1    =  pInfo->Reward10Value1; 
			stReward.Item_Info[9].RewardValue2    =	 pInfo->Reward10Value2; 

			mapSNAKELADDER_REWARD_INFO::iterator it = m_mapSnakeLadder_Reward_Info.find(stReward.RewardGroup);
			if( it != m_mapSnakeLadder_Reward_Info.end() )
				(it->second).push_back(stReward);				// 있으니까 벡터에 
			else
			{
				vecSNAKELADDER_REWARD vVac;
				vVac.push_back(stReward);						// 없으니까 벡터를 새로운 맵에..
				m_mapSnakeLadder_Reward_Info.insert(std::make_pair(stReward.RewardGroup, vVac));
			}
		}
	}

	ioINILoader kLoader( rkFileName.c_str() );

	// [common]
	kLoader.SetTitle( "common" );		
	char szSendID[MAX_PATH] = {0,};
	kLoader.LoadString( "reward_send_id", "", szSendID, MAX_PATH );			// 보상 제공자 : DevK
	m_szSendID = szSendID;
	m_iState = kLoader.LoadInt( "reward_state", 0 );						// 보상 상태
	m_iPeriod = kLoader.LoadInt( "reward_period", 0 );						// 선물함 보관 기간
	m_dwMent = kLoader.LoadInt( "reward_ment", 0 );							// 선물함 멘트 코드
	m_iRatio = kLoader.LoadInt( "board_rate", 0 );							
		
	return true;
}

// 단계별 개발자K 날아갈 확률
const DWORD DiceGameManager::GetInvalidityRate( int iStep )
{
	if( iStep < 0 && iStep > this->GetRewardStepMax() + 1 )
		return 0;

	mapInvalidityRate::iterator iter = m_mapInvalidityRate.find( iStep );
	if( iter == m_mapInvalidityRate.end() )
		return 0;
	
	DWORD dwRate = iter->second;

	return dwRate;
}

// 단계별 보상 구성품들. 확률 계산 뒤 해당 구성품 반환
const void DiceGameManager::GetOneStepReward( int iStep, BYTE &byIndex )
{
	if( iStep < 0 && iStep > this->GetRewardStepMax() + 1 )
		return;

	mapAllReward::iterator iter = m_mapDiceGameReward.find( iStep );
	if( iter == m_mapDiceGameReward.end() )
		return;

	mapOneStepReward mapReward = iter->second;

	IORandom mRand;
	mRand.SetRandomSeed( timeGetTime() );
	DWORD dwRand = mRand.Random( 0, m_dwRewardRandomMax[iStep] );
	DWORD dwCurRate = 0;

	mapOneStepReward::iterator iter2 = mapReward.begin();
	for( ; iter2 != mapReward.end(); ++iter2 )
	{
		if( COMPARE( dwRand, dwCurRate, dwCurRate + iter2->second.m_dwRate ) )
		{
			byIndex	= iter2->second.m_byIndex;
		}
		dwCurRate += iter2->second.m_dwRate;
	}
}

int  DiceGameManager::GetRNDRewardIndex(int iGroupNum)
{
	mapSNAKELADDER_REWARD_INFO::iterator it = m_mapSnakeLadder_Reward_Info.find(iGroupNum);
	if( it != m_mapSnakeLadder_Reward_Info.end() )
	{
		vecSNAKELADDER_REWARD vReward = it->second;		
		int iRewardCount = vReward.size();
		if( iRewardCount )
		{			
			int idx = (int)(rand()%iRewardCount);
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[DICE_GAME] GetRNDRewardIndex iRewardCount[%d] idx[%d] ",iRewardCount,vReward[idx] );
			return vReward[idx].Index;
		}		
	}
	return 0;
}

BYTE  DiceGameManager::GetRNDBoradIndex( BYTE BoradIndex)
{
	int iCnt = GetBoardTotalCount();
	if( iCnt == 1)
		return 1;
	////////////////////////////////////////////////////////////////
	srand(timeGetTime());	
	int ArrBoard[10] = {0,};
	int Ratio = GetRatio();
	int NormalRatio = ( 10000 - Ratio)/ (iCnt - 1);

	if( BoradIndex )
	{
		int RndValue = (int)(rand()%10000);			// 0~ 999 
		for(int i=0;i<iCnt; i++)
		{
			if( i == BoradIndex - 1 )
				ArrBoard[i] = Ratio;
			else
				ArrBoard[i] = NormalRatio;


			if( ArrBoard[i] <= RndValue )
				RndValue = RndValue - ArrBoard[i];
			else
				return i + 1;
		}
	}
	else
	{
		return (int)(rand()%iCnt) + 1;			// 0~ 999 	
	}
	return 1;
}

bool DiceGameManager::GetRewardInfoByIndex(int iGroupNum, int iIndex, int iArrNum, ioUserEtcItem::ETCITEMSLOT &rkEtcItem)
{
	mapSNAKELADDER_REWARD_INFO::iterator it = m_mapSnakeLadder_Reward_Info.find(iGroupNum);
	if( it != m_mapSnakeLadder_Reward_Info.end() )
	{
		vecSNAKELADDER_REWARD vReward = it->second;
		vecSNAKELADDER_REWARD::iterator it_Reward = vReward.begin();
		for(;it_Reward != vReward.end();it_Reward++ )
		{
			if( iIndex == it_Reward->Index )
			{
				rkEtcItem.m_iType	= it_Reward->Item_Info[iArrNum].RewardType;
				rkEtcItem.m_iValue1 = it_Reward->Item_Info[iArrNum].RewardValue1;
				rkEtcItem.m_iValue2 = it_Reward->Item_Info[iArrNum].RewardValue2;				
				return true;
			}	
		}
	}	
	return false;
}


bool DiceGameManager::GetMoveEndPosition(int iGroupNum, int iStart, int &End)
{
	mapSNAKELADDER_MOVE_INFO::iterator it = m_mapSnakeLadder_Move_Info.find(iGroupNum);
	if( it != m_mapSnakeLadder_Move_Info.end() )
	{
		vecSNAKELADDER_MOVE vMove = it->second;
		vecSNAKELADDER_MOVE::iterator it_move = vMove.begin();

		for(;it_move != vMove.end();it_move++ )
		{
			if( iStart == it_move->SnakeStart )
			{
				End = it_move->SnakeEnd;
				return true;
			}
		}
	}
	return false;
}


bool DiceGameManager::IsSnakePoint(int iGroupNum,int iPosition)
{
	mapSNAKELADDER_MOVE_INFO::iterator it = m_mapSnakeLadder_Move_Info.find(iGroupNum);
	if( it != m_mapSnakeLadder_Move_Info.end() )
	{
		vecSNAKELADDER_MOVE vMove = it->second;
		vecSNAKELADDER_MOVE::iterator it_move = vMove.begin();

		for(;it_move != vMove.end();it_move++ )
		{
			if(it_move->SnakeStart == iPosition || it_move->SnakeEnd == iPosition  )
				return true;			
		}
	}
	return false;
}



