

#include "stdafx.h"

#include "ioBuffMaker.h"

#include "BuffDefine.h"

template<> ioBuffMaker* Singleton< ioBuffMaker >::ms_Singleton = 0;

ioBuffMaker::ioBuffMaker()
{
	m_BuffMap.clear();
}

ioBuffMaker::~ioBuffMaker()
{
	ClearBuffList();
}

void ioBuffMaker::ClearBuffList()
{
	BuffMap::iterator iter;
	for( iter=m_BuffMap.begin() ; iter!=m_BuffMap.end() ; ++iter )
	{
		SAFEDELETE(iter->second);
	}
	m_BuffMap.clear();
}

void ioBuffMaker::LoadBuffList( const char *szFileName )
{
	ioINILoader kLoader( szFileName );

	char szBuf[MAX_PATH] = "";
	char szTitle[MAX_PATH] = "";
	char szBuffType[MAX_PATH] = "";
	char szGroupName[MAX_PATH] = "";

	kLoader.SetTitle_e( "common_info" );
	int iGroupCnt = kLoader.LoadInt_e( "buff_group_cnt", 0 );

	int i=0;
	for( i=0 ; i<iGroupCnt ; i++ )
	{
		// 그룹 호출
		wsprintf_e( szTitle, "buff_group%d", i );
		kLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		wsprintf_e( szGroupName, "config/%s", szBuf );

		wsprintf_e( szBuffType, "buff_group%d_type", i );
		int iBuffCategory = kLoader.LoadInt( szBuffType, 0 );

		ioINILoader kSetBuffLoader( szGroupName );
		kSetBuffLoader.SetTitle_e( "common" );

		int iBuffCnt = kSetBuffLoader.LoadInt_e( "buff_cnt", 0 );
		for( int j=0; j < iBuffCnt; ++j )
		{
			// 그룹에 속한 버프 추가
			wsprintf_e( szTitle, "buff%d", j+1 );
			kSetBuffLoader.SetTitle( szTitle );

			int iTypeID = kSetBuffLoader.LoadInt_e( "type", 0 );

			ioBuff *pBuff = CreateBuffTemplete( iTypeID, iBuffCategory );
			if( pBuff )
			{
				pBuff->SetGroupName( szGroupName );
				pBuff->SetTitleName( szTitle );
				pBuff->LoadProperty( kSetBuffLoader );

				m_BuffMap.insert( BuffMap::value_type( pBuff->GetName(), pBuff ) );
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "ioBuffMaker::LoadBuffList - %d : %s Unknown TypeID, %s", iTypeID, szTitle, szGroupName );
			}
		}
	}
}

ioBuff* ioBuffMaker::CreateBuff( const ioHashString &rkName )
{
	BuffMap::iterator iter = m_BuffMap.find( rkName );
	if( iter != m_BuffMap.end() )
	{
		return iter->second->Clone();
	}

	LOG.PrintTimeAndLog( 0, "ioBuffMaker::CreatBuff - %s Not Exist Buff", rkName.c_str() );

	/*LOG.PrintTimeAndLog( 0, "ioBuffMaker::CreatBuff - (%d - %d) - (%d - %d)",
		ErrorReport::GetMajorPosition(),	
		ErrorReport::GetMinorPosition(),
		ErrorReport::GetEngineMajorPos(),
		ErrorReport::GetEngineMinorPos() );*/

	return NULL;
}

int ioBuffMaker::GetBuffType( const ioHashString &rkName ) const
{
	const ioBuff *pBuff = GetBuffConst( rkName );
	if( pBuff )
		return pBuff->GetTypeID();

	return BT_NONE;
}

const ioBuff* ioBuffMaker::GetBuffConst( const ioHashString &rkName ) const
{
	BuffMap::const_iterator iter = m_BuffMap.find( rkName );
	if( iter!=m_BuffMap.end() )
	{
		return iter->second;
	}

	LOG.PrintTimeAndLog( 0, "ioBuffMaker::GetBuffConst - %s Not Exist Buff", rkName.c_str() );
	return NULL;
}

ioBuff* ioBuffMaker::CreateBuffTemplete( int iTypeID, int iBuffCategory )
{
	if ( iBuffCategory == BC_NORMAL )
		return CreateNormalBuffTemplete( iTypeID, iBuffCategory );
	else if ( iBuffCategory == BC_ACCESOORY )
		return CreateAccessoryBuffTemplete( iTypeID, iBuffCategory );
	else
	{
		LOG.PrintTimeAndLog( 0, "%s - Buff Category Wrong Num : %d", __FUNCTION__, iBuffCategory );
		return NULL;
	}
}

ioBuffMaker& ioBuffMaker::GetSingleton()
{
	return Singleton< ioBuffMaker >::GetSingleton();
}

ioBuff* ioBuffMaker::CreateNormalBuffTemplete( int iTypeID, int iBuffCategory )
{
	ioBuff *pBuff = NULL;

	switch( iTypeID )
	{
	case BT_DAMAGE_UPDOWN:
		pBuff = new ioDamageUpDownBuff;
		break;
	case BT_SPEED_UPDOWN:
		pBuff = new ioSpeedUpDownBuff;
		break;
	case BT_WOUNDED_UPDOWN:
		pBuff = new ioWoundedUpDownBuff;
		break;
	case BT_RECOVERY:
		pBuff = new ioRecoveryBuff;
		break;
	case BT_RESTORATION:
		pBuff = new ioRestorationBuff;
		break;
	case BT_FLYING_CONTRAL:
		pBuff = new ioFlyBuff;
		break;
	case BT_FLAME_STATUE:
		pBuff = new ioBurnBuff;
		break;
	case BT_FROZEN_STATUE:
		pBuff = new ioFrozenBuff;
		break;
	case BT_STUN_STATUE:
		pBuff = new ioStunBuff;
		break;
	case BT_TOXIC_STATUE:
		pBuff = new ioPoisoningBuff;
		break;
	case BT_STAT_ENHANCE:
		pBuff = new ioStatEnhanceBuff;
		break;
	case BT_PROTECTION:
		pBuff = new ioProtectionBuff;
		break;
	case BT_PUSH_BACK:
		pBuff = new ioPushBackBuff;
		break;
	case BT_DURATION_SCALE:
		pBuff = new ioDurationScaleBuff;
		break;
	case BT_POTION:
		pBuff = new ioPotionBuff;
		break;
	case BT_REFLECT:
		pBuff = new ioReflectBuff;
		break;
	case BT_KING_SCALE:
		pBuff = new ioKingScaleBuff;
		break;
	case BT_RECOVERY_GAUGE:
		pBuff = new ioRecoveryGaugeBuff;
		break;
	case BT_RESTORATION_GAUGE:
		pBuff = new ioRestorationGaugeBuff;
		break;
	case BT_CAMERA_BUFF:
		pBuff = new ioCameraBuff;
		break;
	case BT_CATCH_BUFF:
		pBuff = new ioCatchBuff;
		break;
	case BT_STEALTH:
		pBuff = new ioStealthBuff;
		break;
	case BT_HIDE:
		pBuff = new ioHideBuff;
		break;
	case BT_WEAK_BOMB:
		pBuff = new ioWeakBombBuff;
		break;
	case BT_WEAK_MAGIC:
		pBuff = new ioWeakMagicBuff;
		break;
	case BT_GAUGE_CLEAR:
		pBuff = new ioGaugeClearBuff;
		break;
	case BT_FEAR:
		pBuff = new ioFearBuff;
		break;
	case BT_DOUBLE_JUMP:
		pBuff = new ioDoubleJumpBuff;
		break;
	case BT_ABSOLUTE_PROTECTION:
		pBuff = new ioAbsoluteProtectionBuff;
		break;
	case BT_ELETRIC:
		pBuff = new ioEletricBuff;
		break;
	case BT_MANA_SHIELD:
		pBuff = new ioManaShieldBuff;
		break;
	case BT_WARP:
		pBuff = new ioWarpStateBuff;
		break;
	case BT_CLOSEORDER:
		pBuff = new ioCloseOrderBuff;
		break;
	case BT_STATE_PROTECTION:
		pBuff = new ioStateProtectionBuff;
		break;
	case BT_FLOAT_BUFF:
		pBuff = new ioFloatBuff;
		break;
	case BT_JUMP_UPDOWN:
		pBuff = new ioJumpPowerRateBuff;
		break;
	case BT_LANDING_RATE:
		pBuff = new ioLandingRateBuff;
		break;
	case BT_SCREEN_BLIND:
		pBuff = new ioScreenBlindBuff;
		break;
	case BT_PUSH_BLOW_ENHANCE:
		pBuff = new ioPushBlowEnhanceBuff;
		break;
	case BT_BREAK_FALL:
		pBuff = new ioBreakFallBuff;
		break;
	case BT_BLOW_PUSH_POWER_BUFF:
		pBuff = new ioBlowPushPowerBuff;
		break;
	case BT_HP_CONVERT:
		pBuff = new ioHPConvertBuff;
		break;
	case BT_STOP_MOTION:
		pBuff = new ioStopMotionBuff;
		break;
	case BT_DAMAGE_CONVERT:
		pBuff = new ioDamageConvertBuff;
		break;
	case BT_SYSTEM_DAMAGE_RATE:
		pBuff = new ioSystemDamageRateBuff;
		break;
	case BT_REVERSE_DIRECTION:
		pBuff = new ioReverseDirectionBuff;
		break;
	case BT_HP_STEAL:
		pBuff = new ioHPStealBuff;
		break;
	case BT_HEAD_SCALE:
		pBuff = new ioHeadScaleBuff;
		break;
	case BT_NOWOUND:
		pBuff = new ioNoWoundBuff;
		break;
	case BT_DISABLE_DASH:
		pBuff = new ioDisableDashBuff;
		break;
	case BT_CHANGE_MOTION:
		pBuff = new ioChangeMotionBuff;
		break;
	case BT_GAUGE_STEAL:
		pBuff = new ioGaugeStealBuff;
		break;
	case BT_RETURN_ATTACK:
		pBuff = new ioReturnAttackBuff;
		break;
	case BT_FLOAT_MODE_EVENT_BUFF:
		pBuff = new ioFloatModeEventBuff;
		break;
	case BT_ACCUMULATE:
		pBuff = new ioAccumulateBuff;
		break;
	case BT_LIMIT_MOVE:
		pBuff = new ioLimitMoveBuff;
		break;
	case BT_BUFF_FLY_JUMP:
		pBuff = new ioFlyJumpBuff;
		break;
	case BT_DOWN_JUMPPING_BUFF:
		pBuff = new ioDownJumppingBuff;
		break;
	case BT_DEFENSE_BREAK_BUFF:
		pBuff = new ioDefenseBreakBuff;
		break;
	case BT_SELECT_COLLISION_BUFF:
		pBuff = new ioSelectCollisionBuff;
		break;
	case BT_RANGE_PROTECT_BUFF:
		pBuff = new ioRangeProtectionBuff;
		break;
	case BT_LIMIT_ACTION:
		pBuff = new ioLimitActionBuff;
		break;
	case BT_CURSE_BUFF:
		pBuff = new ioCurseBuff;
		break;
	case BT_DOWN_ROLLING_BUFF:
		pBuff = new ioDownRollingBuff;
		break;
	case BT_FRICTION_UP_DOWN:
		pBuff = new ioFrictionUpDownBuff;
		break;
	case BT_FRONT_BACK_WOUND:
		pBuff = new ioFrontBackWoundBuff;
		break;
	case BT_LAND_STUN:
		pBuff = new ioLandStunBuff;
		break;
	case BT_AIR_ATTACK:
		pBuff = new ioAirAttackBuff;
		break;
	case BT_LAND_ROLLING_BUFF:
		pBuff = new ioLandRollingBuff;
		break;
	case BT_NOT_MOVE_PROTECT:
		pBuff = new ioNotMoveProtectBuff;
		break;
	case BT_ICE_BUFF:
		pBuff = new ioIceBuff;
		break;
	case BT_DUMMY_BUFF:
		pBuff = new ioDummyBuff;
		break;
	case BT_SKIP_STUN:
		pBuff = new ioSkipStunBuff;
		break;
	case BT_WOUND_COLOR_CHANGE:
		pBuff = new ioWoundColorChangeBuff;
		break;
	case BT_CHANGE_MOVE_SPEED:
		pBuff = new ioChangeMoveSpeedBuff;
		break;
	case BT_STOP_PASSIVE:
		pBuff = new ioStopPassiveBuff;
		break;
	case BT_ADD_SPECIAL_GAUGE:
		pBuff = new ioSpecialGaugeBuff;
		break;
	case BT_REFUD_GAUGE:
		pBuff = new ioRefudBuff;
		break;
	case BT_PREVENT_DROP_ITEM:
		pBuff = new ioPreventDropItemBuff;
		break;
	case BT_CHANGE_SKIN_COLOR:
		pBuff = new ioChangeSkinColorBuff;
		break;
	case BT_ENABLE_WOUND_STUN_STATUE:
		pBuff = new ioEnableWoundStunBuff;
		break;
	case BT_PULL_BUFF:
		pBuff = new ioPullBuff;
		break;
	case BT_CHANGE_SOLDIER:
		pBuff = new ioChangeSoldierBuff;
		break;
	case BT_GHOST_STATE:
		pBuff = new ioGhostStateBuff;
		break;
	case BT_CALL_BY_STATE:
		pBuff = new ioCallByStateBuff;
		break;
	case BT_WITCH_FLY_STATE:
		pBuff = new ioWitchFlyBuff;
		break;
	case BT_RANDOM_MOVE:
		pBuff = new ioRandomMoveBuff;
		break;
	case BT_TELEPORT_ON_END_BUFF:
		pBuff = new ioTeleportOnEndBuff;
		break;
	case BT_AUTO_FIRE:
		pBuff = new ioAutoFireBuff;
		break;
	case BT_CHANGE_MESH:
		pBuff = new ioChangeMeshBuff;
		break;
	case BT_DEFENSE_DAMAGE:
		pBuff = new ioDefenseDamageBuff;
		break;
	case BT_ADHESIVE_STATE:
		pBuff = new ioAdhesiveBuff;
		break;
	case BT_SEAL_SKILL:
		pBuff = new ioSealSkillBuff;
		break;
	case BT_CALL_BUFF_ON_ATTACK:
		pBuff = new ioCallBuffOnAttackBuff;
		break;
	case BT_MAGIC_STUN:
		pBuff = new ioMagicStunBuff;
		break;
	case BT_SPEED_ACCUMLATE:
		pBuff = new ioSpeedAccumlateBuff;
		break;
	case BT_CONTROL_WEAPON:
		pBuff = new ioControlWeaponBuff;
		break;
	case BT_WALL_COLLISION:
		pBuff = new ioWallCollisionBuff;
		break;
	case BT_DEFENSE_DAMAGE2:
		pBuff = new ioDefenseDamageBuff2;
		break;
	case BT_ACTIVE_COUNT:
		pBuff = new ioActiveCountBuff;
		break;
	case BT_INFECTION:
		pBuff = new ioInfectionBuff;
		break;
	case BT_TARGET_EQUIP_OBJECTITEM:
		pBuff = new ioTargetEquipObjectItemBuff;
		break;
	case BT_CHANGE_TEAM:
		pBuff = new ioChangeTeamBuff;
		break;
	case BT_SCREEN_BLIND2:
		pBuff = new ioScreenBlindBuff2;
		break;
	case BT_TRACE_MINE_STATE:
		pBuff = new ioTraceMineStateBuff;
		break;
	case BT_GAUGE_STEAL2:
		pBuff = new ioGaugeStealBuff2;
		break;
	case BT_RESTORATION_GAUGE2:
		pBuff = new ioRestorationGaugeBuff2;
		break;
	case BT_DISABLE_RECOVERY_SKILL_GAUGE:
		pBuff = new ioDisableRecoverySkillGaugeBuff;
		break;
	case BT_HIDE2:
		pBuff = new ioHide2Buff;
		break;
	case BT_HIT_COUNT_HOLD:
		pBuff = new ioHitCountHoldBuff;
		break;
	case BT_CREATE_WEAPON:
		pBuff = new ioCreateWeaponBuff;
		break;
	case BT_AFFECT_WOUNDED_VALUE:
		pBuff = new ioAffectWoundedValueBuff;
		break;
	case BT_EXTRA_CALL:
		pBuff = new ioExtraCallBuff;
		break;
	case BT_CALL_BY_NPC:
		pBuff = new ioCallByNpcBuff;
		break;
	case BT_CREATE_DUMMY_CHAR:
		pBuff = new ioCreateDummyCharBuff;
		break;
	case BT_GRAPPLING_PUSH:
		pBuff = new ioGrapplingPushBuff;
		break;
	case BT_MAGNETIC:
		pBuff = new ioMagneticBuff;
		break;
	case BT_CREATE_DUMMY_CHAR2:
		pBuff = new ioCreateDummyChar2Buff;
		break;
	case BT_WEAPON_ATTACH:
		pBuff = new ioWeaponAttachBuff;
		break;
	case BT_CREATE_DUMMY_CHAR3:
		pBuff = new ioCreateDummyChar3Buff;
		break;
	case BT_POSITION:
		pBuff = new ioPositionBuff;
		break;
	case BT_GARAVITY_UP_DOWN:
		pBuff = new ioGravityUpDownBuff;
		break;
	case BT_ESCAPE_DROP_ZONE:
		pBuff = new ioEscapeDropZoneBuff;
		break;
	case BT_PUSH_DIRECTION:
		pBuff = new ioPushDirectionBuff;
		break;
	case BT_WINDLORD:
		pBuff = new ioWindLordBuff;
		break;
	case BT_CALL_RANDOM:
		pBuff = new ioCallRandomBuff;
		break;
	case BT_POSITION2:
		pBuff = new ioPositionBuff2;
		break;
	case BT_CHECK_LIFE:
		pBuff = new ioCheckLifeBuff;
		break;
	case BT_BUFF_MULTI_FLY_JUMP:
		pBuff = new ioMultiFlyJumpBuff;
		break;
	case BT_SPECIAL_GAUGE_RATE:
		pBuff = new ioSpecialGaugeBuff2;
		break;
	case BT_WIND_MAGNETIC:
		pBuff = new ioWindMagneticBuff;
		break;
	case BT_DROP_ZONE:
		pBuff = new ioDropZoneBuff;
		break;
	case BT_DISPEL_BUFF:
		pBuff = new ioDispelBuff;
		break;
	case BT_TRY_OUT_CHHECK:
		pBuff = new ioTryOutCheckBuff;
		break;
	case BT_TRY_OUT_CHHECK2:
		pBuff = new ioTryOutCheckBuff2;
		break;
	case BT_CONCENTRATE_TARGET:
		pBuff = new ioConcentrateTargetBuff;
		break;
	case BT_CREATE_DUMMY_CHAR4:
		pBuff = new ioCreateDummyChar4Buff;
		break;
	case BT_TARGET_CONTROL:
		pBuff = new ioTargetControlBuff;
		break;
	case BT_CREATE_BUFF:
		pBuff = new ioCreateBuff;
		break;
	case BT_SKILL_GAUGE_CHECK:
		pBuff = new ioSkillGaugeCheckBuff;
		break;
	case BT_REFLECT_DAMAGE:
		pBuff = new ioReflectDamageBuff;
		break;
	case BT_ALPHA_RATE:
		pBuff = new ioAlphaRateBuff;
		break;
	case BT_CONTROL_WEAPON2:
		pBuff = new ioControlWeaponBuff2;
		break;
	case BT_CALL_DESTROY:
		pBuff = new ioDestroyCallBuff;
		break;
	case BT_CREATE_WEAPON_COUNT:
		pBuff = new ioCreateWeaponCountBuff;
		break;
	case BT_ROTATE_WEAPON:
		pBuff = new ioRotateWeaponBuff;
		break;
	case BT_WEAK_AIRBOMB:
		pBuff = new ioWeakAirBombBuff;
		break;
	case BT_DUMMY_ATTACH:
		pBuff = new ioDummyAttachBuff;
		break;
	case BT_CREATE_DUMMY_CHAR5:
		pBuff = new ioCreateDummyChar5Buff;
		break;
	case BT_LOOK_WEAPON:
		pBuff = new ioLookatWeaponBuff;
		break;
	case BT_BUFF_FLY_JUMP2:
		pBuff = new ioFlyJumpBuff2;
		break;
	case BT_TARGET_WEAPON:
		pBuff = new ioTargetCreateWeaponBuff;
		break;
	case BT_LOOK_DUMMY:
		pBuff = new ioLookatDummyBuff;
		break;
	case BT_CHECK_CHAIN:
		pBuff = new ioCheckChainBuff;
		break;
	case BT_STATE_PROTECTION_BY_PASSIVE:
		pBuff = new ioStateProtectionByPassiveBuff;
		break;
	case BT_TARGET_MOVE:
		pBuff = new ioTargetMoveBuff;
		break;
	case BT_SELF_CONTROL:
		pBuff = new ioSelfControlBuff;
		break;
	case BT_TARGET_DUMMY:
		pBuff = new ioTargetDummyBuff;
		break;
	case BT_ACTIVE_COUNT2:
		pBuff = new ioActiveCountBuff2;
		break;
	case BT_CREATE_DUMMY_CHAR6:
		pBuff = new ioCreateDummyChar6Buff;
		break;
	case BT_AIR_BUFF:
		pBuff = new ioAirBuff;
		break;
	case BT_HIDE3:
		pBuff = new ioHide3Buff;
		break;
	case BT_WARP_FIND_DUMMY:
		pBuff = new ioWarpFindDummyBuff;
		break;
	case BT_MARK_BUFF:
		pBuff = new ioMarkBuff;
		break;
	case BT_DUMMY_ATTACH2:
		pBuff = new ioDummyAttachBuff2;
		break;
	case BT_CHECK_LIFE2:
		pBuff = new ioCheckLifeBuff2;
		break;
	case BT_CREATE_OBJECT_ITEM:
		pBuff = new ioCreateObjectItemBuff;
		break;
	case BT_DUMMY_ATTACH3:
		pBuff = new ioDummyAttachBuff3;
		break;
	case BT_CHECK_JUMP:
		pBuff = new ioCheckJumpBuff;
		break;
	case BT_HELICOPTER_ATTACH:
		pBuff = new ioHelipCopterAttachBuff;
		break;
	case BT_FLOAT2:
		pBuff = new ioFloatBuff2;
		break;
	case BT_TARGET_WEAPON2:
		pBuff = new ioTargetCreateWeaponBuff2;
		break;
	case BT_HIDE4:
		pBuff = new ioHide4Buff;
		break;
	case BT_TARGET_RUSH_ATTACK:
		pBuff = new ioTargetRushAttackBuff;
		break;
	case BT_THUNDER_BIRD_HOLD:
		pBuff = new ioThunderBirdHoldBuff;
		break;
	case BT_CHECK_STATE_END_ATTACK:
		pBuff = new ioCheckStateEndAttackBuff;
		break;
	case BT_CHECK_STATE_END_CALL_BUFF:
		pBuff = new ioCheckStateEndCallBuff;
		break;		
	case BT_MAP_EFFECT_BUFF:
		pBuff = new ioMapEffectBuff;
		break;
	case BT_CREATE_DUMMY_BY_ATTACKED_BUFF:
		pBuff = new ioCreateDummyByAttackedBuff;
		break;
	case BT_MOVE_ATTACK:
		pBuff = new ioMoveAttackBuff;
		break;
	case BT_DUMMY_CHAIN:
		pBuff = new ioDummyChainBuff;
		break;
	case BT_CREATE_DUMMY_CHAR7:
		pBuff = new ioCreateDummyChar7Buff;
		break;
	case BT_CHECK_CHAIN2:
		pBuff = new ioCheckChainBuff2;
		break;
	case BT_PASSENGER_BUFF:
		pBuff = new ioPassengerBuff;
		break;
	case BT_CREATE_DUMMY_CHAR8:
		pBuff = new ioCreateDummyChar8Buff;
		break;
	case BT_FLY_JUMP_EXTRA_ATTACK:
		pBuff = new ioFlyJumpExtraAttackBuff;
		break;
	case BT_DUMMY_CHECK_BUFF:
		pBuff = new ioDummyCheckBuff;
		break;
	case BT_CREATE_WEAPON_BUFF2:
		pBuff = new ioCreateWeaponBuff2;
		break;
	case BT_TARGET_MOVE_ATTACK:
		pBuff = new ioTargetMoveAttackBuff;
		break;
	case BT_WOUNDED_ADD_BUFF:
		pBuff = new ioWoundedAddBuff;
		break;
	case BT_CREATE_WEAPON_BUFF3:
		pBuff = new ioCreateWeaponBuff3;
		break;
	case BT_ATTACHED_DUMMY:
		pBuff = new ioAttachedDummyBuff;
		break;
	case BT_ATTACK_COUNT_BUFF:
		pBuff = new ioAttackCountBuff;
		break;
	case BT_CREATE_DUMMY_CHAR9:
		pBuff = new ioCreateDummyChar9Buff;
		break;
	case BT_HIT_COUNT_HOLD2:
		pBuff = new ioHitCountHoldBuff2;
		break;
	case BT_CHECK_TARGET:
		pBuff = new ioCheckTargetBuff;
		break;
	case BT_ATTACHED_CREATOR_CHAR:
		pBuff = new ioAttachedCreatorCharBuff;
		break;
	case BT_NOVICE_GRADE_EFFECT:
		pBuff = new ioNoviceGradeEffectBuff;
		break;
	case BT_CALL_BUFF_ON_ATTACK2:
		pBuff = new ioCallBuffOnAttackBuff2;
		break;
	case BT_DEFENCE_CHANGE_BUFF:
		pBuff = new ioDefenceChangeBuff;
		break;
	case BT_COUNT_GATHER_ATTACK:
		pBuff = new ioCountGatherAttackBuff;
		break;
	case BT_KEY_MARK_ATTACK:
		pBuff = new ioKeyMarkAttackBuff;
		break;
	case BT_BUFF_JUMP_ATTACK:
		pBuff = new ioJumpAttackBuff;
		break;
	case BT_CALL_AND_DIE:
		pBuff = new ioCallAndDieBuff;
		break;
	case BT_ACTIVE_COUNT3:
		pBuff = new ioActiveCountBuff3;
		break;
	case BT_ATTACHED_DUMMY2:
		pBuff = new ioAttachedDummyBuff2;
		break;
	case BT_CREATE_AREA_WEAPON:
		pBuff = new ioCreateAreaWeaponBuff;
		break;
	case BT_MOVE_ATTACK2:
		pBuff = new ioMoveAttackBuff2;
		break;
	case BT_TARGET_WEAPON3:
		pBuff = new ioTargetCreateWeaponBuff3;
		break;
	case BT_SKILL_ATTACK_BUFF:
		pBuff = new ioSkillAttackBuff;
		break;
	case BT_WEAPON_FOLLOWED_OWNER:
		pBuff = new ioWeaponFollowedOwnerBuff;
		break;
	case BT_TARGET_WEAPON4:
		pBuff = new ioTargetCreateWeaponBuff4;
		break;
	case BT_FLY_FOLLOW_STATE:
		pBuff = new ioFlyFollowStateBuff;
		break;
	case BT_FLY_FOLLOW_STATE2:
		pBuff = new ioFlyFollowStateBuff2;
		break;
	case BT_MOVE_AREA_WEAPON:
		pBuff = new ioMoveAreaWeaponBuff;
		break;
	case BT_CREATE_WEAPON_BUFF4:
		pBuff = new ioCreateWeaponBuff4;
		break;
	case BT_CREATE_DUMMY_BY_ATTACKED_BUFF2:
		pBuff = new ioCreateDummyByAttackedBuff2;
		break;
	case BT_TRAP_EYE_BUFF:
		pBuff = new ioTrapEyeBuff;
		break;
	case BT_BUFF_AIR_WALK:
		pBuff = new ioAirWalkBuff;
		break;
	case BT_BLOW_PUSH_POWER_BUFF2:
		pBuff = new ioBlowPushPowerBuff2;
		break;
	case BT_WARP_BUFF:
		pBuff = new ioWarpBuff;
		break;
	case BT_CREATE_WEAPON_BUFF5:
		pBuff = new ioCreateWeaponBuff5;
		break;
	case BT_WEAPON_IGNORE_BUFF:
		pBuff = new ioWeaponIgnoreBuff;
		break;
	case BT_WEAPON_REVERSE_IGNORE_BUFF:
		pBuff = new ioWeaponReverseIgnoreBuff;
		break;
	case BT_KEY_TELEPORT_ATTACK:
		pBuff = new ioKeyTeleportAttackBuff;
		break;
	case BT_TARGET_TELEPORT_ATTACK_BUFF:
		pBuff = new ioTargetTeleportAttackBuff;
		break;
	case BT_JUMP_BOOST_BUFF:
		pBuff = new ioJumpBoostBuff;
		break;
	case BT_STEAL_EQUIPMENT_BUFF:
		pBuff = new ioStealEquipmentBuff;
		break;
	case BT_ATTACK_KEY_ESCAPE_BUFF:
		pBuff = new ioAttackKeyEscapeBuff;
		break;
	case BT_ATTACK_RANGE_CHECK_BUFF:
		pBuff = new ioAttackRangeCheckBuff;
		break;
	case BT_SUB_WEAPON_ATTACK_BUFF:
		pBuff = new ioSubWeaponAttackBuff;
		break;
	case BT_EXCEPT_GROWTH_BUFF:
		pBuff = new ioExceptGrowthBuff;
		break;
	case BT_RANGE_FEAR_BUFF:
		pBuff = new ioRangeFearBuff;
		break;
	case BT_MAGNETIC_FLYING_CONTRAL:	// 2020-12-09
		pBuff = new ioMagneticFlyBuff;
		break;
	}

	if( pBuff )
	{
		pBuff->SetBuffCategory( iBuffCategory );
		pBuff->SetTypeID( iTypeID );
		return pBuff;
	}

	LOG.PrintTimeAndLog( 0, "ioBuffMaker::CreateNormalBuffTemplete() Create Fail Buff: %d", iTypeID );

	return NULL;
}

ioBuff* ioBuffMaker::CreateAccessoryBuffTemplete( int iTypeID, int iBuffCategory )
{
	ioBuff *pBuff = NULL;

	switch( iTypeID )
	{
	case BT_ACC_SPEED_UPDOWN:
		pBuff = new ioAccSpeedUpDownBuff;
		break;
	case BT_ACC_DAMAGE_UPDOWN:
		pBuff = new ioAccDamageUpDownBuff;
		break;
	case BT_ACC_SYSTEM_DAMAGE_RATE:
		pBuff = new ioAccSystemDamageRateBuff;
		break;
	case BT_ACC_ARMOR_CLASS_UP_DOWN_BUFF:
		pBuff = new ioAccArmorClassUpDownBuff;
		break;
	case BT_ACC_RESTORATION_GAUGE:
		pBuff = new ioAccRestorationGaugeBuff;
		break;
	case BT_ACC_CRITICAL_RATE_BUFF:
		pBuff = new ioAccCriticalRateBuff;
		break;
	case BT_ACC_CRITICAL_DAMAGE_RATE_BUFF:
		pBuff = new ioAccCriticalDamageRateBuff;
		break;
	case BT_ACC_ARMOR_PENTRATION_BUFF:
		pBuff = new ioAccArmorPentrationBuff;
		break;
	case BT_ACC_LANDING_RATE_BUFF:
		pBuff = new ioAccLandingRateBuff;
		break;
	case BT_ACC_CAMERA_BUFF:
		pBuff = new ioAccCameraBuff;
		break;
	case BT_ACC_GETUP_PROTECTION_DELAY_BUFF:
		pBuff = new ioAccGetUpProtectionDelayBuff;
		break;
	}

	if( pBuff )
	{
		pBuff->SetBuffCategory( iBuffCategory );
		pBuff->SetTypeID( iTypeID );
		return pBuff;
	}

	LOG.PrintTimeAndLog( 0, "%s Create Fail Buff: %d", __FUNCTION__, iTypeID );

	return NULL;
}
