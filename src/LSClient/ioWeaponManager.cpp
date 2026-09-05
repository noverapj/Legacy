

#include "stdafx.h"

#include "WeaponDefine.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

template<> ioWeaponManager* Singleton< ioWeaponManager >::ms_Singleton = 0;

ioWeaponManager::ioWeaponManager( ioSceneManager *pSceneMgr )
{
	m_pSceneMgr = pSceneMgr;
	m_pPlayStage = NULL;

	m_vCallWeaponInfoList.clear();

	m_iLocalIndex = 0;
}

ioWeaponManager::~ioWeaponManager()
{
	ErrorReport::SetPosition( 3100, 1 );
	ClearWeaponInfo();
	ErrorReport::SetPosition( 3100, 2 );

	m_CheckWeaponInfoList.clear();
	m_ApplyWeaponInfoList.clear();
	ErrorReport::SetPosition( 3100, 3 );
	m_ApplyWeaponChecker.clear();
	ErrorReport::SetPosition( 3100, 4 );

	/*
	for(GroupLoaderList::iterator iter = m_vGroupLoaderList.begin(); iter != m_vGroupLoaderList.end(); ++iter)
	{
		ioINILoader *pLoader = (*iter);
		SAFEDELETE( pLoader );
	}
	m_vGroupLoaderList.clear();
	*/

	for(GroupLoaderMap::iterator iter = m_GroupLoaderMap.begin(); iter != m_GroupLoaderMap.end(); ++iter)
	{
		ioINILoader *pLoader = iter->second;
		SAFEDELETE( pLoader );
	}
	m_GroupLoaderMap.clear();
}

void ioWeaponManager::ClearWeaponInfo()
{
	DestroyWeaponList();
	m_vCallWeaponInfoList.clear();
}

void ioWeaponManager::InitManager()
{
	m_pPlayStage = NULL;

	LoadMercenaryInfo();
	LoadNPCInfo();
	LoadAccessoryInfo();
}

void ioWeaponManager::SetPlayStage( ioPlayStage *pStage )
{
	m_pPlayStage = pStage;
}

void ioWeaponManager::LoadMercenaryInfo()
{
	m_vWeaponAttributeList.clear();

	for(GroupLoaderMap::iterator iter = m_GroupLoaderMap.begin(); iter != m_GroupLoaderMap.end(); ++iter)
	{
		ioINILoader *pLoader = iter->second;
		SAFEDELETE( pLoader );
	}
	m_GroupLoaderMap.clear();

	ioINILoader_e kLoader( "config/sp2_attack_attribute.ini" );
	kLoader.SetTitle_e( "common" );

	int iGroupCnt = kLoader.LoadInt_e( "group_cnt", 0 );

	char szFileName[MAX_PATH] = "";
	char szTitle[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";


	// Attribute Load
	for( int i=0; i < iGroupCnt; i++ )
	{
		int iSetItemNum = i;

		wsprintf_e( szTitle, "group%d", iSetItemNum );
		kLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		wsprintf_e( szFileName, "config/%s", szBuf );
	
		ioINILoader *pSetItemLoader = new ioINILoader;
		if( !pSetItemLoader )
			continue;

		pSetItemLoader->SetFileName( szFileName );
		int iAttributeCnt = pSetItemLoader->LoadInt_e( "common", "attribute_cnt", 0 );

		for( int j=0; j < iAttributeCnt; ++j )
		{
			int iAttributeIndex = iSetItemNum * 100 + j + 1;
			wsprintf_e( szTitle, "attribute%d", iAttributeIndex );

			pSetItemLoader->SetTitle( szTitle );

			ParseWeaponAttribute( *pSetItemLoader, iAttributeIndex );
		}

		//m_vGroupLoaderList.push_back( pSetItemLoader );
		m_GroupLoaderMap.insert( GroupLoaderMap::value_type(iSetItemNum, pSetItemLoader) );
	}

	ioWeapon::InitStaticValue( kLoader );
	LoadActionStop( kLoader );
}

void ioWeaponManager::LoadNPCInfo()
{
	ioINILoader_e kLoader( "config/sp2_attack_attribute_npc.ini" );
	kLoader.SetTitle_e( "common" );

	int iGroupCnt = kLoader.LoadInt_e( "group_cnt", 0 );

	char szFileName[MAX_PATH] = "";
	char szTitle[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";


	// Attribute Load
	for( int i=0; i < iGroupCnt; i++ )
	{
		int iSetItemNum = i;

		wsprintf_e( szTitle, "group%d", iSetItemNum );
		kLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		wsprintf_e( szFileName, "config/%s", szBuf );

		ioINILoader *pSetItemLoader = new ioINILoader;
		if( !pSetItemLoader )
			continue;

		pSetItemLoader->SetFileName( szFileName );

		iSetItemNum += 1000;

		int iAttributeCnt = pSetItemLoader->LoadInt_e( "common", "attribute_cnt", 0 );
		for( int j=0; j < iAttributeCnt; ++j )
		{
			int iAttributeIndex = iSetItemNum * 100 + j + 1;
			wsprintf_e( szTitle, "attribute%d", iAttributeIndex );

			pSetItemLoader->SetTitle( szTitle );

			ParseWeaponAttribute( *pSetItemLoader, iAttributeIndex );
		}

		//m_vGroupLoaderList.push_back( pSetItemLoader );
		m_GroupLoaderMap.insert( GroupLoaderMap::value_type(iSetItemNum, pSetItemLoader) );
	}
}

void ioWeaponManager::LoadAccessoryInfo()
{
	ioINILoader_e kLoader( "config/sp2_accessory_attack_attribute.ini" );
	kLoader.SetTitle_e( "common" );

	int iGroupCnt = kLoader.LoadInt_e( "sub_group_cnt", 0 );

	char szFileName[MAX_PATH] = "";
	char szTitle[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";

	for( int i=1; i <= MAX_ACC_INVENTORY; ++i )
	{
		for( int j=0; j < iGroupCnt; ++j )
		{
			int iSetItemNum = i*10000 + j;
			wsprintf_e( szTitle, "group%d_%d", i, j );
			kLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
			wsprintf_e( szFileName, "config/%s", szBuf );

			ioINILoader *pSetItemLoader = new ioINILoader;
			if( !pSetItemLoader )
				continue;
			pSetItemLoader->SetFileName( szFileName );
			int iAttributeCnt = pSetItemLoader->LoadInt_e( "common", "attribute_cnt", 0 );

			for( int k=0; k < iAttributeCnt; ++k )
			{
				int iAttributeIndex = iSetItemNum * 100 + k + 1;
				wsprintf_e( szTitle, "attribute%d", iAttributeIndex );

				pSetItemLoader->SetTitle( szTitle );

				ParseWeaponAttribute( *pSetItemLoader, iAttributeIndex );
			}
			m_GroupLoaderMap.insert( GroupLoaderMap::value_type(iSetItemNum, pSetItemLoader) );
		}
	}
}

void ioWeaponManager::LoadActionStop( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	rkLoader.SetTitle_e( "action_stop_type" );

	rkLoader.LoadString_e( "contact", "none", szBuf, MAX_PATH );
	ioContactWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "contact_die", "none", szBuf, MAX_PATH );
	ioContactWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "missile", "none", szBuf, MAX_PATH );
	ioMissileWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "missile_die", "none", szBuf, MAX_PATH );
	ioMissileWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "throw", "none", szBuf, MAX_PATH );
	ioThrowWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "throw_die", "none", szBuf, MAX_PATH );
	ioThrowWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "bomb", "none", szBuf, MAX_PATH );
	ioBombWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "bomb_die", "none", szBuf, MAX_PATH );
	ioBombWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "bomb_missile", "none", szBuf, MAX_PATH );
	ioBombMissileWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "bomb_missile_die", "none", szBuf, MAX_PATH );
	ioBombMissileWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "mine", "none", szBuf, MAX_PATH );
	ioMineWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "mine_die", "none", szBuf, MAX_PATH );
	ioMineWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "nuclear", "none", szBuf, MAX_PATH );
	ioNuclearWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "nuclear_die", "none", szBuf, MAX_PATH );
	ioNuclearWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "flame", "none", szBuf, MAX_PATH );
	ioFlameWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "flame_die", "none", szBuf, MAX_PATH );
	ioFlameWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "flamewave", "none", szBuf, MAX_PATH );
	ioFlamewaveWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "flamewave_die", "none", szBuf, MAX_PATH );
	ioFlamewaveWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "zoneeffect", "none", szBuf, MAX_PATH );
	ioZoneEffectWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "zoneeffect_die", "none", szBuf, MAX_PATH );
	ioZoneEffectWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "splash", "none", szBuf, MAX_PATH );
	ioSplashWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "splash_die", "none", szBuf, MAX_PATH );
	ioSplashWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "arrow", "none", szBuf, MAX_PATH );
	ioArrowWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "arrow_die", "none", szBuf, MAX_PATH );
	ioArrowWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "blizzard", "none", szBuf, MAX_PATH );
	ioBlizzardWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "blizzard_die", "none", szBuf, MAX_PATH );
	ioBlizzardWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "throw_bomb", "none", szBuf, MAX_PATH );
	ioThrowBombWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "throw_bomb_die", "none", szBuf, MAX_PATH );
	ioThrowBombWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "float_throw", "none", szBuf, MAX_PATH );
	ioFloatThrowWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "float_throw_die", "none", szBuf, MAX_PATH );
	ioFloatThrowWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "throw_return", "none", szBuf, MAX_PATH );
	ioThrowReturnWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "throw_return_die", "none", szBuf, MAX_PATH );
	ioThrowReturnWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "explosion", "none", szBuf, MAX_PATH );
	ioExplosionWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "explosion_die", "none", szBuf, MAX_PATH );
	ioExplosionWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "cannon_ball", "none", szBuf, MAX_PATH );
	ioCannonBallWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "cannon_ball_die", "none", szBuf, MAX_PATH );
	ioCannonBallWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "rotation", "none", szBuf, MAX_PATH );
	ioRotationWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "rotation_die", "none", szBuf, MAX_PATH );
	ioRotationWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "trace", "none", szBuf, MAX_PATH );
	ioTraceWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "trace_die", "none", szBuf, MAX_PATH );
	ioTraceWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "bound_bomb", "none", szBuf, MAX_PATH );
	ioBoundBombWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "bound_bomb_die", "none", szBuf, MAX_PATH );
	ioBoundBombWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "target", "none", szBuf, MAX_PATH );
	ioTargetWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "target_die", "none", szBuf, MAX_PATH );
	ioTargetWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "chain", "none", szBuf, MAX_PATH );
	ioChainWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "chain_die", "none", szBuf, MAX_PATH );
	ioChainWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "ball", "none", szBuf, MAX_PATH );
	ioBallWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "ball_die", "none", szBuf, MAX_PATH );
	ioBallWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "ignore_missile", "none", szBuf, MAX_PATH );
	ioIgnoreMissileWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "ignore_missile_die", "none", szBuf, MAX_PATH );
	ioIgnoreMissileWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "area", "none", szBuf, MAX_PATH );
	ioAreaEffectWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "area_die", "none", szBuf, MAX_PATH );
	ioAreaEffectWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "map_attach", "none", szBuf, MAX_PATH );
	ioMapAttachWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "map_attach_die", "none", szBuf, MAX_PATH );
	ioMapAttachWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "limit_attach", "none", szBuf, MAX_PATH );
	ioLimitAttachWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "limit_attach_die", "none", szBuf, MAX_PATH );
	ioLimitAttachWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "owner_attack", "none", szBuf, MAX_PATH );
	ioOwnerAttackWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "owner_attack_die", "none", szBuf, MAX_PATH );
	ioOwnerAttackWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "run_contact", "none", szBuf, MAX_PATH );
	ioRunContactWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "run_contact_die", "none", szBuf, MAX_PATH );
	ioRunContactWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "whip_contact", "none", szBuf, MAX_PATH );
	ioWhipWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "whip_contact_die", "none", szBuf, MAX_PATH );
	ioWhipWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "ice_missile_contact", "none", szBuf, MAX_PATH );
	ioIceMissileWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "ice_missile_contact_die", "none", szBuf, MAX_PATH );
	ioIceMissileWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "summon_missile_contact", "none", szBuf, MAX_PATH );
	ioSummonMissileWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "summon_missile_contact_die", "none", szBuf, MAX_PATH );
	ioSummonMissileWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "ground_trace", "none", szBuf, MAX_PATH );
	ioGroundTraceWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "ground_trace_die", "none", szBuf, MAX_PATH );
	ioGroundTraceWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "control_return", "none", szBuf, MAX_PATH );
	ioControlReturnWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "control_return_die", "none", szBuf, MAX_PATH );
	ioControlReturnWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "control_missile", "none", szBuf, MAX_PATH );
	ioControlMissileWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "control_missile_die", "none", szBuf, MAX_PATH );
	ioControlMissileWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "trace_return", "none", szBuf, MAX_PATH );
	ioTraceReturnWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "trace_return_die", "none", szBuf, MAX_PATH );
	ioTraceReturnWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "tsunami", "none", szBuf, MAX_PATH );
	ioTsunamiWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "tsunami_die", "none", szBuf, MAX_PATH );
	ioTsunamiWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "tracetype2", "none", szBuf, MAX_PATH );
	ioTraceWeaponType2::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "tracetype2_die", "none", szBuf, MAX_PATH );
	ioTraceWeaponType2::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "normal", "none", szBuf, MAX_PATH );
	ioNormalWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "normal_die", "none", szBuf, MAX_PATH );
	ioNormalWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "gungnir", "none", szBuf, MAX_PATH );
	ioAdhesiveMissileWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "gungnir_die", "none", szBuf, MAX_PATH );
	ioAdhesiveMissileWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "limit_attach2", "none", szBuf, MAX_PATH );
	ioLimitAttachWeapon2::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "limit_attach2_die", "none", szBuf, MAX_PATH );
	ioLimitAttachWeapon2::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "contact2", "none", szBuf, MAX_PATH );
	ioContactWeapon2::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "contact2_die", "none", szBuf, MAX_PATH );
	ioContactWeapon2::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "bomb_missile2", "none", szBuf, MAX_PATH );
	ioBombMissileWeapon2::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "bomb_missile2_die", "none", szBuf, MAX_PATH );
	ioBombMissileWeapon2::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "gungnir2", "none", szBuf, MAX_PATH );
	ioAdhesiveMissileWeapon2::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "gungnir2_die", "none", szBuf, MAX_PATH );
	ioAdhesiveMissileWeapon2::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "zone_effect2", "none", szBuf, MAX_PATH );
	ioZoneEffect2Weapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "zone_effect2_die", "none", szBuf, MAX_PATH );
	ioZoneEffect2Weapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "uroborus", "none", szBuf, MAX_PATH );
	ioUroborusWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "uroborus_die", "none", szBuf, MAX_PATH );
	ioUroborusWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "uroborus2", "none", szBuf, MAX_PATH );
	ioUroborusWeapon2::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "uroborus2_die", "none", szBuf, MAX_PATH );
	ioUroborusWeapon2::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "tracetype3", "none", szBuf, MAX_PATH );
	ioTraceWeaponType2::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "tracetype3_die", "none", szBuf, MAX_PATH );
	ioTraceWeaponType2::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "missile2", "none", szBuf, MAX_PATH );
	ioMissileWeapon2::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "missile2_die", "none", szBuf, MAX_PATH );
	ioMissileWeapon2::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "call_attribute_arrow", "none", szBuf, MAX_PATH );
	ioCallAttributeArrowWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "call_attribute_arrow_die", "none", szBuf, MAX_PATH );
	ioCallAttributeArrowWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "return_pingpong", "none", szBuf, MAX_PATH );
	ioReturnPingpongWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "return_pingpong_die", "none", szBuf, MAX_PATH );
	ioReturnPingpongWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "missile3", "none", szBuf, MAX_PATH );
	ioMissileWeapon3::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "missile3_die", "none", szBuf, MAX_PATH );
	ioMissileWeapon3::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "lobelia", "none", szBuf, MAX_PATH );
	ioLobeliaWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "lobelia_die", "none", szBuf, MAX_PATH );
	ioLobeliaWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "sylphid_missile", "none", szBuf, MAX_PATH );
	ioSylphidMissileWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "sylphid_missile_die", "none", szBuf, MAX_PATH );
	ioSylphidMissileWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "guide", "none", szBuf, MAX_PATH );
	ioGuideWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "guide_die", "none", szBuf, MAX_PATH );
	ioGuideWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "ball2", "none", szBuf, MAX_PATH );
	ioGuideWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "ball2_die", "none", szBuf, MAX_PATH );
	ioGuideWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "guide2", "none", szBuf, MAX_PATH );
	ioGuideWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "guide2_die", "none", szBuf, MAX_PATH );
	ioGuideWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "trace4", "none", szBuf, MAX_PATH );
	ioTraceWeapon4::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "trace4_die", "none", szBuf, MAX_PATH );
	ioTraceWeapon4::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "fall_weapon", "none", szBuf, MAX_PATH );
	ioFallWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "fall_weapon_die", "none", szBuf, MAX_PATH );
	ioFallWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "trace5", "none", szBuf, MAX_PATH );
	ioTraceWeapon4::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "trace5_die", "none", szBuf, MAX_PATH );
	ioTraceWeapon4::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "double_ice_missile", "none", szBuf, MAX_PATH );
	ioDoubleIceMissileWaepon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "double_ice_missile_die", "none", szBuf, MAX_PATH );
	ioDoubleIceMissileWaepon::m_DieActionStopType = ParseActionStop( szBuf );

 	rkLoader.LoadString_e( "smart_bomb", "none", szBuf, MAX_PATH );
 	ioSmartBombWeapon::m_ActionStopType = ParseActionStop( szBuf );
 	rkLoader.LoadString_e( "smart_bomb_die", "none", szBuf, MAX_PATH );
 	ioSmartBombWeapon::m_DieActionStopType = ParseActionStop( szBuf );
	
	rkLoader.LoadString_e( "pull_weapon", "none", szBuf, MAX_PATH );
	ioPullWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "pull_weapon_die", "none", szBuf, MAX_PATH );
	ioPullWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "gungnir3", "none", szBuf, MAX_PATH );
	ioAdhesiveMissileWeapon3::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "gungnir3_die", "none", szBuf, MAX_PATH );
	ioAdhesiveMissileWeapon3::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "graffiti_weapon", "none", szBuf, MAX_PATH );
	ioGraffitiWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "graffiti_weapon_die", "none", szBuf, MAX_PATH );
	ioGraffitiWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "mine2", "none", szBuf, MAX_PATH );
	ioMineWeapon2::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "mine_die2", "none", szBuf, MAX_PATH );
	ioMineWeapon2::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "mine3", "none", szBuf, MAX_PATH );
	ioMineWeapon3::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "mine_die3", "none", szBuf, MAX_PATH );
	ioMineWeapon3::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "mine4", "none", szBuf, MAX_PATH );
	ioMineWeapon4::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "mine_die4", "none", szBuf, MAX_PATH );
	ioMineWeapon4::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "trace6", "none", szBuf, MAX_PATH );
	ioTraceWeapon6::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "trace6_die", "none", szBuf, MAX_PATH );
	ioTraceWeapon6::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "control_sword", "none", szBuf, MAX_PATH );
	ioControlSwordWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "control_sword_die", "none", szBuf, MAX_PATH );
	ioControlSwordWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "pull_weapon2", "none", szBuf, MAX_PATH );
	ioPullWeapon2::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "pull_weapon2_die", "none", szBuf, MAX_PATH );
	ioPullWeapon2::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "ground_trace2", "none", szBuf, MAX_PATH );
	ioGroundTraceWeapon2::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "ground_trace2_die", "none", szBuf, MAX_PATH );
	ioGroundTraceWeapon2::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "select_position_move", "none", szBuf, MAX_PATH );
	ioSelectPositionMoveWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "select_position_move_die", "none", szBuf, MAX_PATH );
	ioSelectPositionMoveWeapon::m_DieActionStopType = ParseActionStop( szBuf );	

	rkLoader.LoadString_e( "gungnir4", "none", szBuf, MAX_PATH );
	ioAdhesiveMissileWeapon4::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "gungnir4_die", "none", szBuf, MAX_PATH );
	ioAdhesiveMissileWeapon4::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "bomb_missile4", "none", szBuf, MAX_PATH );
	ioBombMissileWeapon4::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "bomb_missile4_die", "none", szBuf, MAX_PATH );
	ioBombMissileWeapon4::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "return_missile", "none", szBuf, MAX_PATH );
	ioReturnMissileWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "return_missile_die", "none", szBuf, MAX_PATH );
	ioReturnMissileWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "rotate_target_weapon", "none", szBuf, MAX_PATH );
	ioRotateTargetWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "rotate_target_weapon_die", "none", szBuf, MAX_PATH );
	ioRotateTargetWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "rotation2", "none", szBuf, MAX_PATH );
	ioRotationWeapon2::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "rotation_die2", "none", szBuf, MAX_PATH );
	ioRotationWeapon2::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "control_mesh_weapon", "none", szBuf, MAX_PATH );
	ioControlMeshWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "control_mesh_weapon_die", "none", szBuf, MAX_PATH );
	ioControlMeshWeapon::m_DieActionStopType = ParseActionStop( szBuf );

	rkLoader.LoadString_e( "attach_aera_weapon", "none", szBuf, MAX_PATH );
	ioAttachAeraWeapon::m_ActionStopType = ParseActionStop( szBuf );
	rkLoader.LoadString_e( "attach_aera_weapon_die", "none", szBuf, MAX_PATH );
	ioAttachAeraWeapon::m_DieActionStopType = ParseActionStop( szBuf );
}

void ioWeaponManager::ParseWeaponAttribute( ioINILoader &rkLoader, int iAttributeIndex )
{
	WeaponAttribute kAttribute;

	kAttribute.m_iAttributeIndex = iAttributeIndex;
	kAttribute.m_WeaponType = (AttackWeaponType)rkLoader.LoadInt_e( "type", 0 );
	kAttribute.m_BlowTargetType = (BlowTargetType)rkLoader.LoadInt_e( "blow_target", BTT_WEAK );
	kAttribute.m_PiercingType = (CharPiercingType)rkLoader.LoadInt_e( "char_piercing", CPT_NONE );
	kAttribute.m_DefenseBreakType = (DefenseBreakType)rkLoader.LoadInt_e( "defense_break_type", DBT_NONE );
	kAttribute.m_DefenseSuccessType = (DefenseSuccessType)rkLoader.LoadInt_e( "defense_success_ani_type", DST_WEAK );
	kAttribute.m_EnableSideType = (EnableSideType)rkLoader.LoadInt_e( "enable_side_type", EST_NORMAL );
	kAttribute.m_EscapeCatchType = (EscapeCatchType)rkLoader.LoadInt_e( "enable_escape_catch", ECT_NONE );

	if( !COMPARE( kAttribute.m_PiercingType, CPT_NONE, CPT_MAX ) )
		kAttribute.m_PiercingType = CPT_NONE;

	if( !COMPARE( kAttribute.m_DefenseBreakType, DBT_NONE, DBT_MAX ) )
		kAttribute.m_DefenseBreakType = DBT_NONE;

	if( !COMPARE( kAttribute.m_DefenseSuccessType, DST_WEAK, DST_MAX ) )
		kAttribute.m_DefenseSuccessType = DST_WEAK;

	if( !COMPARE( kAttribute.m_EnableSideType, EST_NORMAL, EST_MAX ) )
		kAttribute.m_EnableSideType = EST_NORMAL;

	kAttribute.m_ElementType  = (ElementType)rkLoader.LoadInt_e( "element_type", ET_NONE );
	kAttribute.m_StatType	= (AttackStatType)rkLoader.LoadInt_e( "stat_type", 0 );
	kAttribute.m_AttackMethodType = (AttackMethodType)rkLoader.LoadInt_e( "attack_method_type", AMT_NONE );

	kAttribute.m_iShakeCamera = rkLoader.LoadInt_e( "shake_camera", 0 );

	kAttribute.m_fPushPower = rkLoader.LoadFloat_e( "push_power", FLOAT1 );
	kAttribute.m_fBlowPower = rkLoader.LoadFloat_e( "blow_power", FLOAT1 );

	kAttribute.m_bNoCalcFloatingCnt = rkLoader.LoadBool_e( "no_calc_blow_cnt_power", false );
	
	kAttribute.m_fAirPushPower = rkLoader.LoadFloat_e( "air_push_power", -FLOAT1 );
	if( kAttribute.m_fAirPushPower < 0.0f )
		kAttribute.m_fAirPushPower = kAttribute.m_fPushPower;

	kAttribute.m_fAirBlowPower = rkLoader.LoadFloat_e( "air_blow_power", -FLOAT1 );
	if( kAttribute.m_fAirBlowPower < 0.0f )
		kAttribute.m_fAirBlowPower = kAttribute.m_fBlowPower;


	kAttribute.m_fBallPushPower = rkLoader.LoadFloat_e( "ball_push_power", FLOAT1 );
	kAttribute.m_fBallBlowPower = rkLoader.LoadFloat_e( "ball_blow_power", FLOAT1 );

	kAttribute.m_fBallAirPushPower = rkLoader.LoadFloat_e( "ball_air_push_power", -FLOAT1 );
	if( kAttribute.m_fBallAirPushPower < 0.0f )
		kAttribute.m_fBallAirPushPower = kAttribute.m_fBallPushPower;

	kAttribute.m_fBallAirBlowPower = rkLoader.LoadFloat_e( "ball_air_blow_power", -FLOAT1 );
	if( kAttribute.m_fBallAirBlowPower < 0.0f )
		kAttribute.m_fBallAirBlowPower = kAttribute.m_fBallBlowPower;

	kAttribute.m_fGangsiPushPower = rkLoader.LoadFloat_e( "gangsi_push_power", FLOAT1 );
	kAttribute.m_fGangsiBlowPower = rkLoader.LoadFloat_e( "gangsi_blow_power", 0.0f );

	kAttribute.m_fGangsiAirPushPower = rkLoader.LoadFloat_e( "gangsi_air_push_power", FLOAT1 );	
	if( kAttribute.m_fGangsiAirPushPower < 0.0f )
		kAttribute.m_fGangsiAirPushPower = kAttribute.m_fGangsiPushPower;

	kAttribute.m_fGangsiAirBlowPower = rkLoader.LoadFloat_e( "gangsi_air_blow_power", 0.0f );
	if( kAttribute.m_fGangsiAirBlowPower < 0.0f )
		kAttribute.m_fGangsiAirBlowPower = kAttribute.m_fGangsiBlowPower;

	kAttribute.m_fDefensePushPower = rkLoader.LoadFloat_e( "defense_push_power", -FLOAT1 );
	if( kAttribute.m_fDefensePushPower < 0.0f )
		kAttribute.m_fDefensePushPower = kAttribute.m_fPushPower;

	kAttribute.m_fPushAngle = rkLoader.LoadFloat_e( "push_angle", 0.0f );

	kAttribute.m_fDefenseDamageRate = rkLoader.LoadFloat_e( "defense_damage_rate", 0.0f );
	kAttribute.m_fDefenseDamageRate2 = rkLoader.LoadFloat_e( "defense_damage_rate2", FLOAT1 );

	kAttribute.m_fDamageRate = rkLoader.LoadFloat_e( "damage_rate", 7.0f );

	kAttribute.m_fDefenseEnableRate = rkLoader.LoadFloat_e( "wounded_defense_enable_rate", 0.0f );
	kAttribute.m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", 0.0f );

	kAttribute.m_fWereWolfChangeDamage = rkLoader.LoadFloat_e( "werewolf_change_damage", 0.0f );

	kAttribute.m_bEnableSylphid = rkLoader.LoadBool_e( "enable_sylphid", false );
	kAttribute.m_iSylphidNeed = rkLoader.LoadInt_e( "sylphid_need", 0 );

	kAttribute.m_bEnablePossession = rkLoader.LoadBool_e( "enable_possession", false );

	kAttribute.m_WeaponExtraGaugeType = (WeaponExtraGaugeType)rkLoader.LoadInt_e( "extra_gauge_type", WGT_NONE );

	kAttribute.m_dwIgnoreWeaponTime = (DWORD)rkLoader.LoadInt_e( "weapon_ignore_num_time", 0 );

	kAttribute.m_dwTypeProtectedTime = (DWORD)rkLoader.LoadInt_e( "weapon_ignore_type_time", 0 );
	kAttribute.m_dwProtectedType = (DWORD)rkLoader.LoadInt_e( "weapon_ignore_type_time_type", 0 );

	kAttribute.m_bUseGrapplingTarget = rkLoader.LoadBool_e( "use_grappling_target", false );

	char szBuf[MAX_PATH], szBuf2[MAX_PATH];
	rkLoader.LoadString_e( "explosion_effect", "", szBuf, MAX_PATH );
	kAttribute.m_ExplosionEffect = szBuf;

	rkLoader.LoadString_e( "extra_explosion_effect", "", szBuf, MAX_PATH );
	kAttribute.m_ExtraExplosionEffect = szBuf;

	rkLoader.LoadString_e( "weapon_effect", "", szBuf, MAX_PATH );
	kAttribute.m_WeaponEffect = szBuf;

	rkLoader.LoadString_e( "weapon_end_effect", "", szBuf, MAX_PATH );
	kAttribute.m_WeaponEndEffect = szBuf;

	kAttribute.m_bNoExplosionEffectWithChar = rkLoader.LoadBool_e( "no_explosion_effect_char", false );

	rkLoader.LoadString_e( "explosion_sound", "", szBuf, MAX_PATH );
	kAttribute.m_ExplosionSound = szBuf;

	rkLoader.LoadString_e( "wounded_sound", "", szBuf, MAX_PATH );
	kAttribute.m_WoundedSound = szBuf;

	rkLoader.LoadString_e( "defensed_sound", "", szBuf, MAX_PATH );
	kAttribute.m_DefensedSound = szBuf;
	
	rkLoader.LoadString_e( "loop_sound", "", szBuf, MAX_PATH );
	kAttribute.m_LoopSound = szBuf;

	kAttribute.m_fMoveSpeed = rkLoader.LoadFloat_e( "move_speed", 0.0f );
	kAttribute.m_fMoveRange = rkLoader.LoadFloat_e( "move_range", FLOAT100 );
	kAttribute.m_fFloatPower = rkLoader.LoadFloat_e( "float_power", 0.0f );
	kAttribute.m_fMapCollisionBoxRate = rkLoader.LoadFloat_e( "map_collisionbox_rate", FLOAT1 );
	kAttribute.m_fScaleRate = rkLoader.LoadFloat_e( "scale_rate", 0.0f );
	kAttribute.m_iBlockingRate = rkLoader.LoadInt_e( "blocking_rate", 0 );
	kAttribute.m_iNpcBlockingType = rkLoader.LoadInt_e( "npc_blocking_type", 0 );
	kAttribute.m_iNpcBlockingRate = rkLoader.LoadInt_e( "npc_blocking_rate", kAttribute.m_iBlockingRate );
	kAttribute.m_iGangsiBlockingRate = rkLoader.LoadInt_e( "gangsi_blocking_rate", 100 );
	kAttribute.m_iFCBlockingRate = rkLoader.LoadInt_e( "FC_blocking_rate", 100 );

	kAttribute.m_dwCollisionDelay = rkLoader.LoadInt_e( "collision_delay", 0 );
	kAttribute.m_dwPiercingLiveTime = rkLoader.LoadInt_e( "piercing_live_time", 0 );

	kAttribute.m_dwRefreshDuration = rkLoader.LoadInt_e( "collision_list_refresh_time", 0 );
	kAttribute.m_dwIgnoreDuration = rkLoader.LoadInt_e( "weapon_ignore_time", 0 );

	kAttribute.m_fDownStateBlowPowerRate = rkLoader.LoadFloat_e( "down_state_blow_power_rate", FLOAT1 );
	kAttribute.m_fDownStatePushPowerRate = rkLoader.LoadFloat_e( "down_state_push_power_rate", FLOAT1 );

	kAttribute.m_fBoundBlowJumpAmt = rkLoader.LoadFloat_e( "bound_blow_jump_power", 0.0f );
	kAttribute.m_fBoundBlowForceAmt = rkLoader.LoadFloat_e( "bound_blow_force_power", 0.0f );
	kAttribute.m_fBoundBlowGravityRate = rkLoader.LoadFloat_e( "bound_blow_gravity_rate", FLOAT1 );
	kAttribute.m_fBoundBlowBoundDamage = rkLoader.LoadFloat_e( "bound_blow_damage_rate", 0.0f );

	rkLoader.LoadString_e( "extend_wound_ani", "", szBuf, MAX_PATH );
	kAttribute.m_ExtendWoundAni = szBuf;
	kAttribute.m_dwExtendWoundDuration = rkLoader.LoadInt_e( "extend_wound_duration", 0 );
	kAttribute.m_fExtendWoundJumpAmt = rkLoader.LoadFloat_e( "extend_wound_jump_power", 0.0f );
	kAttribute.m_fExtendWoundForceAmt = rkLoader.LoadFloat_e( "extend_wound_force_power", 0.0f );

	BlowWoundAniInfo kInfo;
	ParseBlowWoundAniInfo( kInfo, rkLoader );
	kAttribute.m_BlowWoundAniInfo = kInfo;

	kAttribute.m_dwWeaponFeature = ParseWeaponFeature( rkLoader );	

	//Buff List
	// Air
	int iAirCount = rkLoader.LoadInt_e( "target_air_buff_cnt", 0 );
	kAttribute.m_TargetAirBuffList.reserve( iAirCount );

	for( int i=0; i<iAirCount; i++ )
	{
		wsprintf_e( szBuf2, "target_air_buff%d", i+1 );
		rkLoader.LoadString( szBuf2, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;	
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				kAttribute.m_TargetAirBuffList.push_back( tBuffInfo );
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "ioWeaponManager::ParseWeaponAttribute - %d Attribute %d Buff Type Error",
										 kAttribute.m_iAttributeIndex, i+1 );
			}
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioWeaponManager::ParseWeaponAttribute - %d Attribute %d Buff Name Error",
									 kAttribute.m_iAttributeIndex, i+1 );
		}
	}

	// Normal
	int iCount = rkLoader.LoadInt_e( "target_buff_cnt", 0 );
	kAttribute.m_TargetBuffList.reserve( iCount );

	for( int i=0; i<iCount; i++ )
	{
		wsprintf_e( szBuf2, "target_buff%d", i+1 );
		rkLoader.LoadString( szBuf2, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;	
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				kAttribute.m_TargetBuffList.push_back( tBuffInfo );
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "ioWeaponManager::ParseWeaponAttribute - %d Attribute %d Buff Type Error",
										kAttribute.m_iAttributeIndex, i+1 );
			}
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioWeaponManager::ParseWeaponAttribute - %d Attribute %d Buff Name Error",
									kAttribute.m_iAttributeIndex, i+1 );
		}
	}

	// Down
	int iDownCount = rkLoader.LoadInt_e( "target_down_buff_cnt", 0 );
	kAttribute.m_TargetDownBuffList.reserve( iDownCount );

	for( int i=0; i<iDownCount; i++ )
	{
		wsprintf_e( szBuf2, "target_down_buff%d", i+1 );
		rkLoader.LoadString( szBuf2, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;	
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				kAttribute.m_TargetDownBuffList.push_back( tBuffInfo );
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "ioWeaponManager::ParseWeaponAttribute - %d Attribute %d Buff Type Error",
										 kAttribute.m_iAttributeIndex, i+1 );
			}
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioWeaponManager::ParseWeaponAttribute - %d Attribute %d Buff Name Error",
									 kAttribute.m_iAttributeIndex, i+1 );
		}
	}

	// Attack 2020-02-19 
	int iAttackCount = rkLoader.LoadInt_e( "target_attack_buff_cnt", 0 );
	kAttribute.m_bTargetAttackBuffActive = rkLoader.LoadBool_e( "target_attack_buff_active", false );
	kAttribute.m_TargetAttackBuffList.reserve( iAttackCount );

	for( int i=0; i<iAttackCount; i++ )
	{
		wsprintf_e( szBuf2, "target_attack_buff%d", i+1 );
		rkLoader.LoadString( szBuf2, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;	
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				kAttribute.m_TargetAttackBuffList.push_back( tBuffInfo );
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "ioWeaponManager::ParseWeaponAttribute - %d Attribute %d Buff Type Error",
										 kAttribute.m_iAttributeIndex, i+1 );
			}
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioWeaponManager::ParseWeaponAttribute - %d Attribute %d Buff Name Error",
									 kAttribute.m_iAttributeIndex, i+1 );
		}
	}	

	// 2020-02-19 Dash
	int iDashCount = rkLoader.LoadInt_e( "target_dash_buff_cnt", 0 );
	kAttribute.m_bTargetDashBuffActive = rkLoader.LoadBool_e( "target_dash_buff_active", false );
	kAttribute.m_TargetDashBuffList.reserve( iDashCount );

	for( int i=0; i<iDashCount; i++ )
	{
		wsprintf_e( szBuf2, "target_dash_buff%d", i+1 );
		rkLoader.LoadString( szBuf2, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;	
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				kAttribute.m_TargetDashBuffList.push_back( tBuffInfo );
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "ioWeaponManager::ParseWeaponAttribute - %d Attribute %d Buff Type Error",
										 kAttribute.m_iAttributeIndex, i+1 );
			}
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioWeaponManager::ParseWeaponAttribute - %d Attribute %d Buff Name Error",
									 kAttribute.m_iAttributeIndex, i+1 );
		}
	}	

	int iDefenseCount = rkLoader.LoadInt_e( "target_defense_buff_cnt", 0 );
	kAttribute.m_TargetDefenseBuffList.reserve( iDefenseCount );
	for( int i=0; i<iDefenseCount; i++ )
	{
		wsprintf_e( szBuf2, "target_defense_buff%d", i+1 );
		rkLoader.LoadString( szBuf2, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;	
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				kAttribute.m_TargetDefenseBuffList.push_back( tBuffInfo );
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "ioWeaponManager::ParseWeaponAttribute - %d Attribute %d Buff Type Error",
										 kAttribute.m_iAttributeIndex, i+1 );
			}
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioWeaponManager::ParseWeaponAttribute - %d Attribute %d Buff Name Error",
									 kAttribute.m_iAttributeIndex, i+1 );
		}
	}

	rkLoader.LoadString_e( "damage_increase_buff", "", szBuf, MAX_PATH );
	kAttribute.m_DamageIncreaseBuff = szBuf;
	rkLoader.LoadString_e( "push_increase_buff", "", szBuf, MAX_PATH );
	kAttribute.m_PushIncreaseBuff = szBuf;
	rkLoader.LoadString_e( "blow_increase_buff", "", szBuf, MAX_PATH );
	kAttribute.m_BlowIncreaseBuff = szBuf;

	//AreaWeapon
	rkLoader.LoadString_e( "collision_area_weapon_name", "", szBuf, MAX_PATH );
	kAttribute.m_AreaWeaponName = szBuf;

	// DefenseBreak Buff
	{
		kAttribute.m_bDefenseBreakBuffEnable = rkLoader.LoadBool_e( "defense_break_buff_enable", false );

		char szBreakBuff[MAX_PATH] = "";
		rkLoader.LoadString_e( "defense_break_buff", "", szBreakBuff, MAX_PATH );

		kAttribute.m_DefenseBreakBuff.m_BuffName = szBreakBuff;
		
		if( !kAttribute.m_DefenseBreakBuff.m_BuffName.IsEmpty() )
		{
			kAttribute.m_DefenseBreakBuff.m_iBuffType = g_BuffMaker.GetBuffType( szBreakBuff );
			if( kAttribute.m_DefenseBreakBuff.m_iBuffType == BT_NONE )
			{
				kAttribute.m_DefenseBreakBuff.m_BuffName.Clear();
			}
		}
	}

	// defense cancel
	rkLoader.LoadString_e( "defense_cancel_ani", "", szBuf, MAX_PATH );
	kAttribute.m_DefenseCancelAni = szBuf;
	kAttribute.m_fDefenseCancelAniRate = rkLoader.LoadFloat_e( "defense_cancel_ani_rate", FLOAT1 );

	//
	kAttribute.m_bSelectCollisionBuffEnable = rkLoader.LoadBool_e( "select_collision_buff_enable", false );
	kAttribute.m_bSelectCollisionBuffEnableReverse = rkLoader.LoadBool_e( "select_collision_buff_enable_reverse", false );

	m_vWeaponAttributeList.push_back( kAttribute );
}

DWORD ioWeaponManager::ParseWeaponFeature( ioINILoader &rkLoader )
{
	DWORD dwFeature = 0;

	if( rkLoader.LoadBool_e( "frozen_break", false ) )
		dwFeature |= WF_FROZEN_BREAK;

	if( rkLoader.LoadBool_e( "push_power_linear", false ) )
		dwFeature |= WF_PUSH_POWER_LINEAR;

	if( rkLoader.LoadBool_e( "air_push_power_linear", false ) )
		dwFeature |= WF_AIR_PUSH_POWER_LINEAR;
	
	if( rkLoader.LoadBool_e( "must_item_drop", false ) )
		dwFeature |= WF_MUST_ITEM_DROP;

	int iAttackDirectionType = rkLoader.LoadInt_e( "attack_direction_type", 1 );	
	if( iAttackDirectionType == 0 )
		dwFeature |= WF_ATTACK_DIRECTION_STRAIGHT;

	if( rkLoader.LoadBool_e( "defense_success_ani_type", true ) )
		dwFeature |= WF_WEAK_ANI_DEFENSE_SUCCESS;

	if( rkLoader.LoadBool_e( "team_attack_no_damage_enable", false ) )
		dwFeature |= WF_TEAM_ATTACK_NO_DAMAGE;

	if( rkLoader.LoadBool_e( "no_item_drop", false ) )
		dwFeature |= WF_NO_ITEM_DROP;

	if( rkLoader.LoadBool_e( "attacker_look", false ) )
		dwFeature |= WF_ATTACKER_LOOK;

	if( rkLoader.LoadBool_e( "attack_dir_look", false ) )
		dwFeature |= WF_DIR_LOOK;

	if( rkLoader.LoadBool_e( "enable_down_attack", false ) )
		dwFeature |= WF_DOWN_ATTACK_WEAPON;

	if( rkLoader.LoadBool_e( "disable_hit_count", false ) )
		dwFeature |= WF_DISABLE_HIT_COUNT;

	if( rkLoader.LoadBool_e( "push_dir_change", false ) )
		dwFeature |= WF_PUSH_DIRECTION_NOTCHANGE;

	return dwFeature;
}

ActionStopType ioWeaponManager::ParseActionStop( const char *szType )
{
	char szBuf[MAX_PATH];
	strcpy( szBuf, szType );
	strlwr( szBuf );

	_ENCSTR("both", szEncBoth);
	if( !strcmp( szBuf, szEncBoth ) )
		return AST_BOTH;

	_ENCSTR("attacker", szEncAttacker);
	if( !strcmp( szBuf, szEncAttacker ) )
		return AST_ATTACKER;

	_ENCSTR("target", szEncTarget);
	if( !strcmp( szBuf, szEncTarget ) )
		return AST_TARGET;

	return AST_NONE;
}

void ioWeaponManager::ParseBlowWoundAniInfo( BlowWoundAniInfo &rkInfo, ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	rkLoader.LoadString_e( "blowwound_blowing_ani", "", szBuf, MAX_PATH );
	rkInfo.m_BlowingAni = szBuf;
	rkLoader.LoadString_e( "blowwound_blowing_bk_ani", "", szBuf, MAX_PATH );
	rkInfo.m_BlowingBackAni = szBuf;

	rkLoader.LoadString_e( "blowwound_jump_blowing_ani", "", szBuf, MAX_PATH );
	rkInfo.m_JumpBlowingAni = szBuf;
	rkLoader.LoadString_e( "blowwound_jump_blowing_bk_ani", "", szBuf, MAX_PATH );
	rkInfo.m_JumpBlowingBackAni = szBuf;

	rkLoader.LoadString_e( "blowwound_down_ani", "", szBuf, MAX_PATH );
	rkInfo.m_DownAni = szBuf;
	rkLoader.LoadString_e( "blowwound_down_bk_ani", "", szBuf, MAX_PATH );
	rkInfo.m_DownBackAni = szBuf;
}

void ioWeaponManager::SetReturnPingpong( ioBaseChar *pOwner )
{
	ioWeapon *pWeapon = NULL;
	ActiveWeaponList::iterator iter;

	for( iter = m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		pWeapon = *iter;

		if( !pWeapon )	continue;
		ioReturnPingpongWeapon *pRPWeapon = ToReturnPingpongWeapon( pWeapon );
		if( !pRPWeapon ) continue;

		if( pWeapon->IsLive() && !pWeapon->GetOwnerName().IsEmpty() &&
			pWeapon->GetOwnerName() == pOwner->GetCharName() )
		{
			if( pRPWeapon->EnablePingpong() )
			{
				D3DXVECTOR3 vPos = pOwner->GetAimTargetPos();
				pRPWeapon->SetReturn( vPos );
			}
		}
	}
}

ioWeapon* ioWeaponManager::CreateWeapon( int iAttackIndex, ioBaseChar *pOwner )
{
	if( !pOwner )
	{
		LOG.PrintTimeAndLog( 0, "ioWeaponManager::CreateWeapon - Not Exist Owner" );

		return NULL;
	}

	const WeaponAttribute *pAttribute = FindAttribute( iAttackIndex );
	if( !pAttribute )
	{
		LOG.PrintTimeAndLog( 0, "ioWeaponManager::CreateWeapon - Attribute overflow(%d/%d)",
								iAttackIndex,
								GetAttackAttributeCnt() );
		return NULL;
	}

	/*
	if( pAttribute->m_WeaponType == AWT_RETURN_PINGPONG )
	{
		ioWeapon *pWeapon = NULL;
		ActiveWeaponList::iterator iter;

		for( iter = m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
		{
			pWeapon = *iter;

			if( !pWeapon )	continue;

			if( pWeapon->IsLive() && !pWeapon->GetOwnerName().IsEmpty() &&
				pWeapon->GetOwnerName() == pOwner->GetCharName() )
			{
				ioReturnPingpongWeapon *pRPWeapon = ToReturnPingpongWeapon( pWeapon );
				if( pRPWeapon && pRPWeapon->EnablePingpong() )
				{
					D3DXVECTOR3 vPos = pOwner->GetAimTargetPos();
					pRPWeapon->SetReturn( vPos );
				}
			}
		}
	}
	*/

	ioWeapon *pWeapon = NULL;
	switch( pAttribute->m_WeaponType )
	{
	case AWT_CONTACT:
		pWeapon = new ioContactWeapon( pOwner, pAttribute );
		break;
	case AWT_MISSILE:
		pWeapon = new ioMissileWeapon( pOwner, pAttribute );
		break;
	case AWT_THROW:
		pWeapon = new ioThrowWeapon( pOwner, pAttribute );
		break;
	case AWT_BOMB:
		pWeapon = new ioBombWeapon( pOwner, pAttribute );
		break;
	case AWT_MINE:
		pWeapon = new ioMineWeapon( pOwner, pAttribute );
		break;
	case AWT_NUCLEAR:
		pWeapon = new ioNuclearWeapon( pOwner, pAttribute );
		break;
	case AWT_FLAME:
		pWeapon = new ioFlameWeapon( pOwner, pAttribute );
		break;
	case AWT_ZONE_EFFECT:
		pWeapon = new ioZoneEffectWeapon( pOwner, pAttribute );
		break;
	case AWT_FLAMEWAVE:
		pWeapon = new ioFlamewaveWeapon( pOwner, pAttribute );
		break;
	case AWT_SPLASH:
		pWeapon = new ioSplashWeapon( pOwner, pAttribute );
		break;
	case AWT_ARROW:
		pWeapon = new ioArrowWeapon( pOwner, pAttribute );
		break;
	case AWT_BLIZZARD:
		pWeapon = new ioBlizzardWeapon( pOwner, pAttribute );
		break;
	case AWT_THROWBOMB:
		pWeapon = new ioThrowBombWeapon( pOwner, pAttribute );
		break;
	case AWT_FLOAT_THROW:
		pWeapon = new ioFloatThrowWeapon( pOwner, pAttribute );
		break;
	case AWT_THROW_RETURN:
		pWeapon = new ioThrowReturnWeapon( pOwner, pAttribute );
		break;
	case AWT_EXPLOSION:
		pWeapon = new ioExplosionWeapon( pOwner, pAttribute );
		break;
	case AWT_BOMB_MISSILE:
		pWeapon = new ioBombMissileWeapon( pOwner, pAttribute );
		break;
	case AWT_CANNON_BALL:
		pWeapon = new ioCannonBallWeapon( pOwner, pAttribute );
		break;
	case AWT_ROTATE:
		pWeapon = new ioRotationWeapon( pOwner, pAttribute );
		break;
	case AWT_TRACE:
		pWeapon = new ioTraceWeapon( pOwner, pAttribute );
		break;
	case AWT_BOUNDBOMB:
		pWeapon = new ioBoundBombWeapon( pOwner, pAttribute );
		break;
	case AWT_TARGET:
		pWeapon = new ioTargetWeapon( pOwner, pAttribute );
		break;
	case AWT_CHAIN:
		pWeapon = new ioChainWeapon( pOwner, pAttribute );
		break;
	case AWT_BALL_WEAPON:
		pWeapon = new ioBallWeapon( pOwner, pAttribute );
		break;
	case AWT_IGNORE_MISSILE:
		pWeapon = new ioIgnoreMissileWeapon( pOwner, pAttribute );
		break;
	case AWT_AREA_EFFECT:
		pWeapon = new ioAreaEffectWeapon( pOwner, pAttribute );
		break;
	case AWT_MAP_ATTACH:
		pWeapon = new ioMapAttachWeapon( pOwner, pAttribute );
		break;
	case AWT_LIMIT_ATTACH:
		pWeapon = new ioLimitAttachWeapon( pOwner, pAttribute );
		break;
	case AWT_OWNER_ATTACK:
		pWeapon = new ioOwnerAttackWeapon( pOwner, pAttribute );
		break;
	case AWT_RUN_CONTACT:
		pWeapon = new ioRunContactWeapon( pOwner, pAttribute );
		break;
	case AWT_WHIP:
		pWeapon = new ioWhipWeapon( pOwner, pAttribute );
		break;
	case AWT_ICE_MISSILE:
		pWeapon = new ioIceMissileWeapon( pOwner, pAttribute );
		break;
	case AWT_SUMMON_MISSILE:
		pWeapon = new ioSummonMissileWeapon( pOwner, pAttribute );
		break;
	case AWT_SUMMON_MISSILE2:
		pWeapon = new ioSummonMissileWeapon2( pOwner, pAttribute );
		break;
	case AWT_GROUND_TRACE:
		pWeapon = new ioGroundTraceWeapon( pOwner, pAttribute );
		break;
	case AWT_CONTROL_RETURN:
		pWeapon = new ioControlReturnWeapon( pOwner, pAttribute );
		break;
	case AWT_CONTROL_MISSILE:
		pWeapon = new ioControlMissileWeapon( pOwner, pAttribute );
		break;
	case AWT_TRACE_RETURN:
		pWeapon = new ioTraceReturnWeapon( pOwner, pAttribute );
		break;
	case AWT_TSUNAMI:
		pWeapon = new ioTsunamiWeapon( pOwner, pAttribute );
		break;
	case AWT_TRACE2:
		pWeapon = new ioTraceWeaponType2( pOwner, pAttribute );
		break;
	case AWT_NORMAL:
		pWeapon = new ioNormalWeapon( pOwner, pAttribute );
		break;
	case AWT_ADHESIVE_MISSILE:
		pWeapon = new ioAdhesiveMissileWeapon( pOwner, pAttribute );
		break;
	case AWT_LIMIT_ATTACH2:
		pWeapon = new ioLimitAttachWeapon2( pOwner, pAttribute );
		break;
	case AWT_CONTACT2:
		pWeapon = new ioContactWeapon2( pOwner, pAttribute );
		break;
	case AWT_BOMB_MISSILE2:
		pWeapon = new ioBombMissileWeapon2( pOwner, pAttribute );
		break;
	case AWT_ADHESIVE_MISSILE2:
		pWeapon = new ioAdhesiveMissileWeapon2( pOwner, pAttribute );
		break;
	case AWT_ZONE_EFFECT2:
		pWeapon = new ioZoneEffect2Weapon( pOwner, pAttribute );
		break;
	case AWT_UROBORUS:
		pWeapon = new ioUroborusWeapon( pOwner, pAttribute );
		break;
	case AWT_UROBORUS2:
		pWeapon = new ioUroborusWeapon2( pOwner, pAttribute );
		break;
	case AWT_TRACE3:
		pWeapon = new ioTraceWeaponType3( pOwner, pAttribute );
		break;
	case AWT_MISSILE2:
		pWeapon = new ioMissileWeapon2( pOwner, pAttribute );
		break;
	case AWT_CALL_ATTRIBUTE_ARROW:
		pWeapon = new ioCallAttributeArrowWeapon( pOwner, pAttribute );
		break;
	case AWT_RETURN_PINGPONG:
		pWeapon = new ioReturnPingpongWeapon( pOwner, pAttribute );
		break;
	case AWT_MISSILE3:
		pWeapon = new ioMissileWeapon3( pOwner, pAttribute );
		break;
	case AWT_LOBELIA:
		pWeapon = new ioLobeliaWeapon( pOwner, pAttribute );
		break;
	case AWT_SYLPHID_MISSILE:
		pWeapon = new ioSylphidMissileWeapon( pOwner, pAttribute );
		break;
	case AWT_GUIDE:
		pWeapon = new ioGuideWeapon( pOwner, pAttribute );
		break;
	case AWT_BALL2_WEAPON:
		pWeapon = new ioBall2Weapon( pOwner, pAttribute );
		break;
	case AWT_GUIDE2:
		pWeapon = new ioGuide2Weapon( pOwner, pAttribute );
		break;
	case AWT_TRACE4:
		pWeapon = new ioTraceWeapon4( pOwner, pAttribute );
		break;
	case AWT_FALL_WEAPON:
		pWeapon = new ioFallWeapon( pOwner, pAttribute );
		break;
	case AWT_TRACE5:
		pWeapon = new ioTraceWeapon5( pOwner, pAttribute );
		break;
	case AWT_DOUBLE_ICE_MISSILE_WEAPON:
		pWeapon = new ioDoubleIceMissileWaepon( pOwner, pAttribute );
		break;
	case AWT_SMART_BOOM_WEAPON:
		pWeapon = new ioSmartBombWeapon( pOwner, pAttribute );
		break;
	case AWT_PULL_WEAPON:
		pWeapon = new ioPullWeapon( pOwner, pAttribute );
		break;
	case AWT_ADHESIVE_MISSILE3:
		pWeapon = new ioAdhesiveMissileWeapon3( pOwner, pAttribute );
		break;
	case AWT_GRAFFITI:
		pWeapon = new ioGraffitiWeapon( pOwner, pAttribute );
		break;
	case AWT_MINE2:
		pWeapon = new ioMineWeapon2( pOwner, pAttribute );
		break;
	case AWT_TRACE6:
		pWeapon = new ioTraceWeapon6( pOwner, pAttribute );
		break;	
	case AWT_CONTROL_SWORD:
		pWeapon = new ioControlSwordWeapon( pOwner, pAttribute );
		break;
	case AWT_PULL_WEAPON2:
		pWeapon = new ioPullWeapon2( pOwner, pAttribute );
		break;
	case AWT_GROUND_TRACE2:
		pWeapon = new ioGroundTraceWeapon2( pOwner, pAttribute );
		break;
	case AWT_SELECT_POSITION_MOVE:
		pWeapon = new ioSelectPositionMoveWeapon( pOwner, pAttribute );
		break;
	case AWT_SIMPLE_BOX:
		pWeapon = new ioSimpleBoxWeapon( pOwner, pAttribute );
		break;
	case AWT_ADHESIVE_MISSILE4:
		pWeapon = new ioAdhesiveMissileWeapon4( pOwner, pAttribute );
		break;
	case AWT_BOMB_MISSILE3:
		pWeapon = new ioBombMissileWeapon3( pOwner, pAttribute );
		break;
	case AWT_TRACE7:
		pWeapon = new ioTraceWeapon7( pOwner, pAttribute );
		break;
	case AWT_UROBORUS3:
		pWeapon = new ioUroborusWeapon3( pOwner, pAttribute );
		break;
	case AWT_LUCIFER_AIR_GRAP:
		pWeapon = new ioLuciferAirGrapWeapon( pOwner, pAttribute );
		break;
	case AWT_TRACE_OWNER:
		pWeapon = new ioTraceOwnerWeapon( pOwner, pAttribute );
		break;
	case AWT_IGNORE_MISSILE2:
		pWeapon = new ioIgnoreMissileWeapon2( pOwner, pAttribute );
		break;
	case AWT_ONE_TARGET_WEAPON:
		pWeapon = new ioOneTargetWeapon( pOwner, pAttribute );
		break;
	case AWT_BOMB_MISSILE4:
		pWeapon = new ioBombMissileWeapon4( pOwner, pAttribute );
		break;
	case AWT_ATTACH_WEAPON:
		pWeapon = new ioAttachWeapon( pOwner, pAttribute );
		break;
	case AWT_RETURN_MISSILE_WEAPON:
		pWeapon = new ioReturnMissileWeapon( pOwner, pAttribute );
		break;
	case AWT_UROBORUS4:
		pWeapon = new ioUroborusWeapon4( pOwner, pAttribute );
		break;
	case AWT_FIRE_WORKS_STEP1_WEAPON:
		pWeapon = new ioFireWorksStep1Weapon( pOwner, pAttribute );
		break;
	case AWT_FIRE_THOW_BOM_WEAPON:
		pWeapon = new ioFloatThrowBomWeapon( pOwner, pAttribute );
		break;
	case AWT_THROWBOMB2:
		pWeapon = new ioThrowBomb2Weapon( pOwner, pAttribute );
		break;
	case AWT_ROTATE_TARGET_WEAPON:
		pWeapon = new ioRotateTargetWeapon( pOwner, pAttribute );
		break;
	case AWT_MINE3:
		pWeapon = new ioMineWeapon3( pOwner, pAttribute );
		break;
	case AWT_RETURN_MISSILE_WEAPON2:
		pWeapon = new ioReturnMissileWeapon2( pOwner, pAttribute );
		break;
	case AWT_ZONE_MOVE_WEAPON:
		pWeapon = new ioZoneMoveWeapon( pOwner, pAttribute );
		break;
	case AWT_PRISON_WEAPON:
		pWeapon = new ioPrisonWeapon( pOwner, pAttribute );
		break;
	case AWT_UROBORUS5:
		pWeapon = new ioUroborusWeapon5( pOwner, pAttribute );
		break;
	case AWT_ROTATE2:
		pWeapon = new ioRotationWeapon2( pOwner, pAttribute );
		break;
	case AWT_CONTROL_MESH:
		pWeapon = new ioControlMeshWeapon( pOwner, pAttribute );
		break;
	case AWT_MINE4:
		pWeapon = new ioMineWeapon4( pOwner, pAttribute );
		break;
	case AWT_DESTROY_CALL:
		pWeapon = new ioDestroyCallWeapon( pOwner, pAttribute );
		break;
	case AWT_ATTACH_AREA:
		pWeapon = new ioAttachAeraWeapon( pOwner, pAttribute );
		break;
	case AWT_MISSILE4:
		pWeapon = new ioMissileWeapon4( pOwner, pAttribute );
		break;
	case AWT_UROBORUS6:
		pWeapon = new ioUroborusWeapon6( pOwner, pAttribute );
		break;
	case AWT_ROTATE_ATTACH_CHAR:
		pWeapon = new ioRotateAttachCharWeapon( pOwner, pAttribute );
		break;
	case AWT_PULL_WEAPON3:
		pWeapon = new ioPullWeapon3( pOwner, pAttribute );
		break;
	case AWT_CONTACT3:
		pWeapon = new ioContactWeapon3( pOwner, pAttribute );
		break;
	case AWT_BOMB_MISSILE5:
		pWeapon = new ioBombMissileWeapon5( pOwner, pAttribute );
		break;
	case AWT_MINE5:
		pWeapon = new ioMineWeapon5( pOwner, pAttribute );
		break;
	case AWT_MISSILE5:
		pWeapon = new ioMissileWeapon5( pOwner, pAttribute );
		break;
	case AWT_PULL_WEAPON4:
		pWeapon = new ioPullWeapon4( pOwner, pAttribute );
		break;
	case AWT_ROTATE_RETURN_WEAPON:
		pWeapon = new ioRotateReturnWeapon( pOwner, pAttribute );
		break;
	case AWT_UROBORUS7:
		pWeapon = new ioUroborusWeapon7( pOwner, pAttribute );
		break;
	case AWT_CONTROL_RETURN2:
		pWeapon = new ioControlReturnWeapon2( pOwner, pAttribute );
		break;
	case AWT_GROUND_TRACE3:
		pWeapon = new ioGroundTraceWeapon3( pOwner, pAttribute );
		break;
	case AWT_THROWBOMB3:
		pWeapon = new ioThrowBomb3Weapon( pOwner, pAttribute );
		break;
	case AWT_MINE6:
		pWeapon = new ioMineWeapon6( pOwner, pAttribute );
		break;
	case AWT_PRISON_WEAPON2:
		pWeapon = new ioPrisonWeapon2( pOwner, pAttribute );
		break;
	case AWT_NO_COLLISION_WEAPON:
		pWeapon = new ioNoCollisionWeapon( pOwner, pAttribute );
		break;
	case AWT_MISSILE6:
		pWeapon = new ioMissileWeapon6( pOwner, pAttribute );
		break;
	case AWT_CONTINIOUS_CREATE:
		pWeapon = new ioContinuousCreateWeapon( pOwner, pAttribute );
		break;
	case AWT_PRISON_WEAPON3:
		pWeapon = new ioPrisonWeapon3( pOwner, pAttribute );
		break;
	default:
		LOG.PrintTimeAndLog( 0, "ioWeaponManager::CreateWeapon - Unknown WeaponType (%d)",
								pAttribute->m_WeaponType );
		return NULL;
	}

	ioSceneNode *pSNode = m_pSceneMgr->GetRootSceneNode()->CreateChildSceneNode();
	if( !pSNode )
	{
		LOG.PrintTimeAndLog( 0, "ioWeaponManager::CreateWeapon - SceneNode Failed" );
		delete pWeapon;
		return NULL;
	}

	pSNode->SetScale( pOwner->GetWorldScale() );
	pWeapon->SetSceneNode( pSNode );

	char szTitle[MAX_PATH] = "";

	int iGroupNum = iAttackIndex / 100;
	GroupLoaderMap::iterator iter = m_GroupLoaderMap.find( iGroupNum );
	if( iter != m_GroupLoaderMap.end() )
	{
		wsprintf_e( szTitle, "attribute%d", iAttackIndex );
		
		iter->second->SetTitle(szTitle);
		pWeapon->LoadProperty( *iter->second );
	}

	/*
	int iGroupSize = m_vGroupLoaderList.size();
	int iGroupNum = iAttackIndex / 100;

	if( COMPARE( iGroupNum, 0, iGroupSize ) )
	{
		wsprintf_e( szTitle, "attribute%d", iAttackIndex );
		
		m_vGroupLoaderList[iGroupNum]->SetTitle( szTitle );
		pWeapon->LoadProperty( *m_vGroupLoaderList[iGroupNum] );
	}
	*/

	m_vWeaponList.push_back( pWeapon );

	#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		LOG.PrintTimeAndLog( 0, "CreateWeapon - %d, Time: %d", iAttackIndex, FRAMEGETTIME() );
	}
	#endif

	return pWeapon;
}

void ioWeaponManager::ProcessWeaponList( float fTimePerSec )
{
	ActiveWeaponList::iterator iter;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		ioWeapon *pWeapon = (*iter);
		if( pWeapon->IsLive() )
		{
			pWeapon->UpdatePreWorldPosition();
			pWeapon->Process( fTimePerSec, m_pPlayStage );
		}
	}

	ProcessCallWeaponInfo();

	//
	ProcessApplyWeaponInfoList();
}

void ioWeaponManager::ClearDeadWeaponList()
{
	ActiveWeaponList::iterator iter = m_vWeaponList.begin();
	while( iter != m_vWeaponList.end() )
	{
		ioWeapon *pWeapon = *iter;

		if( pWeapon->IsLive() )
		{
			++iter;
		}
		else
		{
			iter = m_vWeaponList.erase( iter );
			DestroyWeapon( pWeapon );
		}
	}
}

void ioWeaponManager::DestroyWeaponList()
{
	ErrorReport::SetPosition( 3100, 101 );
	ActiveWeaponList::iterator iter;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		ErrorReport::SetPosition( 3100, 102 );
		(*iter)->SetWeaponDead();
		ErrorReport::SetPosition( 3100, 103 );
	}

	ErrorReport::SetPosition( 3100, 104 );
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		ErrorReport::SetPosition( 3100, 105 );
		DestroyWeapon( *iter );
		ErrorReport::SetPosition( 3100, 106 );
	}

	ErrorReport::SetPosition( 3100, 107 );
	m_vWeaponList.clear();

	ErrorReport::SetPosition( 3100, 108 );
	ClearMissingMineIndex();
	ErrorReport::SetPosition( 3100, 109 );
}

void ioWeaponManager::DestroyReserveOwnerWeapon( const ioHashString &rkOwner )
{
	if( rkOwner.IsEmpty() )
		return;

	ioWeapon *pWeapon = NULL;
	ActiveWeaponList::iterator iter;
	for( iter = m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		pWeapon = *iter;

		if( pWeapon->GetOwnerName() == rkOwner )
		{
			pWeapon->SetWeaponDead();
		}
	}
}

void ioWeaponManager::DestroyReserveOwenrWeaponExceptCharChange( const ioHashString &rkOwner )
{
	if( rkOwner.IsEmpty() )
		return;

	ioWeapon *pWeapon = NULL;
	ActiveWeaponList::iterator iter;
	for( iter = m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		pWeapon = *iter;

		if( pWeapon->GetOwnerName() == rkOwner && pWeapon->GetDestroyByCharChange() )
		{
			pWeapon->SetWeaponDead();
		}
	}
}

void ioWeaponManager::DestroyReserveContactWeapon( const ioHashString &rkOwner )
{
	if( rkOwner.IsEmpty() )
		return;

	ioWeapon *pWeapon = NULL;
	ActiveWeaponList::iterator iter;
	for( iter = m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		pWeapon = *iter;

		bool bContactType = false;
		if( pWeapon->GetType() == ioWeapon::WT_CONTACT ||
			pWeapon->GetType() == ioWeapon::WT_RUN_CONTACT ||
			pWeapon->GetType() == ioWeapon::WT_CONTACT2 ||
			pWeapon->GetType() == ioWeapon::WT_ATTACH_WEAPON ||
			pWeapon->GetType() == ioWeapon::WT_CONTACT3)
			bContactType = true;

		if( 0 < pWeapon->GetWeaponByDummyChar() )
			continue;

		if( pWeapon->GetOwnerName() == rkOwner && bContactType )
		{
			pWeapon->SetWeaponDead();
		}
	}
}

void ioWeaponManager::DestroyReserveAdhesive3( const ioHashString &rkOwner )
{
	if( rkOwner.IsEmpty() )
		return;

	ioWeapon *pWeapon = NULL;
	ActiveWeaponList::iterator iter;
	for( iter = m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		pWeapon = *iter;

		ioAdhesiveMissileWeapon3 *pAdhesive3 = ToAdhesiveMissileWeapon3( pWeapon );
		if( !pAdhesive3 )
			continue;

		if( pAdhesive3->GetOwnerName() == rkOwner && pAdhesive3->IsLive() && pAdhesive3->IsOwnerAttach() )
		{
			pAdhesive3->Explosion( m_pPlayStage );
		}
	}
}

void ioWeaponManager::DestroyReserveAdhesive4( const ioHashString &rkOwner )
{
	if( rkOwner.IsEmpty() )
		return;

	ioWeapon *pWeapon = NULL;
	ActiveWeaponList::iterator iter;
	for( iter = m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		pWeapon = *iter;

		ioAdhesiveMissileWeapon4 *pAdhesive4 = ToAdhesiveMissileWeapon4( pWeapon );
		if( !pAdhesive4 )
			continue;

		if( pAdhesive4->GetOwnerName() == rkOwner && pAdhesive4->IsLive() )
		{
			pAdhesive4->Explosion( m_pPlayStage );
		}
	}
}

void ioWeaponManager::CheckMaxWeaponCnt( ioWeapon *pWeapon, int iMaxCnt )
{
	if( iMaxCnt <= 0 )
		return;

	ioMineWeapon *pMineWeapon = ToMineWeapon(pWeapon);
	ioMineWeapon2 *pTraceMine = ToMineWeapon2(pWeapon);
	ioMineWeapon3 *pTraceMine3 = ToMineWeapon3(pWeapon);
	ioMineWeapon4 *pAirMine = ToMineWeapon4( pWeapon );
	ioMineWeapon6 *pMineWeapon6 = ToMineWeapon6(pWeapon);
	ioReturnPingpongWeapon *pReturnPingpongWeapon = ToReturnPingpongWeapon( pWeapon );

	if( pMineWeapon )
	{
		typedef std::vector< ioMineWeapon* > MineList;
		MineList vMineList;

		ioMineWeapon *pCurWeapon = NULL;

		ioHashString szOwnerName = pWeapon->GetOwnerName();
		ioHashString szCreateItem = pWeapon->GetCreateItem();

		ActiveWeaponList::iterator iter;
		for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
		{
			pCurWeapon = ToMineWeapon( (*iter) );
			if( !pCurWeapon ) continue;

			if( pCurWeapon->GetOwnerName() == szOwnerName &&
				pCurWeapon->GetMineType() == pMineWeapon->GetMineType() )
			{
				vMineList.push_back( pCurWeapon );
			}
		}

		int iMineCnt = vMineList.size();
		if( iMineCnt <= iMaxCnt )
			return;

		DWORD dwCreateTime = -1;	// 오버플로우를 시켜서 최대값으로 세팅
		ioMineWeapon *pDeleteMine = NULL;
		for( int i=0 ; i<iMineCnt ; i++ )
		{
			pCurWeapon = vMineList[i];
			if( dwCreateTime > pCurWeapon->GetCreateTime() )
			{
				dwCreateTime = pCurWeapon->GetCreateTime();
				pDeleteMine = pCurWeapon;
			}
		}

		if( pDeleteMine )
		{
			if( pDeleteMine->IsNormalDead() )
				pDeleteMine->NormalDeadMine( m_pPlayStage, true );
			else
				pDeleteMine->ExplosionMine( m_pPlayStage, true );
		}
	}

	if( pTraceMine3 )
	{
		typedef std::vector< ioMineWeapon3* > MineList;
		MineList vMineList;

		ioMineWeapon3 *pCurWeapon = NULL;

		ioHashString szOwnerName = pWeapon->GetOwnerName();
		ioHashString szCreateItem = pWeapon->GetCreateItem();

		ActiveWeaponList::iterator iter;
		for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
		{
			pCurWeapon = ToMineWeapon3( (*iter) );
			if( !pCurWeapon ) continue;

			if( pCurWeapon->GetOwnerName() == szOwnerName &&
				pCurWeapon->GetMineType() == pTraceMine3->GetMineType() )
			{
				vMineList.push_back( pCurWeapon );
			}
		}

		int iMineCnt = vMineList.size();
		if( iMineCnt <= iMaxCnt )
			return;

		DWORD dwCreateTime = -1;	// 오버플로우를 시켜서 최대값으로 세팅
		ioMineWeapon3 *pDeleteMine3 = NULL;
		for( int i=0 ; i<iMineCnt ; i++ )
		{
			pCurWeapon = vMineList[i];
			if( dwCreateTime > pCurWeapon->GetCreateTime() )
			{
				dwCreateTime = pCurWeapon->GetCreateTime();
				pDeleteMine3 = pCurWeapon;
			}
		}

		if( pDeleteMine3 )
		{
			if( pDeleteMine3->IsNormalDead() )
				pDeleteMine3->NormalDeadMine( m_pPlayStage, true );
			else
				pDeleteMine3->ExplosionMine( m_pPlayStage, true );
		}
	}

	if( pTraceMine )
	{
		typedef std::vector< ioMineWeapon2* > MineList;
		MineList vMineList;

		ioMineWeapon2 *pCurWeapon = NULL;

		ActiveWeaponList::iterator iter;
		for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
		{
			pCurWeapon = ToMineWeapon2( (*iter) );
			if( !pCurWeapon )
				continue;

			if( pCurWeapon->GetMineState() != ioMineWeapon2::MS_CHARGE &&
				pCurWeapon->GetMineState() != ioMineWeapon2::MS_SENSING )
				continue;

			ioHashString szOwnerName = pWeapon->GetOwnerName();

			if( pCurWeapon->GetOwnerName() == szOwnerName &&
				pCurWeapon->GetMineType() == pTraceMine->GetMineType())
			{
				vMineList.push_back( pCurWeapon );
			}
		}

		int iMineCnt = vMineList.size();
		if( iMineCnt <= iMaxCnt )
			return;

		DWORD dwCreateTime = -1;	// 오버플로우를 시켜서 최대값으로 세팅
		ioMineWeapon2 *pDeleteMine = NULL;
		for( int i=0 ; i<iMineCnt ; i++ )
		{
			pCurWeapon = vMineList[i];
			if( dwCreateTime > pCurWeapon->GetCreateTime() )
			{
				dwCreateTime = pCurWeapon->GetCreateTime();
				pDeleteMine = pCurWeapon;
			}
		}

		if( pDeleteMine )
		{
			pDeleteMine->SetWaitFlyMine( m_pPlayStage, true );
		}
	}

	if( pAirMine )
	{
		typedef std::vector< ioMineWeapon4* > MineList;
		MineList vMineList;

		ioMineWeapon4 *pCurWeapon = NULL;

		ioHashString szOwnerName = pWeapon->GetOwnerName();
		ioHashString szCreateItem = pWeapon->GetCreateItem();

		ActiveWeaponList::iterator iter;
		for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
		{
			pCurWeapon = ToMineWeapon4( (*iter) );
			if( !pCurWeapon ) continue;

			if( pCurWeapon->GetOwnerName() == szOwnerName &&
				pCurWeapon->GetMineType() == pAirMine->GetMineType() )
			{
				vMineList.push_back( pCurWeapon );
			}
		}

		int iMineCnt = vMineList.size();
		if( iMineCnt <= iMaxCnt )
			return;

		DWORD dwCreateTime = -1;	// 오버플로우를 시켜서 최대값으로 세팅
		ioMineWeapon4 *pDeleteMine = NULL;
		for( int i=0 ; i<iMineCnt ; i++ )
		{
			pCurWeapon = vMineList[i];
			if( dwCreateTime > pCurWeapon->GetCreateTime() )
			{
				dwCreateTime = pCurWeapon->GetCreateTime();
				pDeleteMine = pCurWeapon;
			}
		}

		if( pDeleteMine )
		{
			if( pDeleteMine->IsNormalDead() )
				pDeleteMine->NormalDeadMine( m_pPlayStage, true );
			else
				pDeleteMine->ExplosionMine( m_pPlayStage, true );
		}
	}

	if( pReturnPingpongWeapon )
	{
		typedef std::vector< ioReturnPingpongWeapon* > WeaponList;
		WeaponList vWeaponList;

		ioReturnPingpongWeapon *pCurWeapon = NULL;

		ioHashString szOwnerName = pWeapon->GetOwnerName();
		ioHashString szCreateItem = pWeapon->GetCreateItem();

		ActiveWeaponList::iterator iter;
		for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
		{
			pCurWeapon = ToReturnPingpongWeapon( (*iter) );
			if( !pCurWeapon ) continue;
			if( !pCurWeapon->IsLive() ) continue;
			if( pCurWeapon->DisableMaxCount() )	continue;

			if( pCurWeapon->GetOwnerName() == szOwnerName )
				vWeaponList.push_back( pCurWeapon );
		}

		int iWeaponCnt = vWeaponList.size();
		if( iWeaponCnt  <= iMaxCnt )
			return;

		DWORD dwCreateTime = -1;	// 오버플로우를 시켜서 최대값으로 세팅
		ioReturnPingpongWeapon *pDeleteWeapon = NULL;
		for( int i=0 ; i<iWeaponCnt  ; i++ )
		{
			pCurWeapon = vWeaponList[i];
			if( dwCreateTime > pCurWeapon->GetCreateTime() )
			{
				dwCreateTime = pCurWeapon->GetCreateTime();
				pDeleteWeapon = pCurWeapon;
			}
		}

		if( pDeleteWeapon )
		{
			// 수정 필요할 것 같음.
			pDeleteWeapon->SetWeaponDead();
		}
	}

	if( pMineWeapon6 )
	{
		typedef std::vector< ioMineWeapon6* > MineList;
		MineList vMineList;

		ioMineWeapon6 *pCurWeapon6 = NULL;

		ioHashString szOwnerName = pWeapon->GetOwnerName();
		ioHashString szCreateItem = pWeapon->GetCreateItem();

		ActiveWeaponList::iterator iter;
		for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
		{
			pCurWeapon6 = ToMineWeapon6( (*iter) );
			if( !pCurWeapon6 ) continue;

			if( pCurWeapon6->GetOwnerName() == szOwnerName &&
				pCurWeapon6->GetMineType() == pMineWeapon6->GetMineType() )
			{
				vMineList.push_back( pCurWeapon6 );
			}
		}

		int iMineCnt = vMineList.size();
		if( iMineCnt <= iMaxCnt )
			return;

		DWORD dwCreateTime = -1;	// 오버플로우를 시켜서 최대값으로 세팅
		ioMineWeapon6 *pDeleteMine = NULL;
		for( int i=0 ; i<iMineCnt ; i++ )
		{
			pCurWeapon6 = vMineList[i];
			if( dwCreateTime > pCurWeapon6->GetCreateTime() )
			{
				dwCreateTime = pCurWeapon6->GetCreateTime();
				pDeleteMine = pCurWeapon6;
			}
		}

		if( pDeleteMine )
		{
			if( pDeleteMine->IsNormalDead() )
				pDeleteMine->NormalDeadMine( m_pPlayStage, true );
			else
				pDeleteMine->ExplosionMine( m_pPlayStage, true );
		}
	}
}

bool ioWeaponManager::CheckRotationWeaponByItem( const ioHashString &szName, int iItemIndex )
{
	if( szName.IsEmpty() )
		return false;

	ioWeapon *pWeapon = NULL;
	ActiveWeaponList::iterator iter;
	for( iter = m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		pWeapon = *iter;

		if( !pWeapon->IsLive() ) continue;

		ioRotationWeapon *pRotation = ToRotationWeapon( pWeapon );
		if( pWeapon->GetOwnerName() == szName && pRotation )
		{
			if( iItemIndex == pRotation->GetCreateItemIndex() )
				return true;
		}
	}

	return false;
}


void ioWeaponManager::RemoveRotationWeaponByWound( const ioHashString &szName )
{
	if( szName.IsEmpty() )
		return;

	ioWeapon *pWeapon = NULL;
	ActiveWeaponList::iterator iter;
	for( iter = m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		pWeapon = *iter;

		if( !pWeapon ) continue;
		if( !pWeapon->IsLive() ) continue;
		if( pWeapon->GetOwnerName() != szName ) continue;

		ioRotationWeapon *pRotation = ToRotationWeapon( pWeapon );
		if( pRotation && pRotation->IsRemoveByWoundState() )
		{
			pRotation->SetWeaponDead();
			return;
		}
	}
}

void ioWeaponManager::RemoveAllTrackingWeapon( ioPlayStage* pStage, bool bSend )
{
	if( !pStage )
		return;

	m_vTrackingMineBufferList.clear();

	ActiveWeaponList::iterator iter = m_vWeaponList.begin();
	for( ; iter!=m_vWeaponList.end() ; ++iter )
	{
		ioMineWeapon2 *pMine = ToMineWeapon2( *iter );
		if( pMine )
		{
			if( pMine->GetMineState() == ioMineWeapon2::MS_CHARGE ||
				pMine->GetMineState() == ioMineWeapon2::MS_SENSING )
				pMine->SetWaitFlyMine( pStage, bSend );
		}
	}
}

void ioWeaponManager::RemoveAllTracking3Weapon( ioPlayStage* pStage, bool bSend )
{
	if( !pStage )
		return;

	m_vTracking3MineBufferList.clear();

	ActiveWeaponList::iterator iter = m_vWeaponList.begin();
	for( ; iter!=m_vWeaponList.end() ; ++iter )
	{
		ioMineWeapon3 *pMine = ToMineWeapon3( *iter );
		if( pMine )
		{
			if( pMine->GetMineState() == ioMineWeapon3::MS_CHARGE ||
				pMine->GetMineState() == ioMineWeapon3::MS_SENSING )
				//pMine->SetWaitFlyMine( pStage, bSend );
				pMine->SetWeaponDead();
		}
	}
}

void ioWeaponManager::CheckMapAttachWeaponAction( const ioHashString &szName )
{
	return;

	ioMapAttachWeapon *pCurWeapon = NULL;

	ActiveWeaponList::iterator iter;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		pCurWeapon = ToMapAttachWeapon( (*iter) );
		if( !pCurWeapon ) continue;

		if( pCurWeapon->GetOwnerName() == szName )
		{
		}
	}
}

void ioWeaponManager::CheckMaxMapAttachWeaponCnt( ioWeapon *pWeapon, int iMaxCnt )
{
	if( iMaxCnt <= 0 )
		return;

	typedef std::vector< ioMapAttachWeapon* > MapAttachList;
	MapAttachList vMapAttachList;

	ioMapAttachWeapon *pCurWeapon = NULL;
	ioHashString szOwnerName = pWeapon->GetOwnerName();
	ioHashString szCreateItem = pWeapon->GetCreateItem();

	ActiveWeaponList::iterator iter;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		pCurWeapon = ToMapAttachWeapon( (*iter) );
		if( !pCurWeapon ) continue;

		if( pCurWeapon->GetOwnerName() == szOwnerName )
		{
			vMapAttachList.push_back( pCurWeapon );
		}
	}

	int iAttachCnt = vMapAttachList.size();
	if( iAttachCnt <= iMaxCnt )
		return;

	DWORD dwCreateTime = -1;	// 오버플로우를 시켜서 최대값으로 세팅
	ioMapAttachWeapon *pDelete = NULL;
	for( int i=0 ; i<iAttachCnt ; i++ )
	{
		pCurWeapon = vMapAttachList[i];
		if( dwCreateTime > pCurWeapon->GetCreateTime() )
		{
			dwCreateTime = pCurWeapon->GetCreateTime();
			pDelete = pCurWeapon;
		}
	}

	if( pDelete )
	{
		pDelete->Explosion( m_pPlayStage );
	}
}

ioBaseChar* ioWeaponManager::GetWeaponOwnerChar( const ioHashString &szName )
{
	if( m_pPlayStage )
		return m_pPlayStage->GetBaseChar( szName );

	return NULL;
}

ioWeapon* ioWeaponManager::FindWeapon( ioBaseChar *pAttacker, DWORD dwWeaponIdx )
{
	if( !pAttacker )
		return NULL;
	
	ioWeapon *pWeapon = NULL;

	ActiveWeaponList::iterator iter;
	for( iter = m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		pWeapon = *iter;

		if( !pWeapon->IsLive() )
			continue;

		if( pWeapon->GetOwner() == pAttacker && pWeapon->GetWeaponIndex() == dwWeaponIdx )
			return pWeapon;
	}

	return NULL;
}

void ioWeaponManager::FindWeaponByAttributeIndex( IN ioBaseChar *pAttacker, IN DWORD dwAttrIndex, OUT DWORDVec& CurrList )
{
	if( !pAttacker )
		return;

	ioWeapon *pWeapon = NULL;

	ActiveWeaponList::iterator iter;
	for( iter = m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		pWeapon = *iter;

		if( !pWeapon->IsLive() )
			continue;

		if( pWeapon->GetOwner() == pAttacker && pWeapon->GetAttributeIndex() == dwAttrIndex )
		{
			CurrList.push_back( pWeapon->GetWeaponIndex() );
		}
	}
}


void ioWeaponManager::FindWeaponByAttributeType( IN ioBaseChar *pAttacker, IN DWORD dwAttrType, OUT DWORDVec& CurrList )
{
	if( !pAttacker )
		return;

	ioWeapon *pWeapon = NULL;

	ActiveWeaponList::iterator iter;
	for( iter = m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		pWeapon = *iter;

		if( !pWeapon->IsLive() )
			continue;

		if( pWeapon->GetOwner() == pAttacker && pWeapon->IsWeaponType(dwAttrType) )
		{
			CurrList.push_back( pWeapon->GetWeaponIndex() );
		}
	}
}


void ioWeaponManager::DestroyWeapon( ioWeapon *pWeapon )
{
	DWORD dwAreaWeaponIndex = pWeapon->GetAttachAreaWeaponIndex();
	if( dwAreaWeaponIndex > 0 )
	{
		m_pPlayStage->DestroyAreaWeapon( dwAreaWeaponIndex );
		pWeapon->AttachAreaWeaponEndState();
	}

	pWeapon->DestroyResource( m_pPlayStage );

	ioSceneNode *pParentNode = pWeapon->GetParentSceneNode();
	if( pParentNode )
	{
		pParentNode->RemoveAndDestroyAllChildren();
		if( m_pSceneMgr )
			m_pSceneMgr->DestroySceneNode( pParentNode->GetUniqueID() );
	}

	delete pWeapon;
}

const WeaponAttribute* ioWeaponManager::FindAttribute( int iAttributeIdx )
{
	int iAttributeCnt = m_vWeaponAttributeList.size();

	for( int i=0; i < iAttributeCnt; ++i )
	{
		if( iAttributeIdx == m_vWeaponAttributeList[i].m_iAttributeIndex )
			return &m_vWeaponAttributeList[i];
	}

	return NULL;
}

ioWeaponManager::WeaponIterator ioWeaponManager::GetMineWeaponIterator()
{
	m_vMineBufferList.clear();

	ActiveWeaponList::iterator iter = m_vWeaponList.begin();
	for( ; iter!=m_vWeaponList.end() ; ++iter )
	{
		ioMineWeapon *pMine = ToMineWeapon( *iter );
		if( pMine )
		{
			m_vMineBufferList.push_back( pMine );
		}
	}

	return WeaponIterator( m_vMineBufferList.begin(), m_vMineBufferList.end() );
}

ioWeaponManager::WeaponIterator ioWeaponManager::GetTrackingMineWeaponIterator()
{
	m_vTrackingMineBufferList.clear();

	ActiveWeaponList::iterator iter = m_vWeaponList.begin();
	for( ; iter!=m_vWeaponList.end() ; ++iter )
	{
		ioMineWeapon2 *pMine = ToMineWeapon2( *iter );
		if( pMine )
		{			
			m_vTrackingMineBufferList.push_back( pMine );
		}
	}

	return WeaponIterator( m_vTrackingMineBufferList.begin(), m_vTrackingMineBufferList.end() );
}

ioWeaponManager::WeaponIterator ioWeaponManager::GetTracking3MineWeaponIterator()
{
	m_vTracking3MineBufferList.clear();

	ActiveWeaponList::iterator iter = m_vWeaponList.begin();
	for( ; iter!=m_vWeaponList.end() ; ++iter )
	{
		ioMineWeapon3 *pMine = ToMineWeapon3( *iter );
		if( pMine )
		{			
			m_vTracking3MineBufferList.push_back( pMine );
		}
	}

	return WeaponIterator( m_vTracking3MineBufferList.begin(), m_vTracking3MineBufferList.end() );
}

ioWeaponManager::WeaponIterator ioWeaponManager::GetAirMineWeaponIterator()
{
	m_vAirMineBufferList.clear();

	ActiveWeaponList::iterator iter = m_vWeaponList.begin();
	for( ; iter!=m_vWeaponList.end() ; ++iter )
	{
		ioMineWeapon4 *pMine = ToMineWeapon4( *iter );
		if( pMine )
		{
			m_vAirMineBufferList.push_back( pMine );
		}
	}

	return WeaponIterator( m_vAirMineBufferList.begin(), m_vAirMineBufferList.end() );
}

DWORD ioWeaponManager::GetCurTrackingMineCnt( ioBaseChar* pOwner )
{	
	DWORD dwTrackingCnt = 0;
	
	ActiveWeaponList::iterator iter = m_vWeaponList.begin();
	for( ; iter!=m_vWeaponList.end() ; ++iter )
	{
		ioMineWeapon2 *pMine = ToMineWeapon2( *iter );
		if( pMine )
		{
			ioBaseChar* pChar = pMine->GetOwner();
			if( pChar && pChar == pOwner )
			{
				dwTrackingCnt++;
			}
		}
	}

	return dwTrackingCnt;
}

ioWeaponManager::WeaponIterator ioWeaponManager::GetWeaponIterator()
{
	return WeaponIterator( m_vWeaponList.begin(), m_vWeaponList.end() );
}

int ioWeaponManager::GetAttackAttributeCnt() const
{
	return m_vWeaponAttributeList.size();
}

ioWeaponManager& ioWeaponManager::GetSingleton()
{
	return Singleton< ioWeaponManager >::GetSingleton();
}

int ioWeaponManager::GetCurMineWeaponCnt( ioBaseChar *pChar, MineType eType )
{
	if( !pChar )
		return 0;

	int iWeaponCnt = 0;
	ioHashString szOwnerName = pChar->GetCharName();

	ActiveWeaponList::iterator iter;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		ioMineWeapon *pCurMine = ToMineWeapon( *iter );
		ioMineWeapon2 *pCurMine2 = ToMineWeapon2( *iter );
		ioMineWeapon3 *pCurMine3 = ToMineWeapon3( *iter );
		ioMineWeapon4 *pCurMine4 = ToMineWeapon4( *iter );
		ioMineWeapon6 *pCurMine6 = ToMineWeapon6( *iter );

		if( pCurMine )
		{
			if( pCurMine->GetOwnerName() == szOwnerName && pCurMine->GetMineType() == eType )
				iWeaponCnt++;
		}
		else if( pCurMine2 )
		{
			if( pCurMine2->GetOwnerName() == szOwnerName && pCurMine2->GetMineType() == eType )
				iWeaponCnt++;
		}
		else if( pCurMine3 )
		{
			if( pCurMine3->GetOwnerName() == szOwnerName && pCurMine3->GetMineType() == eType )
				iWeaponCnt++;
		}
		else if( pCurMine4 )
		{
			if( pCurMine4->GetOwnerName() == szOwnerName && pCurMine4->GetMineType() == eType )
				iWeaponCnt++;
		}
		else if( pCurMine6 )
		{
			if( pCurMine6->GetOwnerName() == szOwnerName && pCurMine6->GetMineType() == eType )
				iWeaponCnt++;
		}
	}

	return iWeaponCnt;
}

void ioWeaponManager::FillMineInfo( ioBaseChar *pOwner, SP2Packet &rkPacket, MineType eType )
{
	if( !pOwner )
		return;

	ioHashString szOwnerName = pOwner->GetCharName();
	ActiveWeaponList::iterator iter;

	int iCnt = 0;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter, ++iCnt )
	{
		ioMineWeapon	*pCurMine	= ToMineWeapon(  *iter );
		ioMineWeapon2	*pCurMine2	= ToMineWeapon2( *iter );
		ioMineWeapon3	*pCurMine3	= ToMineWeapon3( *iter );
		ioMineWeapon4	*pCurMine4	= ToMineWeapon4( *iter );
		ioMineWeapon6   *pCurMine6  = ToMineWeapon6( *iter );

		if( pCurMine && pCurMine->GetOwnerName() == szOwnerName && pCurMine->GetMineType() == eType )
		{
			rkPacket << pCurMine->GetAttributeIndex();
			rkPacket << pCurMine->GetResistanceAttributeIndex();
			rkPacket << pCurMine->GetCreateItem();
			rkPacket << pCurMine->GetWeaponIndex();
			rkPacket << pCurMine->GetAniCollisionType();
			rkPacket << pCurMine->GetPosition();
			rkPacket << pCurMine->GetSensingStartGapTime();
			rkPacket << pCurMine->GetUniqID();
		}
		else if( pCurMine2 && pCurMine2->GetOwnerName() == szOwnerName && pCurMine2->GetMineType() == eType )
		{
			rkPacket << pCurMine2->GetAttributeIndex();
			rkPacket << pCurMine2->GetResistanceAttributeIndex();
			rkPacket << pCurMine2->GetCreateItem();
			rkPacket << pCurMine2->GetWeaponIndex();
			rkPacket << pCurMine2->GetAniCollisionType();
			rkPacket << pCurMine2->GetPosition();
			rkPacket << pCurMine2->GetSensingStartGapTime();
			rkPacket << pCurMine2->GetUniqID();
		}
		else if( pCurMine3 && pCurMine3->GetOwnerName() == szOwnerName && pCurMine3->GetMineType() == eType )
		{
			rkPacket << pCurMine3->GetAttributeIndex();
			rkPacket << pCurMine3->GetResistanceAttributeIndex();
			rkPacket << pCurMine3->GetCreateItem();
			rkPacket << pCurMine3->GetWeaponIndex();
			rkPacket << pCurMine3->GetAniCollisionType();
			rkPacket << pCurMine3->GetPosition();
			rkPacket << pCurMine3->GetSensingStartGapTime();
			rkPacket << pCurMine3->GetUniqID();
		}
		else if( pCurMine4 && pCurMine4->GetOwnerName() == szOwnerName && pCurMine4->GetMineType() == eType )
		{
			rkPacket << pCurMine4->GetAttributeIndex();
			rkPacket << pCurMine4->GetResistanceAttributeIndex();
			rkPacket << pCurMine4->GetCreateItem();
			rkPacket << pCurMine4->GetWeaponIndex();
			rkPacket << pCurMine4->GetAniCollisionType();
			rkPacket << pCurMine4->GetPosition();
			rkPacket << pCurMine4->GetSensingStartGapTime();
			rkPacket << pCurMine4->GetUniqID();
		}
		else if( pCurMine6 && pCurMine6->GetOwnerName() == szOwnerName && pCurMine6->GetMineType() == eType )
		{
			rkPacket << pCurMine6->GetAttributeIndex();
			rkPacket << pCurMine6->GetResistanceAttributeIndex();
			rkPacket << pCurMine6->GetCreateItem();
			rkPacket << pCurMine6->GetWeaponIndex();
			rkPacket << pCurMine6->GetAniCollisionType();
			rkPacket << pCurMine6->GetPosition();
			rkPacket << pCurMine6->GetSensingStartGapTime();
			rkPacket << pCurMine6->GetUniqID();
		}
	}
}

void ioWeaponManager::FillMapAttachInfo( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	ioHashString szOwnerName = pOwner->GetCharName();
	ActiveWeaponList::iterator iter;

	int iCnt = 0;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter, ++iCnt )
	{
		ioMapAttachWeapon *pCurAttach = ToMapAttachWeapon( *iter );

		if( pCurAttach && pCurAttach->GetOwnerName() == szOwnerName )
		{
			rkPacket << pCurAttach->GetAttributeIndex();
			rkPacket << pCurAttach->GetResistanceAttributeIndex();
			rkPacket << pCurAttach->GetCreateItem();
			rkPacket << pCurAttach->GetWeaponIndex();
			rkPacket << pCurAttach->GetAniCollisionType();
			rkPacket << pCurAttach->GetPosition();
		}
	}
}

void ioWeaponManager::RegisterMissingMineIndex( DWORD dwIndex )
{
	m_MissingMineIndex.insert( dwIndex );
}
	
bool ioWeaponManager::CheckMissingIngMineIndex( DWORD dwIndex )
{
	MissingMineIndex::iterator iter = m_MissingMineIndex.find( dwIndex );
	if( iter != m_MissingMineIndex.end() )
	{
		m_MissingMineIndex.erase( dwIndex );
		return true;
	}

	return false;
}
	
void ioWeaponManager::ClearMissingMineIndex()
{
	m_MissingMineIndex.clear();
}

int ioWeaponManager::GetCurMapAttachWeaponCnt( ioBaseChar *pOwner )
{
	if( !pOwner )
		return 0;

	ioHashString szOwnerName = pOwner->GetCharName();

	int iAttachCnt = 0;
	ActiveWeaponList::iterator iter;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		ioMapAttachWeapon *pAttach = ToMapAttachWeapon( *iter );

		if( pAttach && pAttach->GetOwnerName() == szOwnerName )
		{
			iAttachCnt++;
		}
	}

	return iAttachCnt;
}

void ioWeaponManager::CheckMaxSummonWeaponCnt( ioWeapon *pWeapon, int iMaxCnt )
{
	if( iMaxCnt <= 0 )
		return;

	SummonWeaponInfoList vSummonList;

	ioSummonMissileWeapon *pCurWeapon = NULL;
	ioHashString szOwnerName = pWeapon->GetOwnerName();
	ioHashString szCreateItem = pWeapon->GetCreateItem();

	ActiveWeaponList::iterator iter;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		pCurWeapon = ToSummonMissileWeapon( (*iter) );
		if( !pCurWeapon ) continue;
		if( pCurWeapon->CheckWeaponBySkill() ) continue;

		if( pCurWeapon->GetOwnerName() == szOwnerName )
		{
			SummonWeaponInfo kInfo;
			kInfo.m_dwCreateTime = pCurWeapon->GetCreateTime();
			kInfo.m_pWeapon = pCurWeapon;

			vSummonList.push_back( kInfo );
		}
	}

	int iAttachCnt = vSummonList.size();
	if( iAttachCnt <= iMaxCnt )
		return;

	std::sort( vSummonList.begin(), vSummonList.end(), SummonSort() );

	int iGapCnt = iAttachCnt - iMaxCnt;
	for( int i=0; i < iGapCnt; ++i )
	{
		ioSummonMissileWeapon *pDelete = NULL;

		pCurWeapon = vSummonList[i].m_pWeapon;
		if( pCurWeapon )
		{
			pCurWeapon->SetExplosionWait();
		}
	}
}

bool ioWeaponManager::HasControlReturnWeapon( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	ioControlReturnWeapon *pCurWeapon = NULL;
	ActiveWeaponList::iterator iter;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		pCurWeapon = ToControlReturnWeapon( (*iter) );
		if( !pCurWeapon ) continue;

		if( pCurWeapon->GetOwnerName() == pOwner->GetCharName() )
		{
			return true;
		}
	}

	return false;
}

bool ioWeaponManager::HasWeaponType( ioBaseChar *pOwner, int iType )
{
	if( !pOwner ) return false;

	ioWeapon *pCurWeapon = NULL;
	ActiveWeaponList::iterator iter;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		pCurWeapon = (*iter);
		if( !pCurWeapon ) continue;

		if( pCurWeapon->GetOwnerName() == pOwner->GetCharName() && pCurWeapon->GetType() == iType )
		{
			return true;
		}
	}

	return false;
}

void ioWeaponManager::CheckControlMissileWeaponClear( const ioHashString &szOwnerName )
{
	ioControlMissileWeapon *pCurWeapon = NULL;
	ActiveWeaponList::iterator iter;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		ioControlMissileWeapon *pCurWeapon = ToControlMissileWeapon( *iter );
		if( pCurWeapon && pCurWeapon->GetOwnerName() == szOwnerName )
		{
			pCurWeapon->SetWeaponDead();
		}
	}
}

bool ioWeaponManager::HasSkillDependencyWeapon( const ioHashString &szOwnerName, const ioHashString &szSkillName )
{
	ActiveWeaponList::iterator iter;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		ioWeapon *pCurWeapon = ( *iter );
		if( !pCurWeapon ) continue;

		if( pCurWeapon->GetOwnerName() == szOwnerName && pCurWeapon->IsSkillDependency(szSkillName) )
			return true;
	}

	return false;
}

void ioWeaponManager::CheckSkillDependencyWeaponAction( const ioHashString &szOwnerName, const ioHashString &szSkillName )
{
	ActiveWeaponList::iterator iter;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		ioWeapon *pCurWeapon = ( *iter );
		if( !pCurWeapon ) continue;

		if( pCurWeapon->GetOwnerName() == szOwnerName && pCurWeapon->IsSkillDependency(szSkillName) )
		{
			pCurWeapon->SetSkillDependencyAction();
		}
	}
}

void ioWeaponManager::CheckSkillDependencyWeaponDead( const ioHashString &szOwnerName, const ioHashString &szSkillName )
{
	ActiveWeaponList::iterator iter;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		ioWeapon *pCurWeapon = ( *iter );
		if( !pCurWeapon ) continue;

		if( pCurWeapon->GetOwnerName() == szOwnerName && pCurWeapon->IsSkillDependency(szSkillName) )
		{
			if( ToBombMissileWeapon2( pCurWeapon ) )
			{
				ToBombMissileWeapon2( pCurWeapon )->Explosion( m_pPlayStage );
			}
			else if( ToDoubleIceMissileWaepon( pCurWeapon ) )
			{
				ToDoubleIceMissileWaepon( pCurWeapon )->Explosion( m_pPlayStage );
			}
			else
			{
				pCurWeapon->SetWeaponDead();
			}
		}
	}
}

void ioWeaponManager::CheckLobeliaWeaponAction( const ioHashString &szName )
{
	ioLobeliaWeapon *pCurWeapon = NULL;

	ActiveWeaponList::iterator iter;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		pCurWeapon = ToLobeliaWeapon( (*iter) );
		if( !pCurWeapon ) continue;

		if( pCurWeapon->GetOwnerName() == szName )
		{
			pCurWeapon->Explosion( m_pPlayStage );
		}
	}
}

void ioWeaponManager::CheckMaxLobeliaWeaponCnt( ioWeapon *pWeapon, int iMaxCnt )
{
	if( iMaxCnt <= 0 )
		return;

	typedef std::vector< ioLobeliaWeapon* > LobeliaList;
	LobeliaList vLobeliaList;

	ioLobeliaWeapon *pCurWeapon = NULL;
	ioHashString szOwnerName = pWeapon->GetOwnerName();
	ioHashString szCreateItem = pWeapon->GetCreateItem();

	ActiveWeaponList::iterator iter;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		pCurWeapon = ToLobeliaWeapon( (*iter) );
		if( !pCurWeapon ) continue;

		if( pCurWeapon->GetOwnerName() == szOwnerName )
		{
			vLobeliaList.push_back( pCurWeapon );
		}
	}

	int iAttachCnt = vLobeliaList.size();
	if( iAttachCnt <= iMaxCnt )
		return;

	DWORD dwCreateTime = -1;	// 오버플로우를 시켜서 최대값으로 세팅
	ioLobeliaWeapon *pDelete = NULL;
	for( int i=0 ; i<iAttachCnt ; i++ )
	{
		pCurWeapon = vLobeliaList[i];
		if( dwCreateTime > pCurWeapon->GetCreateTime() )
		{
			dwCreateTime = pCurWeapon->GetCreateTime();
			pDelete = pCurWeapon;
		}
	}

	if( pDelete )
	{
		pDelete->Explosion( m_pPlayStage );
	}
}

int ioWeaponManager::GetCurLobeliaWeaponCnt( ioBaseChar *pOwner )
{
	if( !pOwner )
		return 0;

	ioHashString szOwnerName = pOwner->GetCharName();

	int iAttachCnt = 0;
	ActiveWeaponList::iterator iter;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		ioLobeliaWeapon *pAttach = ToLobeliaWeapon( *iter );

		if( pAttach && pAttach->GetOwnerName() == szOwnerName )
		{
			iAttachCnt++;
		}
	}

	return iAttachCnt;
}

int ioWeaponManager::CheckSetWeaponSylphid( ioBaseChar *pOwner )
{
	if( !pOwner )
		return 0;

	ioHashString szOwnerName = pOwner->GetCharName();

	int iNeedCnt = 0;
	ActiveWeaponList::iterator iter;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		ioWeapon *pWeapon = (*iter);
		if( pWeapon && pWeapon->GetOwnerName() == szOwnerName )
		{
			iNeedCnt += pWeapon->SetSylphid();
		}
	}

	return iNeedCnt;
}

void ioWeaponManager::CheckSylphidMissileWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon )
		return;

	typedef std::vector< ioSylphidMissileWeapon* > SylphidMissileList;
	SylphidMissileList vSylphidList;

	ioSylphidMissileWeapon *pCurWeapon = NULL;
	ioHashString szOwnerName = pWeapon->GetOwnerName();
	ioHashString szCreateItem = pWeapon->GetCreateItem();

	ActiveWeaponList::iterator iter;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		pCurWeapon = ToSylphidMissileWeapon( (*iter) );
		if( !pCurWeapon ) continue;

		if( pCurWeapon->GetOwnerName() == szOwnerName &&
			pCurWeapon->GetAttributeIndex() == pWeapon->GetAttributeIndex() )
		{
			vSylphidList.push_back( pCurWeapon );
		}
	}

	int iAttachCnt = vSylphidList.size();

	if( iAttachCnt <= 1 )
		return;
	
	DWORD dwCreateTime = -1;	// 오버플로우를 시켜서 최대값으로 세팅
	ioSylphidMissileWeapon *pDelete = NULL;
	for( int i=0 ; i<iAttachCnt ; i++ )
	{
		pCurWeapon = vSylphidList[i];
		if( dwCreateTime > pCurWeapon->GetCreateTime() )
		{
			dwCreateTime = pCurWeapon->GetCreateTime();
			pDelete = pCurWeapon;
		}
	}

	if( pDelete )
	{
		pDelete->SetWeaponDead();
	}
}

void ioWeaponManager::DestroyReserveZoneWeapon( const ioHashString &rkOwner )
{
	if( rkOwner.IsEmpty() )
		return;

	ioWeapon *pWeapon = NULL;
	ActiveWeaponList::iterator iter;
	for( iter = m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		pWeapon = *iter;

		ioZoneEffectWeapon *pZoneWeapon = ToZoneEffectWeapon(pWeapon);
		if( !pZoneWeapon ) continue;
		if( !pZoneWeapon->IsLive() ) continue;
		if( pZoneWeapon->GetOwnerName() != rkOwner )
			continue;

		if( pZoneWeapon->CheckDestroyOwnerAttach() )
		{
			pZoneWeapon->SetWeaponDead();
		}
	}
}

void ioWeaponManager::SetReserveCallWeaponInfo( const ioHashString &rkName, const WeaponInfo &rkWeaponInfo,
												const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir,
												DWORD dwGenerateTime, DWORD pOwnerWeaponIdx,
												bool bSend )
{
	CallWeaponInfo kInfo;

	kInfo.m_CreateChar = rkName;
	kInfo.m_WeaponInfo = rkWeaponInfo;
	kInfo.m_vPos = vPos;
	kInfo.m_vDir = vDir;	
	kInfo.m_dwGenerateTime = dwGenerateTime;
	kInfo.m_dwOwnerWeaponIdx = pOwnerWeaponIdx;
	kInfo.m_bSend = bSend;

	m_vCallWeaponInfoList.push_back( kInfo );

	std::sort( m_vCallWeaponInfoList.begin(), m_vCallWeaponInfoList.end(), CallWeaponInfoSort() );
}

void ioWeaponManager::SetReserveCallWeaponCollisionInfo( const ioHashString &rkName, const WeaponInfo &rkWeaponInfo,
													     const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir,
													     DWORD dwGenerateTime, DWORD pOwnerWeaponIdx,
													     exReserveWeaponCollisionInfo exCollisionInfo,
														 bool bSend )
{
	CallWeaponInfo kInfo;

	kInfo.m_CreateChar = rkName;
	kInfo.m_WeaponInfo = rkWeaponInfo;
	kInfo.m_vPos = vPos;
	kInfo.m_vDir = vDir;	
	kInfo.m_dwGenerateTime = dwGenerateTime;
	kInfo.m_dwOwnerWeaponIdx = pOwnerWeaponIdx;
	kInfo.m_exCollisionInfo = exCollisionInfo;
	kInfo.m_bSend = bSend;

	m_vCallWeaponInfoList.push_back( kInfo );

	std::sort( m_vCallWeaponInfoList.begin(), m_vCallWeaponInfoList.end(), CallWeaponInfoSort() );
}

void ioWeaponManager::ProcessCallWeaponInfo()
{
	if( m_vCallWeaponInfoList.empty() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	CallWeaponInfoList::iterator iter = m_vCallWeaponInfoList.begin();
	while( iter != m_vCallWeaponInfoList.end() )
	{
		CallWeaponInfo &rkInfo = (*iter);

		ioBaseChar *pChar = m_pPlayStage->GetBaseChar( rkInfo.m_CreateChar );
		if( !pChar )
		{
			iter = m_vCallWeaponInfoList.erase( iter );
			continue;
		}

		if( rkInfo.m_dwGenerateTime < dwCurTime )
		{
			ioWeapon* pWeapon = pChar->WeaponByWeaponExplicitUnit( rkInfo.m_WeaponInfo,
				FTT_SKILL_ATTACK,
				rkInfo.m_vPos,
				rkInfo.m_vDir,
				rkInfo.m_exCollisionInfo,
				rkInfo.m_bSend, true );
			
			if( pWeapon )
			{	
				ioWeapon* pOwnerWeapon = FindWeapon( pChar, rkInfo.m_dwOwnerWeaponIdx );
				if( pOwnerWeapon )			
					pOwnerWeapon->ApplyExtraReserveWeaponByWeaponInfo( pWeapon );
				
				ioTraceWeapon6* pTrace6 = ToTraceWeapon6( pWeapon );
				if( pTrace6 )
				{
					pTrace6->SetOwnerWeaponCollisionInfo( rkInfo.m_exCollisionInfo );
				}

				ioZoneEffectWeapon *pZone = ToZoneEffectWeapon(pWeapon);
				if( pZone && pOwnerWeapon )
				{
					D3DXVECTOR3 vDir = pOwnerWeapon->GetMoveDir();
					pZone->SetCollisionDir( vDir );
					pZone->SetZoneValue( false, false );
				}
			}

			iter = m_vCallWeaponInfoList.erase( iter );
			continue;
		}

		++iter;
	}
}

void ioWeaponManager::CheckAdhesiveWeapon3( const ioHashString &szOwnerName )
{
	ioAdhesiveMissileWeapon3 *pAdhesive3 = NULL;

	ActiveWeaponList::iterator iter;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		ioAdhesiveMissileWeapon3 *pCurWeapon = ToAdhesiveMissileWeapon3( *iter );
		if( pCurWeapon && pCurWeapon->GetOwnerName() == szOwnerName )
		{
			pCurWeapon->CheckOwnerAttach( false );
		}
	}
}

void ioWeaponManager::CheckAdhesiveWeapon4( const ioHashString &szOwnerName )
{
	ioAdhesiveMissileWeapon4 *pAdhesive4 = NULL;

	ActiveWeaponList::iterator iter;
	for( iter = m_vWeaponList.begin() ; iter != m_vWeaponList.end(); ++iter )
	{
		ioAdhesiveMissileWeapon4 *pCurWeapon = ToAdhesiveMissileWeapon4( *iter );
		if( pCurWeapon && pCurWeapon->GetOwnerName() == szOwnerName )
		{
			pCurWeapon->SetState( ioAdhesiveMissileWeapon4::WS_CHOP );
			pCurWeapon->SebCollisionMapForSkill( true );
		}
	}
}

void ioWeaponManager::CheckRotateAttachCharWeapon( const ioHashString &szOwnerName )
{
	ActiveWeaponList::iterator iter;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		ioRotateAttachCharWeapon *pCurWeapon = ToRotateAttachCharWeapon( *iter );
		if( pCurWeapon && pCurWeapon->GetOwnerName() == szOwnerName )
		{
			pCurWeapon->SetAttachChar( false );
		}
	}
}

bool ioWeaponManager::CheckGraffitiWeaponAction( const ioHashString &szName )
{
	typedef std::vector< ioGraffitiWeapon* > vGraffitiList;
	vGraffitiList vWeaponList;

	ioGraffitiWeapon *pCurWeapon = NULL;
	ActiveWeaponList::iterator iter;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		pCurWeapon = ToGraffitiWeapon( (*iter) );
		if( !pCurWeapon ) continue;

		if( pCurWeapon->IsLive() && !pCurWeapon->IsExplosion() && pCurWeapon->GetOwnerName() == szName )
		{
			vWeaponList.push_back( pCurWeapon );
		}
	}

	int iAttachCnt = vWeaponList.size();
	if( iAttachCnt <= 0 )
		return false;

	DWORD dwCreateTime = -1;	// 오버플로우를 시켜서 최대값으로 세팅
	ioGraffitiWeapon *pDelete = NULL;
	for( int i=0 ; i<iAttachCnt ; i++ )
	{
		pCurWeapon = vWeaponList[i];
		if( dwCreateTime > pCurWeapon->GetCreateTime() )
		{
			dwCreateTime = pCurWeapon->GetCreateTime();
			pDelete = pCurWeapon;
		}
	}

	if( pDelete )
	{
		pDelete->Explosion( m_pPlayStage );
	}

	return true;
}

void ioWeaponManager::CheckMaxGraffitiWeaponCnt( ioWeapon *pWeapon, int iMaxCnt )
{
	if( iMaxCnt <= 0 )
		return;

	typedef std::vector< ioGraffitiWeapon* > vGraffitiList;
	vGraffitiList vWeaponList;

	ioGraffitiWeapon *pCurWeapon = NULL;
	ioHashString szOwnerName = pWeapon->GetOwnerName();
	ioHashString szCreateItem = pWeapon->GetCreateItem();

	ActiveWeaponList::iterator iter;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		pCurWeapon = ToGraffitiWeapon( (*iter) );
		if( !pCurWeapon ) continue;

		if( pCurWeapon->IsLive() && !pCurWeapon->IsExplosion() && pCurWeapon->GetOwnerName() == szOwnerName )
		{
			vWeaponList.push_back( pCurWeapon );
		}
	}

	int iAttachCnt = vWeaponList.size();
	if( iAttachCnt <= iMaxCnt )
		return;

	DWORD dwCreateTime = -1;	// 오버플로우를 시켜서 최대값으로 세팅
	ioGraffitiWeapon *pDelete = NULL;
	for( int i=0 ; i<iAttachCnt ; i++ )
	{
		pCurWeapon = vWeaponList[i];
		if( dwCreateTime > pCurWeapon->GetCreateTime() )
		{
			dwCreateTime = pCurWeapon->GetCreateTime();
			pDelete = pCurWeapon;
		}
	}

	if( pDelete )
	{
		pDelete->Explosion( m_pPlayStage );
	}
}

int ioWeaponManager::GetCurGraffitiWeaponCnt( ioBaseChar *pOwner )
{
	if( !pOwner )
		return 0;

	ioHashString szOwnerName = pOwner->GetCharName();

	int iAttachCnt = 0;
	ActiveWeaponList::iterator iter;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		ioGraffitiWeapon *pAttach = ToGraffitiWeapon( *iter );

		if( pAttach && pAttach->GetOwnerName() == szOwnerName )
		{
			iAttachCnt++;
		}
	}

	return iAttachCnt;
}


int ioWeaponManager::GetWeaponCnt( ioBaseChar *pOwner, int nType )
{
	if( !pOwner )
		return 0;

	int nCnt = 0;
	ActiveWeaponList::iterator iter;
	for( iter=m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		ioWeapon *pWeapon = (*iter);
		if( pWeapon && pWeapon->GetOwnerName() == pOwner->GetCharName() && (int)pWeapon->GetType() == nType )
			nCnt++;
	}

	return nCnt;
}

void ioWeaponManager::AddApplyWeaponChecker( const ioHashString &szCharName )
{
	ApplyWeaponChecker::iterator iter = m_ApplyWeaponChecker.find( szCharName );
	if( iter != m_ApplyWeaponChecker.end() )
	{
		iter->second += 1;
	}
	else
	{
		m_ApplyWeaponChecker.insert( ApplyWeaponChecker::value_type( szCharName, 1 ) );
	}
}

void ioWeaponManager::AddCheckWeaponInfo( const ioHashString &szCharName, ioWeapon *pWeapon, int iType )
{
	switch( iType )
	{
	case AWE_NO_EXPLOSION:
	case AWE_EXPLOSION:
	case AWE_DEFENSE_EXPLOSION:
	case AWE_MISSILE_EXPLOSION:
		return;
	}

	DWORD dwWeaponIndex = 0;
	DWORD dwAttributeIndex = 0;
	if( pWeapon )
	{
		dwWeaponIndex = pWeapon->GetWeaponIndex();
		dwAttributeIndex = pWeapon->GetAttributeIndex();
	}

	ApplyWeaponInfo kInfo;
	kInfo.m_CharName = szCharName;
	kInfo.m_dwWeaponIndex = dwWeaponIndex;
	kInfo.m_iApplyType = iType;
	kInfo.m_dwApplyTime = FRAMEGETTIME();

	m_CheckWeaponInfoList.push_back( kInfo );
}

void ioWeaponManager::ProcessApplyWeaponInfoList()
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = 0;

	// CheckList와 ApplyList 비교처리.
	ApplyWeaponInfoList::iterator iter_check;
	ApplyWeaponInfoList::iterator iter_apply = m_ApplyWeaponInfoList.begin();

	while( iter_apply != m_ApplyWeaponInfoList.end() )
	{
		bool bFind = false;
		iter_check = m_CheckWeaponInfoList.begin();

		while( iter_check != m_CheckWeaponInfoList.end() )
		{
			if( (*iter_apply).m_CharName == (*iter_check).m_CharName &&
				(*iter_apply).m_dwWeaponIndex == (*iter_check).m_dwWeaponIndex &&
				(*iter_apply).m_iApplyType == (*iter_check).m_iApplyType )
			{
				m_CheckWeaponInfoList.erase( iter_check );
				bFind = true;
				break;
			}

			++iter_check;
		}

		if( bFind )
		{
			iter_apply = m_ApplyWeaponInfoList.erase( iter_apply );
			continue;
		}

		++iter_apply;
	}

	// GapTime 체크
	iter_check = m_CheckWeaponInfoList.begin();
	while( iter_check != m_CheckWeaponInfoList.end() )
	{
		dwGapTime = dwCurTime - (*iter_check).m_dwApplyTime;

		if( dwGapTime > 200 )
		{
			AddApplyWeaponChecker( (*iter_check).m_CharName );
			iter_check = m_CheckWeaponInfoList.erase( iter_check );
			continue;
		}

		++iter_check;
	}


	iter_apply = m_ApplyWeaponInfoList.begin();
	while( iter_apply != m_ApplyWeaponInfoList.end() )
	{
		dwGapTime = dwCurTime - (*iter_apply).m_dwApplyTime;

		if( dwGapTime > 200 )
		{
			iter_apply = m_ApplyWeaponInfoList.erase( iter_apply );
			continue;
		}

		++iter_apply;
	}
}

void ioWeaponManager::CheckApplyWeaponWounded( const ioHashString &szCharName, DWORD dwWeaponIndex, DWORD dwAttributeIndex )
{
	ApplyWeaponInfo kInfo;
	kInfo.m_CharName = szCharName;
	kInfo.m_dwWeaponIndex = dwWeaponIndex;
	kInfo.m_iApplyType = AWE_G_EXPLOSION;
	kInfo.m_dwApplyTime = FRAMEGETTIME();

	m_ApplyWeaponInfoList.push_back( kInfo );
}

void ioWeaponManager::CheckApplyWeaponDefense( const ioHashString &szCharName, DWORD dwWeaponIndex, DWORD dwAttributeIndex )
{
	ApplyWeaponInfo kInfo;
	kInfo.m_CharName = szCharName;
	kInfo.m_dwWeaponIndex = dwWeaponIndex;
	kInfo.m_iApplyType = AWE_G_DEFENSE_EXPLOSION;
	kInfo.m_dwApplyTime = FRAMEGETTIME();

	m_ApplyWeaponInfoList.push_back( kInfo );
}

void ioWeaponManager::ClearApplyWeaponInfo()
{
	ApplyWeaponChecker::iterator iter = m_ApplyWeaponChecker.begin();
	while( iter != m_ApplyWeaponChecker.end() )
	{
		LOG.PrintTimeAndLog( 0, "FC - ApplyWeaponChecker : %s, %d", iter->first.c_str(), iter->second );
		++iter;
	}

	m_CheckWeaponInfoList.clear();
	m_ApplyWeaponInfoList.clear();
	
	m_ApplyWeaponChecker.clear();
}

void ioWeaponManager::DestroyDummyCharWeapon( const ioHashString &rkOwner, int iDummyIdx )
{
	if( rkOwner.IsEmpty() )
		return;

	ioWeapon *pWeapon = NULL;
	ActiveWeaponList::iterator iter;
	for( iter = m_vWeaponList.begin() ; iter!=m_vWeaponList.end() ; ++iter )
	{
		pWeapon = *iter;

		int iWeaponByDummy = 0;
		if( pWeapon )
			iWeaponByDummy = pWeapon->GetWeaponByDummyChar();
		
		if( pWeapon->GetOwnerName() == rkOwner && iWeaponByDummy == iDummyIdx )
		{
			pWeapon->SetWeaponDead();
		}
	}
}

WeaponEventType ioWeaponManager::GetWeaponEvent( DWORD dwWeaponCreateIdx )
{
	WeaponEventMap::iterator iter = m_WeaponEventMap.find( dwWeaponCreateIdx );
	if( iter != m_WeaponEventMap.end() )
	{
		return iter->second;
	}

	return WET_NONE;
}

void ioWeaponManager::AddWeaponEvent( DWORD dwWeaponCreateIdx, WeaponEventType eType )
{
	WeaponEventMap::iterator iter = m_WeaponEventMap.find( dwWeaponCreateIdx );
	if( iter == m_WeaponEventMap.end() )
	{
		m_WeaponEventMap.insert( WeaponEventMap::value_type( dwWeaponCreateIdx, eType ) );
	}
	else
	{
		iter->second = eType;
	}
}

void ioWeaponManager::WeaponMapCollisionExecProcedure( ioPlayStage* pStage, ioWeapon* pWeapon )
{
	if( !pStage || !pWeapon )
		return;

	const ioOrientBox &rkWorldAttackBox = pWeapon->GetWorldAttackBox( true );
	if( !rkWorldAttackBox.IsNull() )
	{
		if( !pWeapon->IsIgnoreMapCollisionEffect() )
			pStage->CreateMapEffect( pWeapon->GetExplosionEffectName(), rkWorldAttackBox.GetCenter(), pWeapon->GetScale() );
		pWeapon->PlayWoundedSound( NULL );
	}

	if( pWeapon->IsIgnoreMapCollision() )
		return;
		
	switch( pWeapon->GetType() )
	{
	case ioWeapon::WT_CHAIN:
		{
			ioChainWeapon *pChainWeapon = ToChainWeapon( pWeapon );
			if( pChainWeapon )
				pChainWeapon->SetWaitState();
		}
		break;
	case ioWeapon::WT_MISSILE:
		{
			ioMissileWeapon *pMissileWeapon	= ToMissileWeapon( pWeapon );
			if( pMissileWeapon )
				pMissileWeapon->SetCreateDummy( true );

			pWeapon->SetMapCollision();
			pWeapon->SetWeaponDead();
		}
		break;
	case ioWeapon::WT_TRACE7:
		{
			pWeapon->SetMapCollision();
		}
		break;
	default:
		{
			pWeapon->SetMapCollision();
			pWeapon->SetWeaponDead();
		}
		break;
	}
}


int ioWeaponManager::GetCurLocalIndex()
{
	m_iLocalIndex++;
	return m_iLocalIndex;
}
