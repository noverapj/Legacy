
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "DummyCharEnumType.h"
#include "DummyCharDefine.h"

#include "ioSuperDimensionItem.h"
#include "iolupinItem.h"
#include "ioSoldierAwakeManager.h"

#include "RaceSetting.h"

template<> ioDummyCharManager* Singleton< ioDummyCharManager >::ms_Singleton = 0;

ioDummyCharManager::ioDummyCharManager( ioPlayStage *pStage )
{
	m_iLocalIndex = 0;
	m_pPlayStage = pStage;

	m_DummyCharList.clear();
	m_DummyNoOverLapTrackingList.clear();
	m_DummyCharBaseInfoList.clear();
}

ioDummyCharManager::~ioDummyCharManager()
{
	DestroyDummyCharList();
}

ioDummyCharManager& ioDummyCharManager::GetSingleton()
{
	return Singleton< ioDummyCharManager >::GetSingleton();
}

void ioDummyCharManager::LoadDummyCharBaseInfo()
{
	m_DummyCharBaseInfoList.clear();

	LoadInfoByChar();
	LoadInfoByNPC();
}

void ioDummyCharManager::LoadInfoByChar()
{
	ioINILoader_e kLoader( "config/sp2_dummy_char.ini" );

	char szBuf[MAX_PATH] = "";
	char szTitle[MAX_PATH] = "";
	char szGroupName[MAX_PATH] = "";

	kLoader.SetTitle_e( "common_info" );
	int iGroupCnt = kLoader.LoadInt_e( "dummy_char_group_cnt", 0 );

	for( int i=0; i < iGroupCnt; ++i )
	{
		DummyCharBaseInfo kBaseInfo;

		// ±×·i E￡Aa
		wsprintf_e( szTitle, "dummy_char_group%d", i );
		kLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		wsprintf_e( szGroupName, "config/%s", szBuf );

		kBaseInfo.m_DummyCharPath = szGroupName;

		ioINILoader kSetDummyCharLoader( szGroupName );
		kSetDummyCharLoader.SetTitle_e( "common_info" );

		int iCnt = kSetDummyCharLoader.LoadInt_e( "dummy_char_cnt", 0 );
		for( int j=0; j < iCnt; ++j )
		{
			wsprintf_e( szTitle, "dummy_char%d", j+1 );
			kBaseInfo.m_DummyCharTitle = szTitle;

			kSetDummyCharLoader.SetTitle( szTitle );

			kSetDummyCharLoader.LoadString_e( "dummy_char_name", "", szBuf, MAX_PATH );
			kBaseInfo.m_DummyCharName = szBuf;

			kSetDummyCharLoader.LoadString_e( "dummy_char_model", "", szBuf, MAX_PATH );
			kBaseInfo.m_DummyCharModel = szBuf;

			kSetDummyCharLoader.LoadString_e( "dummy_char_model_woman", "", szBuf, MAX_PATH );
			kBaseInfo.m_DummyCharModelW = szBuf;

			kBaseInfo.m_iDummyCharType = kSetDummyCharLoader.LoadInt_e( "dummy_char_type", 0 );

			D3DXVECTOR3 vScale;
			vScale.x = kSetDummyCharLoader.LoadFloat_e( "scale_x", FLOAT1 );
			vScale.y = kSetDummyCharLoader.LoadFloat_e( "scale_y", FLOAT1 );
			vScale.z = kSetDummyCharLoader.LoadFloat_e( "scale_z", FLOAT1 );

			kBaseInfo.m_vScale = vScale;

			m_DummyCharBaseInfoList.push_back( kBaseInfo );
		}
	}
}

void ioDummyCharManager::LoadInfoByNPC()
{
	ioINILoader_e kLoader( "config/sp2_dummy_char_npc.ini" );

	char szBuf[MAX_PATH] = "";
	char szTitle[MAX_PATH] = "";
	char szGroupName[MAX_PATH] = "";

	kLoader.SetTitle_e( "common_info" );
	int iGroupCnt = kLoader.LoadInt_e( "dummy_char_group_cnt", 0 );

	for( int i=0; i < iGroupCnt; ++i )
	{
		DummyCharBaseInfo kBaseInfo;

		// 그룹 호출
		wsprintf_e( szTitle, "dummy_char_group%d", i );
		kLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		wsprintf_e( szGroupName, "config/%s", szBuf );

		kBaseInfo.m_DummyCharPath = szGroupName;

		ioINILoader kSetDummyCharLoader( szGroupName );
		kSetDummyCharLoader.SetTitle_e( "common_info" );

		int iCnt = kSetDummyCharLoader.LoadInt_e( "dummy_char_cnt", 0 );
		for( int j=0; j < iCnt; ++j )
		{
			wsprintf_e( szTitle, "dummy_char%d", j+1 );
			kBaseInfo.m_DummyCharTitle = szTitle;

			kSetDummyCharLoader.SetTitle( szTitle );

			kSetDummyCharLoader.LoadString_e( "dummy_char_name", "", szBuf, MAX_PATH );
			kBaseInfo.m_DummyCharName = szBuf;

			kSetDummyCharLoader.LoadString_e( "dummy_char_model", "", szBuf, MAX_PATH );
			kBaseInfo.m_DummyCharModel = szBuf;

			kSetDummyCharLoader.LoadString_e( "dummy_char_model_woman", "", szBuf, MAX_PATH );
			kBaseInfo.m_DummyCharModelW = szBuf;

			kBaseInfo.m_iDummyCharType = kSetDummyCharLoader.LoadInt_e( "dummy_char_type", 0 );

			D3DXVECTOR3 vScale;
			vScale.x = kSetDummyCharLoader.LoadFloat_e( "scale_x", FLOAT1 );
			vScale.y = kSetDummyCharLoader.LoadFloat_e( "scale_y", FLOAT1 );
			vScale.z = kSetDummyCharLoader.LoadFloat_e( "scale_z", FLOAT1 );

			kBaseInfo.m_vScale = vScale;

			m_DummyCharBaseInfoList.push_back( kBaseInfo );
		}
	}
}

ioDummyChar* ioDummyCharManager::CreateDummyChar( ioHashString szModelName, ioPlayMode *pMode, int iType )
{
	if( pMode && !pMode->GetDummyTypeCreateValue() )
	{
		LOG.PrintTimeAndLog( 0, "ioDummyCharManager::CreateDummyCharFail -> CreateValue is False");
		return NULL;
	}

	ioEntityGroup *pGrp = m_pPlayStage->CreateEntityGrpAndSceneNode( szModelName, true );
	if( !pGrp )
	{
		LOG.PrintTimeAndLog( 0, "ioDummyCharManager::CreateDummyChar - %s Fail!", szModelName.c_str() );
		return NULL;
	}

	if( iType == DCT_MAMAHAHA )
	{
		ioMamaHahaDummy* pDummy = new ioMamaHahaDummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_OWNER_CONTROL2 )
	{
		ioOwnerControlDummy2* pDummy = new ioOwnerControlDummy2( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_RAPTOR )
	{
		ioRaptorDummy* pDummy = new ioRaptorDummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_MOVE_ATTACK )
	{
		ioMoveAttackDummy* pDummy = new ioMoveAttackDummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_CROW )
	{
		ioCrowDummy* pDummy = new ioCrowDummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_CURSE_CHAR )
	{
		ioCurseDummyChar* pDummy = new ioCurseDummyChar( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_MOVE_ATTACK2 )
	{
		ioMoveAttack2Dummy* pDummy = new ioMoveAttack2Dummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_OWNER_ATTACH4 )
	{
		ioOwnerAttach4Dummy* pDummy = new ioOwnerAttach4Dummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_OWNER_CONTROL3 )
	{
		ioOwnerControlDummy3* pDummy = new ioOwnerControlDummy3( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_MINE )
	{
		ioDummyMine* pDummy = new ioDummyMine( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_STEEL )
	{
		ioDummySteel* pDummy = new ioDummySteel( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_OWNER_ATTACH5 )
	{
		ioOwnerAttach5Dummy* pDummy = new ioOwnerAttach5Dummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_ATTACH_WEAPON )
	{
		ioAttachWeaponDummy* pDummy = new ioAttachWeaponDummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_TARGET_TRACE6 )
	{
		ioTargetTrace6Dummy* pDummy = new ioTargetTrace6Dummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_TARGET_TRACE7 )
	{
		ioTargetTrace7Dummy* pDummy = new ioTargetTrace7Dummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_MOVE_ATTACK3 )
	{
		ioMoveAttack3Dummy* pDummy = new ioMoveAttack3Dummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_DELAY_ATTACK )
	{
		ioDelayAttackDummy* pDummy = new ioDelayAttackDummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_TOPPLATE )
	{
		ioDummyTopPlate* pDummy = new ioDummyTopPlate( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_EARTH_MAGE )
	{
		ioEarthDummy* pDummy = new ioEarthDummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_TARGET_TRACE8 )
	{
		ioTargetTrace8Dummy* pDummy = new ioTargetTrace8Dummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_WARP_MINE )
	{
		ioDummyWarpMine* pDummy = new ioDummyWarpMine( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_STRUCT )
	{
		ioDummyStruct* pDummy = new ioDummyStruct( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_GRASS )
	{
		ioDummyGrass* pDummy = new ioDummyGrass( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_HELICOPTER )
	{
		ioDummyHelicopter* pDummy = new ioDummyHelicopter( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_TARGET_TRACE9 )
	{
		ioTargetTrace9Dummy* pDummy = new ioTargetTrace9Dummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_TURRET )
	{
		ioDummyTurret* pDummy = new ioDummyTurret( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_JUMP_ATTACK )
	{
		ioDummyJumpAttack* pDummy = new ioDummyJumpAttack( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_VEHICLE )
	{
		ioVehicleDummy* pDummy = new ioVehicleDummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_OWNER_CONTROL4 )
	{
		ioOwnerControlDummy4* pDummy = new ioOwnerControlDummy4( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_GRADE_TURRET )
	{
		ioDummyGradeTurret* pDummy = new ioDummyGradeTurret( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_ENGINEER_GATLING )
	{
		ioEngineerGatlingDummy* pDummy = new ioEngineerGatlingDummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_TARGET_TRACE10 )
	{
		ioTargetTrace10Dummy* pDummy = new ioTargetTrace10Dummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_SHADOW_BOXER )
	{
		ioShadowBoxerDummy* pDummy = new ioShadowBoxerDummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_CRANE )
	{
		ioDummyCrane* pDummy = new ioDummyCrane( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_SUB_CRANE )
	{
		ioDummySubCrane* pDummy = new ioDummySubCrane( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_TARGET_TRACE11 )
	{
		ioTargetTrace11Dummy* pDummy = new ioTargetTrace11Dummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_OWNER_CONTROL5 )
	{
		ioOwnerControlDummy5* pDummy = new ioOwnerControlDummy5( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_NONE_TRACE2 )
	{
		ioNoneTraceDummy2* pDummy = new ioNoneTraceDummy2( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_CIRCLE_TRACE )
	{
		ioCircleTraceDummy* pDummy = new ioCircleTraceDummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_CIRCLE_TRACE2 )
	{
		ioCircleTraceDummy2* pDummy = new ioCircleTraceDummy2( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_POLE )
	{
		ioPoleDummy* pDummy = new ioPoleDummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_TARGET_TRACE12 )
	{
		ioTargetTrace12Dummy* pDummy = new ioTargetTrace12Dummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_CHAIN )
	{
		ioDummyChain* pDummy = new ioDummyChain( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_TARGET_TRACE13 )
	{
		ioTargetTrace13Dummy* pDummy = new ioTargetTrace13Dummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_TARGET_TRACE14 )
	{
		ioTargetTrace14Dummy* pDummy = new ioTargetTrace14Dummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_TARGET_TRACE15 )
	{
		ioTargetTrace15Dummy* pDummy = new ioTargetTrace15Dummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_ELEPHANT_SOUL )
	{
		ioElephantSoulDummy* pDummy = new ioElephantSoulDummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_TRACING_RUSH )
	{
		ioTracingRushDummy* pDummy = new ioTracingRushDummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_BUNKER )
	{
		ioBunkerDummy* pDummy = new ioBunkerDummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_TURRET2 )
	{
		ioDummyTurret2* pDummy = new ioDummyTurret2( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_OWNER_ATTACH6 )
	{
		ioOwnerAttach6Dummy* pDummy = new ioOwnerAttach6Dummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_PAPPY )
	{
		ioPappyDummy* pDummy = new ioPappyDummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_TARGET_TRACE16 )
	{
		ioTargetTrace16Dummy *pDummy = new ioTargetTrace16Dummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_DRUID_EAGLE )
	{
		ioDruidEagleDummy* pDummy = new ioDruidEagleDummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_TARGET_TRACE17 )
	{
		ioTargetTrace17Dummy* pDummy = new ioTargetTrace17Dummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_OWNER_TRACE2 )
	{

		ioOwnerTraceDummy2* pDummy = new ioOwnerTraceDummy2( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_TARGET_TRACE18 )
	{
		ioTargetTrace18Dummy* pDummy = new ioTargetTrace18Dummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_BUFF_DUMMY )
	{
		ioBuffDummy* pDummy = new ioBuffDummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_GARGOYLE_DUMMY )
	{
		ioGargoyleDummy* pDummy = new ioGargoyleDummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_CANNON_ITEM_DUMMY )
	{
		ioCannonWeaponDummy* pDummy = new ioCannonWeaponDummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_FORKCRANE_DUMMY )
	{
		ioForkcraneDummy* pDummy = new ioForkcraneDummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_MOVE_ATTACK4 )
	{
		ioMoveAttack4Dummy* pDummy = new ioMoveAttack4Dummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_OWNER_CONTROL6 )
	{
		ioOwnerControlDummy6* pDummy = new ioOwnerControlDummy6( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_SHIELD_DOME )
	{
		ioShieldDomeDummy* pDummy = new ioShieldDomeDummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_INVINCIBILE_PILLAR )
	{
		ioDummyInvinciblePillar* pDummy = new ioDummyInvinciblePillar( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_OWNER_TRACE_WOUND_ANI )
	{
		ioOwnerTraceWoundAniDummy* pDummy = new ioOwnerTraceWoundAniDummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_CHANGE_AREA_WEAPON )
	{
		ioChangeAreaWeaponDummy* pDummy = new ioChangeAreaWeaponDummy( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else
	{
		ioDummyChar* pDummy = new ioDummyChar( pGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}

	return NULL;
}

ioDummyChar* ioDummyCharManager::CreateDummyCharByOwnerGroup( ioBaseChar *pOwner, ioPlayMode *pMode, int iType )
{
	if( !pOwner )
		return NULL;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return NULL;

	if( pMode && !pMode->GetDummyTypeCreateValue() )
	{
		LOG.PrintTimeAndLog( 0, "ioDummyCharManager::CreateDummyCharFail -> CreateValue is False");
		return NULL;
	}

	ioHashString szModelName = pGrp->GetFileName();
	ioEntityGroup *pCopyGrp = m_pPlayStage->CreateEntityGrpAndSceneNode( szModelName, true );
	if( !pCopyGrp )
	{
		LOG.PrintTimeAndLog( 0, "ioDummyCharManager::CreateDummyCharByOwnerGroup - Fail! : EntityGroup id Null." );
		return NULL;
	}

	pCopyGrp->DestroyAllEntity();

	int iSize = pGrp->GetNumEntities();
	for( int i=0; i<iSize; ++i )
	{
		ioEntity *pEntity = pGrp->GetEntity(i);
		if ( pEntity && pEntity->IsVisible() )
		{
			ioEntity *pNewEntity = pCopyGrp->AddEntity( pEntity->GetMeshName() );
			pOwner->AddEntityEvent( pNewEntity );
		}
	}

	CHARACTER kCharInfo = pOwner->GetCharacterInfo();

	// skin color
	D3DCOLORVALUE kSkin = RaceSetting::GetSkinColor( kCharInfo.m_skin_color );
	D3DCOLORVALUE kSkinOutLine = RaceSetting::GetSkinOutLineColor( kCharInfo.m_skin_color );
	Help::ChangeSkinColor( pCopyGrp, kSkin, kSkinOutLine );

	//Hair Color
	ioHashString szPartsName;
	ioItem *pItem = pOwner->GetEquipedItem( ES_HELMET );
	ioHelmetItem *pHelmet = ToHelmetItem( pItem );
	ioEquipSlot *pEquip = pOwner->GetEquipSlot();
	if( pHelmet && pEquip )
	{
		ioHelmetItem::HelmetHair eHairType = pHelmet->GetChangeHair( pOwner->IsMale() );
		switch( eHairType )
		{
		case ioHelmetItem::HH_HALF_SKIP:
			szPartsName = pEquip->GetDefaultHalfHairMesh();
			break;
		case ioHelmetItem::HH_BIND_SKIP:
			szPartsName = pEquip->GetDefaultBindHairMesh();
			break;
		case ioHelmetItem::HH_NO_SKIP:
			szPartsName = pEquip->GetDefaultHairMesh();
			break;
		}
	}
	else if( pEquip )
		szPartsName = pEquip->GetDefaultHairMesh();


	ioHashString szColorTex = RaceSetting::GetHairColorTex( kCharInfo.m_hair, kCharInfo.m_hair_color );
	D3DCOLORVALUE kHairOutLine  = RaceSetting::GetHairOutLineColor( kCharInfo.m_hair, kCharInfo.m_hair_color );

	ioEntity *pEntity = pCopyGrp->GetEntity(szPartsName);
	if( pEntity )
	{
		Help::ChangeColorTexAndOutLine( pEntity, szColorTex, &kHairOutLine );
	}

	for( int i=0; i<ES_CLOAK+1; ++i )
	{
		ioItem *pEquipItem = pOwner->GetEquipedItem( i );
		if( pEquipItem && pEquipItem->IsVisible() )
		{
			pCopyGrp->AttachEffect( pEquipItem->GetItemEquipEffect( pOwner->IsMale() ), NULL );
			//초시공 전사 예외 처리///////////////////////////////////////////////////
			if( i == ES_WEAPON && ToSuperDimensionItem( pEquipItem ) )
			{
				ioSuperDimensionItem* pSuperDimensionItem = ToSuperDimensionItem( pEquipItem );
				if( pSuperDimensionItem && pSuperDimensionItem->GetWeaponStep() >= 0 )
				{
					ioEffect *pEffect = pCopyGrp->AttachEffect( ToSuperDimensionItem( pEquipItem )->GetExtraEffectName(), NULL );
					if( pEffect )
					{
						float fScale = pSuperDimensionItem->GetActualScale();
						pEffect->ScaleActualSize( fScale );
					}
				}
			}
			//////////////////////////////////////////////////////////////////////////
		}
	}

	//Add Spring Rope
	for( int i=ES_ARMOR; i<ES_CLOAK+1; ++i )
	{
		ioItem *pEquipItem = pOwner->GetEquipedItem( i );
		if( pEquipItem && pEquipItem->IsVisible() )
		{
			ioHashString szSpringRope;
			const RopeSpringParam *pSpringParam = NULL;
			if ( i == ES_ARMOR )
			{
				ioArmorItem* pArmorItem = ToArmorItem( pEquipItem );
				if ( pArmorItem )
				{
					if( pArmorItem->HasSpringRope() )
					{
						szSpringRope = pArmorItem->GetSpringBiped();
						pSpringParam = pArmorItem->GetSpringParam();
					}
				}
			}
			else if ( i == ES_HELMET )
			{
				ioHelmetItem* pHelmetItem = ToHelmetItem( pEquipItem );
				if ( pHelmetItem )
				{
					if( pHelmetItem->HasSpringRope() )
					{
						szSpringRope = pHelmetItem->GetSpringBiped();
						pSpringParam = pHelmetItem->GetSpringParam();
					}
				}
			}
			else if ( i == ES_CLOAK )
			{
				ioCloakItem* pCloakItem = ToCloakItem( pEquipItem );
				if ( pCloakItem )
				{
					if( pCloakItem->HasSpringRope() )
					{
						szSpringRope = pCloakItem->GetSpringBiped();
						pSpringParam = pCloakItem->GetSpringParam();
					}
				}
			}

			if( !szSpringRope.IsEmpty() && pSpringParam && pCopyGrp )
				pCopyGrp->AddRopeSpringCurve( szSpringRope, pSpringParam );
		}
	}

	if( pOwner && pCopyGrp )
	{		
		pCopyGrp->AttachEffect( g_AwakeMgr.GetAwakeEffect( static_cast<AwakeType>( pOwner->GetAwakeType() ) ).c_str(), NULL );
	}

	if( iType == DCT_COPY_OWNER )
	{
		ioCopyOwnerDummyChar* pDummy = new ioCopyOwnerDummyChar( pCopyGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if ( iType == DCT_COPY_OWNER2 )
	{
		ioCopyOwnerDummyChar2* pDummy = new ioCopyOwnerDummyChar2( pCopyGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_COPY_OWNER3 )
	{
		ioCopyOwnerDummyChar3* pDummy = new ioCopyOwnerDummyChar3( pCopyGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_COPY_OWNER4 )
	{
		ioCopyOwnerDummyChar4* pDummy = new ioCopyOwnerDummyChar4( pCopyGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else if( iType == DCT_COPY_OWNER5 )
	{
		ioCopyOwnerDummyChar5* pDummy = new ioCopyOwnerDummyChar5( pCopyGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}
	else
	{
		ioDummyChar* pDummy = new ioDummyChar( pCopyGrp, pMode );
		m_DummyCharList.push_back( pDummy );
		return pDummy;
	}

	return NULL;
}

int ioDummyCharManager::GetCurLocalIndex( bool bIncreaseIndex )
{
	if( bIncreaseIndex )
		m_iLocalIndex++;
	return m_iLocalIndex;
}

void ioDummyCharManager::SetCurLocalIndex( int iIndex )
{
	m_iLocalIndex = iIndex;
}

void ioDummyCharManager::ClearDeadDummyChar()
{
	DummyCharList::iterator iter = m_DummyCharList.begin();
	while( iter != m_DummyCharList.end() )
	{
		ioDummyChar *pDummy = ToDummyChar( *iter );
		if( pDummy && pDummy->GetState() == DCS_DIE )
		{
			iter = m_DummyCharList.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

void ioDummyCharManager::ClearDummyCharByOwner( ioBaseChar *pChar )
{
	if( !pChar ) return;

	int iDummyCnt = m_DummyCharList.size();
	for( int i=0; i < iDummyCnt; i++ )
	{
		if( m_DummyCharList[i]->GetOwnerName() == pChar->GetCharName() )
		{
			m_DummyCharList[i]->SetDieState( true );
		}
	}
}

ioDummyChar* ioDummyCharManager::FindDummyChar( ioBaseChar *pChar, int iIndex )
{
	if( !pChar ) return NULL;

	int iDummyCnt = m_DummyCharList.size();
	for( int i=0; i < iDummyCnt; i++ )
	{
		if( m_DummyCharList[i]->GetOwnerName() != pChar->GetCharName() )
			continue;

		if( m_DummyCharList[i]->GetDummyCharIdx() == iIndex )
			return m_DummyCharList[i];
	}

	return NULL;
}

bool ioDummyCharManager::HasMamaHahaDummy( ioBaseChar *pChar )
{
	if( !pChar ) return false;

	int iDummyCnt = m_DummyCharList.size();
	for( int i=0; i < iDummyCnt; i++ )
	{
		if( m_DummyCharList[i]->GetOwnerName() != pChar->GetCharName() )
			continue;

		if( m_DummyCharList[i]->GetType() == DCT_MAMAHAHA )
			return true;
	}

	return false;
}

void ioDummyCharManager::DestroyMamaHahaDummy( ioBaseChar *pChar )
{
	if( !pChar ) return;

	int iDummyCnt = m_DummyCharList.size();
	for( int i=0; i < iDummyCnt; i++ )
	{
		if( m_DummyCharList[i]->GetOwnerName() != pChar->GetCharName() )
			continue;

		if( m_DummyCharList[i]->GetType() == DCT_MAMAHAHA )
		{
			m_DummyCharList[i]->SetDieState( true );
		}
	}

	return;
}

ioDummyChar* ioDummyCharManager::FindDummyCharToName( ioBaseChar *pChar, const ioHashString &szDummyCharName )
{
	if( !pChar ) return NULL;

	int iDummyCnt = m_DummyCharList.size();
	for( int i=0; i < iDummyCnt; i++ )
	{
		if( m_DummyCharList[i]->GetOwnerName() != pChar->GetCharName() )
			continue;

		if( m_DummyCharList[i]->GetDummyCharName() == szDummyCharName )
			return m_DummyCharList[i];
	}

	return NULL;
}

ioDummyChar* ioDummyCharManager::FindMyDummyCharToName( ioBaseChar *pChar, const ioHashString &szDummyCharName )
{
	if( !pChar ) return NULL;

	int iDummyCnt = m_DummyCharList.size();
	for( int i=0; i < iDummyCnt; i++ )
	{
		if( m_DummyCharList[i]->GetCreatorName() != pChar->GetCharName() )
			continue;

		if( m_DummyCharList[i]->GetDummyCharName() == szDummyCharName )
			return m_DummyCharList[i];
	}

	return NULL;
}

void ioDummyCharManager::FindAllDummyCharToName( IN ioBaseChar *pChar, IN const ioHashString &szDummyCharName, OUT DummyCharList& List )
{
	if( !pChar )
		return;

	int iDummyCnt = m_DummyCharList.size();
	for( int i = 0; i < iDummyCnt; i++ )
	{
		if( m_DummyCharList[i]->GetOwnerName() != pChar->GetCharName() )
			continue;

		if( m_DummyCharList[i]->GetDummyCharName() == szDummyCharName )
			List.push_back( m_DummyCharList[i] );
	}
}

int ioDummyCharManager::GetDummyCharCntToName( ioBaseChar *pChar, const ioHashString &szDummyCharName )
{
	if( !pChar ) 
		return 0;

	int nReturnCnt = 0;
	int iDummyCnt = m_DummyCharList.size();
	for( int i=0; i < iDummyCnt; i++ )
	{
		if( m_DummyCharList[i]->GetOwnerName() != pChar->GetCharName() )
			continue;

		if( m_DummyCharList[i]->GetDummyCharName() == szDummyCharName )
			nReturnCnt++;
	}

	return nReturnCnt;
}

void ioDummyCharManager::DestroyDummyCharList()
{
	m_DummyCharList.clear();
	m_pPlayStage->DestroyAllDummyChar();
}

void ioDummyCharManager::SetReserveAttackCode( ioBaseChar *pChar, int iCode )
{
	if( !pChar ) return;
	int iDummyCnt = m_DummyCharList.size();

	ioItem *pItem = pChar->GetEquipedItem( ES_WEAPON );
	iolupinItem* pLupin = ToLupinItem(pItem);
	bool bCheckLupinDummyAttackCode = false;
	if( pLupin )
		bCheckLupinDummyAttackCode = pLupin->CheckLupinDummy( iCode );

	for( int i=0; i < iDummyCnt; i++ )
	{
		if( !m_DummyCharList[i] )
			continue;

		if( m_DummyCharList[i]->GetType() == DCT_OWNER_CONTROL )
			continue;

		if( m_DummyCharList[i]->GetType() == DCT_OWNER_ATTACH2 )
			continue;

		if( m_DummyCharList[i]->GetType() == DCT_OWNER_ATTACH3 )
			continue;

		if( m_DummyCharList[i]->GetOwnerName() != pChar->GetCharName() )
			continue;

		m_DummyCharList[i]->SetReserveAttackCode( iCode );
	}

	if( bCheckLupinDummyAttackCode )
	{
		ioItem *pItem = pChar->GetEquipedItem( ES_WEAPON );
		iolupinItem* pLupin = ToLupinItem(pItem);
		if( pLupin )
			pLupin->CreateDummyImmediatelyAttack( pChar, iCode );
	}
}

int ioDummyCharManager::GetDummyCharCnt( ioBaseChar *pChar, bool bSync )
{
	if( !pChar ) return 0;

	int iOwnerCnt = 0;
	int iDummyCnt = m_DummyCharList.size();
	for( int i=0; i < iDummyCnt; i++ )
	{
		if( !m_DummyCharList[i] )
			continue;

		if( m_DummyCharList[i]->GetOwnerName() != pChar->GetCharName() )
			continue;

		if( bSync && m_DummyCharList[i]->GetDummyGrassType() != ioDummyChar::DGT_NONE )
			continue;

		if( ToForkcraneDummy( m_DummyCharList[i] ) )
			continue;

		iOwnerCnt++;
	}

	return iOwnerCnt;
}

void ioDummyCharManager::FillDummyCharInfo( ioBaseChar *pChar, SP2Packet &rkPacket )
{
	if( !pChar ) return;

	int iDummyCnt = m_DummyCharList.size();
	for( int i=0; i < iDummyCnt; i++ )
	{
		ioDummyChar *pDummy = m_DummyCharList[i];
		if( !pDummy )
			continue;

		if( pDummy->GetOwnerName() != pChar->GetCharName() )
			continue;

		if( pDummy->GetDummyGrassType() != ioDummyChar::DGT_NONE )
			continue;

		if( ToForkcraneDummy( pDummy ) )
			continue;

		// 이 더미는 셋팅에서 설정한 대로 동기화 시키지 않음
		if( !pDummy->CheckSyncForOtherUser() )
			continue;

		rkPacket << pDummy->GetDummyCharName();
		rkPacket << pDummy->GetDummyCharIdx();
		rkPacket << pDummy->IsMale();
		rkPacket << pDummy->GetHP().m_fMaxValue;
		rkPacket << pDummy->GetHP().m_fCurValue;
		rkPacket << pDummy->GetWorldPosition();
		//rkPacket << pDummy->GetStartAngle();
		rkPacket << pDummy->GetCurAngle();
	}
}

void ioDummyCharManager::FillDummyCharMidSync( ioBaseChar *pChar, SP2Packet &rkPacket )
{
	if( !pChar )
		return;

	std::vector<ioDummyChar*> vDummyList;
	for( int i = 0; i < (int)m_DummyCharList.size(); ++i )
	{
		ioDummyChar *pDummy = m_DummyCharList[i];
		if( !pDummy )
			continue;

		if( pDummy->GetOwnerName() != pChar->GetCharName() )
			continue;		

		if( !pDummy->IsDummyCharMidSync() )
			continue;

		vDummyList.push_back( pDummy );
	}

	rkPacket << (int)vDummyList.size();
	for( std::vector<ioDummyChar*>::iterator iter = vDummyList.begin(); iter != vDummyList.end(); ++iter )
	{
		ioDummyChar* pDummy = *iter;

		rkPacket << pDummy->GetDummyCharIdx();
		pDummy->FillDummyCharMidSync( pChar, rkPacket );
	}
}

void ioDummyCharManager::ApplyDummyCharMidSync( ioBaseChar *pChar, SP2Packet &rkPacket )
{
	int iSize = 0;
	rkPacket >> iSize;

	for( int i = 0; i < iSize; ++i )
	{
		int iDummyIndex = 0;
		rkPacket >> iDummyIndex;

		ioDummyChar* pDummy = FindDummyChar( pChar, iDummyIndex );
		if( pDummy )
			pDummy->ApplyDummyCharMidSync( pChar, rkPacket );
	}
}

void ioDummyCharManager::DestroyDummyCharByNameList( ioBaseChar *pChar, ioHashStringVec &rvList )
{
	if( !pChar ) return;

	int iDummyCnt = m_DummyCharList.size();
	for( int i=0; i < iDummyCnt; i++ )
	{
		ioDummyChar *pDummy = m_DummyCharList[i];
		if( !pDummy )
			continue;

		if( pDummy->GetOwnerName() != pChar->GetCharName() )
			continue;

		if( pDummy->GetState() == DCS_DIE )
			continue;

		int iCnt = rvList.size();
		for( int j=0; j < iCnt; ++j )
		{
			if( pDummy->GetDummyCharName() == rvList[j] )
			{
				pDummy->SetDieState( true );
			}
		}
	}
}


void ioDummyCharManager::DestroyDummyCharByName( ioBaseChar *pChar, const ioHashString &szName )
{
	if( !pChar ) return;

	int iDummyCnt = m_DummyCharList.size();
	for( int i=0; i < iDummyCnt; i++ )
	{
		ioDummyChar *pDummy = m_DummyCharList[i];
		if( !pDummy )
			continue;

		if( pDummy->GetOwnerName() != pChar->GetCharName() )
			continue;

		if( pDummy->GetState() == DCS_DIE )
			continue;

		if( pDummy->GetDummyCharName() == szName )
			pDummy->SetDieState( true );
	}
}


bool ioDummyCharManager::GeDisablePosition( D3DXVECTOR3 vPos, float fRadius )
{
	int iDummyCnt = m_DummyCharList.size();
	for( int i=0; i < iDummyCnt; i++ )
	{
		ioDummyChar *pDummy = m_DummyCharList[i];
		if( !pDummy )
			continue;
		
		D3DXVECTOR3 vPoint;
		if( pDummy->CheckSpherePoint( vPos, fRadius, vPoint ) )		
			return true;		
	}

	return false;
}

void ioDummyCharManager::CheckEntityCollision( ioBaseChar *pChar, ioPlayEntity *pEntity, int iIndex, bool bDefense )
{
	ioDummyChar *pDummy = FindDummyChar( pChar, iIndex );
	if( pDummy )
	{
		pDummy->CheckEntityCollision( pEntity, bDefense );
	}
}


bool ioDummyCharManager::IsNoOverLapTrackingDummy( const ioHashString& stAttacker, const ioHashString& stTarget )
{
	int nCnt = m_DummyNoOverLapTrackingList.size();
	if( stAttacker.IsEmpty() || stTarget.IsEmpty() )
	{
		assert( false );
		return false;
	}

	for( int i = 0; i < nCnt; i++ )
	{
		const StDummyNoOverLapTracking& pInfo = m_DummyNoOverLapTrackingList[i];
		if( pInfo.m_stCreaterName == stAttacker && pInfo.m_stTargetName == stTarget )
			return false;
	}

	return true;
}


bool ioDummyCharManager::AddNoOverLapTrackingDummy( const ioHashString& stAttacker, const ioHashString& stTarget )
{
	if( !IsNoOverLapTrackingDummy( stAttacker, stTarget ) )
		return false;

	StDummyNoOverLapTracking stInfo;
	stInfo.m_stCreaterName = stAttacker;
	stInfo.m_stTargetName = stTarget;

	m_DummyNoOverLapTrackingList.push_back( stInfo );

	return true;
}


bool ioDummyCharManager::EraseNoOverLapTrackingDummy( const ioHashString& stAttacker, const ioHashString& stTarget )
{
	int nCnt = m_DummyNoOverLapTrackingList.size();
	if( stAttacker.IsEmpty() || stTarget.IsEmpty() )
	{
		assert( false );
		return false;
	}

	for( int i = 0; i < nCnt; i++ )
	{
		const StDummyNoOverLapTracking& pInfo = m_DummyNoOverLapTrackingList[i];
		if( pInfo.m_stCreaterName == stAttacker && pInfo.m_stTargetName == stTarget )
		{
			m_DummyNoOverLapTrackingList.erase( m_DummyNoOverLapTrackingList.begin() + i );
			return true;
		}
	}

	assert( false );
	return false;
}

void ioDummyCharManager::ResetMoveState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	int iDummyCnt = m_DummyCharList.size();
	for( int i=0; i < iDummyCnt; i++ )
	{
		ioDummyChar *pDummy = m_DummyCharList[i];
		if( !pDummy )
			continue;

		if( pDummy->GetOwnerName() != pChar->GetCharName() )
			continue;

		ioRaptorDummy *pRaptor = ToRaptorDummy( pDummy );
		if( pRaptor )
		{
			pRaptor->SetMoveStateMid();
		}

		ioVehicleDummy *pVehicle = ToVehicleDummy( pDummy );
		if( pVehicle )
		{
			pVehicle->SetMoveStateMid();
		}
	}
}

void ioDummyCharManager::ResetDashState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	int iDummyCnt = m_DummyCharList.size();
	for( int i=0; i < iDummyCnt; i++ )
	{
		ioDummyChar *pDummy = m_DummyCharList[i];
		if( !pDummy )
			continue;

		if( pDummy->GetOwnerName() != pChar->GetCharName() )
			continue;

		ioRaptorDummy *pRaptor = ToRaptorDummy( pDummy );
		if( pRaptor )
		{
			pRaptor->SetDashStateMid();
		}
	}
}


bool ioDummyCharManager::GetDummyCharBaseInfo( IN const ioHashString &szName, OUT DummyCharBaseInfo &rkInfo )
{
	rkInfo.Init();

	if( m_DummyCharBaseInfoList.empty() )
		return false;

	int iInfoSize = m_DummyCharBaseInfoList.size();
	for( int i=0; i < iInfoSize; ++i )
	{
		if( m_DummyCharBaseInfoList[i].m_DummyCharName == szName )
		{
			rkInfo = m_DummyCharBaseInfoList[i];
			return true;
		}
	}

	return false;
}

int ioDummyCharManager::GetCurDummyCharCnt( ioBaseChar *pOwner, ioHashString szDummyName )
{
	if ( !pOwner || szDummyName.IsEmpty() )
		return 0;

	int nCnt = 0;

	int iDummyCnt = m_DummyCharList.size();
	for( int i=0; i < iDummyCnt; i++ )
	{
		if( m_DummyCharList[i]->GetOwnerName() != pOwner->GetCharName() )
			continue;

		if( m_DummyCharList[i]->GetDummyCharName() == szDummyName )
			nCnt++;
	}

	return nCnt;
}

bool ioDummyCharManager::CheckMaxCreateDummyCnt( ioBaseChar *pOwner, ioDummyChar *pDummy, int nMaxCnt )
{
	//최대 갯수가 0이면 종료
	if( nMaxCnt <= 0 || !pOwner || !pDummy )
		return false;
	
	//더미 마인일 경우에만 연산
	ioDummyMine *pDummyMine = ToDummyMine( pDummy );
	if( pDummyMine )
	{
		//더미 임시 리스트
		typedef std::vector< ioDummyMine* > DummyList;
		DummyList vDummyList;

		int nDummyCnt = m_DummyCharList.size();
		for( int i=0; i < nDummyCnt; i++ )
		{
			//주인 이름이 같으면서
			if( m_DummyCharList[i]->GetOwnerName() != pOwner->GetCharName() )
				continue;

			//더미 마인인 경우 추가
			ioDummyMine *pNewDummyMine = ToDummyMine( m_DummyCharList[i] );
			if ( pNewDummyMine )
				vDummyList.push_back( pNewDummyMine );
		}		

		//비어 있다면 종료
		if ( vDummyList.empty() )
			return false;

		//개수 확인
		int nMineCnt = vDummyList.size();
		if( nMineCnt <= nMaxCnt )
			return true;

		//최대 개수가 넘어 갔을 경우
		ioDummyMine *pDelDummyMine = NULL;
		DWORD dwCreateTime = -1;	// 오버플로우를 시켜서 최대값으로 세팅
		for( int i=0 ; i<nMineCnt ; i++ )
		{
			ioDummyMine *pCurDummy = vDummyList[i];

			if( dwCreateTime > pCurDummy->GetCreateTime() )
			{
				dwCreateTime = pCurDummy->GetCreateTime();
				pDelDummyMine = pCurDummy;
			}
		}

		if( pDelDummyMine )
			pDelDummyMine->SetExplosionState();

		return true;
	}

	return false;
}


void ioDummyCharManager::ClearMapDummy()
{
	DummyCharList::iterator iter = m_DummyCharList.begin();
	while( iter != m_DummyCharList.end() )
	{
		ioDummyChar *pDummy = (ioDummyChar*)(*iter);
		if( !pDummy )
		{
			++iter;
			continue;
		}

		if( pDummy->GetDummyGrassType() != ioDummyChar::DGT_NONE )
		{
			pDummy->SetDieState(false);
			//m_pPlayStage->EraseGameEntity( iter );
			//iter = m_DummyCharList.erase( iter );
		}
		else
			++iter;
	}
}

int ioDummyCharManager::GetDummyGroupCount( const ioHashString& szOwnerName )
{
	int iCount = 0;
	for( ioDummyGroupList::iterator iter = m_DummyGroupList.begin(); iter != m_DummyGroupList.end(); ++iter )
	{
		ioDummyGroup& Group = *iter;
		if( Group.GetOnwerName() == szOwnerName )
		{
			iCount++;
		}
	}

	return iCount;
}

ioDummyGroup* ioDummyCharManager::GetDummyGroup( const ioHashString& szOwnerName, const ioHashString& szIdentityName )
{
	for( ioDummyGroupList::iterator iter = m_DummyGroupList.begin(); iter != m_DummyGroupList.end(); ++iter )
	{
		ioDummyGroup& Group = *iter;
		if( Group.GetOnwerName() == szOwnerName && Group.GetIdentityName() == szIdentityName )
		{
			return &Group;
		}
	}

	return NULL;
}

ioDummyGroup* ioDummyCharManager::AddDummyGroup( const ioHashString& szOwnerName, const ioHashString& szIdentityName )
{
	ioDummyGroup* pGroup = GetDummyGroup( szOwnerName, szIdentityName );
	if( !pGroup )
	{
		ioDummyGroup Group( m_pPlayStage, szOwnerName, szIdentityName );
		m_DummyGroupList.push_back( Group );
		return &m_DummyGroupList.back();
	}

	return pGroup;
}

void ioDummyCharManager::ProcessCheckDummyGroup()
{
	for( ioDummyGroupList::iterator iter = m_DummyGroupList.begin(); iter != m_DummyGroupList.end(); ++iter )
	{
		ioDummyGroup& Group = *iter;
		Group.CheckDieDummy();
	}
}

void ioDummyCharManager::FillDummyGroupInfo( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	rkPacket << (int)m_DummyGroupList.size();
	for( ioDummyGroupList::iterator iter = m_DummyGroupList.begin(); iter != m_DummyGroupList.end(); ++iter )
	{
		ioDummyGroup& Group = *iter;
		if( Group.GetOnwerName() != pOwner->GetCharName() )
			continue;

		rkPacket << Group.m_IdentityName;
		int iSize = Group.m_DummyList.size();
		rkPacket << iSize;
		for( int i = 0; i < iSize; ++i )
		{
			rkPacket << (int)Group.m_DummyList[i];
		}
	}
}

void ioDummyCharManager::ApplyDummyGroupInfo( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iGroupCount = 0;
	rkPacket >> iGroupCount;

	for( int iGroup = 0; iGroup < iGroupCount; ++iGroup )
	{
		ioHashString szIdentityName;
		rkPacket >> szIdentityName;

		ioDummyGroup* pGroup = AddDummyGroup( pOwner->GetCharName(), szIdentityName );
		int iCount = 0;
		rkPacket >> iCount;
		for( int i = 0; i < iCount; ++i )
		{
			int iDummyCharIndex = 0;
			rkPacket >> iDummyCharIndex;
			pGroup->AddDummy( iDummyCharIndex );
		}
	}
}

bool ioDummyCharManager::IsDropZoneCollision( ioBaseChar *pChar )
{
	if( !pChar ) return false;

	int iDummyCnt = m_DummyCharList.size();
	for( int i=0; i < iDummyCnt; i++ )
	{
		ioDummyChar *pDummy = m_DummyCharList[i];
		if( !pDummy )
			continue;

		if( pDummy->GetOwnerName() != pChar->GetCharName() )
			continue;

		if( pDummy->GetWorldPosition().y >= 0.1f )
			continue;
		
		return true;
	}
	return false;
}