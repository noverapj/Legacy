
#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioBowItem.h"

#include "WeaponDefine.h"

ioBowItem::ioBowItem()
{
	m_iCurBullet = 0;
	m_iMaxBullet = 0;
	m_fReloadTimeRate = FLOAT1;

	m_dwAimTime = 1000;
	m_fAimAngle = 45.0f;
	m_fAimRange = FLOAT1000;
	m_fMinRange = FLOAT100;

	m_fMarkerRange = 0.0f;

	m_fReloadTimeModifyRate = FLOAT1;
	m_fFireTimeModifyRate = FLOAT1;

	m_iCurBullet = 0;

	m_bSetReloadMoveAni = false;

	m_dwReloadStartTime = 0;
	m_dwReloadEndTime = 0;
	m_dwNextReloadTime = 0;
	m_dwReloadGapTime = 0;

	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;
	m_dwFireDelayTime = 0;

	m_dwChargeStartTime = 0;

	m_dwAimStartTime = 0;

	m_iCurCombo = 0;
	m_FireState = FS_NONE;
	m_LevelTimeRate.clear();

	m_bSetSkillAimed = false;
	m_bSetAimedDir = false;

	m_bFirstAimMoveMotion = true;
	m_bAimMotionSetted = false;
	m_fCurAngle = 0.0f;
	m_dwTrackingEndTime = 0;
	m_dwFireStateEndTime = 0;
	m_AimStartDirKey = ioUserKeyInput::DKI_NONE;

	m_bAimEffect = false;
	m_bAimJumpAttack = false;
	m_dwCheckAimCancelTime = 0;

	m_fAirJumpPowerRate = 0.0f;

	m_bZeroAimState = false;
	m_dwZeroAimStartTime = 0;

	m_ZeroAimedEffectID = -1;
	
	m_bChangeDir = false;

	m_dwCurCoolTime = 0;
	m_dwCheckCoolTime = 0;

	m_iNormalExtendAddAttackIndex = 0;
}

ioBowItem::ioBowItem( const ioBowItem &rhs )
: ioWeaponItem( rhs ),
 m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
 m_ExtendZeroAimed( rhs.m_ExtendZeroAimed ),
 m_ReloadAnimation( rhs.m_ReloadAnimation ),
 m_ReloadEffect( rhs.m_ReloadEffect ),
 m_ReloadMoveAnimation( rhs.m_ReloadMoveAnimation ),
 m_AimUp( rhs.m_AimUp ),
 m_AimCenter( rhs.m_AimCenter ),
 m_AimDown( rhs.m_AimDown ),
 m_ZeroAimUp( rhs.m_ZeroAimUp ),
 m_ZeroAimCenter( rhs.m_ZeroAimCenter ),
 m_ZeroAimDown( rhs.m_ZeroAimDown ),
 m_JumpAimUp( rhs.m_JumpAimUp ),
 m_JumpAimCenter( rhs.m_JumpAimCenter ),
 m_JumpAimDown( rhs.m_JumpAimDown ),
 m_FireUp( rhs.m_FireUp ),
 m_FireCenter( rhs.m_FireCenter ),
 m_FireDown( rhs.m_FireDown ),
 m_ZeroFireUp( rhs.m_ZeroFireUp ),
 m_ZeroFireCenter( rhs.m_ZeroFireCenter ),
 m_ZeroFireDown( rhs.m_ZeroFireDown ),
 m_JumpFireUp( rhs.m_JumpFireUp ),
 m_JumpFireCenter( rhs.m_JumpFireCenter ),
 m_JumpFireDown( rhs.m_JumpFireDown ),
 m_BranchDelayAni( rhs.m_BranchDelayAni ),
 m_dwZeroAimedCheckTime( rhs.m_dwZeroAimedCheckTime ),
 m_AimEffect( rhs.m_AimEffect ),
 m_ZeroAimedEffect( rhs.m_ZeroAimedEffect ),
 m_dwJumpMarkerDuration( rhs.m_dwJumpMarkerDuration ),
 m_AimMoveFront( rhs.m_AimMoveFront ),
 m_AimMoveBack( rhs.m_AimMoveBack ),
 m_AimMoveLeft( rhs.m_AimMoveLeft ),
 m_AimMoveRight( rhs.m_AimMoveRight ),
 m_fAimMoveAniRate( rhs.m_fAimMoveAniRate ),
 m_AimJumpReadyAnimation( rhs.m_AimJumpReadyAnimation ),
 m_AimJumppingAnimation( rhs.m_AimJumppingAnimation ),
 m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
 m_bEnableZeroAiming( rhs.m_bEnableZeroAiming ),
 m_bEnableDownAim( rhs.m_bEnableDownAim ),
 m_vSecondAttributeList( rhs.m_vSecondAttributeList ),
 m_vSecondZeroAttributeList( rhs.m_vSecondZeroAttributeList ),
 m_fZeroAimSpeedRate( rhs.m_fZeroAimSpeedRate ),
 m_fMaxMarkerRangeRate( rhs.m_fMaxMarkerRangeRate ),
 m_fMaxAimRangeRate( rhs.m_fMaxAimRangeRate ),
 m_fMaxAimAngleRate( rhs.m_fMaxAimAngleRate ),
 m_FireEffect( rhs.m_FireEffect ),
 m_bEnablePowerCharge( rhs.m_bEnablePowerCharge ),
 m_bContactZeroAim( rhs.m_bContactZeroAim ),
 m_fTargetAimRate( rhs.m_fTargetAimRate ),
 m_fDownTargetAimRate( rhs.m_fDownTargetAimRate ),
 m_iMaxWeaponCnt( rhs.m_iMaxWeaponCnt ),
 m_BulletType( rhs.m_BulletType ),
 m_dwMaxCoolTime( rhs.m_dwMaxCoolTime ),
 m_dwNeedCoolTime( rhs.m_dwNeedCoolTime ),
 m_MissFire( rhs.m_MissFire ),
 m_fMissFireRate( rhs.m_fMissFireRate ),
 m_bUseBulletChangeDir( rhs.m_bUseBulletChangeDir ),
 m_szDestoyrDummyCharOnReleased( rhs.m_szDestoyrDummyCharOnReleased ),
 m_vZeroExtendAddAtrributeList( rhs.m_vZeroExtendAddAtrributeList ),
 m_vExtendExtraAttributeList( rhs.m_vExtendExtraAttributeList )
{
	m_iMaxBullet = rhs.m_iMaxBullet;
	m_fReloadTimeRate = rhs.m_fReloadTimeRate;

	m_dwAimTime = rhs.m_dwAimTime;
	m_fAimAngle = rhs.m_fAimAngle;
	m_fAimRange = rhs.m_fAimRange;
	m_fMinRange = rhs.m_fMinRange;

	m_fMarkerRange = rhs.m_fMarkerRange;

	if( m_iMaxBullet > 0 )
		m_iCurBullet = m_iMaxBullet;
	else
		m_iCurBullet = 0;

	m_fReloadTimeModifyRate = FLOAT1;
	m_fFireTimeModifyRate = FLOAT1;

	m_bSetReloadMoveAni = false;

	m_dwReloadStartTime = 0;
	m_dwReloadEndTime = 0;
	m_dwNextReloadTime = 0;
	m_dwReloadGapTime = 0;

	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;
	m_dwFireDelayTime = 0;

	m_dwChargeStartTime = 0;

	m_dwAimStartTime = 0;

	m_iCurCombo = 0;
	m_FireState = FS_NONE;
	m_bSetSkillAimed = false;
	m_bSetAimedDir = false;
	m_LevelTimeRate = rhs.m_LevelTimeRate;

	m_bFirstAimMoveMotion = true;
	m_bAimMotionSetted = false;
	m_fCurAngle = 0.0f;
	m_dwTrackingEndTime = 0;
	m_dwFireStateEndTime = 0;
	m_AimStartDirKey = ioUserKeyInput::DKI_NONE;

	m_bAimEffect = false;
	m_bAimJumpAttack = false;
	m_dwCheckAimCancelTime = 0;

	m_fAirJumpPowerRate = 0.0f;

	m_bZeroAimState = false;
	m_dwZeroAimStartTime = 0;

	m_ZeroAimedEffectID = -1;

	m_bChangeDir = false;

	m_dwCurCoolTime = 0;
	m_dwCheckCoolTime = 0;

	m_iNormalExtendAddAttackIndex = 0;
}

ioBowItem::~ioBowItem()
{
	m_LevelTimeRate.clear();

	m_vSecondAttributeList.clear();
	m_vSecondZeroAttributeList.clear();
}

void ioBowItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_iMaxWeaponCnt = rkLoader.LoadInt_e( "max_weapon_cnt", 1 );

	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	m_vExtendAttributeList.clear();
	m_vExtendAttributeList.reserve( iExtendCnt );
	for( int i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}

	LoadAttackAttribute_e( "extend_zero_aimed", m_ExtendZeroAimed, rkLoader );

	int iSecondCnt = rkLoader.LoadInt_e( "second_attack_cnt", 0 );
	m_vSecondAttributeList.clear();
	m_vSecondAttributeList.clear();
	for( i=0; i < iSecondCnt; ++i )
	{
		wsprintf_e( szBuf, "second_attack%d_id", i+1 );
		int iIndex = rkLoader.LoadInt( szBuf, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szBuf, "second_attack%d_limite", i+1 );
		float fLimiteValue = rkLoader.LoadFloat( szBuf, 0.0f );
		
		SecondAttribute kSecondAttr;
		kSecondAttr.m_iAttributeIndex = iIndex;
		kSecondAttr.m_fLimiteValue = fLimiteValue;
		m_vSecondAttributeList.push_back( kSecondAttr );
	}

	m_vSecondZeroAttributeList.clear();
	int iSecondZeroCnt = rkLoader.LoadInt_e( "second_zero_attack_cnt", 0 );
	for( i=0; i < iSecondZeroCnt; ++i )
	{
		wsprintf_e( szBuf, "second_zero_attack%d_id", i+1 );
		int iIndex = rkLoader.LoadInt( szBuf, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szBuf, "second_zero_attack%d_limite", i+1 );
		float fLimiteValue = rkLoader.LoadFloat( szBuf, 0.0f );

		SecondAttribute kSeconZeroAttr;
		kSeconZeroAttr.m_iAttributeIndex = iIndex;
		kSeconZeroAttr.m_fLimiteValue = fLimiteValue;
		m_vSecondZeroAttributeList.push_back( kSeconZeroAttr );
	}

	char szKey[MAX_PATH];
	m_AttackReadyAniList.clear();
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szKey, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	rkLoader.LoadString_e( "reload_animation", "", szBuf, MAX_PATH );
	m_ReloadAnimation = szBuf;
	rkLoader.LoadString_e( "reload_move_animation", "", szBuf, MAX_PATH );
	m_ReloadMoveAnimation = szBuf;
	rkLoader.LoadString_e( "reload_effect", "", szBuf, MAX_PATH );
	m_ReloadEffect = szBuf;

	rkLoader.LoadString_e( "aim_up_animation", "", szBuf, MAX_PATH );
	m_AimUp = szBuf;
	rkLoader.LoadString_e( "aim_center_animation", "", szBuf, MAX_PATH );
	m_AimCenter = szBuf;
	rkLoader.LoadString_e( "aim_down_animation", "", szBuf, MAX_PATH );
	m_AimDown = szBuf;

	rkLoader.LoadString_e( "zero_aim_up_animation", "", szBuf, MAX_PATH );
	m_ZeroAimUp = szBuf;
	rkLoader.LoadString_e( "zero_aim_center_animation", "", szBuf, MAX_PATH );
	m_ZeroAimCenter = szBuf;
	rkLoader.LoadString_e( "zero_aim_down_animation", "", szBuf, MAX_PATH );
	m_ZeroAimDown = szBuf;

	rkLoader.LoadString_e( "jump_aim_up_animation", "", szBuf, MAX_PATH );
	m_JumpAimUp = szBuf;
	rkLoader.LoadString_e( "jump_aim_center_animation", "", szBuf, MAX_PATH );
	m_JumpAimCenter = szBuf;
	rkLoader.LoadString_e( "jump_aim_down_animation", "", szBuf, MAX_PATH );
	m_JumpAimDown = szBuf;

	rkLoader.LoadString_e( "fire_up_animation", "", szBuf, MAX_PATH );
	m_FireUp = szBuf;
	rkLoader.LoadString_e( "fire_center_animation", "", szBuf, MAX_PATH );
	m_FireCenter = szBuf;
	rkLoader.LoadString_e( "fire_down_animation", "", szBuf, MAX_PATH );
	m_FireDown = szBuf;

	rkLoader.LoadString_e( "zero_fire_up_animation", "", szBuf, MAX_PATH );
	m_ZeroFireUp = szBuf;
	rkLoader.LoadString_e( "zero_fire_center_animation", "", szBuf, MAX_PATH );
	m_ZeroFireCenter = szBuf;
	rkLoader.LoadString_e( "zero_fire_down_animation", "", szBuf, MAX_PATH );
	m_ZeroFireDown = szBuf;

	rkLoader.LoadString_e( "jump_fire_up_animation", "", szBuf, MAX_PATH );
	m_JumpFireUp = szBuf;
	rkLoader.LoadString_e( "jump_fire_center_animation", "", szBuf, MAX_PATH );
	m_JumpFireCenter = szBuf;
	rkLoader.LoadString_e( "jump_fire_down_animation", "", szBuf, MAX_PATH );
	m_JumpFireDown = szBuf;

	rkLoader.LoadString_e( "branch_delay_animation", "", szBuf, MAX_PATH );
	m_BranchDelayAni = szBuf;

	rkLoader.LoadString_e( "aim_move_front", "", szBuf, MAX_PATH );
	m_AimMoveFront = szBuf;
	rkLoader.LoadString_e( "aim_move_back", "", szBuf, MAX_PATH );
	m_AimMoveBack = szBuf;
	rkLoader.LoadString_e( "aim_move_left", "", szBuf, MAX_PATH );
	m_AimMoveLeft = szBuf;
	rkLoader.LoadString_e( "aim_move_right", "", szBuf, MAX_PATH );
	m_AimMoveRight = szBuf;

	m_fAimMoveAniRate = rkLoader.LoadFloat_e( "aim_move_ani_rate", FLOAT1 );

	m_iMaxBullet = rkLoader.LoadInt_e( "max_bullet", 0 );
	m_fReloadTimeRate  = rkLoader.LoadFloat_e( "reload_time_rate", FLOAT1 );

	m_bEnableZeroAiming = rkLoader.LoadBool_e( "enable_zero_aiming", false );
	m_bEnableDownAim = rkLoader.LoadBool_e( "enable_down_aim", false );

	m_dwZeroAimedCheckTime = rkLoader.LoadInt_e( "zero_aimed_check_time", 1 );
	rkLoader.LoadString_e( "aim_effect", "", szBuf, MAX_PATH );
	m_AimEffect = szBuf;
	rkLoader.LoadString_e( "zero_aimed_effect", "", szBuf, MAX_PATH );
	m_ZeroAimedEffect = szBuf;

	m_fZeroAimSpeedRate = rkLoader.LoadFloat_e( "zero_aim_speed_rate", 0.0f );

	m_LevelTimeRate.clear();
	int iLevelCount = rkLoader.LoadInt_e( "level_count", 0 );
	for( i=0; i<iLevelCount; i++ )
	{
		wsprintf_e( szBuf, "level_time_rate%d", i);
		
		float fRate = rkLoader.LoadFloat( szBuf, -FLOAT1 );
		if( fRate == -FLOAT1 && i > 0 )
		{
			if( i>0 )
				fRate = m_LevelTimeRate[i-1];
			else
				fRate = FLOAT1;
		}

		m_LevelTimeRate.push_back( fRate );
	}

	m_dwJumpMarkerDuration = rkLoader.LoadFloat_e( "jump_marker_duration", 0 );

	m_dwAimTime = rkLoader.LoadInt_e( "aim_time", 1000 );
	m_fAimAngle = rkLoader.LoadFloat_e( "aim_angle", 45.0f );
	m_fAimRange = rkLoader.LoadFloat_e( "aim_range", FLOAT1000 );
	m_fMinRange = rkLoader.LoadFloat_e( "aim_min_range", 0.0f );

	m_fMarkerRange = rkLoader.LoadFloat_e( "marker_range", 0.0f );

	m_fMaxMarkerRangeRate = rkLoader.LoadFloat_e( "gathering_max_mark_range_rate", FLOAT1 );
	m_fMaxAimRangeRate = rkLoader.LoadFloat_e( "gathering_max_aim_range_rate", FLOAT1 );
	m_fMaxAimAngleRate = rkLoader.LoadFloat_e( "gathering_max_aim_angle_rate", FLOAT1 );


	rkLoader.LoadString_e( "fire_effect", "", szBuf, MAX_PATH );
	m_FireEffect = szBuf;

	m_bContactZeroAim = rkLoader.LoadBool_e( "contact_zero_aim", false );

	//
	m_fTargetAimRate = rkLoader.LoadFloat_e( "target_aim_rate", FLOAT08 );
	m_fDownTargetAimRate = rkLoader.LoadFloat_e( "down_target_aim_rate", 0.0f );

	//
	m_BulletType = (BulletCountType)rkLoader.LoadInt_e( "bullet_type", BCT_COUNT );

	m_dwMaxCoolTime = (DWORD)rkLoader.LoadInt_e( "max_cool_time", 0 );
	m_dwNeedCoolTime = (DWORD)rkLoader.LoadInt_e( "use_cool_time", 0 );

	rkLoader.LoadString_e( "miss_fire_animation", "", szBuf, MAX_PATH );
	m_MissFire = szBuf;
	m_fMissFireRate = rkLoader.LoadFloat_e( "miss_fire_ani_rate", FLOAT1 );

	m_bUseBulletChangeDir = rkLoader.LoadBool_e( "use_bullet_charge_dir", false );

	rkLoader.LoadString_e( "destory_dummy_on_released", "", szBuf, MAX_PATH );
	m_szDestoyrDummyCharOnReleased = szBuf;

	int iExtendNormalAddCnt = rkLoader.LoadInt_e( "zero_extend_attack_cnt",0 );
	m_vZeroExtendAddAtrributeList.clear();
	m_vZeroExtendAddAtrributeList.reserve( iExtendNormalAddCnt );
	for( int i = 0; i < iExtendNormalAddCnt; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "zero_extend_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vZeroExtendAddAtrributeList.push_back( kAttribute );
	}

	int iExtendExtraCnt = rkLoader.LoadInt_e( "extend_extra_attack_cnt", 0 );
	m_vExtendExtraAttributeList.clear();
	m_vExtendExtraAttributeList.reserve( iExtendExtraCnt );
	for( int i=0 ; i<iExtendExtraCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_extra_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendExtraAttributeList.push_back( kAttribute );
	}
}

ioItem* ioBowItem::Clone()
{
	return new ioBowItem( *this );
}

bool ioBowItem::SetExtendDefenseState( ioBaseChar *pOwner )
{
	if( m_BulletType == BCT_CHARGE )
		return false;

	if( m_iCurBullet == m_iMaxBullet)
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_ReloadAnimation );
	if( iAniID == -1 )	return false;

	float fTimeRate = m_fReloadTimeRate;

	int iLevelCount = m_LevelTimeRate.size();
	if( iLevelCount > 1 )
	{
		int iDividCnt, iNeedCnt;
		iDividCnt = m_iMaxBullet / iLevelCount;
		iNeedCnt = m_iMaxBullet - m_iCurBullet;

		LevelTimeRate::iterator iter = m_LevelTimeRate.begin();
		for( int i=1; i <= iLevelCount; i++ )
		{
			if( iNeedCnt <= iDividCnt * i )
			{
				fTimeRate *= *iter;
				break;
			}

			++iter;
		}
	}
	else
	{
		fTimeRate = m_fReloadTimeRate;
	}

	m_fReloadTimeModifyRate = m_fReloadTimeRate;

	m_bSetReloadMoveAni = false;
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fReloadTimeModifyRate );

	m_dwReloadStartTime = FRAMEGETTIME();
	m_dwReloadEndTime = m_dwReloadStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate - FLOAT100;

	m_dwReloadGapTime = ( m_dwReloadEndTime - m_dwReloadStartTime ) / m_iMaxBullet;
	m_dwNextReloadTime = m_dwReloadStartTime + m_dwReloadGapTime;

	pOwner->SetState( CS_EXTEND_DEFENSE );
	pOwner->SetReloadMoveEnable( true );

	if( !m_ReloadEffect.IsEmpty() )
	{
		pOwner->ShowWeaponItemMesh( false );
		pOwner->AttachEffect( m_ReloadEffect );
	}

	return true;
}

void ioBowItem::CheckExtendDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwReloadEndTime + pOwner->GetActionStopTotalDelay() > dwCurTime )
	{
		if( m_dwNextReloadTime < dwCurTime )
		{
			m_iCurBullet++;
			m_iCurBullet = min( m_iCurBullet, m_iMaxBullet );
			m_dwNextReloadTime += m_dwReloadGapTime;
		}
	}
	else
	{
		m_iCurBullet = m_iMaxBullet;
		m_bSetReloadMoveAni = false;
		m_FireState = FS_NONE;
		pOwner->SetReloadMoveEnable( false );
		pOwner->SetState( CS_DELAY );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << false;	//AimJump
			kPacket << (int)m_FireState;
			kPacket << m_iCurCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	g_TutorialMgr.ActionWeaponCharge();
}

void ioBowItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	m_fCurChargeRate = 0.0f;
	m_iNormalExtendAddAttackIndex = 0;

	if( m_bAimEffect )
	{
		m_bAimEffect = false;
		pOwner->EndEffect( m_AimEffect, false );
	}

	if( m_ZeroAimedEffectID != -1 )
	{
		pOwner->EndEffect( m_ZeroAimedEffectID, false );
		m_ZeroAimedEffectID = -1;
	}

	int iExtendCnt = m_vExtendAttributeList.size();
	if( COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		m_FireState = FS_CHARGING;
		m_dwChargeStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;

		m_dwZeroAimStartTime = 0;
		m_bZeroAimState = false;
		m_bChangeDir = false;

		m_bAimJumpAttack = false;
		m_fCurAngle = 0.0f;
		m_dwTrackingEndTime = 0;
		m_dwCheckAimCancelTime = 0;

		pOwner->SetState( CS_ATTACK );
		pOwner->SetChargingState( true );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->SetActionAniForTime( m_AttackReadyAniList[iCurCombo], FLOAT100, FLOAT100, m_dwChargeTime );
	}
	else
	{
		ioAttackableItem::SetNormalAttack( pOwner, iCurCombo );
		m_FireState = FS_NONE;
	}
}

void ioBowItem::ChangeToAiming( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->StopAutoTargetTracking();

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_bEnableZeroAiming )
	{
		if( m_bZeroAimState )
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_ZERO_AIMED );
		else
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
	}
	else
	{
		SetZeroAim( pOwner, false );
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
	}

	if( m_bEnableZeroAiming )
	{
		if( m_dwZeroAimStartTime == 0 )
			m_dwZeroAimStartTime = dwCurTime;
	}
	else
	{
		m_dwZeroAimStartTime = 0;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( pOwner->IsJumpState() )
		pGrp->SetLoopAni( m_JumpAimCenter, FLOAT100 );
	else
		pGrp->SetLoopAni( m_AimCenter, FLOAT100 );

	m_FireState = FS_AIMING;
	pOwner->SetAttackMoveEnable( true );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );
	
	m_AimStartDirKey = pOwner->GetCurDirKey();

	m_bFirstAimMoveMotion = true;
	m_bAimMotionSetted = true;

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	m_vStartDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	m_AimedTarget.Clear();
	m_bAimedBall = false;

	m_bSetAimedDir = false;
	m_bAimedState = false;

	if( !m_bAimEffect )
	{
		pOwner->AttachEffect( m_AimEffect );
		m_bAimEffect = true;
	}

	g_TutorialMgr.ActionChangeToAiming();
}

void ioBowItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_FireState )
	{
	case FS_CHARGING:
		OnCharging( pOwner );
		break;
	case FS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}

		ioAttackableItem::CheckNormalAttack( pOwner );
		break;
	case FS_AIMING:
		OnAiming( pOwner );
		break;
	case FS_AIM_WAIT:
		OnAimWait( pOwner );
		break;
	case FS_AIMED:
		OnAimed( pOwner );
		break;
	case FS_NORMAL_FIRE:
	case FS_AIMED_FIRE:
		OnFire( pOwner );
		break;
	case FS_NORAML_EXTEND_FIRE:
	case FS_AIMED_EXTEND_FIRE:
		OnExtendFire( pOwner );
		break;
	}

	if( m_bEnablePowerCharge &&	(m_FireState == FS_AIMING || m_FireState == FS_AIM_WAIT || m_FireState == FS_AIMED) )
	{
		CheckChargeRate();
		CheckTargetMarker( pOwner );
	}

	if( pOwner->IsCanAttackMove() &&
		m_FireState != FS_AIMING &&
		m_FireState != FS_AIM_WAIT &&
		m_FireState != FS_AIMED )
	{
		pOwner->SetAttackMoveEnable( false );
	}

	if( m_FireState != FS_AIMED && m_FireState != FS_AIMING &&
		m_FireState != FS_AIM_WAIT && m_dwZeroAimStartTime > 0 )
	{
		m_dwZeroAimStartTime = 0;
	}

	if( !m_bZeroAimState )
	{
		if( m_ZeroAimedEffectID != -1 )
		{
			pOwner->EndEffect( m_ZeroAimedEffectID, false );
			m_ZeroAimedEffectID = -1;
		}
	}
	else if( m_bEnableZeroAiming && m_bZeroAimState )
	{
		if( m_ZeroAimedEffectID == -1 )
		{
			ioEffect *pEffect = pOwner->AttachEffect( m_ZeroAimedEffect );
			if( pEffect )
				m_ZeroAimedEffectID = pEffect->GetUniqueID();
		}
	}

	if( pOwner->GetState() == CS_ATTACK )
		UpdateAimDir( pOwner );

	CheckAimMoveAniState( pOwner );
}

void ioBowItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargeStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if( m_iCurBullet >= 1 || m_iMaxBullet == 0 )
			{
				ChangeToAiming( pOwner );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << false;	//AimJump
					kPacket << (int)m_FireState;
					kPacket << m_iCurCombo;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
			else
			{
				pOwner->NormalAttackOver();

				if( m_iCurBullet < 1 && SetExtendDefenseState( pOwner ) )
				{
					if( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_EXTEND_DEFENSE );
						kPacket << pOwner->GetCharName();
						kPacket << pOwner->GetHP().m_fCurValue;
						kPacket << m_iCurBullet;
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
				}
			}
		}
	}
	else
	{
		pOwner->SetChargingState( false );
		ChangeToNormalAttack( pOwner );
	}
}

void ioBowItem::OnAiming( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() && pOwner->GetState() != CS_JUMP )	// 버튼 뗌, 일반 공격으로 전환
	{
		switch( m_BulletType )
		{
		case BCT_COUNT:
			ChangeToNormalFire( pOwner );
			return;
		case BCT_CHARGE:
			if( IsCanFire( pOwner, 1 ) )
			{
				ChangeToNormalFire( pOwner );
			}
			else
			{
				pOwner->SetMissFireState( m_MissFire, m_fMissFireRate );

				SetNoneState( pOwner );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << false;	//AimJump
					kPacket << (int)m_FireState;
					kPacket << m_iCurCombo;
					kPacket << true;	// MissFire
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
			return;
		}
	}

	if( !SetAimJumpState( pOwner ) )
		return;

	if( pOwner->GetState() != CS_JUMP && pOwner->IsDefenseKeyDown() )
	{
		if( IsCanFire(0) )
		{
			int iSkillNum = pOwner->GetSkillKeyInput();
			if( iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(0, SUT_NORMAL))
			{
				m_FireState = FS_NONE;
				return;
			}
		}
	}

	if( m_bEnableZeroAiming )
	{
		bool bZeroAim = CheckZeroAim( pOwner );

		if( bZeroAim && pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_FireState;
			kPacket << m_AimedTarget;
			kPacket << m_bAimedBall;
			kPacket << m_iCurCombo;
			kPacket << m_bZeroAimState;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}			

		if( bZeroAim )
			g_TutorialMgr.ActionZeroAimed();
		else
			g_TutorialMgr.ActionAimed();
	}

	ioHashString szTarget;
	m_AimedTarget.Clear();
	m_bAimedBall = false;

	float fAimAngle = GetChargeAimAngle();
	float fAimRange = GetChargeAimRange();

	if( pOwner->CheckAimedTargetByBow( fAimAngle, m_fMinRange, fAimRange, szTarget, m_bEnableDownAim ) )
	{
		m_AimedTarget = szTarget;
	}

	// 타겟 비교
	if( CompareAimedTarget( pOwner ) )
	{
		m_AimedTarget.Clear();
		m_bAimedBall = true;
	}

	if( !m_AimedTarget.IsEmpty() || m_bAimedBall )
		ChangeToAimWaitState( pOwner, m_AimedTarget );	// 타겟 세팅됨
}

void ioBowItem::OnAimWait( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() && pOwner->GetState() != CS_JUMP )	// 버튼 뗌, 일반 공격으로 전환
	{
		switch( m_BulletType )
		{
		case BCT_COUNT:
			ChangeToNormalFire( pOwner );
			return;
		case BCT_CHARGE:
			if( IsCanFire( pOwner, 1 ) )
			{
				ChangeToNormalFire( pOwner );
			}
			else
			{
				pOwner->SetMissFireState( m_MissFire, m_fMissFireRate );

				SetNoneState( pOwner );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << false;	//AimJump
					kPacket << (int)m_FireState;
					kPacket << m_iCurCombo;
					kPacket << true;	// MissFire
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
			return;
		}
	}

	if( !SetAimJumpState( pOwner ) )
		return;

	if( pOwner->GetState() != CS_JUMP && pOwner->IsDefenseKeyDown() )
	{
		if( IsCanFire(0) )
		{
			int iSkillNum = pOwner->GetSkillKeyInput();
			if( iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(0, SUT_NORMAL))
			{
				m_FireState = FS_NONE;
				return;
			}
		}
	}

	DWORD dwTimeGap = FRAMEGETTIME() - m_dwAimStartTime;
	if( pOwner->GetState() != CS_JUMP && dwTimeGap > m_dwAimTime )	// 조준 완료
	{
		ChangeToAimedState( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_FireState;
			kPacket << m_AimedTarget;
			kPacket << m_bAimedBall;
			kPacket << m_iCurCombo;
			kPacket << false;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else if( pOwner->GetState() == CS_JUMP && dwTimeGap > m_dwAimTime/2 )	// 조준 완료
	{
		ChangeToAimedState( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_FireState;
			kPacket << m_AimedTarget;
			kPacket << m_bAimedBall;
			kPacket << m_iCurCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else
	{
		if( !CheckCurrentTargetValid( pOwner, true ) )
		{
			m_dwTrackingEndTime = FRAMEGETTIME();

			// 대상을 제거한후 조준대기로 넘겨야함
			m_AimedTarget.Clear();
			m_bAimedBall = false;

			pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
			ChangeToAiming( pOwner );

			/*
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << false;	//AimJump
				kPacket << (int)m_FireState;
				kPacket << m_iCurCombo;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			*/
		}
	}
}

void ioBowItem::OnAimed( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() )
	{
		if( pOwner->GetState() != CS_JUMP && !m_bZeroAimState )
		{
			bool bZeroAim = CheckZeroAim( pOwner );

			if( bZeroAim && pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_FireState;
				kPacket << m_AimedTarget;
				kPacket << m_bAimedBall;
				kPacket << m_iCurCombo;
				kPacket << m_bZeroAimState;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}

			if( bZeroAim )
				g_TutorialMgr.ActionZeroAimed();
			else
				g_TutorialMgr.ActionAimed();
		}

		if( !pOwner->IsAttackKeyDown() && pOwner->GetState() != CS_JUMP )
		{
			bool bNormalFire = false;

			switch( m_BulletType )
			{
			case BCT_COUNT:
				if( m_bChargeCharRotate && !CheckCurrentTargetValid( pOwner, true ) )
					bNormalFire = true;
				break;
			case BCT_CHARGE:
				if( IsCanFire( pOwner, 1 ) )
				{
					if( m_bChargeCharRotate && !CheckCurrentTargetValid( pOwner, true ) )
						bNormalFire = true;
				}
				else
				{
					pOwner->SetMissFireState( m_MissFire, m_fMissFireRate );

					SetNoneState( pOwner );

					if( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
						FillItemGaugePacketHeader( pOwner, kPacket );
						kPacket << false;	//AimJump
						kPacket << (int)m_FireState;
						kPacket << m_iCurCombo;
						kPacket << true;	// MissFire
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
					return;
				}
				break;
			}

			if( bNormalFire )
			{
				m_bAimedBall = false;
				m_bAimedState = false;
				ChangeToNormalFire( pOwner );
				return;
			}
			else
			{
				ChangeToAimedFire( pOwner );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << (int)m_FireState;
					kPacket << m_AimedTarget;
					kPacket << m_bAimedBall;
					kPacket << m_iCurCombo;
					kPacket << m_vAimedDir;
					kPacket << m_bSetAimedDir;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				g_TutorialMgr.ActionAimedFireDeveloperK(m_AimedTarget);
			}
		}
		else	// AIMED FIRE..
		{
			if( !SetAimJumpState( pOwner ) )
				return;

			if( CheckCurrentTargetValid( pOwner, true ) )
			{
				TrackingTarget( pOwner );
			
				if( pOwner->GetState() != CS_JUMP && pOwner->IsDefenseKeyDown() )
				{
					if( IsCanFire(0) )
					{
						int iSkillNum = pOwner->GetSkillKeyInput();
						if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(0, SUT_NORMAL))
						{
							m_bSetSkillAimed = true;
							m_FireState = FS_NONE;
							pOwner->SetChargingState( false );
							if( pOwner->IsNeedSendNetwork() )
							{
								SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
								FillItemGaugePacketHeader( pOwner, kPacket );
								kPacket << (int)m_FireState;
								kPacket << m_AimedTarget;
								kPacket << m_bAimedBall;
								kPacket << m_iCurCombo;
								kPacket << m_vAimedDir;
								kPacket << m_bSetAimedDir;
								kPacket << m_bSetSkillAimed;
								P2PNetwork::SendToAllPlayingUser( kPacket );
							}							
							return;
						}
					}
				}
			}
			else
			{
				m_dwTrackingEndTime = FRAMEGETTIME();

				// 대상을 제거한후 조준대기로 넘겨야함
				m_AimedTarget.Clear();
				m_bAimedBall = false;

				pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
				ChangeToAiming( pOwner );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << false;	//AimJump
					kPacket << (int)m_FireState;
					kPacket << m_iCurCombo;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
	}
	else
	{
		if( CheckCurrentTargetValid( pOwner, false ) )
		{
			TrackingTarget( pOwner );
		}
	}
}

void ioBowItem::OnFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_bAimEffect && m_dwFireStateEndTime < dwCurTime )
	{
		m_bAimEffect = false;
		pOwner->EndEffect( m_AimEffect, false );
	}

	if( pOwner->CheckDefaultNormalAttack( m_dwFireStartTime,
										  m_dwFireEndTime,
										  m_dwFireDelayTime ) )
	{
		m_FireState = FS_NONE;
	}

	if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() && m_dwKeyReserveTime && dwCurTime > m_dwKeyReserveTime )
	{
		if ( m_bZeroAimState && (int)m_vZeroExtendAddAtrributeList.size() > m_iNormalExtendAddAttackIndex )
		{
			bool bAimed = (m_FireState == FS_AIMED_FIRE) ? true : false;
			pOwner->InitAttackReserve();
			pOwner->ClearReserveKeyInput();
			ChangeExtendAttack( pOwner, bAimed, m_iNormalExtendAddAttackIndex );
		}
		else if ( !m_bZeroAimState && (int)m_vExtendExtraAttributeList.size() > m_iNormalExtendAddAttackIndex )
		{
			bool bAimed = (m_FireState == FS_AIMED_FIRE) ? true : false;
			pOwner->InitAttackReserve();
			pOwner->ClearReserveKeyInput();
			ChangeExtendAttack( pOwner, bAimed, m_iNormalExtendAddAttackIndex );
		}
	}
}

void ioBowItem::OnJumpFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwAttackOverTime = m_dwFireStartTime + m_dwFireEndTime + m_dwFireDelayTime;

	if( dwCurTime >= dwAttackOverTime )
	{
		m_FireState = FS_NONE;
		pOwner->SetMoveDirByRotate( pOwner->GetTargetRot() );
	}
}

void ioBowItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	m_fCurChargeRate = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if ( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_FireState = FS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << false;	//AimJump
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBowItem::ChangeToAimWaitState( ioBaseChar *pOwner, const ioHashString &szTarget )
{
	m_dwAimStartTime = FRAMEGETTIME();
	m_FireState = FS_AIM_WAIT;

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMING );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << false;	//AimJump
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		kPacket << m_AimedTarget;
		kPacket << m_bAimedBall;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBowItem::ChangeToAimedState( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();

	if( m_bZeroAimState )
	{
		if( !pGrp->GetLoopInPlay( m_ZeroAimUp ) )
			pGrp->SetLoopAni( m_ZeroAimUp, 0.0f, 0.0f );

		if( !pGrp->GetLoopInPlay( m_ZeroAimDown ) )
			pGrp->SetLoopAni( m_ZeroAimDown, 0.0f, 0.0f );

		if( !pGrp->GetLoopInPlay( m_ZeroAimCenter ) )
			pGrp->SetLoopAni( m_ZeroAimCenter, 0.0f, FLOAT1 );
	}
	else if( pOwner->GetState() != CS_JUMP )
	{
		pGrp->SetLoopAni( m_AimUp, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_AimDown, 0.0f, 0.0f );

		if( !pGrp->GetLoopInPlay( m_AimCenter ) )
			pGrp->SetLoopAni( m_AimCenter, 0.0f, FLOAT1 );
	}
	else
	{
		pGrp->SetLoopAni( m_JumpAimUp, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_JumpAimDown, 0.0f, 0.0f );

		if( !pGrp->GetLoopInPlay( m_JumpAimCenter ) )
			pGrp->SetLoopAni( m_JumpAimCenter, 0.0f, FLOAT1 );
	}

	m_FireState = FS_AIMED;

	if( m_dwZeroAimStartTime == 0 && !m_bZeroAimState )
	{
		m_dwZeroAimStartTime = FRAMEGETTIME();
	}

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

	if( m_bZeroAimState )
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_ZERO_AIMED );
	else
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );

	if( CheckCurrentTargetValid( pOwner, false ) )
		TrackingTarget( pOwner );
}

void ioBowItem::ChangeToNormalFire( ioBaseChar *pOwner )
{
	int iExtendAttackCnt = m_vExtendAttributeList.size();
	if( iExtendAttackCnt <= m_iCurCombo )
		return;

	if( pOwner->GetState() == CS_JUMP )
		return;

	//
	CheckChargeRate();

	m_AimedTarget.Clear();
	
	const AttackAttribute *pAttr = NULL;
	if( m_bZeroAimState )
		pAttr = &m_ExtendZeroAimed;
	else
		pAttr = &m_vExtendAttributeList[m_iCurCombo];

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int	  iAniID	  = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float fTimeRate   = pAttr->m_fAttackAniRate * m_fFireTimeModifyRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;
	m_dwFireDelayTime = pAttr->m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID,
							     pAttr->m_vWeaponInfoList,
							     FTT_NORMAL_ATTACK,
							     fTimeRate,
							     dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	m_dwFireStateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStateEndTime += FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwFireStartTime = FRAMEGETTIME();
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
	if( pAttr->m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fEndAniRate = pAttr->m_fAttackEndAniRate;
		pGrp->SetActionAni( pAttr->m_AttackEndAnimation,
							0.0f, FLOAT100,
							1.0f, FLOAT1/fEndAniRate,
							m_dwFireEndTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( pAttr->m_AttackEndAnimation );
		m_dwFireDelayTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndAniRate;
	}


	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	m_FireState = FS_NORMAL_FIRE;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if ( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += FRAMEGETTIME();

	if( iExtendAttackCnt <= m_iCurCombo + 1 )
		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	else
		pOwner->ApplyNormalAttackResult( m_iCurCombo );

	pOwner->SetChargingState( false );

	DWORD dwComboTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" )*fTimeRate;
	pOwner->SetComboCheckTime( dwComboTime, true );

	DecreaseCoolTime();

	if( m_bUseBulletChangeDir )
		g_WeaponMgr.SetReturnPingpong( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << false;
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	g_TutorialMgr.ActionNormalFire();
}

bool ioBowItem::CheckCurrentTargetValid( ioBaseChar *pOwner, bool bAngleAlsoCheck )
{
	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !pTarget && !m_bAimedBall )
		return false;
	else if( !pTarget && !pBall )
	{
		m_bAimedBall = false;
		return false;
	}

	D3DXVECTOR3 vTargetPos;
	if( pTarget )
	{
		if( !pTarget->IsEnableAimTarget(m_bEnableDownAim) )
			return false;

		if( pTarget->HasHideBuff() )
			return false;

		if( pTarget->IsChatModeState( false ) )
			return false;

		vTargetPos = pTarget->GetWorldPosition();
	}
	else if( m_bAimedBall && pBall )
	{
		vTargetPos = pBall->GetWorldPosition();
	}

	float fAimRange = GetChargeAimRange();
	float fAimAngle = GetChargeAimAngle();

	D3DXVECTOR3 vDiff = vTargetPos - pOwner->GetWorldPosition();
	if( D3DXVec3LengthSq( &vDiff ) > fAimRange * fAimRange )
		return false;

	if( D3DXVec3LengthSq( &vDiff ) < m_fMinRange * m_fMinRange )
		return false;

	if( bAngleAlsoCheck )
	{
		vDiff.y = 0.0f;
		D3DXVec3Normalize( &vDiff, &vDiff );

		if( m_bChargeCharRotate )
		{
			if( pOwner->IsJumpState() )
			{
				float fHalfCosine = cosf( DEGtoRAD( m_fAimAngle * FLOAT05 ) );
				if( D3DXVec3Dot( &m_vStartDir, &vDiff ) < fHalfCosine )
					return false;
			}
			else
			{
				D3DXVECTOR3 vCharDir;
				D3DXQUATERNION qtRot = pOwner->GetTargetRot();
				vCharDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

				float fHalfCosine = cosf( DEGtoRAD( m_fAimAngle * FLOAT05 ) );
				if( D3DXVec3Dot( &vCharDir, &vDiff ) < fHalfCosine )
					return false;
			}
		}
		else
		{
			float fHalfCosine = cosf( DEGtoRAD( fAimAngle * FLOAT05 ) );
			if( D3DXVec3Dot( &m_vStartDir, &vDiff ) < fHalfCosine )
				return false;
		}
	}

	return true;
}

void ioBowItem::UpdateAimDir( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioBiped *pBiped = pGrp->GetBiped("Bip01 Spine1");
	if(!pBiped)	return;

	DWORD dwCurTime = FRAMEGETTIME();
	bool bStateCheck = false;
	switch( m_FireState )
	{
	case FS_AIMING:
	case FS_AIM_WAIT:
	case FS_AIMED:
	case FS_AIMED_FIRE:
	case FS_AIMED_EXTEND_FIRE:
		bStateCheck = true;
	}

	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !bStateCheck )
	{
		m_fCurAngle = 0.0f;
		pBiped->ReleaseExtraRotation();
		return;
	}

	if( !pTarget && m_bChargeCharRotate && m_bChangeDir )
	{
		m_fCurAngle = 0.0f;
		pBiped->ReleaseExtraRotation();
		return;
	}

	D3DXVECTOR3 vTargetPos;
	if( pTarget )
		vTargetPos = pTarget->GetWorldPosition();
	else if( m_bAimedBall && pBall )
		vTargetPos = pBall->GetWorldPosition();

	float fRotAngle = 0.0f;

	if( m_bAimJumpAttack || m_FireState == FS_AIMED_FIRE )
	{
		if( m_dwFireStateEndTime < dwCurTime )
		{
			float fTotal, fGap;
			DWORD dwEndTime = m_dwFireStartTime + m_dwFireEndTime;
			fTotal = fGap = 0.0f;

			if( dwEndTime > m_dwFireStateEndTime )
				fTotal = (float)(dwEndTime - m_dwFireStateEndTime);

			if( dwEndTime > dwCurTime )
				fGap = (float)(dwEndTime - dwCurTime);

			float fRate = 0.0f;
			if( fTotal > 0.0f )
				fRate = fGap / fTotal;

			fRotAngle = m_fCurAngle * fRate;
		}
		else
		{
			fRotAngle = m_fCurAngle;
		}
	}
	else
	{
		bool bTarget = true;
		if( pTarget && !pTarget->IsEnableAimTarget(m_bEnableDownAim) )
			bTarget = false;
		else if( m_bAimedBall && !pBall )
			bTarget = false;
		else if( !pTarget && !m_bAimedBall )
			bTarget = false;

		if( !bTarget )
		{
			float fTotal, fGap;
			DWORD dwEndTime = m_dwTrackingEndTime + 500;
			fTotal = fGap = 0.0f;

			fTotal = FLOAT500;
			if( dwEndTime > dwCurTime )
				fGap = (float)(dwEndTime - dwCurTime);
			else
				m_dwTrackingEndTime = 0;

			if( m_dwTrackingEndTime == 0 )
			{
				m_fCurAngle = 0.0f;
				pBiped->ReleaseExtraRotation();
				return;
			}

			float fRate = 0.0f;
			if( fTotal > 0.0f )
				fRate = fGap / fTotal;

			fRotAngle = m_fCurAngle * fRate;
		}
		else
		{
			D3DXVECTOR3 vSightDir = vTargetPos - pOwner->GetWorldPosition();
			D3DXVec3Normalize( &vSightDir, &vSightDir );

			D3DXVECTOR3 vXZDir( vSightDir.x, 0.0f, vSightDir.z );
			D3DXVec3Normalize( &vXZDir, &vXZDir );

			D3DXQUATERNION qtTargetRot;
			ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

			float fCurYaw, fTargetYaw;
			fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
			fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
			fCurYaw = RADtoDEG( ioMath::QuaterToYaw(pOwner->GetTargetRot()) );
			fCurYaw = ioMath::ArrangeHead( fCurYaw );

			m_fCurAngle = DEGtoRAD( ioMath::AngleGap( fCurYaw, fTargetYaw ) );

			float fAimTimeRate = FLOAT1;
			DWORD dwTimeGap = dwCurTime - m_dwAimStartTime;
			if( dwTimeGap < m_dwAimTime )
			{
				fAimTimeRate = (float)dwTimeGap / (float)m_dwAimTime;
			}
			
			fRotAngle = m_fCurAngle * fAimTimeRate;
		}
	}

	D3DXQUATERNION qtExtraRot;
	D3DXQuaternionRotationYawPitchRoll( &qtExtraRot, 0.0f, fRotAngle, 0.0f );
	pBiped->SetExtraRotation( qtExtraRot, true );
}

void ioBowItem::TrackingTarget( ioBaseChar *pOwner )
{
	bool bJumpState = false;
	if( pOwner->IsJumpState() )
		bJumpState = true;

	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioAnimation *pUp, *pCenter, *pDown;

	if( bJumpState )
	{
		pUp = pGrp->GetLoopInPlay( m_JumpAimUp );
		pCenter = pGrp->GetLoopInPlay( m_JumpAimCenter );
		pDown = pGrp->GetLoopInPlay( m_JumpAimDown );
	}
	else if( m_bZeroAimState )
	{
		pUp = pGrp->GetLoopInPlay( m_ZeroAimUp );
		pCenter = pGrp->GetLoopInPlay( m_ZeroAimCenter );
		pDown = pGrp->GetLoopInPlay( m_ZeroAimDown );
	}
	else
	{
		pUp = pGrp->GetLoopInPlay( m_AimUp );
		pCenter = pGrp->GetLoopInPlay( m_AimCenter );
		pDown = pGrp->GetLoopInPlay( m_AimDown );
	}

	D3DXVECTOR3 vAimedDir;
	if( pTarget )
		vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
	else if( m_bAimedBall && pBall )
		vAimedDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	float fAniWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
	if( vAimedDir.y >= 0.0f )	// up, center
	{
		if( !pUp || !pCenter || !pDown )
		{
			return;
		}

		pUp->ForceWeight( fAniWeight );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( 0.0f );
	}
	else	// center, down
	{
		if( !pUp || !pCenter || !pDown )
		{
			return;
		}
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( fAniWeight );
	}

	m_vAimedDir = vAimedDir;
	m_bSetAimedDir = true;
	m_bAimedState = true;
}

void ioBowItem::ChangeToAimedFire( ioBaseChar *pOwner )
{
	int iExtendAttackCnt = m_vExtendAttributeList.size();
	if( iExtendAttackCnt <= m_iCurCombo )
		return;

	if( pOwner->GetState() == CS_JUMP )
		return;

	//
	CheckChargeRate();

	const AttackAttribute *pAttr = NULL;
	if( m_bZeroAimState )
		pAttr = &m_ExtendZeroAimed;
	else
		pAttr = &m_vExtendAttributeList[m_iCurCombo];

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int	  iAniID	  = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;
	m_dwFireDelayTime = pAttr->m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID,
							     pAttr->m_vWeaponInfoList,
							     FTT_NORMAL_ATTACK,
							     fTimeRate,
							     dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwFireStartTime = FRAMEGETTIME();
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	m_dwFireStateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStateEndTime += FRAMEGETTIME();

	if( m_bZeroAimState && m_bContactZeroAim )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
		if( pTarget )
		{
			pOwner->SetTargetRotToTargetPos( pTarget, true );
		}

		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1/fTimeRate );
	}
	else
	{
		float fWeight = GetAimedUpDownWeight( m_vAimedDir, 85.0f );
		if( pAttr->m_AttackEndAnimation.IsEmpty() )
		{
			if( m_vAimedDir.y >= 0.0f )
			{
				AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true );
			}
			else
			{
				AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true );
			}
		}
		else
		{
			if( m_vAimedDir.y >= 0.0f )
			{
				AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, false );
			}
			else
			{
				AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, false );
			}

			float fEndAniRate = pAttr->m_fAttackEndAniRate;
			pGrp->SetActionAni( pAttr->m_AttackEndAnimation,
				0.0f, FLOAT100,
				1.0f, FLOAT1/fEndAniRate,
				m_dwFireEndTime, 0.0f, true );

			int iEndAniID = pGrp->GetAnimationIdx( pAttr->m_AttackEndAnimation );
			m_dwFireDelayTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndAniRate;
		}
	}

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if ( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += FRAMEGETTIME();

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	m_FireState = FS_AIMED_FIRE;

	if( iExtendAttackCnt <= m_iCurCombo + 1 )
		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	else
		pOwner->ApplyNormalAttackResult( m_iCurCombo );

	pOwner->SetChargingState( false );

	DWORD dwComboTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" )*fTimeRate;
	pOwner->SetComboCheckTime( dwComboTime, true );

	DecreaseCoolTime();

	if( m_bUseBulletChangeDir )
		g_WeaponMgr.SetReturnPingpong( pOwner );
}

float ioBowItem::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
{
	D3DXVECTOR3 vXZDir( vAimDir.x, 0.0f, vAimDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	float fAngle = D3DXVec3Dot( &vXZDir, &vAimDir );
	fAngle = max( -FLOAT1, min( fAngle, FLOAT1 ) );
	fAngle = acosf( fAngle );
	fAngle = RADtoDEG( fAngle );
	fAngle = max( 0.0f, min( fAngle, fMaxAngle ) );

	float fWeight = fAngle / fMaxAngle;

	return fWeight;
}

void ioBowItem::AimedFireToUp( ioEntityGroup *pGrp,
							   float fWeight,
							   float fTimeRate,
							   DWORD dwPreDelay,
							   bool bHold )
{
	float fFadeOut = FLOAT100;
	if( !bHold )
		fFadeOut = 0.0f;

	bool bAniFx1 = false;
	bool bAniFx2 = true;

	if( fWeight > FLOAT05 )
	{
		bAniFx1 = true;
		bAniFx2 = false;
	}

	ioHashString szFireUp, szFireCenter;
	if( m_bAimJumpAttack )
	{
		szFireUp = m_JumpFireUp;
		szFireCenter = m_JumpFireCenter;
	}
	else if( m_bZeroAimState )
	{
		szFireUp = m_ZeroFireUp;
		szFireCenter = m_ZeroFireCenter;
	}
	else
	{
		szFireUp = m_FireUp;
		szFireCenter = m_FireCenter;
	}

	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( szFireUp,
							FLOAT100, fFadeOut,
							fWeight, FLOAT1/fTimeRate,
							0.0f, dwPreDelay,
							bHold, bAniFx1 );
	}

	pGrp->SetActionAni( szFireCenter,
						FLOAT100, fFadeOut,
						1.0f - fWeight, FLOAT1/fTimeRate,
						0.0f, dwPreDelay,
						bHold, bAniFx2 );
}

void ioBowItem::AimedFireToDown( ioEntityGroup *pGrp,
								 float fWeight,
								 float fTimeRate,
								 DWORD dwPreDelay,
								 bool bHold )
{
	float fFadeOut = FLOAT100;
	if( !bHold )
		fFadeOut = 0.0f;
	bool bAniFx1 = true;
	bool bAniFx2 = false;
	
	if( fWeight > FLOAT05 )
	{
		bAniFx1 = false;
		bAniFx2 = true;
	}
	
	ioHashString szFireDown, szFireCenter;
	if( m_bAimJumpAttack )
	{
		szFireDown = m_JumpFireDown;
		szFireCenter = m_JumpFireCenter;
	}
	else if( m_bZeroAimState )
	{
		szFireDown = m_ZeroFireDown;
		szFireCenter = m_ZeroFireCenter;
	}
	else
	{
		szFireDown = m_FireDown;
		szFireCenter = m_FireCenter;
	}

	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( szFireCenter,
							FLOAT100, fFadeOut,
							1.0f - fWeight, FLOAT1/fTimeRate,
							0.0f, dwPreDelay,
							bHold, bAniFx1 );
	}

	pGrp->SetActionAni( szFireDown,
						FLOAT100, fFadeOut,
						fWeight, FLOAT1/fTimeRate,
						0.0f, dwPreDelay,
						bHold, bAniFx2 );
}

void ioBowItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iFireState;
	bool bZeroAimState;
	rkPacket >> iFireState;
	rkPacket >> m_AimedTarget;
	rkPacket >> m_bAimedBall;
	rkPacket >> m_iCurCombo;

	switch( iFireState )
	{
	case FS_AIMED_FIRE:
		rkPacket >> m_vAimedDir;
		rkPacket >> m_bSetAimedDir;
		ChangeToAimedFire( pOwner );
		break;
	case FS_AIMING:
		rkPacket >> bZeroAimState;
		if( bZeroAimState )
			SetZeroAim( pOwner, bZeroAimState );
		break;
	case FS_AIMED:
		rkPacket >> bZeroAimState;
		if( bZeroAimState )
			SetZeroAim( pOwner, bZeroAimState );
		else
			ChangeToAimedState( pOwner );
		break;
	case FS_NONE:
		rkPacket >> m_vAimedDir;
		rkPacket >> m_bSetAimedDir;
		rkPacket >> m_bSetSkillAimed;
		break;
	case FS_AIMED_EXTEND_FIRE:
	case FS_NORAML_EXTEND_FIRE:
		{
			bool bAimed;
			rkPacket >> m_vAimedDir;
			rkPacket >> m_bSetAimedDir;
			rkPacket >> bAimed;
			m_iNormalExtendAddAttackIndex = m_iCurCombo;
			ChangeExtendAttack( pOwner, bAimed, m_iNormalExtendAddAttackIndex );
		}
		break;
	}
}

void ioBowItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	bool bAimJumpState;
	rkPacket >> bAimJumpState;

	if( bAimJumpState )
	{
		int iAimState;
		rkPacket >> iAimState;

		bool bFullTime = false;

		switch( iAimState )
		{
		case AJS_READY:
			SetAimJumpReady( pOwner );
			break;
		case AJS_JUMPPING:
			rkPacket >> bFullTime;
			SetAimJumpping( pOwner, bFullTime );
			break;
		case AJS_CANCEL:
			SetAimJumpCancel( pOwner );
			break;
		}

		return;
	}

	int iFireState;
	rkPacket >> iFireState;
	rkPacket >> m_iCurCombo;

	switch( iFireState )
	{
	case FS_NONE:
		m_FireState = FS_NONE;
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetReloadMoveEnable( false );
		pOwner->SetState( CS_DELAY );
		break;
	case FS_CHARGING:
		pOwner->NormalAttackOver();
		break;
	case FS_AIM_WAIT:
		rkPacket >> m_AimedTarget;
		rkPacket >> m_bAimedBall;
		m_dwAimStartTime = FRAMEGETTIME();
		break;
	case FS_AIMING:
		// 1. FS_AIM_WAIT 중에 Lost Target
		// 2. FS_AIMED 중에 Lost Target
		m_dwTrackingEndTime = FRAMEGETTIME();

		// 대상을 제거한후 조준대기로 넘겨야함
		m_AimedTarget.Clear();
		m_bAimedBall = false;

		pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
		ChangeToAiming( pOwner );
		break;
	case FS_NORMAL_ATTACK:
		ChangeToNormalAttack( pOwner );
		break;
	case FS_NORMAL_FIRE:
		// 1. FS_AIMING 중에 Key off
		// 2. FS_AIM_WAIT 중에 key off
		ChangeToNormalFire( pOwner );
		break;
	}
}

void ioBowItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioArrowWeapon *pArrow = ToArrowWeapon( pWeapon );
	ioMissileWeapon *pMissile = ToMissileWeapon( pWeapon );
	ioReturnPingpongWeapon *pReturnPingpong = ToReturnPingpongWeapon( pWeapon );
	ioZoneEffectWeapon * pZoneEffect = ToZoneEffectWeapon( pWeapon );

	if( pReturnPingpong )
	{
		g_WeaponMgr.CheckMaxWeaponCnt( pWeapon, m_iMaxWeaponCnt );
	}

	if( pArrow || pMissile || pReturnPingpong )
		WasteBullet();

	D3DXVECTOR3 vAimedDir = m_vAimedDir;
	if( pZoneEffect )
	{
		ioBaseChar *pChar = pZoneEffect->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZoneEffect->SetCollisionDir( vDir );
			pZoneEffect->SetZoneValue( true, false );
		}
	}
	
	if( bJumpAttack )
	{
		if( m_bAimedState )
		{
			vAimedDir = ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
			pWeapon->SetMoveDir( vAimedDir );
		}

		if( pArrow || pMissile )
			SetFireEffect( pWeapon->GetOwner(), vAimedDir );

		m_bSetAimedDir = false;

		if( pArrow && m_vSecondAttributeList.size() > 0 )
		{
			pArrow->SetSecondAttribute( m_vSecondAttributeList );
		}
		else if( pMissile && m_vSecondAttributeList.size() > 0 )
		{
			pMissile->SetSecondAttribute( m_vSecondAttributeList );
		}
	}
	else
	{
		if( m_FireState == FS_AIMED_FIRE || m_FireState == FS_AIMED_EXTEND_FIRE )
		{
			vAimedDir = ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
			pWeapon->SetMoveDir( vAimedDir );
			m_bSetAimedDir = false;
		}

		if( pArrow || pMissile )
			SetFireEffect( pWeapon->GetOwner(), vAimedDir );

		if( m_bZeroAimState )
		{
			if( pArrow && m_vSecondZeroAttributeList.size() > 0 )
			{
				pArrow->SetSecondAttribute( m_vSecondZeroAttributeList );
			}
			else if( pMissile && m_vSecondZeroAttributeList.size() > 0 )
			{
				pMissile->SetSecondAttribute( m_vSecondZeroAttributeList );
			}
		}
		else
		{
			if( pArrow && m_vSecondAttributeList.size() > 0 )
			{
				pArrow->SetSecondAttribute( m_vSecondAttributeList );
			}
			else if( pMissile && m_vSecondAttributeList.size() > 0 )
			{
				pMissile->SetSecondAttribute( m_vSecondAttributeList );
			}
		}

		if( m_fCurChargeRate > 0.0f )
		{
			float fCurDamageRate, fCurForceRate, fCurRangeRate, fCurSpeedRate, fCurBlowRate, fCurFloatRate;
			float fCurLiveTimeRate;
			fCurDamageRate = fCurForceRate = fCurRangeRate = fCurSpeedRate = fCurBlowRate = fCurFloatRate = FLOAT1;
			fCurLiveTimeRate = FLOAT1;

			float fCurMaxDamage = max( 0.0f, (m_fMaxDamageRate - FLOAT1) );
			float fCurMaxForce = max( 0.0f, (m_fMaxForceRate - FLOAT1) );
			float fCurMaxBlow = max( 0.0f, (m_fMaxBlowRate - FLOAT1) );

			float fCurMaxRange = max( 0.0f, (m_fMaxRangeRate - FLOAT1) );
			float fCurMaxSpeed = max( 0.0f, (m_fMaxSpeedRate - FLOAT1) );
			float fCurMaxFloat = max( 0.0f, (m_fMaxFloatRate - FLOAT1) );
			float fCurMaxLiveTime = max( 0.0f, (m_fMaxLiveTimeRate - FLOAT1) );

			fCurDamageRate += fCurMaxDamage * m_fCurChargeRate;
			fCurForceRate += fCurMaxForce * m_fCurChargeRate;
			fCurBlowRate += fCurMaxBlow * m_fCurChargeRate;

			fCurRangeRate += fCurMaxRange * m_fCurChargeRate;
			fCurSpeedRate += fCurMaxSpeed * m_fCurChargeRate;
			fCurFloatRate += fCurMaxFloat * m_fCurChargeRate;

			fCurLiveTimeRate += fCurMaxLiveTime * m_fCurChargeRate;

			pWeapon->SetOwnerChargeDamageRate( fCurDamageRate );
			pWeapon->SetOwnerChargeForceRate( fCurForceRate );
			pWeapon->SetOwnerChargeBlowRate( fCurBlowRate );

			pWeapon->SetOwnerChargeRangeRate( fCurRangeRate );
			pWeapon->SetOwnerChargeSpeedRate( fCurSpeedRate );
			pWeapon->SetOwnerChargeFloatRate( fCurFloatRate );
			
			pWeapon->SetOwnerChargeLiveTimeRate( fCurLiveTimeRate );
		}
	}

	m_fCurChargeRate = 0.0f;
	m_dwAimStartTime = 0;
}

ioWeaponItem::WeaponSubType ioBowItem::GetSubType() const
{
	return WST_BOW;
}

void ioBowItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	pOwner->SetTargetMarker( ioTargetMarker::MT_BOW_ITEM,
							 m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker,"" );

	pOwner->SetTargetMarkerRange( m_fMarkerRange );

	m_dwCheckCoolTime = FRAMEGETTIME();
}

void ioBowItem::OnReleased( ioBaseChar *pOwner )
{
	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "","" );

	m_fReloadTimeModifyRate = FLOAT1;
	m_fFireTimeModifyRate = FLOAT1;

	ioWeaponItem::OnReleased( pOwner );

	if( !m_szDestoyrDummyCharOnReleased.IsEmpty() )
	{
		ioDummyChar* pDummy = g_DummyCharMgr.FindMyDummyCharToName( pOwner, m_szDestoyrDummyCharOnReleased );
		if( pDummy )
		{
			pDummy->SendDieState( true );
		}
	}
}

bool ioBowItem::IsCanFire( const ioBaseChar* pOwner, int iFireCnt, bool bNormalAttack ) const
{
	switch( m_BulletType )
	{
	case BCT_COUNT:
		{
			if( m_iMaxBullet == 0 )
				return true;

			if( GetJumpType() != JT_AIM_JUMP && GetJumpType() != JT_AUTO_SHOT_JUMP )
				return true;

			if( bNormalAttack )
				return true;

			if( m_iCurBullet >= iFireCnt )
				return true;
		}
		break;
	case BCT_CHARGE:
		{
			if( bNormalAttack )
				return true;

			if( iFireCnt > 0 )
			{
				DWORD dwCurTime = FRAMEGETTIME();
				DWORD dwGapTime = dwCurTime - m_dwCheckCoolTime;

				DWORD dwCurCoolTime = m_dwCurCoolTime + dwGapTime;
				dwCurCoolTime = max( 0, min( dwCurCoolTime, m_dwMaxCoolTime ) );

				if( dwCurCoolTime < m_dwNeedCoolTime )
					return false;
			}
		}
		// HARDCODE :: break 아닌점 유의
		return true;
	}

	return false;
}

void ioBowItem::ModifyFireTimeRate( float fRate )
{
	m_fFireTimeModifyRate *= fRate;
}

void ioBowItem::WasteBullet()
{
	if( m_iMaxBullet == 0 ) return;

	m_iCurBullet--;
	m_iCurBullet = max( m_iCurBullet, 0 );
}

void ioBowItem::SetAimedSkillUse( bool bSkillUsed )
{
	m_bSetSkillAimed = bSkillUsed;
}

void ioBowItem::SetAimedDirSet( bool bAimedSet )
{
	m_bSetAimedDir = bAimedSet;
}

D3DXVECTOR3 ioBowItem::ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon )
{
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !pTarget && !m_bAimedBall )
		return m_vAimedDir;

	ioBall *pBall = pOwner->FindBall();
	if( !pTarget && !pBall ) return m_vAimedDir;

	D3DXVECTOR3 vTargetPos;
	D3DXVECTOR3 vWeaponPos = pWeapon->GetPosition();

	if( pTarget )
	{
		if( CheckDownAimState( pTarget ) )
			vTargetPos = pTarget->GetMidPositionByRate( m_fDownTargetAimRate );
		else
			vTargetPos = pTarget->GetMidPositionByRate( m_fTargetAimRate );
	}
	else if( m_bAimedBall && pBall )
		vTargetPos = pBall->GetMidPositionByRate();

	D3DXVECTOR3 vAimedDir =  vTargetPos - vWeaponPos;
	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	
	vAimedDir.x = m_vAimedDir.x;
	vAimedDir.z = m_vAimedDir.z;

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	return vAimedDir;
}

bool ioBowItem::CheckDownAimState( ioBaseChar *pTarget )
{
	if( !pTarget ) return false;

	CharState eState = pTarget->GetState();

	if( eState == CS_FROZEN && pTarget->GetBlowFrozenState() && m_bEnableDownAim )
		return true;

	if( eState == CS_BLOW_WOUNDED && !pTarget->IsFloatingState() && m_bEnableDownAim )
		return true;

	if( eState == CS_BOUND_BLOW_WOUNDED && !pTarget->IsFloatingState() && m_bEnableDownAim )
		return true;

	if( eState == CS_ETC_STATE && pTarget->IsApplyDownState(false) && m_bEnableDownAim )
		return true;

	if( eState == CS_USING_SKILL && pTarget->IsApplyDownState(false) && m_bEnableDownAim )
		return true;

	if( eState == CS_STOP_MOTION && pTarget->GetBlowStopMotionState() && m_bEnableDownAim )
		return true;

	return false;
}

bool ioBowItem::CheckAimState(bool bAimMove)
{
	if( !bAimMove )
	{
		switch( m_FireState )
		{
		case FS_AIMING:
		case FS_AIM_WAIT:
		case FS_AIMED:
			return true;
		}
	}

	switch( m_FireState )
	{
	case FS_AIMING:
	case FS_AIM_WAIT:
	case FS_AIMED:
		return true;
	}

	return false;
}

bool ioBowItem::SetAimState( ioBaseChar *pChar )
{
	if( m_iMaxBullet > 0 )
	{
		if( pChar->IsNeedProcess() && m_iCurBullet < 1 )
			return false;
	}

	if( pChar->GetState() == CS_JUMP && CheckAimState() )
	{
		m_dwCheckAimCancelTime = 0;
		pChar->GetGroup()->ClearAllActionAni( FLOAT100, true );
		pChar->SetState( CS_ATTACK );
		pChar->SetAttackMoveEnable( true );
		m_bFirstAimMoveMotion = true;
		m_bAimMotionSetted = true;

		if( m_FireState == FS_AIMED || m_FireState == FS_AIMING )
		{
			if( !m_bEnableZeroAiming )
			{
				m_dwZeroAimStartTime = FRAMEGETTIME();
			}
		}
	}
	else
	{
		ChangeToAiming( pChar );

		if( !pChar->IsJumpState() && pChar->GetState() != CS_ATTACK )
		{
			pChar->SetState( CS_ATTACK );
		}
	}

	pChar->SetChargingState( true );
	m_bAimJumpAttack = false;

	return true;
}

void ioBowItem::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	m_AimedTarget.Clear();
	m_bAimedBall = false;

	m_bSetSkillAimed = false;
	m_bSetAimedDir = false;
	m_bAimedState = false;

	m_bZeroAimState = false;
	m_dwZeroAimStartTime = 0;

	m_FireState = FS_NONE;
	m_AimJumpState = AJS_NONE;
}

bool ioBowItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	ClearAirJump();

	AttackType eExtendType = eType;
	if( GetJumpType() == JT_AIM_JUMP )
		eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioBowItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );

	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioBowItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;
	m_dwFireDelayTime = pAttr->m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID,
							     pAttr->m_vWeaponInfoList,
							     FTT_JUMP_ATTACK,
							     fTimeRate,
							     dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	m_dwFireStateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStateEndTime += FRAMEGETTIME();

	m_bAimJumpAttack = true;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	// 에니메이션 처리
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwFireStartTime = FRAMEGETTIME();
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	if( m_FireState == FS_AIMED )
	{
		float fWeight = GetAimedUpDownWeight( m_vAimedDir, 85.0f );
		if( pAttr->m_AttackEndAnimation.IsEmpty() )
		{
			if( m_vAimedDir.y >= 0.0f )
			{
				AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true );
			}
			else
			{
				AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true );
			}
		}
		else
		{
			if( m_vAimedDir.y >= 0.0f )
			{
				AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, false );
			}
			else
			{
				AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, false );
			}

			float fEndAniRate = pAttr->m_fAttackEndAniRate;
			pGrp->SetActionAni( pAttr->m_AttackEndAnimation,
								0.0f, FLOAT100,
								1.0f, FLOAT1/fEndAniRate,
								m_dwFireEndTime, 0.0f, true );

			int iEndAniID = pGrp->GetAnimationIdx( pAttr->m_AttackEndAnimation );
			m_dwFireDelayTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndAniRate;
		}
	}
	else
	{
		if( pAttr->m_AttackEndAnimation.IsEmpty() )
		{
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
		}
		else
		{
			pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

			float fEndAniRate = pAttr->m_fAttackEndAniRate;
			pGrp->SetActionAni( pAttr->m_AttackEndAnimation,
								0.0f, FLOAT100,
								1.0f, FLOAT1/fEndAniRate,
								m_dwFireEndTime, 0.0f, true );

			int iEndAniID = pGrp->GetAnimationIdx( pAttr->m_AttackEndAnimation );
			m_dwFireDelayTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndAniRate;
		}
	}

	if( m_FireState == FS_AIMED )
		m_FireState = FS_AIMED_FIRE;
	else
		m_FireState = FS_NORMAL_FIRE;

	if( m_bUseBulletChangeDir )
		g_WeaponMgr.SetReturnPingpong( pOwner );

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	pOwner->SetAttackMoveEnable( false );

	if( m_ZeroAimedEffectID != -1 )
	{
		pOwner->EndEffect( m_ZeroAimedEffectID, false );
		m_ZeroAimedEffectID = -1;
	}

	DecreaseCoolTime();

	return true;
}

void ioBowItem::CheckJumpAttack( ioBaseChar *pOwner )
{
	int iMaxCnt = m_vAirJumpTimeList.size();

	if( m_iCurAirJump < iMaxCnt )
	{
		DWORD dwCurAirJumpTime = 0;
		std::list< float >::iterator iter = m_vAirJumpTimeList.begin();

		for( int i=0; iter != m_vAirJumpTimeList.end(); i++ )
		{
			if( m_iCurAirJump == i )
			{
				dwCurAirJumpTime = (DWORD)(*iter);
				break;
			}
			
			iter++;
		}

		if( dwCurAirJumpTime > 0 && dwCurAirJumpTime < FRAMEGETTIME() )
		{
			if( m_fAirJumpPower >= 0.0f )
			{
				D3DXQUATERNION qtTargetRot = pOwner->GetTargetRot();
				pOwner->SetAirJumpPower( m_fAirJumpPower*m_fAirJumpPowerRate );
				D3DXVECTOR3 vAttDir =  qtTargetRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
				if( m_fAirForcePower < 0.0f )
					pOwner->SetForcePower( -vAttDir, fabs(m_fAirForcePower), FLOAT1, true );
				else
					pOwner->SetForcePower( vAttDir, fabs(m_fAirForcePower), FLOAT1, true );
			}
			m_iCurAirJump++;
		}
	}

	if( m_bAimEffect && m_dwFireStateEndTime < FRAMEGETTIME() )
	{
		m_bAimEffect = false;
		pOwner->EndEffect( m_AimEffect, false );
	}

	UpdateAimDir( pOwner );
}

bool ioBowItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_FireState == FS_CHARGING ||
			m_FireState == FS_AIMING ||
			m_FireState == FS_AIM_WAIT ||
			m_FireState == FS_AIMED )
			return true;
	}

	return false;
}

void ioBowItem::CheckAimMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanAttackMove() )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		bool bChangeDir = pOwner->IsChangeDirection();
		if( bChangeDir || m_bFirstAimMoveMotion )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx(m_AimMoveFront);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveBack);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveLeft);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveRight);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			
			m_bAimMotionSetted = false;
			m_bFirstAimMoveMotion = false;

			if( bChangeDir )
				m_bChangeDir = true;

			if( m_bChargeCharRotate )
			{
				pGrp->SetLoopAni( m_AimMoveFront, FLOAT100, FLOAT1, FLOAT1/m_fAimMoveAniRate );
			}
			else
			{
				ioUserKeyInput::DirKeyInput eXformDir;
				eXformDir = pOwner->GetXformDir( m_AimStartDirKey );

				switch( eXformDir )
				{
				case ioUserKeyInput::DKI_UP:
					pGrp->SetLoopAni( m_AimMoveFront, FLOAT100, FLOAT1, m_fAimMoveAniRate );
					break;
				case ioUserKeyInput::DKI_RIGHTUP:
				case ioUserKeyInput::DKI_RIGHT:
				case ioUserKeyInput::DKI_RIGHTDOWN:
					pGrp->SetLoopAni( m_AimMoveRight, FLOAT100, FLOAT1, m_fAimMoveAniRate );
					break;
				case ioUserKeyInput::DKI_DOWN:
					pGrp->SetLoopAni( m_AimMoveBack, FLOAT100, FLOAT1, m_fAimMoveAniRate );
					break;
				case ioUserKeyInput::DKI_LEFTDOWN:
				case ioUserKeyInput::DKI_LEFT:
				case ioUserKeyInput::DKI_LEFTUP:
					pGrp->SetLoopAni( m_AimMoveLeft, FLOAT100, FLOAT1, m_fAimMoveAniRate );
					break;
				}
			}

			g_TutorialMgr.ActionAimingMove();
		}
		else
		{
			m_bChangeDir = false;
		}
	}
	else
	{
		if( !m_bAimMotionSetted )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx(m_AimMoveFront);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveBack);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveLeft);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveRight);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );

			m_bAimMotionSetted = true;
			m_bChangeDir = false;
		}
	}
}

void ioBowItem::OnEndExtendDefense( ioBaseChar *pOwner )
{
	if( !m_ReloadEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_ReloadEffect, false );
		pOwner->ShowWeaponItemMesh( true );
	}
}

void ioBowItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	if( m_ZeroAimedEffectID != -1 )
	{
		pOwner->EndEffect( m_ZeroAimedEffectID, false );
		m_ZeroAimedEffectID = -1;
	}

	if( m_bAimEffect )
	{
		if( m_AimJumpState != AJS_READY || bReleaseItem )
		{
			m_bAimEffect = false;
			pOwner->EndEffect( m_AimEffect, false );
		}
	}
}

bool ioBowItem::SetAimJumpState( ioBaseChar *pChar )
{
	if( pChar->GetState() == CS_JUMP )
		return true;
	
	if( GetJumpType() != JT_AIM_JUMP && pChar->IsJumpKeyPress() && pChar->IsCanJumpState() && !pChar->IsBuffLimitJump() )
	{
		pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE );
		pChar->SetJumpState();
		m_FireState = FS_NONE;
		g_TutorialMgr.ActionAimingCancel();
		return false;
	}

	if( pChar->IsJumpKeyPress() )
	{
		if( m_dwCheckAimCancelTime == 0 )
		{
			if( m_pExtendJump && GetJumpType() == JT_AIM_JUMP )
			{
				// AimJump Ready
				SetAimJumpReady( pChar );
			}
		}
	}

	if( m_dwCheckAimCancelTime > 0 )
	{
		if( pChar->IsJumpKeyDown() && !pChar->IsHasCrown() )
		{
			if( m_dwCheckAimCancelTime < FRAMEGETTIME() )
			{
				if( m_pExtendJump && GetJumpType() == JT_AIM_JUMP )
				{
					// 조준해제
					//SetAimJumpCancel( pChar );
					SetAimJumpping( pChar, true );
					return false;
				}
			}
		}
		else
		{
			// AimJump Set
			SetAimJumpping( pChar );
			return false;
		}
	}

	return true;
}

void ioBowItem::SetAimJumpReady( ioBaseChar *pChar )
{
	m_dwCheckAimCancelTime = FRAMEGETTIME();
	m_dwCheckAimCancelTime += GetJumpChargeTime( pChar );

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( m_AimJumpReadyAnimation.IsEmpty() )
		pGrp->SetActionAni( pChar->GetJumpReadyAnimationIdx(), FLOAT100, FLOAT100, FLOAT1, FLOAT1, 0.0f, 0.0f, true );
	else
		pGrp->SetActionAni( m_AimJumpReadyAnimation, FLOAT100, FLOAT100, FLOAT1, FLOAT1, 0.0f, 0.0f, true );

	m_AimJumpState = AJS_READY;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << true;		// AimJumpState
		kPacket << (int)m_AimJumpState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBowItem::SetAimJumpping( ioBaseChar *pChar, bool bFullTime )
{
	m_dwCheckAimCancelTime = 0;
	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetLoopAni( m_JumpAimCenter, FLOAT100, FLOAT1 );
	pGrp->SetLoopAni( m_JumpAimUp, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_JumpAimDown, 0.0f, 0.0f );

	if( m_bZeroAimState )
	{
		if( !m_bEnableZeroAiming )
		{
			switch( m_FireState )
			{
			case FS_AIMING:
			case FS_AIM_WAIT:
				pChar->SetTargetMarkerState( ioTargetMarker::MS_READY );
				break;
			case FS_AIMED:
				pChar->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
				break;
			}

			SetZeroAim( pChar, false );
		}
	}
	pChar->SetAimJumpState( bFullTime );

	m_AimJumpState = AJS_JUMPPING;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << true;		// AimJumpState
		kPacket << (int)m_AimJumpState;
		kPacket << bFullTime;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBowItem::SetAimJumpCancel( ioBaseChar *pChar )
{
	m_dwCheckAimCancelTime = 0;
	m_FireState = FS_NONE;

	pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	pChar->SetState( CS_DELAY );

	if( m_ZeroAimedEffectID != -1 )
	{
		pChar->EndEffect( m_ZeroAimedEffectID, false );
		m_ZeroAimedEffectID = -1;
	}

	m_bSetAimedDir = false;
	m_bAimedState = false;
	m_AimJumpState = AJS_CANCEL;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << true;		// AimJumpState
		kPacket << (int)m_AimJumpState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

const ioHashString& ioBowItem::GetJumppingAnimation() const
{
	bool bAim = false;
	switch( m_FireState )
	{
	case FS_AIMING:
	case FS_AIM_WAIT:
	case FS_AIMED:
	case FS_AIMED_FIRE:
		bAim = true;
	}

	if( bAim && !m_AimJumppingAnimation.IsEmpty() )
		return m_AimJumppingAnimation;

	return ioAttackableItem::GetJumppingAnimation();
}

void ioBowItem::CheckAimJumpState( ioBaseChar *pChar )
{
	switch( m_FireState )
	{
	case FS_AIMING:
		OnAiming( pChar );
		break;
	case FS_AIM_WAIT:
		OnAimWait( pChar );
		break;
	case FS_AIMED:
		OnAimed( pChar );
		break;
	case FS_NORMAL_FIRE:
	case FS_AIMED_FIRE:
		OnJumpFire( pChar );
		break;
	}

	if( m_FireState != FS_AIMED && m_FireState != FS_AIMING &&
		m_FireState != FS_AIM_WAIT && m_dwZeroAimStartTime > 0 )
	{
		if( !m_bEnableZeroAiming )
			m_dwZeroAimStartTime = 0;
	}

	if( m_bEnablePowerCharge &&	(m_FireState == FS_AIMING || m_FireState == FS_AIM_WAIT || m_FireState == FS_AIMED) )
	{
		CheckChargeRate();
		CheckTargetMarker( pChar );
	}

	if( !m_bEnableZeroAiming && m_bZeroAimState )
	{
		if( m_ZeroAimedEffectID != -1 )
		{
			pChar->EndEffect( m_ZeroAimedEffectID, false );
			m_ZeroAimedEffectID = -1;
		}
	}
	else if( m_bEnableZeroAiming && m_bZeroAimState )
	{
		if( m_ZeroAimedEffectID == -1 )
		{
			ioEffect *pEffect = pChar->AttachEffect( m_ZeroAimedEffect );
			if( pEffect )
				m_ZeroAimedEffectID = pEffect->GetUniqueID();
		}
	}

	UpdateAimDir( pChar );
}


bool ioBowItem::CheckLanding( ioBaseChar *pChar )
{
	if( !CheckAimState() )
	{
		pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE );
		pChar->GetGroup()->ReleaseAllExtraRotate();
		SetNoneState( pChar );
		return true;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( 0.0f );

	pGrp->SetLoopAni( m_AimCenter, 0.0f );
	pGrp->SetLoopAni( m_AimUp, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_AimDown, 0.0f, 0.0f );
	return false;
}

bool ioBowItem::CheckZeroAim( ioBaseChar *pOwner )
{
	if( m_bZeroAimState ) return false;

	if( m_dwZeroAimStartTime == 0 ) return false;

	if( m_dwZeroAimStartTime+m_dwZeroAimedCheckTime > FRAMEGETTIME() )
		return false;

	SetZeroAim( pOwner, true );
	return true;
}

void ioBowItem::SetZeroAim( ioBaseChar *pOwner, bool bSet )
{
	if( bSet )
	{
		m_bZeroAimState = true;
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_ZERO_AIMED );

		if( m_ZeroAimedEffectID != -1 )
		{
			pOwner->EndEffect( m_ZeroAimedEffectID, false );
			m_ZeroAimedEffectID = -1;
		}

		ioEffect *pEffect = pOwner->AttachEffect( m_ZeroAimedEffect );
		if( pEffect )
			m_ZeroAimedEffectID = pEffect->GetUniqueID();

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );

		pGrp->SetLoopAni( m_ZeroAimUp, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_ZeroAimDown, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_ZeroAimCenter, 0.0f, FLOAT1 );

		m_bFirstAimMoveMotion = true;
	}
	else
	{
		m_bZeroAimState = false;
		m_dwZeroAimStartTime = 0;

		if( m_ZeroAimedEffectID != -1 )
		{
			pOwner->EndEffect( m_ZeroAimedEffectID, false );
			m_ZeroAimedEffectID = -1;
		}
	}
}

bool ioBowItem::CheckExtendDefenseEnable( ioBaseChar *pChar )
{
	if( m_iCurBullet == m_iMaxBullet)
		return false;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ReloadAnimation );
	if( iAniID == -1 )	return false;

	return true;
}

void ioBowItem::CheckDefenseMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanReloadMove() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		if( !m_bSetReloadMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx(m_ReloadMoveAnimation);
			if( iAniID == -1 )
				return;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = pGrp->GetLoopInPlay( m_ReloadAnimation );
			
			if( pAni )
				dwPlayTime = pAni->GetCurTime();

			pGrp->ClearAllActionAni( FLOAT100, false );
			pGrp->ClearAllLoopAni( FLOAT100, false );
			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/m_fReloadTimeModifyRate );
			m_bSetReloadMoveAni = true;
		}
	}
	else
	{
		if( m_bSetReloadMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx(m_ReloadAnimation);
			if( iAniID == -1 )
				return;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = pGrp->GetLoopInPlay( m_ReloadMoveAnimation );
			
			if( pAni )
				dwPlayTime = pAni->GetCurTime();

			pGrp->ClearAllActionAni( FLOAT100, false );
			pGrp->ClearAllLoopAni( FLOAT100, false );
			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/m_fReloadTimeModifyRate );
			m_bSetReloadMoveAni = false;
		}
	}
}

float ioBowItem::GetAttackSpeedRate() const
{
	if( m_bZeroAimState && m_fZeroAimSpeedRate > 0.0f )
		return m_fZeroAimSpeedRate;
	else
		return m_fAttackSpeedRate;
}

bool ioBowItem::IsAimedState()
{
	if( m_FireState == FS_AIMED )
		return true;

	return false;
}

bool ioBowItem::CompareAimedTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) return false;

	float fAimRange = GetChargeAimRange();
	float fAimAngle = GetChargeAimAngle();

	ioBaseChar *pChar = pOwner->GetBaseChar( m_AimedTarget );

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vCharDir  = pOwner->GetMoveDir();

	float fHalfCosine = cosf( DEGtoRAD( fAimAngle ) * FLOAT05 );

	float fLimitDistSq = fAimRange * fAimRange;
	float fMinDistSq = m_fMinRange * m_fMinRange;

	float fBallDist = std::numeric_limits<float>::infinity();
	float fCharDist = std::numeric_limits<float>::infinity();

	// Ball
	if( pBall )
	{
		vDiff = pBall->GetWorldPosition() - vOwnerPos;
		float fCurDistSq = D3DXVec3LengthSq( &vDiff );

		if( (fLimitDistSq >= fCurDistSq) && (fMinDistSq <= fCurDistSq) )
		{
			vDiff.y = 0.0f;
			D3DXVec3Normalize( &vDiff, &vDiff );

			if( D3DXVec3Dot( &vCharDir, &vDiff ) >= fHalfCosine )
				return true;
		}
	}

	/*
	// Char
	if( pChar )
	{
		vDiff = pChar->GetWorldPosition() - vOwnerPos;
		float fCurDistSq = D3DXVec3LengthSq( &vDiff );

		if( (fLimitDistSq >= fCurDistSq) && (fMinDistSq <= fCurDistSq) )
		{
			vDiff.y = 0.0f;
			D3DXVec3Normalize( &vDiff, &vDiff );

			if( D3DXVec3Dot( &vCharDir, &vDiff ) >= fHalfCosine )
				fCharDist = fCurDistSq;
		}
	}

	if( fBallDist < fCharDist )
	{
		return true;
	}
	*/

	return false;
}

void ioBowItem::CheckChargeRate()
{
	m_fCurChargeRate = 0.0f;

	if( m_bEnablePowerCharge && m_dwZeroAimStartTime > 0 )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		DWORD dwGapTime = dwCurTime - m_dwZeroAimStartTime;

		if( m_dwZeroAimedCheckTime > 0 )
		{
			if( m_dwZeroAimedCheckTime > dwGapTime )
				m_fCurChargeRate = (float)dwGapTime / m_dwZeroAimedCheckTime;
			else
				m_fCurChargeRate = FLOAT1;
		}
	}
}

void ioBowItem::CheckTargetMarker( ioBaseChar *pOwner )
{
	if( m_fCurChargeRate > 0.0f )
	{
		float fCurMarkerRangeRate = FLOAT1;
		float fCurMaxMarkerRange = max( 0.0f, (m_fMaxMarkerRangeRate - FLOAT1) );

		fCurMarkerRangeRate += fCurMaxMarkerRange * m_fCurChargeRate;

		float fCurRange = m_fMarkerRange * fCurMarkerRangeRate;
		pOwner->SetTargetMarkerRange( fCurRange );
	}
}

float ioBowItem::GetChargeAimRange()
{
	if( m_fCurChargeRate > 0.0f )
	{
		float fCurAimRangeRate = FLOAT1;
		float fCurMaxAimRange = max( 0.0f, (m_fMaxAimRangeRate - FLOAT1) );

		fCurAimRangeRate += fCurMaxAimRange * m_fCurChargeRate;

		float fCurRange = m_fAimRange * fCurAimRangeRate;
		return fCurRange;
	}

	return m_fAimRange;
}

float ioBowItem::GetChargeAimAngle()
{
	if( m_fCurChargeRate > 0.0f )
	{
		float fCurAimAngleRate = FLOAT1;
		float fCurMaxAimAngle = max( 0.0f, (m_fMaxAimAngleRate - FLOAT1) );

		fCurAimAngleRate += fCurMaxAimAngle * m_fCurChargeRate;

		float fCurAngle = m_fAimAngle * fCurAimAngleRate;
		return fCurAngle;
	}

	return m_fAimAngle;
}

void ioBowItem::SetFireEffect( ioBaseChar *pOwner, const D3DXVECTOR3 &vDir )
{
	if( !pOwner ) return;
	if( m_FireEffect.IsEmpty() ) return;

	if( m_bAimedState )
	{
		pOwner->CreateMapEffectWithDir( m_FireEffect, vDir );
	}
	else
	{
		D3DXVECTOR3 vNormalDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		pOwner->CreateMapEffectWithDir( m_FireEffect, vNormalDir );
	}
}

bool ioBowItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !pOwner )
		return false;
	if( pOwner->GetState() != CS_ATTACK )
		return false;

	switch( m_FireState )
	{
	case FS_AIMING:
	case FS_AIM_WAIT:
	case FS_AIMED:
		return true;
	case FS_NORMAL_ATTACK:
	case FS_NORMAL_FIRE:
	case FS_AIMED_FIRE:
		if( IsChargeDash() && pOwner->IsEnableReserveKeyInput() )
			return true;
		break;
	}

	return false;
}

int ioBowItem::GetMaxBullet()
{
	if( m_BulletType == BCT_CHARGE )
		return (int)m_dwMaxCoolTime;

	return m_iMaxBullet;
}

int ioBowItem::GetNeedBullet()
{
	return (int)m_dwNeedCoolTime;
}

int ioBowItem::GetCurBullet()
{
	if( m_BulletType == BCT_CHARGE )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		DWORD dwGapTime = dwCurTime - m_dwCheckCoolTime;

		m_dwCheckCoolTime = dwCurTime;

		m_dwCurCoolTime += dwGapTime;
		m_dwCurCoolTime = max( 0, min( m_dwCurCoolTime, m_dwMaxCoolTime ) );

		return (int)m_dwCurCoolTime;
	}

	return m_iCurBullet;
}

void ioBowItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	switch( m_BulletType )
	{
	case BCT_COUNT:
		{
			int iBullet = min( iCurBullet, m_iMaxBullet );
			iBullet = max( 0, iBullet );

			m_iCurBullet = iBullet;
		}
		break;
	case BCT_CHARGE:
		{
			m_dwCheckCoolTime = FRAMEGETTIME();

			m_dwCurCoolTime = (DWORD)iCurBullet;
			m_dwCurCoolTime = max( 0, min( m_dwCurCoolTime, m_dwMaxCoolTime ) );
		}
		break;
	}
}

void ioBowItem::InitCoolTime()
{
	m_dwCurCoolTime = 0;
	m_dwCheckCoolTime = FRAMEGETTIME();
}

void ioBowItem::MaxCoolTime()
{
	m_dwCurCoolTime = m_dwMaxCoolTime;
	m_dwCheckCoolTime = FRAMEGETTIME();
}

void ioBowItem::DecreaseCoolTime()
{
	if( m_dwCurCoolTime < m_dwNeedCoolTime )
	{
		m_dwCurCoolTime = 0;
	}
	else
	{
		m_dwCurCoolTime -= m_dwNeedCoolTime;
	}
}

bool ioBowItem::IsChargeBulletType()
{
	if( m_BulletType == BCT_CHARGE )
		return true;

	return false;
}

//Expansion
void ioBowItem::ChangeExtendAttack( ioBaseChar *pOwner, bool bAimed, int nFireIndex )
{
	if( m_bZeroAimState )
	{
		if ( nFireIndex >= (int)m_vZeroExtendAddAtrributeList.size() )
		{
			m_FireState = FS_NONE;
			return;
		}
	}
	else
	{
		if ( nFireIndex >= (int)m_vExtendExtraAttributeList.size() )
		{
			m_FireState = FS_NONE;
			return;
		}
	}

	const AttackAttribute *pAttr = NULL;
	if( m_bZeroAimState )
		pAttr = &m_vZeroExtendAddAtrributeList[ nFireIndex ];
	else
		pAttr = &m_vExtendExtraAttributeList[ nFireIndex ];

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int	  iAniID	  = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float fTimeRate   = pAttr->m_fAttackAniRate * m_fFireTimeModifyRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;
	m_dwFireDelayTime = pAttr->m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID,
		pAttr->m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwFireStartTime = FRAMEGETTIME();
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	m_dwFireStateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStateEndTime += FRAMEGETTIME();

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( !bAimed )
		m_vAimedDir =  pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		if( bAimed && m_bZeroAimState  )
		{
			ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
			if( pTarget )
			{
				pOwner->SetTargetRotToTargetPos( pTarget, true );
			}
		}
		float fWeight = GetAimedUpDownWeight( m_vAimedDir, 85.0f );
		if( pAttr->m_AttackEndAnimation.IsEmpty() )
		{
			if( m_vAimedDir.y >= 0.0f )
			{
				AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true );
			}
			else
			{
				AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true );
			}
		}
		else
		{
			if( m_vAimedDir.y >= 0.0f )
			{
				AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, false );
			}
			else
			{
				AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, false );
			}

			float fEndAniRate = pAttr->m_fAttackEndAniRate;
			int iEndAniID = pGrp->GetAnimationIdx( pAttr->m_AttackEndAnimation );
			m_dwFireDelayTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndAniRate;
		}
	
	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->SetComboCheckTime( m_dwKeyReserveTime, true );
	m_dwKeyReserveTime += FRAMEGETTIME();

	if( bAimed )
		m_FireState = FS_AIMED_EXTEND_FIRE;
	else
		m_FireState = FS_NORAML_EXTEND_FIRE;

	pOwner->SetChargingState( false );

	//SendPacket

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_AimedTarget;
		kPacket << false;
		kPacket << m_iNormalExtendAddAttackIndex;
		kPacket << m_vAimedDir;
		kPacket << m_bSetAimedDir;
		kPacket << bAimed;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iNormalExtendAddAttackIndex++;
}

void ioBowItem::OnExtendFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwAttackOverTime = m_dwFireStartTime + m_dwFireEndTime + m_dwFireDelayTime;

	if( m_FireState == FS_AIMED_EXTEND_FIRE )
	{
		if( !CheckCurrentTargetValid( pOwner, true ) )
		{
			m_dwTrackingEndTime = FRAMEGETTIME();

			// 대상을 제거한후 조준대기로 넘겨야함
			m_AimedTarget.Clear();
			m_bAimedBall = false;

			m_FireState = FS_NORAML_EXTEND_FIRE;

			pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
		}
	}

	if( pOwner->CheckDefaultNormalAttack( m_dwFireStartTime,
		m_dwFireEndTime,
		m_dwFireDelayTime ) )
	{
		m_FireState = FS_NONE;
		return;
	}

	if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() && m_dwKeyReserveTime && dwCurTime > m_dwKeyReserveTime )
	{
		if ( m_bZeroAimState && (int)m_vZeroExtendAddAtrributeList.size() > m_iNormalExtendAddAttackIndex )
		{
			bool bAimed = (m_FireState == FS_AIMED_FIRE) ? true : false;
			pOwner->InitAttackReserve();
			pOwner->ClearReserveKeyInput();
			ChangeExtendAttack( pOwner, bAimed, m_iNormalExtendAddAttackIndex );
		}
		else if ( !m_bZeroAimState && (int)m_vExtendExtraAttributeList.size() > m_iNormalExtendAddAttackIndex )
		{
			bool bAimed = (m_FireState == FS_AIMED_FIRE) ? true : false;
			pOwner->InitAttackReserve();
			pOwner->ClearReserveKeyInput();
			ChangeExtendAttack( pOwner, bAimed, m_iNormalExtendAddAttackIndex );
		}
	}
}

