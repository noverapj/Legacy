#include "stdafx.h"
#include "ioPetInfoManager.h"
#include "User.h"
#include "ioEtcItemManager.h"
#include <strsafe.h>

template<> ioPetInfoManager *Singleton< ioPetInfoManager >::ms_Singleton = 0;

ioPetInfoManager::ioPetInfoManager()
{
}

ioPetInfoManager::~ioPetInfoManager()
{
	Clear();
}

void ioPetInfoManager::Clear()
{
	m_iMaxPetCount = 0;
	m_iPetSellPeso = 0;
	m_fMaxExpConst = 0.0f;
	m_iMaxRank = 0;
	m_vPetInfoVec.clear();
	m_vPetRankInfoVec.clear();
}

ioPetInfoManager& ioPetInfoManager::GetSingleton()
{
	return Singleton< ioPetInfoManager >::GetSingleton();
}

void ioPetInfoManager::CheckNeedReload()
{
	ioINILoader kLoader;
	kLoader.SetFileName( "config/sp2_pet_info.ini" );
	if( kLoader.ReadBool( "common", "Change", false ) )
	{
		LoadINI();
	}
}

void ioPetInfoManager::LoadINI()
{
	Clear();
	char szKey[MAX_PATH]="";

	ioINILoader kLoader( "config/sp2_pet_info.ini" );

	kLoader.SetTitle( "common" );

	m_iMaxPetCount = kLoader.LoadInt( "max_have_pet", 0 );
	m_fMaxExpConst = kLoader.LoadFloat( "max_exp_const", 0.0 );
	m_iPetSellPeso = kLoader.LoadFloat( "sell_add_peso", 0.0 );
	m_iPetEatAdditiveCount = kLoader.LoadInt( "pet_eat_additive_count", 0 );
	m_iPetEatAdditiveExp = kLoader.LoadInt( "pet_eat_additive_exp", 0 );

	kLoader.SetTitle( "pet_rank_info" );
	m_iMaxRank = kLoader.LoadInt( "max_rank_info_cnt", 0 );

	for( int i = 0; i < m_iMaxRank; i++ )
	{
		PetRankInfo rkPetRankInf;

		StringCbPrintf( szKey, sizeof( szKey ), "rank_info%d_type", i+1 );
		rkPetRankInf.iRank				= kLoader.LoadInt( szKey, -1 );

		StringCbPrintf( szKey, sizeof( szKey ), "rank_info%d_max_level", i+1 );
		rkPetRankInf.iRankMaxLevel		= kLoader.LoadInt( szKey, -1 );

		StringCbPrintf( szKey, sizeof( szKey ), "rank_info%d_value", i+1 );
		rkPetRankInf.iRankConst			= kLoader.LoadFloat( szKey, 0.0f );

		m_vPetRankInfoVec.push_back( rkPetRankInf );
	}

	kLoader.SetTitle( "pet_base_common" );
	int iMaxPetCount = kLoader.LoadInt( "max_base_info_cnt", 0 );

	for( int i=0; i < iMaxPetCount; i++ )
	{
		PetInfo rkPetInfo;

		StringCbPrintf( szKey, sizeof( szKey ), "pet_base_info%d", i+1 );
		kLoader.SetTitle( szKey );

		rkPetInfo.iPetCode				= kLoader.LoadInt( "pet_code", 0 );
		//rkPetInfo.iStartRank			= kLoader.LoadInt( "pet_base_rank", 0 );
		//rkPetInfo.iMaxRank				= kLoader.LoadInt( "pet_max_rank", 0 );

		m_vPetInfoVec.push_back( rkPetInfo );
	}
}

int ioPetInfoManager::GetSellPeso()
{
	return m_iPetSellPeso;
}

float ioPetInfoManager::GetRankConst( const int& iPetRank )
{
	int iPetRankCount = m_vPetRankInfoVec.size();
	for( int i = 0; i < iPetRankCount; i++ )
	{
		if( m_vPetRankInfoVec[i].iRank == iPetRank )
			return m_vPetRankInfoVec[i].iRankConst;
	}

	return 0;
}

ioPetInfoManager::PetRankInfo* ioPetInfoManager::GetPetRankInfo( const int& iPetRank )
{
	int iPetRankCount = m_vPetRankInfoVec.size();
	for( int i = 0; i < iPetRankCount; i++ )
	{
		if( m_vPetRankInfoVec[i].iRank == iPetRank )
			return &m_vPetRankInfoVec[i];
	}
	return NULL;
}

ioPetInfoManager::PetInfo*	ioPetInfoManager::GetPetInfo( const int& iPetCode )
{
	int iPetCount = m_vPetInfoVec.size();

	for( int i = 0; i < iPetCount; i++ )
	{
		if( m_vPetInfoVec[i].iPetCode == iPetCode )
			return &m_vPetInfoVec[i];
	}

	return NULL;
}

void ioPetInfoManager::SetPetLevelUpInfo( ioUserPet::PETSLOT& rkPetSlot, int iAddExp, int iRankMaxLevel )
{
	if( rkPetSlot.m_iMaxExp == 0 )
		SetMaxExp( rkPetSlot );

	int iCurLevel = rkPetSlot.m_iCurLevel;
	int iMaxExp = rkPetSlot.m_iMaxExp;
	int iCurExp = rkPetSlot.m_iCurExp;
	int iLevelCount = 0;

	while( true )
	{
		iCurLevel++;
		if( iCurLevel > iRankMaxLevel )
			break;

		iLevelCount++;
		iAddExp = iAddExp - ( iMaxExp - iCurExp ); //레벨업 후 남은 경험치.
		iCurExp = 0;

		iMaxExp = GetMaxExp( iCurLevel, rkPetSlot.m_iPetRank );
		if( iAddExp >= iMaxExp )
			continue;
		else
			break;
	}

	rkPetSlot.m_iCurLevel += iLevelCount;
	rkPetSlot.m_iCurExp = iAddExp;
	rkPetSlot.m_iMaxExp = iMaxExp;

	if( iCurLevel >= iRankMaxLevel )
		rkPetSlot.m_iCurExp = 0;
}

int ioPetInfoManager::LevelUpCheck( ioUserPet::PETSLOT& rkPetSlot, int& iAddExp, const int& iRankMaxLevel  )
{
	if( rkPetSlot.m_iCurLevel+1 > iRankMaxLevel )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ioPetInfoManager::LevelUpCheck Error - PetLevel Max" );
		return -1;
	}

	if ( (rkPetSlot.m_iMaxExp - rkPetSlot.m_iCurExp) > iAddExp )
		return 0;

	return 1;
}

bool ioPetInfoManager::AddExp( User *pUser, ioUserPet::PETSLOT& rkPetSlot, const int& iEtcItemCode )
{
	int iMaxLevel = GetPetMaxLevel( rkPetSlot.m_iPetRank );

	if( iMaxLevel == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ioPetInfoManager::AddExp Error - No Pet Rank Max Level" );
		return false;
	}

	int iAddExp = GetAddExp( rkPetSlot, iEtcItemCode );
	int iResult = LevelUpCheck( rkPetSlot, iAddExp, iMaxLevel );

	if( iResult == 1 )
	{
		//레벨 업 
		SetPetLevelUpInfo( rkPetSlot, iAddExp, iMaxLevel );
	}
	else if( iResult == 0 )
	{
		//경험치만 추가
		rkPetSlot.m_iCurExp += iAddExp;
	}
	else
		return false; //에러

	pUser->GetUserPetItem()->SetPetData( rkPetSlot );
	return true;
}

int ioPetInfoManager::GetMaxExp( int iCurLevel, int iPetRank )
{
	float fRankConst = GetRankConst( iPetRank );
	if( fRankConst == 0 )
	{
		return 0;
	}

	return ( ( iCurLevel +1 ) * m_fMaxExpConst ) * fRankConst;
}

bool ioPetInfoManager::SetMaxExp( ioUserPet::PETSLOT& rkPetSlot )
{
	float fRankConst = GetRankConst( rkPetSlot.m_iPetRank );
	if( fRankConst == 0 )
	{
		rkPetSlot.m_iMaxExp = 0;
		return false;
	}

	rkPetSlot.m_iMaxExp = ( ( rkPetSlot.m_iCurLevel + 1 ) * m_fMaxExpConst ) * fRankConst ;
	return true;
}

int ioPetInfoManager::GetAddExp( const ioUserPet::PETSLOT& rkPetSlot, const int& iEtcItemCode )
{
	if( iEtcItemCode == ioEtcItem::EIT_ETC_ADDICTIVE_PIECE )
		return m_iPetEatAdditiveExp;

	ioEtcItemPetFeed *pEtcItem = static_cast<ioEtcItemPetFeed*>( g_EtcItemMgr.FindEtcItem(iEtcItemCode) );
	if( !pEtcItem )
		return 0;

	return pEtcItem->GetAddExp();
}	

bool ioPetInfoManager::CheckRightPetCode( const int& iPetCode )
{
	for each( PetInfo info in m_vPetInfoVec )
	{
		if( info.iPetCode == iPetCode )
			return true;
	}

	return false;
}

int ioPetInfoManager::GetPetMaxLevel( const int& iRank )
{
	for each( PetRankInfo info in m_vPetRankInfoVec )
	{
		if( info.iRank == iRank )
			return info.iRankMaxLevel;
	}

	return 0;
}