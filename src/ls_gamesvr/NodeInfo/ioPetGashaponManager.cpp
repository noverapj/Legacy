#include "stdafx.h"
#include "ioPetGashaponManager.h"
#include "ioPetInfoManager.h"
#include "ioUserPet.h"
#include "User.h"
#include "../DataBase/LogDBClient.h"
#include <strsafe.h>

template<> ioPetGashaponManager *Singleton< ioPetGashaponManager >::ms_Singleton = 0;

ioPetGashaponManager::ioPetGashaponManager()
{
	m_PetRankRandom.SetRandomSeed( timeGetTime() );
	m_PetSearchRandom.SetRandomSeed( timeGetTime()+1 );
	m_PetCompoundRandom.SetRandomSeed( timeGetTime()+2 ); 
}

ioPetGashaponManager::~ioPetGashaponManager()
{
	ClearAll();
}

ioPetGashaponManager& ioPetGashaponManager::GetSingleton()
{
	return Singleton< ioPetGashaponManager >::GetSingleton(); 
}

void ioPetGashaponManager::ClearAll()
{
	m_vPetRankRandom.clear();
	m_vAllRankPetRandomInfo.clear();
	m_vGradedTag.clear();
	m_vRandomTotalInRank.clear();
}

void ioPetGashaponManager::SetGradeTag( const int iStartIndex, const int iEndIndex )
{
	RankVectorTag rkTag;

	rkTag.iStartIndex = iStartIndex;
	rkTag.iEndIndex	  = iEndIndex;

	m_vGradedTag.push_back( rkTag );
}

void ioPetGashaponManager::CheckNeedReload()
{
	ioINILoader kLoader;
	kLoader.SetFileName( "config/sp2_pet_gashapon.ini" );
	if( kLoader.ReadBool( "common", "Change", false ) )
	{
		LoadINI();
	}
}

void ioPetGashaponManager::LoadINI()
{
	ClearAll();

	char szKey[MAX_PATH]="";

	ioINILoader kLoader( "config/sp2_pet_gashapon.ini" );

	kLoader.SetTitle( "common" );
	int iTotalCount = kLoader.LoadInt( "total_pet_count", 0 );
	m_vAllRankPetRandomInfo.reserve( iTotalCount );

	kLoader.SetTitle( "buy_random_limit" );
	m_rkPesoBuyLimitRank.iStartRank = kLoader.LoadInt( "peso_start_rank", 0 );
	m_rkPesoBuyLimitRank.iEndRank = kLoader.LoadInt( "peso_end_rank", 0 );
	m_rkGoldBuyLimitRank.iStartRank = kLoader.LoadInt( "gold_start_rank", 0 );
	m_rkGoldBuyLimitRank.iEndRank = kLoader.LoadInt( "gold_end_rank", 0 );

	kLoader.SetTitle( "rank_random" );
	int iRankCount	=	kLoader.LoadInt( "max_rank_count", 0 );
	m_vPetRankRandom.reserve( iRankCount );
	m_vRandomTotalInRank.reserve( iRankCount );
	
	if( iRankCount == 0 )
		return;

	for( int i=0; i < iRankCount; i++ )
	{
		int iRankRandom;
		StringCbPrintf( szKey, sizeof( szKey ), "rank%d_random", i+1 );
		iRankRandom = kLoader.LoadInt( szKey, 0 );

		m_vPetRankRandom.push_back( iRankRandom );

		m_vRandomTotalInRank.push_back( 0 );
	}

	kLoader.SetTitle( "rank_D" );
	int iStartIndex = 0;
	int iSize = kLoader.LoadInt( "max_pet_count", -1 );
	int iEndIndex = iSize;
	SetGradeTag( iStartIndex, iEndIndex-1 );
	
	for( int i=0; i < iSize; i++ )
	{
		PetRandomInfo rkPetRandomInfo;

		StringCbPrintf( szKey, sizeof( szKey ), "pet%d_rand", i+1 );
		rkPetRandomInfo.iPetRandom = kLoader.LoadInt( szKey, -1 );

		StringCbPrintf( szKey, sizeof( szKey ), "pet%d_type", i+1 );
		rkPetRandomInfo.iPresentType = kLoader.LoadInt( szKey, -1 );

		StringCbPrintf( szKey, sizeof( szKey ), "pet%d_value1", i+1 );
		rkPetRandomInfo.iPetCode = kLoader.LoadInt( szKey, -1 );

		StringCbPrintf( szKey, sizeof( szKey ), "pet%d_value2", i+1 );
		rkPetRandomInfo.iPetStartRank = kLoader.LoadInt( szKey, -1 ); 

		m_vAllRankPetRandomInfo.push_back( rkPetRandomInfo );
		m_vRandomTotalInRank[RANKD-1] += rkPetRandomInfo.iPetRandom;
	}

	kLoader.SetTitle( "rank_C" );
	iStartIndex = iEndIndex;
	iSize = kLoader.LoadInt( "max_pet_count", -1 );
	iEndIndex = iSize + iStartIndex;
	SetGradeTag( iStartIndex, iEndIndex-1 );
	
	for( int i=0; i < iSize; i++ )
	{
		PetRandomInfo rkPetRandomInfo;

		StringCbPrintf( szKey, sizeof( szKey ), "pet%d_rand", i+1 );
		rkPetRandomInfo.iPetRandom = kLoader.LoadInt( szKey, -1 );

		StringCbPrintf( szKey, sizeof( szKey ), "pet%d_type", i+1 );
		rkPetRandomInfo.iPresentType = kLoader.LoadInt( szKey, -1 );

		StringCbPrintf( szKey, sizeof( szKey ), "pet%d_value1", i+1 );
		rkPetRandomInfo.iPetCode = kLoader.LoadInt( szKey, -1 );

		StringCbPrintf( szKey, sizeof( szKey ), "pet%d_value2", i+1 );
		rkPetRandomInfo.iPetStartRank = kLoader.LoadInt( szKey, -1 ); 

		m_vAllRankPetRandomInfo.push_back( rkPetRandomInfo );
		m_vRandomTotalInRank[RANKC-1] += rkPetRandomInfo.iPetRandom;
	}

	kLoader.SetTitle( "rank_B" );
	iStartIndex = iEndIndex;
	iSize = kLoader.LoadInt( "max_pet_count", -1 );
	iEndIndex = iSize + iStartIndex;
	SetGradeTag( iStartIndex, iEndIndex-1 );
	
	for( int i=0; i < iSize; i++ )
	{
		PetRandomInfo rkPetRandomInfo;

		StringCbPrintf( szKey, sizeof( szKey ), "pet%d_rand", i+1 );
		rkPetRandomInfo.iPetRandom = kLoader.LoadInt( szKey, -1 );

		StringCbPrintf( szKey, sizeof( szKey ), "pet%d_type", i+1 );
		rkPetRandomInfo.iPresentType = kLoader.LoadInt( szKey, -1 );

		StringCbPrintf( szKey, sizeof( szKey ), "pet%d_value1", i+1 );
		rkPetRandomInfo.iPetCode = kLoader.LoadInt( szKey, -1 );

		StringCbPrintf( szKey, sizeof( szKey ), "pet%d_value2", i+1 );
		rkPetRandomInfo.iPetStartRank = kLoader.LoadInt( szKey, -1 ); 

		m_vAllRankPetRandomInfo.push_back( rkPetRandomInfo );
		m_vRandomTotalInRank[RANKB-1] += rkPetRandomInfo.iPetRandom;
	}

	kLoader.SetTitle( "rank_A" );
	iStartIndex = iEndIndex;
	iSize = kLoader.LoadInt( "max_pet_count", -1 );
	iEndIndex = iSize + iStartIndex;
	SetGradeTag( iStartIndex, iEndIndex-1 );
	
	for( int i=0; i < iEndIndex; i++ )
	{
		PetRandomInfo rkPetRandomInfo;

		StringCbPrintf( szKey, sizeof( szKey ), "pet%d_rand", i+1 );
		rkPetRandomInfo.iPetRandom = kLoader.LoadInt( szKey, -1 );

		StringCbPrintf( szKey, sizeof( szKey ), "pet%d_type", i+1 );
		rkPetRandomInfo.iPresentType = kLoader.LoadInt( szKey, -1 );

		StringCbPrintf( szKey, sizeof( szKey ), "pet%d_value1", i+1 );
		rkPetRandomInfo.iPetCode = kLoader.LoadInt( szKey, -1 );

		StringCbPrintf( szKey, sizeof( szKey ), "pet%d_value2", i+1 );
		rkPetRandomInfo.iPetStartRank = kLoader.LoadInt( szKey, -1 ); 

		m_vAllRankPetRandomInfo.push_back( rkPetRandomInfo );
		m_vRandomTotalInRank[RANKA-1] += rkPetRandomInfo.iPetRandom;
	}

	kLoader.SetTitle( "rank_S" );
	iStartIndex = iEndIndex;
	iSize = kLoader.LoadInt( "max_pet_count", -1 );
	iEndIndex = iSize + iStartIndex;
	SetGradeTag( iStartIndex, iEndIndex-1 );

	for( int i=0; i < iEndIndex; i++ )
	{
		PetRandomInfo rkPetRandomInfo;

		StringCbPrintf( szKey, sizeof( szKey ), "pet%d_rand", i+1 );
		rkPetRandomInfo.iPetRandom = kLoader.LoadInt( szKey, -1 );

		StringCbPrintf( szKey, sizeof( szKey ), "pet%d_type", i+1 );
		rkPetRandomInfo.iPresentType = kLoader.LoadInt( szKey, -1 );

		StringCbPrintf( szKey, sizeof( szKey ), "pet%d_value1", i+1 );
		rkPetRandomInfo.iPetCode = kLoader.LoadInt( szKey, -1 );

		StringCbPrintf( szKey, sizeof( szKey ), "pet%d_value2", i+1 );
		rkPetRandomInfo.iPetStartRank = kLoader.LoadInt( szKey, -1 ); 

		m_vAllRankPetRandomInfo.push_back( rkPetRandomInfo );
		m_vRandomTotalInRank[RANKS-1] += rkPetRandomInfo.iPetRandom;
	}

	kLoader.SetTitle( "diff_pet_compound_rate" );
	m_rkDiffPetCompoundRate.iSuccessRate = kLoader.LoadInt( "success_rate", 0 );
	m_rkDiffPetCompoundRate.iFailRate    = kLoader.LoadInt( "fail_rate", 0 );
}

int ioPetGashaponManager::GetRankRandomTotal( const int iStartRank, const int iEndRank )
{
	int iTotalRate = 0;
	int iRandomVecSize = m_vPetRankRandom.size();

	if( iEndRank >  iRandomVecSize )
		return 0;

	for( int i = iStartRank-1; i < iEndRank; i++ )
	{
		iTotalRate += m_vPetRankRandom[ i ];
	}

	return iTotalRate;
}

int ioPetGashaponManager::GetRankWithRandom( const int iRandom, const int iStartRank, const int iEndRrank )
{
	int iCurPosition = 0;

	for( int i = iStartRank-1; i < iEndRrank; i++ )
	{
		int iCurRate = m_vPetRankRandom[i];
		if( COMPARE( iRandom, iCurPosition, iCurPosition+iCurRate ) )
		{
			return i+1;		//펫 랭크 D는 1, S는 5
		}

		iCurPosition += iCurRate;
	}

	return -1;
}

int ioPetGashaponManager::GetRankWithRandom( const int iRandom )
{
	int iRankrandomSize = m_vPetRankRandom.size();
	int iCurPosition = 0;

	for( int i=0; iRankrandomSize; i++ )
	{
		int iCurRate = m_vPetRankRandom[i];
		if( COMPARE( iRandom, iCurPosition, iCurPosition+iCurRate ) )
		{
			return i+1;		//펫 랭크 D는 1, S는 5
		}

		iCurPosition += iCurRate;
	}

	return -1;
}

int ioPetGashaponManager::GetRandomPetRank( bool bCash )
{
	int iRandomValue = 0;
	int iRandomTotal = 0;
	int iStartRank = 0;
	int iEndRank = 0;

	if( bCash )
	{
		iStartRank = m_rkGoldBuyLimitRank.iStartRank;
		iEndRank = m_rkGoldBuyLimitRank.iEndRank;
		iRandomTotal = GetRankRandomTotal( iStartRank, iEndRank );
		if( iRandomTotal == 0 )
			return -1;

		iRandomValue =  m_PetRankRandom.Random( iRandomTotal ) ;
	}
	else
	{
		iStartRank = m_rkPesoBuyLimitRank.iStartRank;
		iEndRank = m_rkPesoBuyLimitRank.iEndRank;
		iRandomTotal = GetRankRandomTotal( iStartRank, iEndRank );
		if( iRandomTotal == 0 )
			return -1;

		iRandomValue = m_PetRankRandom.Random( iRandomTotal );
	}
	
	return GetRankWithRandom( iRandomValue, iStartRank, iEndRank );
}

void ioPetGashaponManager::GetRandomTotalRateInRank( const int& iRank, int &iTotalRate )
{
	for( int i = 0; i<iRank; i++ )
	{
		iTotalRate += m_vRandomTotalInRank[i];
	}
}

int ioPetGashaponManager::GetPetCodeWithRandom( const int iRank, const int iRandom )
{
	int iCurPos = 0;

	PetRandomInfo rkGetPet;

	for( int i = 0; i <= m_vGradedTag[iRank-1].iEndIndex; i++ )
	{
		int iCurRate = m_vAllRankPetRandomInfo[i].iPetRandom;
		if( COMPARE( iRandom, iCurPos, iCurPos+iCurRate ) )
		{
			return m_vAllRankPetRandomInfo[i].iPetCode;
		}

		iCurPos += iCurRate;
	}

	return 0;
}

void SetPetSlot( ioUserPet::PETSLOT& rkPetInfo, const int& iPetCode, const int& iRank )
{
	rkPetInfo.Init();
	rkPetInfo.m_iPetCode = iPetCode;
	rkPetInfo.m_iPetRank = iRank;
}

int ioPetGashaponManager::GetPetCodeInRank(  const int& iRank )
{
	int iRandomValue = 0;
	int iRandomTotalValue = 0;

	//해당 랭크안의 모든 펫의 랜덤값을 합한 값을 가져온다
	GetRandomTotalRateInRank( iRank, iRandomTotalValue );

	if( iRandomTotalValue == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ioPetGashaponManager::GetRandomTotalRateInRank Error - RandomTotalValue Zero" );
		return -1;
	}

	iRandomValue = m_PetSearchRandom.Random( iRandomTotalValue );

	int iPetCode = GetPetCodeWithRandom( iRank, iRandomValue );

	if( iPetCode == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ioPetGashaponManager::GetPetCodeWithRandom Error -  exceed randomValue" );
		return -1;
	}

	return iPetCode;
}

bool ioPetGashaponManager::GetPetInRank( const int& iRank, ioUserPet::PETSLOT& rkPetInfo )
{
	int iRandomValue = 0;
	int iRandomTotalValue = 0;

	//해당 랭크안의 모든 펫의 랜덤값을 합한 값을 가져온다
	GetRandomTotalRateInRank( iRank, iRandomTotalValue );

	if( iRandomTotalValue == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ioPetGashaponManager::GetRandomTotalRateInRank Error - RandomTotalValue Zero" );
		return false;
	}

	//해당 랭크의 펫을 뽑는다
	iRandomValue = m_PetSearchRandom.Random( iRandomTotalValue );

	int iPetCode = GetPetCodeWithRandom( iRank, iRandomValue );
	if( iPetCode == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ioPetGashaponManager::GetPetCodeWithRandom Error -  exceed randomValue" );
		return false;
	}

	SetPetSlot( rkPetInfo, iPetCode, iRank );

	return true;
}

bool ioPetGashaponManager::PickBuyRandomPet( ioUserPet::PETSLOT& rkPetInfo, bool bCash )
{
	//획득할 펫 랭크 선택
	int iPetRank = GetRandomPetRank( bCash );

	if( iPetRank == -1 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ioPetGashaponManager::GetRandomPet - RankRandomTotal Value Error" );
		return false;
	}

	if( !GetPetInRank( iPetRank, rkPetInfo ) )
		return false;
	
	return true;
}

void SetQueryData( CQueryData &query_data, User *pUser, DWORD dwTargetIndex, DWORD dwVictimIndex )
{
	DWORD dwPacketID = STPK_PET_COMPOUND;
	DWORD dwUserIndex = pUser->GetUserIndex();
	query_data.SetReturnData( &dwUserIndex, sizeof( DWORD ) );
	//query_data.SetReturnData( pUser->GetPublicID().c_str(), ID_NUM_PLUS_ONE );
	query_data.SetReturnData( &dwPacketID, sizeof( DWORD ) );
	query_data.SetReturnData( &dwTargetIndex, sizeof( DWORD ) );
	query_data.SetReturnData( &dwVictimIndex, sizeof( DWORD ) );
}

//펫 교배
bool ioPetGashaponManager::FinalRankCompoundResult( User *pUser, ioUserPet::PETSLOT& rkTargetPet, ioUserPet::PETSLOT& rkVictimPet )
{
	if( !pUser )
		return false;

	//모든 펫 고르고 그중 하나 선택
	ioUserPet::PETSLOT rkPetInfo;

	int iCode = GetPetCodeInRank( g_PetInfoMgr.GetPetMaxRank() );

	if( iCode == -1 )
		return false;

	//코드가 있는 코드인지 확인
	if( !g_PetInfoMgr.CheckRightPetCode( iCode ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ioPetGashaponManager::FinalRankCompoundResult Error - Pet Code not exist %d", iCode );
		return false;
	}

	rkPetInfo.m_iPetCode = iCode;
	rkPetInfo.m_iPetRank = rkTargetPet.m_iPetRank;
	rkPetInfo.m_iCurLevel = rkTargetPet.m_iCurLevel;

	// 교배 로그
	g_LogDBClient.OnInsertPetLog( pUser, rkTargetPet.m_iIndex, rkTargetPet.m_iPetCode, rkTargetPet.m_iPetRank, rkTargetPet.m_iCurLevel, rkTargetPet.m_iCurExp, 0, LogDBClient::PDT_COMPOUND, rkVictimPet.m_iIndex );

	//남은 두 펫은 삭제 처리, 새로운 팻 생성처리
	pUser->GetUserPetItem()->DeleteData( rkTargetPet.m_iIndex );
	pUser->GetUserPetItem()->DeleteData( rkVictimPet.m_iIndex );

	CQueryData query_data;
	SetQueryData( query_data, pUser, rkTargetPet.m_iIndex, rkVictimPet.m_iIndex );
	pUser->GetUserPetItem()->AddData( rkPetInfo, query_data );

	return true;
}

bool ioPetGashaponManager::SamePetCompoundResult( User *pUser, ioUserPet::PETSLOT& rkTargetPet, ioUserPet::PETSLOT& rkVictimPet )
{
	if( !pUser )
		return false;

	//성공 시 타겟 펫이 랭크업된 종류 전송
	ioUserPet::PETSLOT rkPetInfo;
	rkPetInfo.m_iPetCode = rkTargetPet.m_iPetCode;		
	rkPetInfo.m_iPetRank = rkTargetPet.m_iPetRank+1;
	rkPetInfo.m_iCurLevel = rkTargetPet.m_iCurLevel;

	g_LogDBClient.OnInsertPetLog( pUser, rkTargetPet.m_iIndex, rkTargetPet.m_iPetCode, rkTargetPet.m_iPetRank, rkTargetPet.m_iCurLevel, rkTargetPet.m_iCurExp, 0, LogDBClient::PDT_COMPOUND, rkVictimPet.m_iIndex );
		
	pUser->GetUserPetItem()->DeleteData( rkVictimPet.m_iIndex );
	pUser->GetUserPetItem()->DeleteData( rkTargetPet.m_iIndex );

	CQueryData query_data;
	SetQueryData( query_data, pUser, rkTargetPet.m_iIndex, rkVictimPet.m_iIndex );
	pUser->GetUserPetItem()->AddData( rkPetInfo, query_data );
	return true;
}

bool ioPetGashaponManager::CheckCompoundSuccess( )
{
	int iTotalRandom = m_rkDiffPetCompoundRate.iFailRate + m_rkDiffPetCompoundRate.iSuccessRate;
	int iRandom = m_PetCompoundRandom.Random( iTotalRandom );

	if( COMPARE( iRandom, 0, m_rkDiffPetCompoundRate.iFailRate ) )
		return false;

	return true;
}

bool ioPetGashaponManager::DiffPetCompoundResult( User *pUser, ioUserPet::PETSLOT& rkTargetPet, ioUserPet::PETSLOT& rkVictimPet )
{
	if( !pUser )
		return false;
	
	//성공 여부 확인
	bool bResult = CheckCompoundSuccess();
	ioUserPet::PETSLOT rkPetInfo;

	if( bResult )
	{
		//성공 시 타겟 펫이 랭크업된 종류 전송
		rkPetInfo.m_iPetCode = rkTargetPet.m_iPetCode;
		rkPetInfo.m_iPetRank = rkTargetPet.m_iPetRank+1;
	}
	else
	{
		//실패 시 둘다 지우고 하나 새로운 놈으로 선택
		int iCode = GetPetCodeInRank( rkTargetPet.m_iPetRank );

		if( iCode == -1 )
			return false;

		//코드가 있는 코드인지 확인
		if( !g_PetInfoMgr.CheckRightPetCode( iCode ) )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ioPetGashaponManager::FinalRankCompoundResult Error - Pet Code not exist %d", iCode );
			return false;
		}

		rkPetInfo.m_iPetCode = iCode;
		rkPetInfo.m_iPetRank = rkTargetPet.m_iPetRank;
	}
	rkPetInfo.m_iCurLevel = rkTargetPet.m_iCurLevel;

	g_LogDBClient.OnInsertPetLog( pUser, rkTargetPet.m_iIndex, rkTargetPet.m_iPetCode, rkTargetPet.m_iPetRank, rkTargetPet.m_iCurLevel, rkTargetPet.m_iCurExp, 0, LogDBClient::PDT_COMPOUND, rkVictimPet.m_iIndex );
	
	pUser->GetUserPetItem()->DeleteData( rkVictimPet.m_iIndex );
	pUser->GetUserPetItem()->DeleteData( rkTargetPet.m_iIndex );

	CQueryData query_data;
	SetQueryData( query_data, pUser, rkTargetPet.m_iIndex, rkVictimPet.m_iIndex );
	pUser->GetUserPetItem()->AddData( rkPetInfo, query_data );

	return true;
}

bool ioPetGashaponManager::IsMaxLevel( ioUserPet::PETSLOT& rkTargetPet, ioUserPet::PETSLOT& rkVictimPet )
{
	int iMaxLevel = g_PetInfoMgr.GetPetMaxLevel( rkTargetPet.m_iPetRank );

	if( iMaxLevel == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ioPetGashaponManager::IsMaxLevel Error - PetRank is None" );
		return false;
	}

	if( rkTargetPet.m_iCurLevel != iMaxLevel || rkVictimPet.m_iCurLevel != iMaxLevel )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ioPetGashaponManager::IsMaxLevel Error - MaxLevel is Fail" );
		return false;
	}

	return true;
}

int ioPetGashaponManager::PetCompound( User *pUser, ioUserPet::PETSLOT& rkTargetPet, ioUserPet::PETSLOT& rkVictimPet )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ioPetGashaponManager::PetCompound Error - User NULL" );
		return 1;
	}

	if( rkTargetPet.m_iPetRank != rkVictimPet.m_iPetRank )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ioPetGashaponManager::PetCompound Error - Pet Rank Not Same %s %d-%d", pUser->GetPublicID().c_str(), rkTargetPet.m_iPetRank, rkVictimPet.m_iPetRank );
		return 2;
	}

	if( !IsMaxLevel( rkTargetPet, rkVictimPet ) )
		return 3;

	if( rkTargetPet.m_bEquip || rkVictimPet.m_bEquip )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ioPetGashaponManager::PetCompound Error - Pet is Equip" );
		return 4;
	}

	if( g_PetInfoMgr.GetPetMaxRank() == rkTargetPet.m_iPetRank )
	{
		//S랭크 둘을 교배시 S등급으로 랜덤펫 지급
		if( !FinalRankCompoundResult( pUser, rkTargetPet, rkVictimPet ) )
			return 5;
	}
	else if( rkTargetPet.m_iPetCode == rkVictimPet.m_iPetCode )
	{
		//동일한 펫을 교배
		if( !SamePetCompoundResult( pUser, rkTargetPet, rkVictimPet ) )
			return 6;
	}
	else
	{
		//다른 펫을 교배
		if( !DiffPetCompoundResult( pUser, rkTargetPet, rkVictimPet ) )
			return 7;
	}

	return 0;
}


bool ioPetGashaponManager::IsRightPetRank( const int iRank, const int iPetCode )
{
	//해당 랭크부터 최저 랭크까지 펫 정보가 존재 하는지 확인.
	int iSize = m_vAllRankPetRandomInfo.size();

	for( int i=0; i<iSize; i++ )
	{
		if( m_vAllRankPetRandomInfo[i].iPetCode == iPetCode )
		{
			if( m_vAllRankPetRandomInfo[i].iPetStartRank > iRank || iRank > g_PetInfoMgr.GetPetMaxRank() )
				return false;
			
			return true;
		}
	}
	return false;
}