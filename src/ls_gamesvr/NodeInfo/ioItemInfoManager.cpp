

#include "stdafx.h"

#include "User.h"
#include "Item.h"
#include "ioItemInfoManager.h"
#include "iosalemanager.h"
#include "../EtcHelpFunc.h"
#include "../DataBase/DBClient.h"

#define ITEM_CREATE_NO_LIMIT -1

template<> ioItemInfoManager* Singleton< ioItemInfoManager >::ms_Singleton = 0;

ioItemInfoManager::ioItemInfoManager()
{
	m_ReinforceRandom.SetRandomSeed( timeGetTime() );
}

ioItemInfoManager::~ioItemInfoManager()
{
	ClearAllInfo();
}

void ioItemInfoManager::LoadItemInfo( const char *szFileName )
{
	ClearAllInfo();

	ioINILoader kLoader( szFileName );

	int iNumItem = kLoader.LoadInt( "common_info", "item_count", 0 );
	m_InfoList.reserve( iNumItem );

	char szTitle[MAX_PATH];
	for( int i=0 ; i<iNumItem ; i++ )
	{
		wsprintf( szTitle, "item%d", i+1 );
		kLoader.SetTitle( szTitle );

		AddItemInfo( kLoader );
	}
}

void ioItemInfoManager::LoadBaseCharInfo()
{
	ioINILoader kLoader( "clientconfig/sp2_char.ini" );

	kLoader.SetTitle( "common" );
	m_BaseCharInfo.m_fDefaultHP = kLoader.LoadFloat( "default_hp", 174.0f );
	m_BaseCharInfo.m_fDefaultSpeed = kLoader.LoadFloat( "default_speed", 320.0f );

	m_BaseCharInfo.m_dwBlowProtectionTime = kLoader.LoadInt( "blow_state_protection_time", 500 );
	m_BaseCharInfo.m_dwNoInputDelayTime = kLoader.LoadInt( "no_input_delay_time", 3000 );
	m_BaseCharInfo.m_dwStartProtectionTime = kLoader.LoadInt( "start_protection_time", 3000 );
	m_BaseCharInfo.m_dwStartProtectionTime2 = kLoader.LoadInt( "start_protection_time2", 3000 );

	kLoader.SetTitle( "recovery_gauge" );
	m_BaseCharInfo.m_fDefaultRecoveryGauge = kLoader.LoadFloat( "default_recovery_gauge", 0.0f );
	m_BaseCharInfo.m_dwDefaultRecoveryGaugeTic = kLoader.LoadFloat( "default_recovery_gauge_tic", 1000 );
	m_BaseCharInfo.m_fDelayRunGaugeRate = kLoader.LoadFloat( "delayrun_recovery_gauge_rate", 0.0f );
	m_BaseCharInfo.m_fEtcGaugeRate = kLoader.LoadFloat( "etc_recovery_gauge_rate", 0.0f );

	kLoader.SetTitle( "character1" );
	m_BaseCharInfo.m_fDefaultRecover   = kLoader.LoadFloat( "hp_recovery", 0.0f );
	m_BaseCharInfo.m_dwDefaultRecoveryTick = kLoader.LoadInt( "hp_recovery_tick", 0 );
}

void ioItemInfoManager::LoadBaseItemInfo()
{
	// base item
	ioINILoader kLoader( "clientconfig/sp2_item.ini" );
	LoadBaseItemInfo( kLoader );

	// extra item
	ioINILoader kExtraLoader( "clientconfig/sp2_extraitem.ini" );
	LoadBaseItemInfo( kExtraLoader );
}

void ioItemInfoManager::LoadBaseItemInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szGroupName[MAX_PATH] = "";
	char szTitle[MAX_PATH] = "";

	int i = 0;
	rkLoader.SetTitle( "common_info" );

	int iGroupCnt = rkLoader.LoadInt( "item_group_cnt", 0 );
	for( i=0 ; i<iGroupCnt ; i++ )
	{
		// 그룹 호출
		wsprintf( szTitle, "item_group%d", i );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		wsprintf( szGroupName, "clientconfig/%s", szBuf );

		ioINILoader kSetItemLoader( szGroupName );
		kSetItemLoader.SetTitle( "common" );

		int iItemCnt = kSetItemLoader.LoadInt( "item_cnt", 0 );
		for( int j=0; j < iItemCnt; ++j )
		{
			wsprintf( szTitle, "item%d", j+1 );
			kSetItemLoader.SetTitle( szTitle );

			DWORD dwItemCode = kSetItemLoader.LoadInt( "code", 0 );

			ItemInfo *pInfo = GetItemInfoByPool( dwItemCode );
			if( pInfo )
			{
				pInfo->m_fBaseMaxGauge = kSetItemLoader.LoadFloat( "max_skill_gauge", 0 );
				pInfo->m_fBaseArmorClass = kSetItemLoader.LoadFloat( "armor_class", 0 );
				pInfo->m_fBaseSpeedClass = kSetItemLoader.LoadFloat( "speed_class", 0 );
			}
		}
	}
}

void ioItemInfoManager::LoadBaseSkillInfo()
{
	char szBuf[MAX_PATH] = "";
	char szGroupName[MAX_PATH] = "";
	char szTitle[MAX_PATH] = "";

	ioINILoader kLoader( "clientconfig/sp2_skill.ini" );

	int i = 0;

	kLoader.SetTitle( "common_info" );
	int iGroupCnt = kLoader.LoadInt( "skill_group_cnt", 0 );

	for( i=0 ; i<iGroupCnt ; i++ )
	{
		// 그룹 호출
		wsprintf( szTitle, "skill_group%d", i );
		kLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		wsprintf( szGroupName, "clientconfig/%s", szBuf );

		ioINILoader kSetItemLoader( szGroupName );
		kSetItemLoader.SetTitle( "common" );

		int iSkillCnt = kSetItemLoader.LoadInt( "skill_cnt", 0 );
		for( int j=0; j < iSkillCnt; ++j )
		{
			SkillInfo *pSkillInfo = new SkillInfo;
			if( !pSkillInfo )
				continue;

			wsprintf( szTitle, "skill%d", j+1 );
			kSetItemLoader.SetTitle( szTitle );

			ioHashString szSkillName;
			kSetItemLoader.LoadString( "name", "", szBuf, MAX_PATH );
			szSkillName = szBuf;

			pSkillInfo->m_fNeedGauge = kSetItemLoader.LoadFloat( "need_gauge", 1000000.0f );

			m_SkillInfo.insert( SkillInfoNameMap::value_type( szSkillName, pSkillInfo ) );
		}
	}
}

bool ioItemInfoManager::CheckBaseCharInfo( User *pUser,
										   float fDefaultHP, float fDefaultSpeed,
										   DWORD dwBlowProtectionTime,
										   DWORD dwNoInputDelayTime,
										   DWORD dwStartProtectionTime,
										   DWORD dwStartProtectionTime2,
										   float fDefaultRecoveryGauge,
										   DWORD dwDefaultRecoveryGaugeTic,
										   float fDelayRunGaugeRate,
										   float fEtcGaugeRate,
										   float fDefaultRecover,
										   DWORD dwDefaultRecoveryTick )
{
	char szLog[2048] = "";

	if( m_BaseCharInfo.m_fDefaultHP != fDefaultHP )
	{
		sprintf_s( szLog, "CheckBaseCharInfo Not Equal - HP : %s, %f, %f", pUser->GetPublicID().c_str(), m_BaseCharInfo.m_fDefaultHP, fDefaultHP );
		SP2Packet kPacket( LUPK_LOG );
		kPacket << "ServerError";
		kPacket << szLog;
		g_UDPNode.SendLog( kPacket );

		return false;
	}

	if( m_BaseCharInfo.m_fDefaultSpeed != fDefaultSpeed )
	{
		sprintf_s( szLog, "CheckBaseCharInfo Not Equal - Speed : %s, %f, %f", pUser->GetPublicID().c_str(), m_BaseCharInfo.m_fDefaultSpeed, fDefaultSpeed );
		SP2Packet kPacket( LUPK_LOG );
		kPacket << "ServerError";
		kPacket << szLog;
		g_UDPNode.SendLog( kPacket );

		return false;
	}

	if( m_BaseCharInfo.m_dwBlowProtectionTime != dwBlowProtectionTime )
	{
		sprintf_s( szLog, "CheckBaseCharInfo Not Equal - Protection1 : %s, %d, %d", pUser->GetPublicID().c_str(), m_BaseCharInfo.m_dwBlowProtectionTime, dwBlowProtectionTime );
		SP2Packet kPacket( LUPK_LOG );
		kPacket << "ServerError";
		kPacket << szLog;
		g_UDPNode.SendLog( kPacket );

		return false;
	}

	if( m_BaseCharInfo.m_dwNoInputDelayTime != dwNoInputDelayTime )
	{
		sprintf_s( szLog, "CheckBaseCharInfo Not Equal - Protection2 : %s, %d, %d", pUser->GetPublicID().c_str(), m_BaseCharInfo.m_dwNoInputDelayTime, dwNoInputDelayTime );
		SP2Packet kPacket( LUPK_LOG );
		kPacket << "ServerError";
		kPacket << szLog;
		g_UDPNode.SendLog( kPacket );

		return false;
	}

	if( m_BaseCharInfo.m_dwStartProtectionTime != dwStartProtectionTime )
	{
		sprintf_s( szLog, "CheckBaseCharInfo Not Equal - Protection3 : %s, %d, %d", pUser->GetPublicID().c_str(), m_BaseCharInfo.m_dwStartProtectionTime, dwStartProtectionTime );
		SP2Packet kPacket( LUPK_LOG );
		kPacket << "ServerError";
		kPacket << szLog;
		g_UDPNode.SendLog( kPacket );

		return false;
	}

	if( m_BaseCharInfo.m_dwStartProtectionTime2 != dwStartProtectionTime2 )
	{
		sprintf_s( szLog, "CheckBaseCharInfo Not Equal - Protection4 : %s, %d, %d", pUser->GetPublicID().c_str(), m_BaseCharInfo.m_dwStartProtectionTime2, dwStartProtectionTime2 );
		SP2Packet kPacket( LUPK_LOG );
		kPacket << "ServerError";
		kPacket << szLog;
		g_UDPNode.SendLog( kPacket );

		return false;
	}

	if( m_BaseCharInfo.m_fDefaultRecoveryGauge != fDefaultRecoveryGauge )
	{
		sprintf_s( szLog, "CheckBaseCharInfo Not Equal - RecoveryGauge1 : %s, %f, %f", pUser->GetPublicID().c_str(), m_BaseCharInfo.m_fDefaultRecoveryGauge, fDefaultRecoveryGauge );
		SP2Packet kPacket( LUPK_LOG );
		kPacket << "ServerError";
		kPacket << szLog;
		g_UDPNode.SendLog( kPacket );

		return false;
	}

	if( m_BaseCharInfo.m_dwDefaultRecoveryGaugeTic != dwDefaultRecoveryGaugeTic )
	{
		sprintf_s( szLog, "CheckBaseCharInfo Not Equal - RecoveryGauge2 : %s, %d, %d", pUser->GetPublicID().c_str(), m_BaseCharInfo.m_dwDefaultRecoveryGaugeTic, dwDefaultRecoveryGaugeTic );
		SP2Packet kPacket( LUPK_LOG );
		kPacket << "ServerError";
		kPacket << szLog;
		g_UDPNode.SendLog( kPacket );

		return false;
	}

	if( m_BaseCharInfo.m_fDelayRunGaugeRate != fDelayRunGaugeRate )
	{
		sprintf_s( szLog, "CheckBaseCharInfo Not Equal - RecoveryGauge3 : %s, %f, %f", pUser->GetPublicID().c_str(), m_BaseCharInfo.m_fDelayRunGaugeRate, fDelayRunGaugeRate );
		SP2Packet kPacket( LUPK_LOG );
		kPacket << "ServerError";
		kPacket << szLog;
		g_UDPNode.SendLog( kPacket );

		return false;
	}

	if( m_BaseCharInfo.m_fEtcGaugeRate != fEtcGaugeRate )
	{
		sprintf_s( szLog, "CheckBaseCharInfo Not Equal - RecoveryGauge5 : %s, %f, %f", pUser->GetPublicID().c_str(), m_BaseCharInfo.m_fEtcGaugeRate, fEtcGaugeRate );
		SP2Packet kPacket( LUPK_LOG );
		kPacket << "ServerError";
		kPacket << szLog;
		g_UDPNode.SendLog( kPacket );

		return false;
	}

	if( m_BaseCharInfo.m_fDefaultRecover != fDefaultRecover )
	{
		sprintf_s( szLog, "CheckBaseCharInfo Not Equal - RecoveryHP1 : %s, %f, %f", pUser->GetPublicID().c_str(), m_BaseCharInfo.m_fDefaultRecover, fDefaultRecover );
		SP2Packet kPacket( LUPK_LOG );
		kPacket << "ServerError";
		kPacket << szLog;
		g_UDPNode.SendLog( kPacket );

		return false;
	}

	if( m_BaseCharInfo.m_dwDefaultRecoveryTick != dwDefaultRecoveryTick )
	{
		sprintf_s( szLog, "CheckBaseCharInfo Not Equal - RecoveryHP2 : %s, %d, %d", pUser->GetPublicID().c_str(), m_BaseCharInfo.m_dwDefaultRecoveryTick, dwDefaultRecoveryTick );
		SP2Packet kPacket( LUPK_LOG );
		kPacket << "ServerError";
		kPacket << szLog;
		g_UDPNode.SendLog( kPacket );

		return false;
	}

	return true;
}

bool ioItemInfoManager::CheckBaseItemInfo( User *pUser,
										   DWORD dwCode,
										   float fBaseMaxGauge,
										   float fBaseArmorClass,
										   float fBaseSpeedClass )
{
	char szLog[2048] = "";

	const ItemInfo *pItemInfo = GetItemInfo( dwCode );
	if( pItemInfo )
	{
		if( pItemInfo->m_fBaseMaxGauge != fBaseMaxGauge )
		{
			sprintf_s( szLog, "CheckBaseItemInfo Not Equal - MaxGauge : %s, %f, %f", pUser->GetPublicID().c_str(), pItemInfo->m_fBaseMaxGauge, fBaseMaxGauge );
			SP2Packet kPacket( LUPK_LOG );
			kPacket << "ServerError";
			kPacket << szLog;
			g_UDPNode.SendLog( kPacket );

			return false;
		}

		if( pItemInfo->m_fBaseArmorClass != fBaseArmorClass )
		{
			sprintf_s( szLog, "CheckBaseItemInfo Not Equal - ArmorClass : %s, %f, %f", pUser->GetPublicID().c_str(), pItemInfo->m_fBaseArmorClass, fBaseArmorClass );
			SP2Packet kPacket( LUPK_LOG );
			kPacket << "ServerError";
			kPacket << szLog;
			g_UDPNode.SendLog( kPacket );

			return false;
		}

		if( pItemInfo->m_fBaseSpeedClass != fBaseSpeedClass )
		{
			sprintf_s( szLog, "CheckBaseItemInfo Not Equal - SpeedClass : %s, %f, %f", pUser->GetPublicID().c_str(), pItemInfo->m_fBaseSpeedClass, fBaseSpeedClass );
			SP2Packet kPacket( LUPK_LOG );
			kPacket << "ServerError";
			kPacket << szLog;
			g_UDPNode.SendLog( kPacket );

			return false;
		}
	}

	return true;
}

bool ioItemInfoManager::CheckBaseSkillInfo( User *pUser, const ioHashString &szSkillName, float fNeedGauge )
{
	char szLog[2048] = "";

	const SkillInfo *pSkillInfo = GetSkillInfo( szSkillName );
	if( pSkillInfo )
	{
		if( pSkillInfo->m_fNeedGauge != fNeedGauge )
		{
			sprintf_s( szLog, "CheckBaseSkillInfo Not Equal - NeedGauge : %s, %f, %f", pUser->GetPublicID().c_str(), pSkillInfo->m_fNeedGauge, fNeedGauge );
			SP2Packet kPacket( LUPK_LOG );
			kPacket << "ServerError";
			kPacket << szLog;
			g_UDPNode.SendLog( kPacket );

			return false;
		}
	}

	return true;
}

void ioItemInfoManager::AddItemInfo( ioINILoader &rkLoader )
{
	ItemInfo *pInfo = new ItemInfo;

	pInfo->m_iItemCode = rkLoader.LoadInt( "code", 0 );
	if( pInfo->m_iItemCode == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][item]Invalid item code : [%s]", rkLoader.GetTitle() );
		delete pInfo;
		return;
	}

	char szName[MAX_PATH];
	rkLoader.LoadString( "name", "", szName, MAX_PATH );
	if( !strcmp( szName, "" ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "warning][item]Invalid item name : [%s]", rkLoader.GetTitle() );
		delete pInfo;
		return;
	}

	pInfo->m_Name = szName;
	pInfo->m_dwSetCode = rkLoader.LoadInt( "set_item_code", 0 );
	pInfo->m_iGroupIndex = rkLoader.LoadInt( "group", 1 );
	pInfo->m_bNotDeleteItem = rkLoader.LoadBool( "not_delete_item", false );
	pInfo->m_iCrownItemType = rkLoader.LoadInt( "crown_item_type", 0 );
	pInfo->m_iItemTeamType = rkLoader.LoadInt( "item_team_type", 0 );

	pInfo->m_iEnableClass = rkLoader.LoadInt( "enable_class", 0 );
	pInfo->m_iDefaultReinforceMin = rkLoader.LoadInt( "default_reinforce_min", 0 );
	pInfo->m_iDefaultReinforceMax = rkLoader.LoadInt( "default_reinforce_max", 0 );
	pInfo->m_iGradeType = rkLoader.LoadInt( "grade_type", 0 );

	pInfo->m_iCreateMaxLimit = rkLoader.LoadInt( "item_create_limit_count", ITEM_CREATE_NO_LIMIT );
	
	int iLevelSize = rkLoader.LoadInt( "max_level", 0);
	pInfo->ReserveAddExpert(iLevelSize);
	pInfo->ReserveReachExpert(iLevelSize);
	pInfo->ReserveLevelUpPeso(iLevelSize);
	for(int i = 0; i < iLevelSize; i++)
	{
		char szKeyNameAdd[MAX_PATH]="";
		wsprintf(szKeyNameAdd, "level%d_add_expert", i+1);
		int iCurAddExpert = rkLoader.LoadInt(szKeyNameAdd, 0);
		if(iCurAddExpert > 0)
			pInfo->PushBackAddExpert(iCurAddExpert);

		char szKeyNameReach[MAX_PATH]="";
		wsprintf(szKeyNameReach, "level%d_reach_expert", i+1);
		int iCurReachExpert = rkLoader.LoadInt(szKeyNameReach, 0);
		if(iCurReachExpert > 0)
			pInfo->PushBackPeachExpert(iCurReachExpert);

		char szKeyNamePeso[MAX_PATH]="";
		wsprintf(szKeyNamePeso, "level%d_peso", i+1);
		int iCurLevelPeso = rkLoader.LoadInt(szKeyNamePeso, 0);
		if(iCurLevelPeso > 0)
			pInfo->PushBackLevelUpPeso(iCurLevelPeso);
	}

	m_CodeMap.insert( InfoCodeMap::value_type( pInfo->m_iItemCode, pInfo ) );
	m_NameMap.insert( InfoNameMap::value_type( pInfo->m_Name, pInfo ) );
	m_InfoList.push_back( pInfo );

	GroupInfoMap::iterator iGroup = m_GroupMap.find( pInfo->m_iGroupIndex );
	if( iGroup == m_GroupMap.end() )
	{
		std::pair< GroupInfoMap::iterator, bool > retPair;
		retPair = m_GroupMap.insert( GroupInfoMap::value_type( pInfo->m_iGroupIndex,
															   new ConstInfoVector() ) );

		assert( retPair.second );
		iGroup = retPair.first;
	}

	iGroup->second->push_back( pInfo );
}

void ioItemInfoManager::ClearAllInfo()
{
	m_CodeMap.clear();
	m_NameMap.clear();

	GroupInfoMap::iterator iGroup, iGroupEnd;
	iGroupEnd = m_GroupMap.end();
	for( iGroup=m_GroupMap.begin() ; iGroup!=iGroupEnd ; ++iGroup )
	{
		delete iGroup->second;
	}
	m_GroupMap.clear();

	InfoVector::iterator iter, iEnd;
	iEnd = m_InfoList.end();
	for( iter=m_InfoList.begin() ; iter!=iEnd ; ++iter )
	{
		(*iter)->ClearAddExpert();
		(*iter)->ClearReachExpert();
		(*iter)->ClearLevelUpPeso();
		delete *iter;
	}
	m_InfoList.clear();

	SkillInfoNameMap::iterator iSkill, iSkillEnd;
	iSkillEnd = m_SkillInfo.end();
	for( iSkill=m_SkillInfo.begin() ; iSkill!=iSkillEnd ; ++iSkill )
	{
		delete iSkill->second;
	}
	m_SkillInfo.clear();
}

const ItemInfo* ioItemInfoManager::GetItemInfo( int iItemCode ) const
{
	InfoCodeMap::const_iterator iter = m_CodeMap.find( iItemCode );
	if( iter != m_CodeMap.end() )
		return iter->second;

	//유영재 아이템 진화 관련 갑,투,망 체크시 로그가 많이 남아 임시 삭제. 차후 진화장비 적용시 주석 해제.
	//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioItemInfoManager::GetItemInfo - %d Not Exist", iItemCode );
	return NULL;
}

ItemInfo* ioItemInfoManager::GetItemInfoByPool( int iItemCode )
{
	InfoVector::const_iterator iter, iEnd;
	iEnd = m_InfoList.end();
	for( iter=m_InfoList.begin() ; iter!=iEnd ; ++iter )
	{
		ItemInfo *pInfo = *iter;
		if( pInfo->m_iItemCode == iItemCode )
			return pInfo;
	}

	return NULL;
}

DWORDVec ioItemInfoManager::GetSetItemList( DWORD dwSetCode ) const
{
	DWORDVec vSetList;

	InfoVector::const_iterator iter, iEnd;
	iEnd = m_InfoList.end();
	for( iter=m_InfoList.begin() ; iter!=iEnd ; ++iter )
	{
		const ItemInfo *pInfo = *iter;
		if( pInfo->m_dwSetCode == dwSetCode )
		{
			vSetList.push_back( pInfo->m_iItemCode );
		}
	}

	return vSetList;
}

const ItemInfo* ioItemInfoManager::GetItemInfo( const ioHashString &rkName ) const
{
	InfoNameMap::const_iterator iter = m_NameMap.find( rkName );
	if( iter != m_NameMap.end() )
		return iter->second;

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioItemInfoManager::GetItemInfo - %s Not Exist", rkName.c_str() );
	return NULL;
}

const ConstInfoVector* ioItemInfoManager::GetInfoVector( int iGroupIdx ) const
{
	GroupInfoMap::const_iterator iter = m_GroupMap.find( iGroupIdx );
	if( iter != m_GroupMap.end() )
		return iter->second;

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioItemInfoManager::GetInfoVector - %d Group Not Exist", iGroupIdx );
	return NULL;
}

const SkillInfo* ioItemInfoManager::GetSkillInfo( const ioHashString &rkName ) const
{
	SkillInfoNameMap::const_iterator iter = m_SkillInfo.find( rkName );
	if( iter != m_SkillInfo.end() )
		return iter->second;

	return NULL;
}

int ioItemInfoManager::GetDefaultReinforce( int iItemCode )
{
	int iReinforce = 0;

	InfoCodeMap::const_iterator iter = m_CodeMap.find( iItemCode );
	if( iter != m_CodeMap.end() )
	{
		int iGap = iter->second->m_iDefaultReinforceMax - iter->second->m_iDefaultReinforceMin + 1;
		if( iGap > 0 )
		{
			int iRand = m_ReinforceRandom.Random( iGap );
			iReinforce = iter->second->m_iDefaultReinforceMin + iRand;
		}
	}

	return iReinforce;
}

ioItemMaker* ioItemInfoManager::CreateItemMaker()
{
	return new ioItemMaker( this );
}

ioItemInfoManager& ioItemInfoManager::GetSingleton()
{
	return Singleton< ioItemInfoManager >::GetSingleton();
}

//---------------------------------------------------------------------------------------------------

ioItemMaker::ioItemMaker( ioItemInfoManager *pCreator )
{
	m_pCreator = pCreator;
	m_dwNextCreateIndex = 1;
}

ioItemMaker::~ioItemMaker()
{
	m_ItemCreateMap.clear();
}

ioItem* ioItemMaker::CreateItem( int iItemCode )
{
	if(m_pCreator == NULL ) 
		return NULL;
	const ItemInfo *pInfo = m_pCreator->GetItemInfo( iItemCode );
	if( pInfo )
		return AddNewItem( pInfo );

	return NULL;
}

ioItem* ioItemMaker::CreateItem( const ioHashString &rkName )
{
	const ItemInfo *pInfo = m_pCreator->GetItemInfo( rkName );
	if( pInfo )
		return AddNewItem( pInfo );

	return NULL;
}

ioItem* ioItemMaker::AddNewItem( const ItemInfo *pInfo )
{
	ioItem *pItem = new ioItem( this );
	pItem->SetItemCode( pInfo->m_iItemCode );
	pItem->SetItemName( pInfo->m_Name );
	pItem->SetGameIndex( GetNextCreateIndex() );
	pItem->SetEnableDelete( pInfo->m_bNotDeleteItem );
	pItem->SetCrown( pInfo->m_iCrownItemType, pInfo->m_iItemTeamType );

	IncreaseItemCreateCnt( pItem->GetItemCode() );

	return pItem;
}

DWORD ioItemMaker::GetNextCreateIndex()
{
	return ++m_dwNextCreateIndex;
}

void ioItemMaker::NotifyItemDestroyed( int iItemCode )
{
	ItemCreateMap::iterator iter = m_ItemCreateMap.find( iItemCode );
	if( iter != m_ItemCreateMap.end() )
	{
		int iRefCount = iter->second;
		iRefCount--;

		if( iRefCount >= 0 )
		{
			iter->second = iRefCount;
		}
		else
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioItemMaker::NotifyItemDestroyed - %d Item Underflow(%d)",
									iItemCode, iRefCount );
		}
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioItemMaker::NotifyItemDestoryed - %d Not Created", iItemCode );
	}
}

void ioItemMaker::IncreaseItemCreateCnt( int iItemCode )
{
	ItemCreateMap::iterator iter = m_ItemCreateMap.find( iItemCode );
	if( iter != m_ItemCreateMap.end() )
	{
		iter->second++;
	}
	else
	{
		m_ItemCreateMap.insert( ItemCreateMap::value_type( iItemCode, 1 ) );
	}
}

int ioItemMaker::GetCreatedCnt( int iItemCode ) const
{
	ItemCreateMap::const_iterator iter = m_ItemCreateMap.find( iItemCode );
	if( iter != m_ItemCreateMap.end() )
		return iter->second;

	return 0;
}
//////////////////////////////////////////////////////////////////////////
template<> ioItemPriceManager* Singleton< ioItemPriceManager >::ms_Singleton = 0;

ioItemPriceManager::ioItemPriceManager()
{
	ClearAllInfo();
}

ioItemPriceManager::~ioItemPriceManager()
{
	ClearAllInfo();
}

ioItemPriceManager& ioItemPriceManager::GetSingleton()
{
	return Singleton< ioItemPriceManager >::GetSingleton();
}

void ioItemPriceManager::ClearAllInfo()
{
	vPriceData::iterator iter,iEnd;
	iEnd = m_vPriceList.end();
	for(iter = m_vPriceList.begin();iter != iEnd;iter++)
	{
		SAFEDELETE( *iter );
	}
	m_vPriceList.clear();

	vLimitData::iterator LimitIter, LimitIEnd;
	LimitIEnd = m_vPesoLimitDataList.end();
	for( LimitIter = m_vPesoLimitDataList.begin();LimitIter != LimitIEnd;LimitIter++)
	{
		SAFEDELETE( *LimitIter );
	}
	m_vPesoLimitDataList.clear();

	LimitIEnd = m_vCashLimitDataList.end();
	for( LimitIter = m_vCashLimitDataList.begin();LimitIter != LimitIEnd;LimitIter++)
	{
		SAFEDELETE( *LimitIter );
	}
	m_vCashLimitDataList.clear();

	LimitIEnd = m_vPremiumCashLimitDataList.end();
	for( LimitIter = m_vPremiumCashLimitDataList.begin();LimitIter != LimitIEnd;LimitIter++)
	{
		SAFEDELETE( *LimitIter );
	}
	m_vPremiumCashLimitDataList.clear();


	LimitIEnd = m_vRareCashLimitDataList.end();
	for( LimitIter = m_vRareCashLimitDataList.begin();LimitIter != LimitIEnd;LimitIter++)
	{
		SAFEDELETE( *LimitIter );
	}
	m_vRareCashLimitDataList.clear();
    
	m_dwFirstHireLimit = 192;
	m_dwDefaultLimit   = 24;
	m_dwBankruptcyLimit= 24;
	m_iBankruptcyCount = 4;
	m_fLimitExtendDiscount = 1.0f;
	m_iBankruptcyPeso = 0;
	m_fMortmainCharMultiply = 1.0f;
	m_fTimeCharDivision     = 1.0f;     
	m_fMortmainCharDivision = 1.0f; 
	m_fMortmainCharMultiplyCash = 1.0f;
	m_fMortmainPremiumCharMultiplyCash = 1.0f;
	m_fMortmainRareCharMultiplyCash    = 1.0f;

	m_dwCurrentTime = 0;
	m_dwUpdatePriceCollectedTime = 2;
	m_iQuestSoldierCollectedTime = -7200;
	m_dwUpdateCollectedTime      = 0;
}

bool ioItemPriceManager::LoadPriceInfo( const char *szFileName, bool bCreateLoad /*= true */ )
{
	if( bCreateLoad )
		ClearAllInfo();

	ioINILoader kLoader;
	if( bCreateLoad )
		kLoader.LoadFile( szFileName );
	else
		kLoader.ReloadFile( szFileName );

	kLoader.SetTitle( "Collected" );
	m_dwUpdatePriceCollectedTime = kLoader.LoadInt( "UpdatePrice", 2 );
	m_iQuestSoldierCollectedTime = kLoader.LoadInt( "QuestMinusSec", -7200 );

	kLoader.SetTitle( "INFO" );
	m_dwFirstHireLimit = kLoader.LoadInt( "firsthire_limit", 192 );
	m_dwDefaultLimit   = kLoader.LoadInt( "default_limit", 24 );
	m_dwBankruptcyLimit= kLoader.LoadInt( "bankruptcy_limit", 24 );
	m_iBankruptcyCount = kLoader.LoadInt( "bankruptcy_count", 4 );
	m_fLimitExtendDiscount = kLoader.LoadFloat( "extend_discount", 1.0f );
	m_iBankruptcyPeso  = kLoader.LoadInt( "bankruptcy_peso", 0 );
	m_fMortmainCharMultiply = kLoader.LoadFloat( "mortmain_char_multiply", 1.0f );
	m_fTimeCharDivision     = kLoader.LoadFloat( "time_char_division", 1.0f );
	m_fMortmainCharDivision = kLoader.LoadFloat( "mortmain_char_division", 1.0f );
	m_fMortmainCharMultiplyCash = kLoader.LoadFloat( "mortmain_char_multiply_cash", 1.0f );	
	m_fMortmainPremiumCharMultiplyCash = kLoader.LoadFloat( "mortmain_premium_char_multiply_cash", 1.0f );	
	m_fMortmainRareCharMultiplyCash = kLoader.LoadFloat( "mortmain_rare_char_multiply_cash", 1.0f );	

	int i = 0;
	int iMaxClass = kLoader.LoadInt( "MAX_CLASS", 0 );	
	for(i = 0;i < iMaxClass;i++)
	{
		char szTitle[MAX_PATH] = "";
		sprintf_s( szTitle, "CLASS_%d", i+1);
		kLoader.SetTitle( szTitle );
		
		PriceData *pPrice = NULL;
		if( bCreateLoad )
			pPrice = new PriceData;
		else
			pPrice = GetPriceDataToSetCode( i + 1 );

		if( !pPrice )
			continue;

		pPrice->m_iSetCode      = i+1;
		pPrice->m_bActive       = kLoader.LoadBool( "ACTIVE", true );
		pPrice->m_bPcRoomActive = kLoader.LoadBool( "PC_ROOM_ACTIVE", true );
		pPrice->m_bFreeDayHero  = kLoader.LoadBool( "FREE_DAY_ACTIVE", true );
		pPrice->m_iBuyCash      = kLoader.LoadInt( "CASH", 0 );
		pPrice->m_iBonusPeso    = kLoader.LoadInt( "BONUS_PESO", 0 );
		pPrice->m_eType         = (PriceType) kLoader.LoadInt( "TYPE", 0 );
#ifdef HERO_POSSIBLE_CHECK
		pPrice->m_bHeroPossibleUse = kLoader.LoadBool( "HERO_POSSIBLE", true );
#endif

		pPrice->m_iSubscriptionType = kLoader.LoadInt( "SUBSCRIPTION_TYPE", SUBSCRIPTION_NONE );

		if( pPrice->m_bPcRoomActive )
		{
			char szBuf[MAX_PATH]=	"";
			__int64 iStartDate	=	0;
			__int64 iEndDate	=	0;

			kLoader.LoadString( "PC_ROOM_ACTIVE_START", "", szBuf, MAX_PATH );
			if( strlen(szBuf) != 0 )
			{
				if( !GetPcRoomActiveDate(szBuf, iStartDate) )
					return false;

				pPrice->m_iPcRoomStartDate	= iStartDate;
			}

			kLoader.LoadString( "PC_ROOM_ACTIVE_END", "", szBuf, MAX_PATH );
			if( strlen(szBuf) != 0 )
			{
				if( !GetPcRoomActiveDate(szBuf, iEndDate) )
					return false;

				pPrice->m_iPcRoomEndDate	= iEndDate;
			}
		}

		enum { MAX_LOOP = 10, };
		for (int j = 0; j < MAX_LOOP ; j++)
		{
			g_SaleMgr.LoadINI( bCreateLoad, kLoader, ioSaleManager::IT_CLASS, pPrice->m_iSetCode, j, j );
		}

		if( bCreateLoad )
			m_vPriceList.push_back( pPrice );
	}

	// peso
	kLoader.SetTitle( "INFO" );
	int iMaxLImit = kLoader.LoadInt( "MAX_PESO_LIMIT", 0 );
	for(i = 0;i < iMaxLImit;i++)
	{
		char szTitle[MAX_PATH] = "";
		sprintf_s( szTitle, "PESO_LIMIT_%d", i+1 );
		kLoader.SetTitle( szTitle );

		LimitData *pLimit = NULL;
		if( bCreateLoad )
			pLimit = new LimitData;
		else 
			pLimit = GetPesoLimitData(i);

		if( !pLimit )
			continue;

		pLimit->m_iLimitDate = kLoader.LoadInt( "LIMIT_DATE", 24);
		pLimit->m_fLimitPricePer = kLoader.LoadFloat( "LIMIT_PER", 1.0f );

		if( bCreateLoad )
			m_vPesoLimitDataList.push_back( pLimit );
	}

	// cash
	kLoader.SetTitle( "INFO" );
	iMaxLImit = kLoader.LoadInt( "MAX_CASH_LIMIT", 0 );
	for(i = 0;i < iMaxLImit;i++)
	{
		char szTitle[MAX_PATH] = "";
		sprintf_s( szTitle, "CASH_LIMIT_%d", i+1 );
		kLoader.SetTitle( szTitle );

		LimitData *pLimit = NULL;

		if( bCreateLoad )
			pLimit = new LimitData;
		else
			pLimit = GetCashLimitData(i);

		if( !pLimit )
			continue;

		pLimit->m_iLimitDate = kLoader.LoadInt( "LIMIT_DATE", 24);
		pLimit->m_fLimitPricePer = kLoader.LoadFloat( "LIMIT_PER", 1.0f );

		if( bCreateLoad )
			m_vCashLimitDataList.push_back( pLimit );
	}

	// premium cash
	kLoader.SetTitle( "INFO" );
	iMaxLImit = kLoader.LoadInt( "MAX_PREMIUM_CASH_LIMIT", 0 );
	for(i = 0;i < iMaxLImit;i++)
	{
		char szTitle[MAX_PATH] = "";
		sprintf_s( szTitle, "CASH_PREMIUM_LIMIT_%d", i+1 );
		kLoader.SetTitle( szTitle );

		LimitData *pLimit = NULL;

		if( bCreateLoad )
			pLimit = new LimitData;
		else
			pLimit = GetPremiumCashLimitData(i);

		if( !pLimit )
			continue;

		pLimit->m_iLimitDate = kLoader.LoadInt( "LIMIT_DATE", 24);
		pLimit->m_fLimitPricePer = kLoader.LoadFloat( "LIMIT_PER", 1.0f );

		if( bCreateLoad )
			m_vPremiumCashLimitDataList.push_back( pLimit );
	}

	// rare cash
	kLoader.SetTitle( "INFO" );
	iMaxLImit = kLoader.LoadInt( "MAX_RARE_CASH_LIMIT", 0 );
	for(i = 0;i < iMaxLImit;i++)
	{
		char szTitle[MAX_PATH] = "";
		sprintf_s( szTitle, "CASH_RARE_LIMIT_%d", i+1 );
		kLoader.SetTitle( szTitle );

		LimitData *pLimit = NULL;

		if( bCreateLoad )
			pLimit = new LimitData;
		else
			pLimit = GetRareCashLimitData(i);

		if( !pLimit )
			continue;

		pLimit->m_iLimitDate = kLoader.LoadInt( "LIMIT_DATE", 24);
		pLimit->m_fLimitPricePer = kLoader.LoadFloat( "LIMIT_PER", 1.0f );

		if( bCreateLoad )
			m_vRareCashLimitDataList.push_back( pLimit );
	}

	// test
	if( false )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Start LoadPriceInfo : %d", bCreateLoad );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "firsthire_limit             = %d", m_dwFirstHireLimit );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "default_limit               = %d", m_dwDefaultLimit );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "bankruptcy_limit            = %d", m_dwBankruptcyLimit );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "bankruptcy_count            = %d", m_iBankruptcyCount );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "extend_discount             = %f", m_fLimitExtendDiscount );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "bankruptcy_peso             = %d", m_iBankruptcyPeso );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "mortmain_char_multiply      = %f", m_fMortmainCharMultiply );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "time_char_division          = %f", m_fTimeCharDivision );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "mortmain_char_division      = %f", m_fMortmainCharDivision );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "mortmain_char_multiply_cash = %f", m_fMortmainCharMultiplyCash );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "mortmain_premium_char_multiply_cash = %f", m_fMortmainPremiumCharMultiplyCash );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "mortmain_rare_char_multiply_cash = %f", m_fMortmainRareCharMultiplyCash );

		enum { MAX_LOOP = 100, };

		for (int i = 0; i < MAX_LOOP ; i++)
		{
			LimitData *pLimit = GetPesoLimitData(i);
			if( !pLimit )
				break;

			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "PESO LIMIT_DATE = %d", pLimit->m_iLimitDate );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "PESO LIMIT_PER  = %f", pLimit->m_fLimitPricePer );
		}

		for (int i = 0; i < MAX_LOOP ; i++)
		{
			LimitData *pLimit = GetCashLimitData(i);
			if( !pLimit )
				break;

			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "CASH LIMIT_DATE = %d", pLimit->m_iLimitDate );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "CASH LIMIT_PER  = %f", pLimit->m_fLimitPricePer );
		}

		for (int i = 0; i < MAX_LOOP ; i++)
		{
			PriceData *pPrice = GetPriceData(i);
			if( !pPrice )
				break;

			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SetCode        = %d", pPrice->m_iSetCode );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ACTIVE         = %d", (int)pPrice->m_bActive );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "CASH           = %d", pPrice->m_iBuyCash );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BONUS_PESO     = %d", pPrice->m_iBonusPeso );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "TYPE           = %d", (int)pPrice->m_eType );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "PC_ROOM_ACTIVE = %d", (int)pPrice->m_bPcRoomActive );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "FREE_DAY_ACTIVE = %d", (int)pPrice->m_bFreeDayHero );
			
		}

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "End LoadPriceInfo : %d", bCreateLoad );
	}
	//

	return true;
}

bool ioItemPriceManager::GetPcRoomActiveDate(char* szDate, __int64& iDate)
{
	std::vector<std::string> vTokens;
	IntVec vValues;

	Help::TokenizeToSTRING(szDate, " ", vTokens);	//vTokens[0] - 년:월:일  , vTokens[1] - 시 : 분
	if( vTokens.size() > 0 )
	{
		Help::TokenizeToINT(vTokens[0], ":", vValues);
		if( vValues.size() != 3 )
			return false;
					
		int iYear	= vValues[0];
		int iMonth	= vValues[1];
		int iDay	= vValues[2];

		if( !Help::IsAvailableDate(iYear-2000, iMonth, iDay) )
			return false;

		vValues.clear();
		Help::TokenizeToINT(vTokens[1], ":", vValues);
		if( vValues.size() != 2 )
			return false;

		int iHour	= vValues[0];
		int iMinute	= vValues[1];

		if( iHour < 0 || iHour > 23 )
			return false;

		if( iMinute < 0 || iMinute > 59 )
			return false;

		CTime cDate( iYear, iMonth, iDay, iHour, iMinute, 0, 0 );
		iDate = cDate.GetTime();
	}

	return true;
}

int ioItemPriceManager::GetMaxClassInfo()
{
	return m_vPriceList.size();
}

int ioItemPriceManager::GetMaxActiveClass()
{
	int iMaxSize = m_vPriceList.size();
	int iCurSize = 0;
	for(int i = 0;i < iMaxSize;i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		if( !pPrice ) continue;
		if( !pPrice->m_bActive ) continue;

		iCurSize++;
	}
	return iCurSize;
}

int ioItemPriceManager::GetMaxInActiveClass()
{
	int iMaxSize = m_vPriceList.size();
	int iCurSize = 0;
	for(int i = 0;i < iMaxSize;i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		if( !pPrice ) continue;
		if( pPrice->m_bActive ) continue;

		iCurSize++;
	}
	return iCurSize;
}

bool ioItemPriceManager::GetArrayClassActive( int iArray )
{
	if( !COMPARE( iArray, 0, GetMaxClassInfo() ) ) return false;

	PriceData *pPrice = m_vPriceList[iArray];
	if( !pPrice ) return false;

	return pPrice->m_bActive;
}

DWORD ioItemPriceManager::GetArrayClassCode( int iArray )
{
	if( !COMPARE( iArray, 0, GetMaxClassInfo() ) ) return false;

	PriceData *pPrice = m_vPriceList[iArray];
	if( !pPrice ) return false;

	return pPrice->m_iSetCode;
}

void ioItemPriceManager::SetWeekBuyCntAndPeso( boost::unordered_map<int, int>& mClassPrice )
{
	int i	     = 0;
	int iMaxClass = m_vPriceList.size();
	boost::unordered_map<int, int>::iterator it;

	for(i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		int iCode = pPrice->m_iSetCode;
		it = mClassPrice.find(iCode);
		if( it == mClassPrice.end() )
			pPrice->m_iBuyPeso = 0;
		else
			pPrice->m_iBuyPeso = it->second;
	}

	//Sort
	std::sort( m_vPriceList.begin(), m_vPriceList.end(), PriceDataSort() );

	//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][character]character price info" );	
	for(i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][character]class%d Price[%d]: %d Peso / %d Cash", pPrice->m_iSetCode, pPrice->m_bActive, pPrice->m_iBuyPeso, pPrice->m_iBuyCash );
	}	
}

int ioItemPriceManager::GetClassBuyPeso( int iClassType, int iLimitDate, bool bResell /*= false */ )
{
	int iMaxClass = m_vPriceList.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		if( !bResell )
		{
			if( !pPrice->m_bActive ) continue;
		}
		if( pPrice->m_iSetCode != iClassType ) continue;

		int iMaxLimit = m_vPesoLimitDataList.size();
		for(int j = 0;j < iMaxLimit;j++)
		{
			LimitData *pLimit = m_vPesoLimitDataList[j];
			if( pLimit->m_iLimitDate == iLimitDate )
			{
				int iReturnPeso = -1;
				if( j < 9 )
					iReturnPeso = g_SaleMgr.GetPeso( ioSaleManager::IT_CLASS, iClassType, j ); // 기간제 페소는 dwType2가 0~8이다
				if( iReturnPeso != -1 )
					return iReturnPeso;

				if( bResell && pPrice->m_iBuyPeso <= 0 )
					return (float)DEFAULT_RESELL_PESO * pLimit->m_fLimitPricePer;

				return (float)pPrice->m_iBuyPeso * pLimit->m_fLimitPricePer;
			}
		}
	}
	
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioItemPriceManager::GetClassBuyPeso Error : %d - %d", iLimitDate, iClassType );
	return -1;
}

int ioItemPriceManager::GetSubscriptionType( int iClassType )
{
	int iMaxClass = m_vPriceList.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		if( !pPrice->m_bActive ) continue;
		if( pPrice->m_iSetCode != iClassType ) continue;

		return pPrice->m_iSubscriptionType;
	}

	return SUBSCRIPTION_NONE;
}

int ioItemPriceManager::GetClassBuyCash( int iClassType, int iLimitDate )
{
	int iMaxClass = m_vPriceList.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		if( !pPrice->m_bActive ) continue;
		if( pPrice->m_iSetCode != iClassType ) continue;

		int iMaxLimit = 0;	
		if( pPrice->m_eType == PT_PREMIUM )
			iMaxLimit = m_vPremiumCashLimitDataList.size();
		else if( pPrice->m_eType == PT_RARE )
			iMaxLimit = m_vRareCashLimitDataList.size();
		else
			iMaxLimit = m_vCashLimitDataList.size();

		for(int j = 0;j < iMaxLimit;j++)
		{
			LimitData *pLimit = NULL;
			if( pPrice->m_eType == PT_PREMIUM )
				pLimit = m_vPremiumCashLimitDataList[j];
			else if( pPrice->m_eType == PT_RARE )
				pLimit = m_vRareCashLimitDataList[j];
			else
				pLimit = m_vCashLimitDataList[j];

			if( pLimit->m_iLimitDate == iLimitDate )
			{
				int iReturnCash = -1;
				if( j < 9 )
					iReturnCash = g_SaleMgr.GetCash( ioSaleManager::IT_CLASS, iClassType, j ); // 기간제 캐쉬는 dwType2가 0~8이다
				if( iReturnCash != -1 )
					return iReturnCash;
				return (float)pPrice->m_iBuyCash * pLimit->m_fLimitPricePer;
			}
		}
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioItemPriceManager::GetClassBuyCash Error : %d - %d", iLimitDate, iClassType );
	return -1;
}

int ioItemPriceManager::GetMortmainCharPeso( int iClassType, bool bResell /*= false */ )
{
	int iMaxClass = m_vPriceList.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		if( !bResell )
		{
			if( !pPrice->m_bActive ) continue;
		}
		if( pPrice->m_iSetCode != iClassType ) continue;

		int iReturnPeso = g_SaleMgr.GetPeso( ioSaleManager::IT_CLASS, iClassType, 9 ); // 영구 페소는 dwType2가 9이다
		if( iReturnPeso != -1 )
			return iReturnPeso;

		if( bResell && pPrice->m_iBuyPeso <= 0 )
			return (float)DEFAULT_RESELL_PESO * m_fMortmainCharMultiply;

		return (float)pPrice->m_iBuyPeso * m_fMortmainCharMultiply;
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioItemPriceManager::GetMortmainCharPeso Error : %d", iClassType );
	return -1;
}

int ioItemPriceManager::GetMortmainCharCash( int iClassType )
{
	int iMaxClass = m_vPriceList.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		if( !pPrice->m_bActive ) continue;
		if( pPrice->m_iSetCode != iClassType ) continue;

		int iReturnCash = g_SaleMgr.GetCash( ioSaleManager::IT_CLASS, iClassType, 9 ); // 영구 캐쉬는 dwType2가 9이다
		if( iReturnCash != -1 )
			return iReturnCash;

		if( pPrice->m_eType == PT_PREMIUM )
			return (float)pPrice->m_iBuyCash * m_fMortmainPremiumCharMultiplyCash;
		else if( pPrice->m_eType == PT_RARE )
			return (float)pPrice->m_iBuyCash * m_fMortmainRareCharMultiplyCash;

		return (float)pPrice->m_iBuyCash * m_fMortmainCharMultiplyCash;
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioItemPriceManager::GetMortmainCharCash Error : %d", iClassType );
	return -1;
}

int ioItemPriceManager::GetBonusPeso( int iClassType, bool bMortmain )
{
	if( !bMortmain )
		return 0;

	int iMaxClass = m_vPriceList.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		if( !pPrice->m_bActive ) continue;
		if( pPrice->m_iSetCode != iClassType ) continue;

		return pPrice->m_iBonusPeso;
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioItemPriceManager::GetBonusPeso Error : %d", iClassType );
	return -1;
}

int ioItemPriceManager::GetLowPesoClass( int iLimitDate )
{
	if( m_vPriceList.empty() ) return -1;

	int iMaxClass = m_vPriceList.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		if( pPrice->m_bActive )
			return pPrice->m_iBuyPeso;
	}	
	return -1;
}

bool ioItemPriceManager::IsCompareBankruptcy( int iClassType, int iTutorialType )
{
	return true;
	if( m_iBankruptcyCount > (int)m_vPriceList.size() ) return false;
	
	if( iClassType == iTutorialType )
		return true;

	for(int i = 0;i < m_iBankruptcyCount;i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		if( pPrice->m_iSetCode == iClassType )
			return true;		
	}	
	return false;
}

bool ioItemPriceManager::IsCompareLimitDatePeso( int iClassType, int iLimitDate )
{
	int iMaxClass = m_vPriceList.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		if( !pPrice->m_bActive ) continue;
		if( pPrice->m_iSetCode != iClassType ) continue;

		int iMaxLimit = m_vPesoLimitDataList.size();
		for(int j = 0;j < iMaxLimit;j++)
		{
			LimitData *pLimit = m_vPesoLimitDataList[j];
			if( pLimit->m_iLimitDate == iLimitDate )
				return true;
		}

		return true;
	}
	return false;
}

bool ioItemPriceManager::IsCompareLimitDateCash( int iClassType, int iLimitDate )
{
	int iMaxClass = m_vPriceList.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		if( !pPrice->m_bActive ) continue;
		if( pPrice->m_iSetCode != iClassType ) continue;

		int iMaxLimit = 0;
		if( pPrice->m_eType == PT_PREMIUM )
			iMaxLimit = m_vPremiumCashLimitDataList.size();
		else if( pPrice->m_eType == PT_RARE )
			iMaxLimit = m_vRareCashLimitDataList.size();
		else
			iMaxLimit = m_vCashLimitDataList.size();

		for(int j = 0;j < iMaxLimit;j++)
		{
			LimitData *pLimit = NULL;
			if( pPrice->m_eType == PT_PREMIUM )
				pLimit = m_vPremiumCashLimitDataList[j];
			else if( pPrice->m_eType == PT_RARE )
				pLimit = m_vRareCashLimitDataList[j];
			else
				pLimit = m_vCashLimitDataList[j];

			if( pLimit->m_iLimitDate == iLimitDate )
				return true;
		}

		return true;
	}
	return false;
}

void ioItemPriceManager::FillClassBuyPriceInfo( SP2Packet &rkPacket )
{
	//최초 고용 용병 기간, 기본 기간, 파산 지급 기간
	rkPacket << m_dwFirstHireLimit << m_dwDefaultLimit << m_dwBankruptcyLimit << m_iBankruptcyCount << m_fLimitExtendDiscount << m_iBankruptcyPeso << m_fMortmainCharMultiply << m_fTimeCharDivision << m_fMortmainCharDivision << m_fMortmainCharMultiplyCash << m_fMortmainPremiumCharMultiplyCash << m_fMortmainRareCharMultiplyCash;

	int i = 0;
	int iMaxClass = m_vPriceList.size();
	rkPacket << GetMaxClassInfo();
	for(i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		rkPacket << pPrice->m_iSetCode << pPrice->m_bActive << pPrice->m_iBuyPeso << pPrice->m_iBuyCash << pPrice->m_iBonusPeso << (int)pPrice->m_eType;
	}	

	int iMaxLimit = m_vPesoLimitDataList.size();
	rkPacket << iMaxLimit;
	for(i = 0;i < iMaxLimit;i++)
	{
		LimitData *pLimit = m_vPesoLimitDataList[i];
		rkPacket << pLimit->m_iLimitDate << pLimit->m_fLimitPricePer;
	}

	iMaxLimit = m_vCashLimitDataList.size();
	rkPacket << iMaxLimit;
	for(i = 0;i < iMaxLimit;i++)
	{
		LimitData *pLimit = m_vCashLimitDataList[i];
		rkPacket << pLimit->m_iLimitDate << pLimit->m_fLimitPricePer;
	}

	iMaxLimit = m_vPremiumCashLimitDataList.size();
	rkPacket << iMaxLimit;
	for(i = 0;i < iMaxLimit;i++)
	{
		LimitData *pLimit = m_vPremiumCashLimitDataList[i];
		rkPacket << pLimit->m_iLimitDate << pLimit->m_fLimitPricePer;
	}

	iMaxLimit = m_vRareCashLimitDataList.size();
	rkPacket << iMaxLimit;
	for(i = 0;i < iMaxLimit;i++)
	{
		LimitData *pLimit = m_vRareCashLimitDataList[i];
		rkPacket << pLimit->m_iLimitDate << pLimit->m_fLimitPricePer;
	}
}

void ioItemPriceManager::SendClassBuyPriceInfo( User *pUser )
{
	if( pUser == NULL ) return;

	// 패킷을 1024byte 미만으로 나눠 보낸다.
	{   // 첫 정보 전송하여 클라이언트 데이터 초기화 및 기본값들 세팅
		SP2Packet kFirstPacket( STPK_CLASSPRICE );
		PACKET_GUARD_VOID( kFirstPacket.Write(true) ); 
		PACKET_GUARD_VOID( kFirstPacket.Write(m_dwFirstHireLimit) ); 
		PACKET_GUARD_VOID( kFirstPacket.Write(m_dwDefaultLimit) ); 
		PACKET_GUARD_VOID( kFirstPacket.Write(m_dwBankruptcyLimit) ); 
		PACKET_GUARD_VOID( kFirstPacket.Write(m_iBankruptcyCount) ); 
		PACKET_GUARD_VOID( kFirstPacket.Write(m_fLimitExtendDiscount) ); 
		PACKET_GUARD_VOID( kFirstPacket.Write(m_iBankruptcyPeso) ); 
		PACKET_GUARD_VOID( kFirstPacket.Write(m_fMortmainCharMultiply) ); 
		PACKET_GUARD_VOID( kFirstPacket.Write(m_fTimeCharDivision) ); 
		PACKET_GUARD_VOID( kFirstPacket.Write(m_fMortmainCharDivision) ); 
		PACKET_GUARD_VOID( kFirstPacket.Write(m_fMortmainCharMultiplyCash) ); 
		PACKET_GUARD_VOID( kFirstPacket.Write(m_fMortmainPremiumCharMultiplyCash) ); 
		PACKET_GUARD_VOID( kFirstPacket.Write(m_fMortmainRareCharMultiplyCash) ); 

		int i = 0;
		int iMaxLimit = m_vPesoLimitDataList.size();
		PACKET_GUARD_VOID( kFirstPacket.Write(iMaxLimit) );
		for(i = 0;i < iMaxLimit;i++)
		{
			LimitData *pLimit = m_vPesoLimitDataList[i];
			PACKET_GUARD_VOID( kFirstPacket.Write( pLimit->m_iLimitDate) );
			PACKET_GUARD_VOID( kFirstPacket.Write( pLimit->m_fLimitPricePer) );
		}

		iMaxLimit = m_vCashLimitDataList.size();
		PACKET_GUARD_VOID( kFirstPacket.Write(iMaxLimit) );
		for(i = 0;i < iMaxLimit;i++)
		{
			LimitData *pLimit = m_vCashLimitDataList[i];
			PACKET_GUARD_VOID( kFirstPacket.Write(pLimit->m_iLimitDate) );
			PACKET_GUARD_VOID( kFirstPacket.Write(pLimit->m_fLimitPricePer) );
		}

		iMaxLimit = m_vPremiumCashLimitDataList.size();
		PACKET_GUARD_VOID( kFirstPacket.Write(iMaxLimit) );
		for(i = 0;i < iMaxLimit;i++)
		{
			LimitData *pLimit = m_vPremiumCashLimitDataList[i];
			PACKET_GUARD_VOID( kFirstPacket.Write(pLimit->m_iLimitDate) );
			PACKET_GUARD_VOID( kFirstPacket.Write(pLimit->m_fLimitPricePer) );
		}

		iMaxLimit = m_vRareCashLimitDataList.size();
		PACKET_GUARD_VOID( kFirstPacket.Write(iMaxLimit) );
		for(i = 0;i < iMaxLimit;i++)
		{
			LimitData *pLimit = m_vRareCashLimitDataList[i];
			PACKET_GUARD_VOID( kFirstPacket.Write(pLimit->m_iLimitDate) );
			PACKET_GUARD_VOID( kFirstPacket.Write(pLimit->m_fLimitPricePer) );
		}

		pUser->SendMessage( kFirstPacket );
	}

	{
		// 용병 1개당 18byte : 50 * Char = 900byte - 50개씩 나눠 보냄
		const int iSendListSize = 50;
		int iMaxCharCount = (int)m_vPriceList.size();
		for(int iStartArray = 0;iStartArray < iMaxCharCount;)
		{
			int iLoop = iStartArray;
			int iSendSize = min( iMaxCharCount - iStartArray, iSendListSize );		
			SP2Packet kPacket( STPK_CLASSPRICE );
			PACKET_GUARD_VOID( kPacket.Write(false) );
			PACKET_GUARD_VOID( kPacket.Write(iSendSize) );
			for(;iLoop < iStartArray + iSendSize;iLoop++)
			{
				PriceData *pPrice = m_vPriceList[iLoop];		
				PACKET_GUARD_VOID( kPacket.Write(pPrice->m_iSetCode) );
				PACKET_GUARD_VOID( kPacket.Write(pPrice->m_bActive) ); 
				PACKET_GUARD_VOID( kPacket.Write(pPrice->m_iBuyPeso) ); 
				PACKET_GUARD_VOID( kPacket.Write(pPrice->m_iBuyCash) );
				PACKET_GUARD_VOID( kPacket.Write(pPrice->m_iBonusPeso) );
				PACKET_GUARD_VOID( kPacket.Write((int) pPrice->m_eType) );	
				PACKET_GUARD_VOID( kPacket.Write(pPrice->m_iSubscriptionType) );
				PACKET_GUARD_VOID( kPacket.Write(pPrice->m_bPcRoomActive ) );
				PACKET_GUARD_VOID( kPacket.Write(pPrice->m_bFreeDayHero ) );
				PACKET_GUARD_VOID( kPacket.Write(pPrice->m_iPcRoomStartDate ) );
				PACKET_GUARD_VOID( kPacket.Write(pPrice->m_iPcRoomEndDate ) );
			}
			pUser->SendMessage( kPacket );
			iStartArray = iLoop;
		}	
	}
}

bool ioItemPriceManager::IsCashOnly( int iClassType ) 
{
	int iMaxClass = m_vPriceList.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		if( !pPrice->m_bActive ) continue;
		if( pPrice->m_iSetCode != iClassType ) continue;

		if( pPrice->m_iBuyCash > 0 && pPrice->m_iBuyPeso <= 0 )
			return true;
		
	}
	return false;
}

bool ioItemPriceManager::IsPesoOnly( int iClassType ) 
{
	int iMaxClass = m_vPriceList.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		if( !pPrice->m_bActive ) continue;
		if( pPrice->m_iSetCode != iClassType ) continue;

		if( pPrice->m_iBuyCash <= 0 && pPrice->m_iBuyPeso > 0 )
			return true;

	}
	return false;
}

bool ioItemPriceManager::IsCashPeso( int iClassType ) 
{
	int iMaxClass = m_vPriceList.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		if( !pPrice->m_bActive ) continue;
		if( pPrice->m_iSetCode != iClassType ) continue;

		if( pPrice->m_iBuyCash > 0 && pPrice->m_iBuyPeso > 0 )
			return true;

	}
	return false;
}

bool ioItemPriceManager::IsActive( int iClassType )
{
	int iMaxClass = m_vPriceList.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		if( pPrice->m_iSetCode != iClassType ) continue;

		return pPrice->m_bActive;
	}
	return false;
}

bool ioItemPriceManager::IsPcRoomActive( int iClassType )
{
	int iMaxClass = m_vPriceList.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		if( pPrice->m_iSetCode != iClassType ) continue;

		return pPrice->m_bActive;
	}
	return false;
}

int ioItemPriceManager::GetTimeCharResellPeso( int iClassType, int iRemainTime )
{
	return ( (float)( ( (float)GetClassBuyPeso( iClassType, m_dwDefaultLimit, true ) / (float)m_dwDefaultLimit ) * iRemainTime ) / m_fTimeCharDivision );
}

int ioItemPriceManager::GetMortmainCharResellPeso( int iClassType )
{
	return (float) GetMortmainCharPeso( iClassType, true ) / m_fMortmainCharDivision;
}

ioItemPriceManager::PriceData *ioItemPriceManager::GetPriceData( int iArray )
{
	if( !COMPARE( iArray, 0, (int) m_vPriceList.size() ) )
		return NULL;

	return m_vPriceList[iArray];
}

ioItemPriceManager::PriceData *ioItemPriceManager::GetPriceDataToSetCode( int iSetCode )
{
	int iMaxClass = m_vPriceList.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		if( pPrice->m_iSetCode == iSetCode )
			return pPrice;
	}
	return NULL;
}

ioItemPriceManager::LimitData *ioItemPriceManager::GetPesoLimitData( int iArray )
{
	if( !COMPARE( iArray, 0, (int) m_vPesoLimitDataList.size() ) )
		return NULL;

	return m_vPesoLimitDataList[iArray];
}

ioItemPriceManager::LimitData *ioItemPriceManager::GetCashLimitData( int iArray )
{
	if( !COMPARE( iArray, 0, (int) m_vCashLimitDataList.size() ) )
		return NULL;

	return m_vCashLimitDataList[iArray];
}

ioItemPriceManager::LimitData *ioItemPriceManager::GetPremiumCashLimitData( int iArray )
{
	if( !COMPARE( iArray, 0, (int) m_vPremiumCashLimitDataList.size() ) )
		return NULL;

	return m_vPremiumCashLimitDataList[iArray];
}

ioItemPriceManager::LimitData *ioItemPriceManager::GetRareCashLimitData( int iArray )
{
	if( !COMPARE( iArray, 0, (int) m_vRareCashLimitDataList.size() ) )
		return NULL;

	return m_vRareCashLimitDataList[iArray];
}

int ioItemPriceManager::GetCash( int iClassType )
{
	int iMaxClass = m_vPriceList.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		if( !pPrice->m_bActive ) continue;
		if( pPrice->m_iSetCode != iClassType ) 
			continue;

		return pPrice->m_iBuyCash;
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Error : %d", __FUNCTION__, iClassType );
	return -1;
}

void ioItemPriceManager::SetCash( int iClassType, int iCash )
{
	int iMaxClass = m_vPriceList.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		if( !pPrice->m_bActive ) continue;
		if( pPrice->m_iSetCode != iClassType ) 
			continue;

		pPrice->m_iBuyCash = iCash;	
		return;
	}
}

void ioItemPriceManager::SetActive( int iClassType, bool bActive )
{
	int iMaxClass = m_vPriceList.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		if( pPrice->m_iSetCode != iClassType ) 
			continue;

		pPrice->m_bActive = bActive;
		return;
	}
}

void ioItemPriceManager::Process()
{
	if( TIMEGETTIME() - m_dwCurrentTime < 60000 ) return;

	m_dwCurrentTime = TIMEGETTIME();

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	CTimeSpan kAddTime( 0, m_dwUpdatePriceCollectedTime, 0, 0 );
	CTime cCurTime = CTime::GetCurrentTime() - kAddTime;
	DWORD dwCurTime = ( cCurTime.GetMonth() * 100 ) + cCurTime.GetDay();
	if( m_dwUpdateCollectedTime != dwCurTime )
	{		
		m_dwUpdateCollectedTime = dwCurTime;

		UpdatePriceCollected();
	}
}

void ioItemPriceManager::SetQuestSoldierCollected( int iClassType )
{
	int iPlusTime = ((float)m_iQuestSoldierCollectedTime / 60);
	CollectedMap::iterator iter = m_ClassCollectedMap.find( iClassType );
	if( iter == m_ClassCollectedMap.end() )
	{
		// 생성
		m_ClassCollectedMap.insert( CollectedMap::value_type( iClassType, (__int64)iPlusTime ) );
	}
	else
	{
		// 추가
		__int64 &rkClassTime = iter->second;
		rkClassTime += iPlusTime;
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SetQuestSoldierCollected (%d) : %d", (int)m_ClassCollectedMap.size(), iPlusTime );
}

void ioItemPriceManager::SetBuySoldierCollected( int iClassType, int iClassTime )
{
	int iPlusTime = ((float)iClassTime / 60);
	CollectedMap::iterator iter = m_ClassCollectedMap.find( iClassType );
	if( iter == m_ClassCollectedMap.end() )
	{
		// 생성
		m_ClassCollectedMap.insert( CollectedMap::value_type( iClassType, (__int64)iPlusTime ) );
	}
	else
	{
		// 추가
		__int64 &rkClassTime = iter->second;
		rkClassTime += iPlusTime;
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SetBuySoldierCollected (%d) : %d, %d", (int)m_ClassCollectedMap.size(), iClassType, iPlusTime );
}

void ioItemPriceManager::SetGradePlayTimeCollected( int iGradeLevel, int iPlayTime )
{
	int iPlusTime = ((float)iPlayTime / 60);
	CollectedMap::iterator iter = m_GradeCollectedMap.find( iGradeLevel );
	if( iter == m_GradeCollectedMap.end() )
	{
		// 생성
		m_GradeCollectedMap.insert( CollectedMap::value_type( iGradeLevel, (__int64)iPlusTime ) );
	}
	else
	{
		// 추가
		__int64 &rkPlayTime = iter->second;
		rkPlayTime += iPlusTime;
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][item]Grade play time Collected : [%d] [%d] [%d]", (int)m_GradeCollectedMap.size(), iGradeLevel, iPlusTime );
}

void ioItemPriceManager::UpdatePriceCollected()
{
	{   
		CollectedMap::iterator iter = m_ClassCollectedMap.begin();
		for(;iter != m_ClassCollectedMap.end();iter++)
		{
			g_DBClient.OnInsertSoldierPriceCollectedBuyTime( iter->first, iter->second );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][item]UpdatePriceCollected BuyChar : [%d] [%I64d]", iter->first, iter->second );
		}
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][item]Update Price Collected Buy Char End : [%d] [%d]", m_dwUpdateCollectedTime, (int)m_ClassCollectedMap.size() );
		m_ClassCollectedMap.clear();
	}

	{
		CollectedMap::iterator iter = m_GradeCollectedMap.begin();
		for(;iter != m_GradeCollectedMap.end();iter++)
		{
			g_DBClient.OnInsertSoldierPriceCollectedPlayTime( iter->first, iter->second );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "UpdatePriceCollected PlayTime:%d:%I64d", iter->first, iter->second );
		}
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][item]Update Price Collected PlayTime End : [%d] [%d]", m_dwUpdateCollectedTime, (int)m_GradeCollectedMap.size() );
		m_GradeCollectedMap.clear();
	}
}

bool ioItemPriceManager::GetPcRoomHeroActiveDate(int iClassType, __int64& iStartDate, __int64& iEndDate)
{
	int iMaxClass = m_vPriceList.size();
	for( int i = 0; i < iMaxClass; i++)
	{
		PriceData *pPrice = m_vPriceList[i];
		if( !pPrice ) continue;
		if( pPrice->m_iSetCode != iClassType )	continue;
		if( !pPrice->m_bPcRoomActive ) return false;

		iStartDate	= pPrice->m_iPcRoomStartDate;
		iEndDate	= pPrice->m_iPcRoomEndDate;
		return true;
	}

	return true;
}
#ifdef HERO_POSSIBLE_CHECK
bool ioItemPriceManager::IsPossibleHero( int iClassType )
{
	if(iClassType == 0 ) 
		return true;
	for( int i = 0 ; i < m_vPriceList.size() ; ++i )
	{
		PriceData *pPrice = m_vPriceList[i];
		if( !pPrice ) continue;
		if( pPrice->m_iSetCode == iClassType )
		{
			return pPrice->m_bHeroPossibleUse;
		}
	}
	return false;
}
#endif

bool ioItemPriceManager::IsActiveBuyHero(int iClassType)
{
	if(iClassType == 0 ) 
		return false;
	
	bool bState = false;
	for( int i = 0 ; i < m_vPriceList.size() ; ++i )
	{
		PriceData *pPrice = m_vPriceList[i];
		if( !pPrice ) continue;
		if( pPrice->m_iSetCode == iClassType )
		{
			 bState = pPrice->m_bActive;
			 return bState;
		}
	}
	return false;
}

bool ioItemPriceManager::IsActivePcRoomHero(int iClassType)
{
	__int64 iStartDate	= 0;
	__int64 iEndDate	= 0;

	if( !GetPcRoomHeroActiveDate(iClassType, iStartDate, iEndDate) )
		return false;

	if( iStartDate	== 0 || iEndDate	== 0 )
		return true;

	CTime cCurDate	= CTime::GetCurrentTime();

	CTime cStartDate(iStartDate);
	CTime cEndDate(iEndDate);

	if( cCurDate >= iStartDate && cCurDate < iEndDate )
		return true;

	return false;
}