
#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioWestGunExtendItem.h"

#include "WeaponDefine.h"

ioWestGunExtendItem::ioWestGunExtendItem()
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
	m_dwAirJumpTime = 0;

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

	m_dwStartComboTime = 0;
	m_iChargeCombo = 0;
	m_dwKeyReserveTime = 0;
	m_dwComboAttackTimeEnd = 0;
	m_bAimedState = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
//	m_FirstDirKey = m_CurDirKey;

	m_bCanFireMove = false;
	m_bFireReservedFromShotAndMove = false;
}

ioWestGunExtendItem::ioWestGunExtendItem( const ioWestGunExtendItem &rhs )
: ioWeaponItem( rhs ),
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
 m_dwMaxCoolTime( rhs.m_dwMaxCoolTime ),
 m_dwNeedCoolTime( rhs.m_dwNeedCoolTime ),
 m_MissFire( rhs.m_MissFire ),
 m_fMissFireRate( rhs.m_fMissFireRate ),
 m_bUseBulletChangeDir( rhs.m_bUseBulletChangeDir ),
 m_vChargeComboAttListUp( rhs.m_vChargeComboAttListUp ),
 m_vChargeComboAttListCenter( rhs.m_vChargeComboAttListCenter ),
 m_vChargeComboAttListDown( rhs.m_vChargeComboAttListDown ),
 m_vChargeComboZeroAttListUp( rhs.m_vChargeComboZeroAttListUp ),
 m_vChargeComboZeroAttListCenter( rhs.m_vChargeComboZeroAttListCenter ),
 m_vChargeComboZeroAttListDown( rhs.m_vChargeComboZeroAttListDown ),
 m_fChargeComboAttAngle( rhs.m_fChargeComboAttAngle ),
 m_dwMaxComboDuration( rhs.m_dwMaxComboDuration ),
 m_ShotAndMoveInfo( rhs.m_ShotAndMoveInfo ),
 m_dwRollingAttackCheckTime( rhs.m_dwRollingAttackCheckTime )
{
	m_iMaxBullet = rhs.m_iMaxBullet;
	m_fReloadTimeRate = rhs.m_fReloadTimeRate;

	m_dwAimTime = rhs.m_dwAimTime;
	m_fAimAngle = rhs.m_fAimAngle;
	m_fAimRange = rhs.m_fAimRange;
	m_fMinRange = rhs.m_fMinRange;
	m_fAimRangeInFight = rhs.m_fAimRangeInFight;

	m_fMarkerRange = rhs.m_fMarkerRange;

	if( m_iMaxBullet > 0 )
		m_iCurBullet = m_iMaxBullet;
	else
		m_iCurBullet = 0;

	m_fReloadTimeModifyRate = FLOAT1;
	m_fFireTimeModifyRate = FLOAT1;

	m_dwAirJumpTime = 0;

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

	m_dwStartComboTime = 0;
	m_iChargeCombo = 0;
	m_dwKeyReserveTime = 0;
	m_dwComboAttackTimeEnd = 0;
	m_bAimedState = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
//	m_FirstDirKey = m_CurDirKey;

	m_bCanFireMove = false;
	m_bFireReservedFromShotAndMove = false;
}

ioWestGunExtendItem::~ioWestGunExtendItem()
{
	m_LevelTimeRate.clear();

	m_vSecondAttributeList.clear();
	m_vSecondZeroAttributeList.clear();
}

void ioWestGunExtendItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_iMaxWeaponCnt = rkLoader.LoadInt_e( "max_weapon_cnt", 1 );

	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );

	int iSecondCnt = rkLoader.LoadInt_e( "second_attack_cnt", 0 );
	m_vSecondAttributeList.clear();
	m_vSecondAttributeList.clear();
	for( int i=0; i < iSecondCnt; ++i )
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

	//Add yunga
	// combo att
	int iComboMax = rkLoader.LoadInt_e( "charge_combo_max_cnt", 1 );
	m_vChargeComboAttListUp.clear();
	m_vChargeComboAttListCenter.clear();
	m_vChargeComboAttListDown.clear();
	m_vChargeComboAttListUp.reserve( iComboMax );
	m_vChargeComboAttListCenter.reserve( iComboMax );
	m_vChargeComboAttListDown.reserve( iComboMax );

	m_vChargeComboZeroAttListUp.clear();
	m_vChargeComboZeroAttListCenter.clear();
	m_vChargeComboZeroAttListDown.clear();
	m_vChargeComboZeroAttListUp.reserve( iComboMax );
	m_vChargeComboZeroAttListCenter.reserve( iComboMax );
	m_vChargeComboZeroAttListDown.reserve( iComboMax );

	m_fChargeComboAttAngle = rkLoader.LoadFloat_e( "charge_combo_attack_Angle", 45.0f );
	for( int i=0; i < iComboMax; ++i )
	{
		char szBufTmp[MAX_PATH];

		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "charge_combo_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vChargeComboAttListCenter.push_back( kAttribute );

		wsprintf_e( szBuf, "charge_combo_attack%d_up", i+1 );
		rkLoader.LoadString( szBuf, "", szBufTmp, MAX_PATH );
		kAttribute.m_AttackAnimation = szBufTmp;
		m_vChargeComboAttListUp.push_back( kAttribute );

		wsprintf_e( szBuf, "charge_combo_attack%d_down", i+1 );
		rkLoader.LoadString( szBuf, "", szBufTmp, MAX_PATH );
		kAttribute.m_AttackAnimation = szBufTmp;
		m_vChargeComboAttListDown.push_back( kAttribute );
		///////////////////////////////////////////////////////////////
		wsprintf_e( szBuf, "charge_combo_zero_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vChargeComboZeroAttListCenter.push_back( kAttribute );

		wsprintf_e( szBuf, "charge_combo_zero_attack%d_up", i+1 );
		rkLoader.LoadString( szBuf, "", szBufTmp, MAX_PATH );
		kAttribute.m_AttackAnimation = szBufTmp;
		m_vChargeComboZeroAttListUp.push_back( kAttribute );

		wsprintf_e( szBuf, "charge_combo_zero_attack%d_down", i+1 );
		rkLoader.LoadString( szBuf, "", szBufTmp, MAX_PATH );
		kAttribute.m_AttackAnimation = szBufTmp;
		m_vChargeComboZeroAttListDown.push_back( kAttribute );

	}

	m_dwMaxComboDuration = (DWORD)rkLoader.LoadInt_e( "charge_combo_max_time", 1 );
	//////////////////////////////////////////////////////////////////////////

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
	m_fAimRangeInFight = rkLoader.LoadFloat_e( "aim_range_in_fight", FLOAT1000 );

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

	m_dwMaxCoolTime = (DWORD)rkLoader.LoadInt_e( "max_cool_time", 0 );
	m_dwNeedCoolTime = (DWORD)rkLoader.LoadInt_e( "use_cool_time", 0 );

	rkLoader.LoadString_e( "miss_fire_animation", "", szBuf, MAX_PATH );
	m_MissFire = szBuf;
	m_fMissFireRate = rkLoader.LoadFloat_e( "miss_fire_ani_rate", FLOAT1 );

	m_bUseBulletChangeDir = rkLoader.LoadBool_e( "use_bullet_charge_dir", false );


	//쏘고 이동시 정보 로드
	rkLoader.LoadString_e( "charge_move_ani_fr", "", szBuf, MAX_PATH );
	m_ShotAndMoveInfo.m_stShotAndMoveFr = szBuf;

	rkLoader.LoadString_e( "charge_move_ani_bk", "", szBuf, MAX_PATH );
	m_ShotAndMoveInfo.m_stShotAndMoveBk = szBuf;

	rkLoader.LoadString_e( "charge_move_ani_rt", "", szBuf, MAX_PATH );
	m_ShotAndMoveInfo.m_stShotAndMoveRt = szBuf;

	rkLoader.LoadString_e( "charge_move_ani_lt", "", szBuf, MAX_PATH );
	m_ShotAndMoveInfo.m_stShotAndMoveLt = szBuf;

	m_ShotAndMoveInfo.m_fShotAndMoveRateFr		= rkLoader.LoadFloat_e( "charge_move_ani_rate_fr", 0 );
	m_ShotAndMoveInfo.m_fShotAndMoveRateBk		= rkLoader.LoadFloat_e( "charge_move_ani_rate_bk", 0 );
	m_ShotAndMoveInfo.m_fShotAndMoveRateRr		= rkLoader.LoadFloat_e( "charge_move_ani_rate_rt", 0 );
	m_ShotAndMoveInfo.m_fShotAndMoveRateLr		= rkLoader.LoadFloat_e( "charge_move_ani_rate_lt", 0 );

	m_ShotAndMoveInfo.m_fShotAndMoveForce		= rkLoader.LoadFloat_e( "charge_force_move", 0 );
	m_ShotAndMoveInfo.m_fShotAndMoveFriction	= rkLoader.LoadFloat_e( "charge_force_friction", 0 );
	//////////////////////////////////////////////////////////////////////////
	m_dwRollingAttackCheckTime = (DWORD)rkLoader.LoadInt( "rolling_attack_check_time", 3000 );
}


ioItem* ioWestGunExtendItem::Clone()
{
	return new ioWestGunExtendItem( *this );
}


bool ioWestGunExtendItem::SetExtendDefenseState( ioBaseChar *pOwner )
{
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

void ioWestGunExtendItem::CheckExtendDefenseState( ioBaseChar *pOwner )
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
			kPacket << m_iChargeCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	g_TutorialMgr.ActionWeaponCharge();
}

void ioWestGunExtendItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	//LOG.PrintTimeAndLog( 0, "SetNormalAttack");

	ClearCancelInfo();

	if( m_AimedTarget.IsEmpty() )
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	m_fCurChargeRate = 0.0f;

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

	int iExtendCnt = m_vChargeComboAttListCenter.size();
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

void ioWestGunExtendItem::ChangeToAiming( ioBaseChar *pOwner )
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
	pOwner->SetAttackMoveEnable( false );
	pOwner->ChangeDirectionExplicitIfKeyChanged( true );
	
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

void ioWestGunExtendItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_FireState )
	{
	case FS_CHARGING:
		m_bCanFireMove = false;
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
		if( CheckDashInput(pOwner) )
		{
			ChangeMove( pOwner );
			return;
		}
		OnAiming( pOwner );
		break;
	case FS_AIM_WAIT:
		if( CheckDashInput(pOwner) )
		{
			ChangeMove( pOwner );
			return;
		}
		OnAimWait( pOwner );
		break;
	case FS_AIMED:
		if( CheckDashInput(pOwner) )
		{
			ChangeMove( pOwner );
			return;
		}
		OnAimed( pOwner );
		break;
	case FS_NORMAL_FIRE:
	case FS_AIMED_FIRE:
		OnFire( pOwner );

		if( m_iChargeCombo < (int)m_vChargeComboAttListCenter.size() )
			m_bCanFireMove = true;
		else
			m_bCanFireMove = false;

		if( CheckDashInput(pOwner) )
		{
			ChangeMove( pOwner );
			return;
		}
		
		break;
	case FS_COMBO_FIRE:
		if( CheckDashInput(pOwner) )
		{
			ChangeMove( pOwner );
			return;
		}

		OnCombo( pOwner );

		/*if( pOwner->IsAttackKeyDown() )
		{
			SetNormalAttack( pOwner, 0 );
			return;
		}
		else
		{
			pOwner->SetState( CS_DELAY );
			return;
		}*/
		break;
	case FS_MOVE:
		CheckMoveKey( pOwner );

		if( pOwner->IsNeedProcess() && m_bAiming_AimedMove && !pOwner->IsAttackKeyDown() )
		{
			m_bRollingCancelAttack = true;
		}

		if(pOwner->IsNeedProcess() && m_bRollingCancelAttack == true && ( (m_dwRollingStartTime + m_dwRollingAttackCheckTime) < dwCurTime || m_dwFireMoveMotionTime < dwCurTime ) )
		{
			m_bRollingCancelAttack = false;

			MoveEndRotate( pOwner );	

			if( m_bZeroAimState )
			{
				ChangeToNormalFire( pOwner, true );
			}
			else
			{
				ChangeToNormalFire( pOwner, true );
			}

			m_AimedTarget.Clear();
			m_bZeroAimState = false;

			return;
		}

		
		OnFireAndMove(pOwner);
		break;
	case FS_MOVE_END:
		if( pOwner->IsNeedProcess() )
		{
			MoveEndRotate( pOwner );

			if( pOwner->IsAttackKeyDown() )
			{
				SetNormalAttack( pOwner, 0 );
				return;
			}
			else
			{
				pOwner->SetState( CS_DELAY );
				return;
			}
		}
		break;
	}

	if( m_FireState == FS_AIMING ||
		m_FireState == FS_AIM_WAIT ||
		m_FireState == FS_AIMED )
	{
		if( pOwner->IsNeedProcess() && pOwner->ChangeDirectionExplicitIfKeyChanged( false ) )
		{
			if( m_FireState != FS_AIMING )
			{
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

void ioWestGunExtendItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	pOwner->SetAttackMoveEnable( false );
	//pOwner->ChangeDirectionExplicitIfKeyChanged( false );

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
					kPacket << m_iChargeCombo;
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

void ioWestGunExtendItem::OnAiming( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	//pOwner->ChangeDirectionExplicitIfKeyChanged( false );

	if( !pOwner->IsAttackKeyDown() && pOwner->GetState() != CS_JUMP )	// 버튼 뗌, 일반 공격으로 전환
	{
		ChangeToNormalFire( pOwner );
		return;
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
		m_AimedTarget = szTarget;

	// 타겟 비교
	if( CompareAimedTarget( pOwner ) )
	{
		m_AimedTarget.Clear();
		m_bAimedBall = true;
	}

	if( !m_AimedTarget.IsEmpty() || m_bAimedBall )
		ChangeToAimWaitState( pOwner, m_AimedTarget );	// 타겟 세팅됨
}


ioHashString ioWestGunExtendItem::FindAimTarget( ioBaseChar *pOwner, const ioHashString& stChecker )
{
	ioHashString szTarget;
	m_AimedTarget.Clear();

	float fAimAngle = GetChargeAimAngle();
	float fAimRange = GetChargeAimRange();

	if( pOwner->CheckAimedTargetByBow( fAimAngle, m_fMinRange, fAimRange, szTarget, m_bEnableDownAim ) )
	{
		if( stChecker.IsEmpty() )
			m_AimedTarget = szTarget;
		else if( !stChecker.IsEmpty() && stChecker == szTarget )
			m_AimedTarget = szTarget;
		else 
			m_AimedTarget.Clear();
	}

	return m_AimedTarget;
}


void ioWestGunExtendItem::OnAimWait( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	//pOwner->ChangeDirectionExplicitIfKeyChanged( false );

	if( !pOwner->IsAttackKeyDown() && pOwner->GetState() != CS_JUMP )	// 버튼 뗌, 일반 공격으로 전환
	{
		ChangeToNormalFire( pOwner );
		return;
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
		}
	}
}

void ioWestGunExtendItem::OnAimed( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() )
	{
		//pOwner->ChangeDirectionExplicitIfKeyChanged( false );

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
			if( m_bChargeCharRotate && !CheckCurrentTargetValid( pOwner, true ) )
				bNormalFire = true;

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
					kPacket << m_bSetSkillAimed;
					kPacket << pOwner->GetTargetRot();
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
				TrackingTarget( pOwner );//, m_AimedTarget );
			
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
								kPacket << pOwner->GetTargetRot();
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
					kPacket << m_iChargeCombo;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
	}
	else
	{
		if( CheckCurrentTargetValid( pOwner, false ) )
		{
			TrackingTarget( pOwner );//, m_AimedTarget );
		}
	}
}

void ioWestGunExtendItem::OnFire( ioBaseChar *pOwner )
{
	if( m_bAimEffect && m_dwFireStateEndTime < FRAMEGETTIME() )
	{
		m_bAimEffect = false;
		pOwner->EndEffect( m_AimEffect, false );
	}

	/*if( pOwner->CheckDefaultNormalAttack( m_dwFireStartTime,
										  m_dwFireEndTime,
										  m_dwFireDelayTime ) )
	{
		m_FireState = FS_NONE;
	}*/

	ChangeToComboState( pOwner );
}

void ioWestGunExtendItem::OnJumpFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwAttackOverTime = m_dwFireStartTime + m_dwFireEndTime + m_dwFireDelayTime;

	if( dwCurTime >= dwAttackOverTime )
	{
		m_FireState = FS_NONE;
		pOwner->SetMoveDirByRotate( pOwner->GetTargetRot() );
	}
}


void ioWestGunExtendItem::OnCombo( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if(	/*pOwner->IsNeedProcess() && !m_AimedTarget.IsEmpty() &&*/ !CheckCurrentTargetValid(pOwner, true ) )
	{
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
		m_AimedTarget.Clear();
	}

	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwComboAttackTimeEnd < dwCurTime )
	{
		pOwner->SetChargingState( false );
		pOwner->SetState( CS_DELAY );
		return;
	}	

	if( m_dwKeyReserveTime && m_dwKeyReserveTime < dwCurTime )
	{
		/*if( CheckKeyInput(pOwner) )
			return;*/

		CheckReserve( pOwner );

		if( m_iChargeCombo >= (int)m_vChargeComboAttListCenter.size() )
		{
			pOwner->SetChargingState( false );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
			return;
		}

		/*if(	!CheckCurrentTargetValid(pOwner, true ) )
		{
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
			m_AimedTarget.Clear();
		}*/

		if( (m_dwStartComboTime + m_dwMaxComboDuration) > dwCurTime )
		{
			if( pOwner->IsAttackKey() || m_bFireReservedFromShotAndMove )
			{
				//m_bFirstBullet = false;
				m_bFireReservedFromShotAndMove = false;
				ChangeToNextComboState( pOwner );
				m_iChargeCombo++;
			}
		}
		else
		{
			pOwner->SetChargingState( false );
			pOwner->SetState( CS_DELAY );
		}
	}
	/*else if( pOwner->IsNeedProcess() && m_bFirstBullet )
	{
		m_FirstDirKey = pOwner->CheckCurDirKey();
	}*/

	/*if( !m_AimedTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
		if( pTarget )
		{
			D3DXQUATERNION qtTargetRot;
			D3DXVECTOR3 vLookVec = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
			vLookVec.y = 0;
			D3DXVec3Normalize( &vLookVec, &vLookVec );

			ioMath::CalcDirectionQuaternion( qtTargetRot, -vLookVec );
			pOwner->SetTargetRotAndMoveDirChange( qtTargetRot, false );
			pOwner->SetWorldOrientation( qtTargetRot );
		}
	}*/
}


void ioWestGunExtendItem::OnFireAndMove( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	//LOG.PrintConsoleLog( 0, "OnFireAndMove In" );

	if( m_dwMoveForceTime > 0 && m_dwMoveForceTime < dwCurTime )
	{
		//LOG.PrintConsoleLog( 0, "OnFireAndMove Forced (%.2f, %.2f, %.2f)", m_vMoveForceDir.x, m_vMoveForceDir.y, m_vMoveForceDir.z );
		m_dwMoveForceTime = 0;
		pOwner->SetForcePower( m_vMoveForceDir, m_ShotAndMoveInfo.m_fShotAndMoveForce,m_ShotAndMoveInfo.m_fShotAndMoveFriction );
	}

	if( m_dwFireMoveMotionTime < dwCurTime )
	{
		//LOG.PrintConsoleLog( 0, "OnFireAndMove -> End");
		m_FireState = FS_MOVE_END;//FS_COMBO_FIRE;
		
		/*ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
		if( pTarget  )
		{
			D3DXVECTOR3 vTargetPos;
			vTargetPos = pTarget->GetMidPositionByRate();
			pOwner->SetTargetRotToTargetPos( vTargetPos, true, false );
		}*/

		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if(	eDir != ioUserKeyInput::DKI_NONE && pOwner->IsNeedProcess() )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, true );
		}

		return;
	}

	//if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime && pOwner->IsAttackKey() )
		//m_bFireReservedFromShotAndMove = true;
}


void ioWestGunExtendItem::ChangeToNormalAttack( ioBaseChar *pOwner )
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
		kPacket << m_iChargeCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWestGunExtendItem::ChangeToAimWaitState( ioBaseChar *pOwner, const ioHashString &szTarget )
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
		kPacket << m_iChargeCombo;
		kPacket << m_AimedTarget;
		kPacket << m_bAimedBall;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWestGunExtendItem::ChangeToAimedState( ioBaseChar *pOwner )
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
		TrackingTarget( pOwner );//, m_AimedTarget );
}

void ioWestGunExtendItem::ChangeToNormalFire( ioBaseChar *pOwner, bool bQuick )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iExtendAttackCnt = m_vChargeComboAttListCenter.size();
	if( iExtendAttackCnt <= m_iCurCombo )
		return;

	if( pOwner->GetState() == CS_JUMP )
		return;

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if(	eDir != ioUserKeyInput::DKI_NONE && pOwner->IsNeedProcess() )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, true );
		}
	}

	//
	CheckChargeRate();

	m_dwAirJumpTime = 0;
	m_AimedTarget.Clear();

	float fAimAngle = GetChargeAimAngle();
	float fAimRange = GetChargeAimRange();
	ioHashString szTarget;

	if( pOwner->CheckAimedTargetByBow( fAimAngle, m_fMinRange, fAimRange, szTarget, m_bEnableDownAim ) )
		m_AimedTarget = szTarget;

	// 타겟 비교
	if( CompareAimedTarget( pOwner ) )
	{
		m_AimedTarget.Clear();
		m_bAimedBall = true;
	}

	if( !m_AimedTarget.IsEmpty() )
	{
		pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
	}

	TrackingTarget( pOwner );//, m_AimedTarget );
	
	const AttackAttribute *pAttr = NULL;
	/*if( m_bZeroAimState )
		pAttr = &m_ExtendZeroAimed;
	else
		pAttr = &m_vExtendAttributeList[m_iCurCombo];*/
	m_iChargeCombo = 0;

	//pAttr = &m_vChargeComboAttListCenter[m_iChargeCombo];
	pAttr = GetCorrectDirAttibute( pOwner );

	// m_iChargeCombo 값이 다른 유저 클라에 의해 UDP패킷으로 받았을때 변조된 값이 사용 가능한 값이 아닐 경우 GetCorrectDirAttibute함수에서 null 반환 (2019.02.11 SYH)
	if(!pAttr)
		return;

	//LOG.PrintTimeAndLog( 0, "ChangeToNormalFire   %s  Ani", pAttr->m_AttackAnimation );

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

	m_dwAirJumpTime = pGrp->GetAnimationEventTime_e( iAniID, "air_jump" ) * fTimeRate;
	if( m_dwAirJumpTime > 0 )
	{
		m_dwAirJumpTime += FRAMEGETTIME();
	}

	m_dwFireStateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStateEndTime += FRAMEGETTIME();

	if( bQuick )
	{
		pGrp->ClearAllLoopAni( 0.0f, true );
		pGrp->ClearAllActionAni( 0.0f, true );
	}
	else
	{
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );
	}

	m_dwFireStartTime = FRAMEGETTIME();
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
	/*if( pAttr->m_AttackEndAnimation.IsEmpty() )
	{
		if( bQuick )
			pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
		else
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}*/
	float fWeight = GetAimedUpDownWeight( m_vAimedDir, 85.0f );
	if( pAttr->m_AttackEndAnimation.IsEmpty() )
	{
		if( m_vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true, bQuick );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true, bQuick );
		}
	}
	else
	{
		//pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		//pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fEndAniRate = pAttr->m_fAttackEndAniRate;

		if( bQuick )
			pGrp->SetActionAni( pAttr->m_AttackEndAnimation, 0.0f, 0.0f, FLOAT1, FLOAT1/fEndAniRate, m_dwFireEndTime, 0.0f, true );
		else
			pGrp->SetActionAni( pAttr->m_AttackEndAnimation, 0.0f, FLOAT100, FLOAT1, FLOAT1/fEndAniRate,m_dwFireEndTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( pAttr->m_AttackEndAnimation );
		m_dwFireDelayTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndAniRate;
	}

	//yunga
	m_dwComboAttackTimeEnd = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwPreDelay;
	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}
	//////////////////////////////////////////////////////////////////////////


	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	m_FireState = FS_NORMAL_FIRE;

	if( iExtendAttackCnt <= m_iCurCombo + 1 )
		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	else
		pOwner->ApplyNormalAttackResult( m_iCurCombo );

	pOwner->SetChargingState( true );

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
		kPacket << m_iChargeCombo;
		kPacket << bQuick;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iChargeCombo++;
	//WasteBullet();

	g_TutorialMgr.ActionNormalFire();
}


void ioWestGunExtendItem::ChangeToComboState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwStartComboTime = dwCurTime;

	m_FireState = FS_COMBO_FIRE;
}


void ioWestGunExtendItem::ChangeToNextComboState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iExtendAttackCnt = m_vChargeComboAttListCenter.size();
	if( iExtendAttackCnt <= m_iCurCombo )
		return;

	m_dwAirJumpTime = 0;
	
	if( pOwner->GetState() == CS_JUMP )
		return;

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if(	eDir != ioUserKeyInput::DKI_NONE && pOwner->IsNeedProcess() )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, true );
		}
	}

	m_dwAirJumpTime = 0;
	m_AimedTarget.Clear();

	float fAimAngle = GetChargeAimAngle();
	float fAimRange = GetChargeAimRange();
	ioHashString szTarget;

	if( pOwner->CheckAimedTargetByBow( fAimAngle, m_fMinRange, fAimRange, szTarget, m_bEnableDownAim ) )
		m_AimedTarget = szTarget;

	// 타겟 비교
	if( CompareAimedTarget( pOwner ) )
	{
		m_AimedTarget.Clear();
		m_bAimedBall = true;
	}

	if( !m_AimedTarget.IsEmpty() )
	{
		pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
	}

	TrackingTarget( pOwner );//, m_AimedTarget );

	//
	CheckChargeRate();

	const AttackAttribute *pAttr = NULL;
	//pAttr = &m_vChargeComboAttListCenter[m_iChargeCombo];
	pAttr = GetCorrectDirAttibute( pOwner );

	// m_iChargeCombo 값이 다른 유저 클라에 의해 UDP패킷으로 받았을때 변조된 값이 사용 가능한 값이 아닐 경우 GetCorrectDirAttibute함수에서 null 반환 (2019.02.11 SYH)
	if(!pAttr)
		return;

	//LOG.PrintTimeAndLog( 0, "ChangeToNormalFire   %s  Ani", pAttr->m_AttackAnimation );

	if( m_iCurCombo >= (int)m_vChargeComboAttListCenter.size() )
		return;

	//LOG.PrintTimeAndLog( 0, "ioWestGunExtendItem--> %d Combo", m_iCurCombo );

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

	m_dwAirJumpTime = pGrp->GetAnimationEventTime_e( iAniID, "air_jump" ) * fTimeRate;
	if( m_dwAirJumpTime > 0 )
		m_dwAirJumpTime += FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwFireStartTime = FRAMEGETTIME();
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	m_dwFireStateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStateEndTime += FRAMEGETTIME();

	//yunga
	m_dwComboAttackTimeEnd = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwPreDelay;
	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}
	//m_iChargeCombo++;
	//////////////////////////////////////////////////////////////////////////

	//if( m_bZeroAimState && m_bContactZeroAim )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
		if( pTarget )
		{
			pOwner->SetTargetRotToTargetPos( pTarget, true );
		}

		//pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1/fTimeRate );
	}
	//else
	//{
		float fWeight = GetAimedUpDownWeight( m_vAimedDir, 85.0f );
		//if( pAttr->m_AttackEndAnimation.IsEmpty() )
		{
			if( m_vAimedDir.y >= 0.0f )
			{
				AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true, false );
			}
			else
			{
				AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true, false );
			}
		}
		/*else
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
	}*/

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if( m_AimedTarget.IsEmpty() )
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	m_FireState = FS_AIMED_FIRE;

	if( iExtendAttackCnt <= m_iCurCombo + 1 )
		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	else
		pOwner->ApplyNormalAttackResult( m_iCurCombo );

	pOwner->SetChargingState( true );

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
		kPacket << (int)FS_COMBO_FIRE;
		kPacket << m_iCurCombo;
		kPacket << m_iChargeCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	//WasteBullet();
}


bool ioWestGunExtendItem::CheckCurrentTargetValid( ioBaseChar *pOwner, bool bAngleAlsoCheck )
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
			/*if( pOwner->IsJumpState() )
			{
				float fHalfCosine = cosf( DEGtoRAD( m_fAimAngle * FLOAT05 ) );
				if( D3DXVec3Dot( &m_vStartDir, &vDiff ) < fHalfCosine )
					return false;
			}
			else*/
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

void ioWestGunExtendItem::UpdateAimDir( ioBaseChar *pOwner, bool bJumpAim  )
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
		bStateCheck = true;
	}

	if( bJumpAim )
		bStateCheck = true;

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

void ioWestGunExtendItem::TrackingTarget( ioBaseChar *pOwner, const ioHashString& stUp, const ioHashString& stCenter, const ioHashString& stDown )
{
	//LOG.PrintTimeAndLog( 0, "TrackingTarget" );

	bool bJumpState = false;
	if( pOwner->IsJumpState() )
		bJumpState = true;

	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioAnimation *pUp, *pCenter, *pDown;

	if( bJumpState )
	{
		pUp = pGrp->GetLoopInPlay( stUp.IsEmpty() ? m_JumpAimUp : stUp );
		pCenter = pGrp->GetLoopInPlay( stCenter.IsEmpty() ?	m_JumpAimCenter : stCenter );
		pDown = pGrp->GetLoopInPlay( stDown.IsEmpty() ? m_JumpAimDown : stDown );
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

	D3DXVECTOR3 vAimedDir(0, 0, 0);
	if( pTarget )
		vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
	else if( m_bAimedBall && pBall )
		vAimedDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
	else
		vAimedDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	//////////////////////////////////////////////////////////////////////////
	if( pTarget && !bJumpState )
	{
		D3DXVECTOR3 vTargetPos;
		vTargetPos = pTarget->GetMidPositionByRate();
		pOwner->SetTargetRotToTargetPos( vTargetPos, true, false );
	}
	//////////////////////////////////////////////////////////////////////////

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	float fAniWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
	
	m_vAimedDir = vAimedDir;
	m_bSetAimedDir = true;
	m_bAimedState = true;

	if( vAimedDir.y >= 0.0f )	// up, center
	{


		if( !pUp || !pCenter || !pDown )
		{
			return;
		}

		/*if( pTarget == 0 && m_bAimedBall == 0 && pBall == 0 )
		{
			pUp->ForceWeight( 0 );
			pCenter->ForceWeight( FLOAT1 );
			pDown->ForceWeight( 0 );

			m_vAimedDir = vAimedDir;
			return;
		}*/

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

	/*m_vAimedDir = vAimedDir;
	m_bSetAimedDir = true;
	m_bAimedState = true;*/
}

void ioWestGunExtendItem::ChangeToAimedFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iExtendAttackCnt = m_vChargeComboAttListCenter.size();
	if( iExtendAttackCnt <= m_iCurCombo )
		return;

	m_dwAirJumpTime = 0;
	
	if( pOwner->GetState() == CS_JUMP )
		return;

	CheckChargeRate();

	const AttackAttribute *pAttr = NULL;
	m_iChargeCombo = 0;
	
	pAttr = GetCorrectDirAttibute( pOwner );

	// m_iChargeCombo 값이 다른 유저 클라에 의해 UDP패킷으로 받았을때 변조된 값이 사용 가능한 값이 아닐 경우 GetCorrectDirAttibute함수에서 null 반환 (2019.02.11 SYH)
	if(!pAttr)
		return;

	//LOG.PrintTimeAndLog( 0, "ChangeToNormalFire   %s  Ani", pAttr->m_AttackAnimation );

	if( m_iCurCombo >= (int)m_vChargeComboAttListCenter.size() )
		return;

	//LOG.PrintTimeAndLog( 0, "ioWestGunExtendItem--> %d Combo", m_iCurCombo );

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

	m_dwAirJumpTime = pGrp->GetAnimationEventTime_e( iAniID, "air_jump" ) * fTimeRate;
	if( m_dwAirJumpTime > 0 )
		m_dwAirJumpTime += FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwFireStartTime = FRAMEGETTIME();
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	m_dwFireStateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStateEndTime += FRAMEGETTIME();

	//yunga
	m_dwComboAttackTimeEnd = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwPreDelay;
	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}
	m_iChargeCombo++;
	//////////////////////////////////////////////////////////////////////////

	//if( m_bZeroAimState && m_bContactZeroAim )
	/*{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
		if( pTarget )
		{
			pOwner->SetTargetRotToTargetPos( pTarget, true );
		}

		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1/fTimeRate );
	}*/
	//else
	{
		float fWeight = GetAimedUpDownWeight( m_vAimedDir, 85.0f );
		if( pAttr->m_AttackEndAnimation.IsEmpty() )
		{
			if( m_vAimedDir.y >= 0.0f )
			{
				AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true, false );
			}
			else
			{
				AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true, false );
			}
		}
		else
		{
			if( m_vAimedDir.y >= 0.0f )
			{
				AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, false, false );
			}
			else
			{
				AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, false, false );
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

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if( m_AimedTarget.IsEmpty() )
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

//	WasteBullet();
}

float ioWestGunExtendItem::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
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

void ioWestGunExtendItem::AimedFireToUp( ioEntityGroup *pGrp,
							   float fWeight,
							   float fTimeRate,
							   DWORD dwPreDelay,
							   bool bHold,
							   bool bQuick)
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
		if( bQuick)
			pGrp->SetActionAni( szFireUp,
								0.0f, 0.f,
								fWeight, FLOAT1/fTimeRate,
								0.0f, dwPreDelay,
								bHold, bAniFx1 );
		else
			pGrp->SetActionAni( szFireUp,
								FLOAT100, fFadeOut,
								fWeight, FLOAT1/fTimeRate,
			0.0f, dwPreDelay,
			bHold, bAniFx1 );
	}

	if( bQuick )
		pGrp->SetActionAni( szFireCenter,
							0.0f, 0.0f,
							1.0f - fWeight, FLOAT1/fTimeRate,
							0.0f, dwPreDelay,
							bHold, bAniFx2 );
	else
		pGrp->SetActionAni( szFireCenter,
							FLOAT100, fFadeOut,
							1.0f - fWeight, FLOAT1/fTimeRate,
							0.0f, dwPreDelay,
							bHold, bAniFx2 );
}

void ioWestGunExtendItem::AimedFireToDown( ioEntityGroup *pGrp,
								 float fWeight,
								 float fTimeRate,
								 DWORD dwPreDelay,
								 bool bHold,
								 bool bQuick)
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

void ioWestGunExtendItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
		{
			rkPacket >> m_vAimedDir;
			rkPacket >> m_bSetAimedDir;
			rkPacket >> m_bSetSkillAimed;
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );
			ChangeToAimedFire( pOwner );
		}
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
	}
}

void ioWestGunExtendItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	rkPacket >> m_iChargeCombo;

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
		{
			bool bQuick;
			rkPacket >> bQuick;
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );
			// 1. FS_AIMING 중에 Key off
			// 2. FS_AIM_WAIT 중에 key off
			ChangeToNormalFire( pOwner, bQuick );
		}
		break;
	case FS_COMBO_FIRE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );
			ChangeToNextComboState( pOwner );
		}
		break;
	case FS_MOVE:
		{		
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );
			ChangeMove( pOwner );
			rkPacket >> m_vMoveForceDir;
		}
		break;
	}
}

void ioWestGunExtendItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioArrowWeapon *pArrow = ToArrowWeapon( pWeapon );
	ioMissileWeapon *pMissile = ToMissileWeapon( pWeapon );
	ioReturnPingpongWeapon *pReturnPingpong = ToReturnPingpongWeapon( pWeapon );
	ioZoneEffectWeapon * pZoneEffect = ToZoneEffectWeapon( pWeapon );
	ioTraceWeaponType2 *pTrace = ToTraceWeaponType2( pWeapon );

	if( pReturnPingpong )
	{
		g_WeaponMgr.CheckMaxWeaponCnt( pWeapon, m_iMaxWeaponCnt );
	}

	if( pArrow || pMissile || pReturnPingpong || pTrace )
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

	if( pTrace && !m_AimedTarget.IsEmpty() )
	{
		pTrace->SetTraceTarget( m_AimedTarget );
	}

	if( bJumpAttack )
	{
		if( m_bAimedState )
		{
			D3DXVECTOR3 vAimedDir = ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
			pWeapon->SetMoveDir( vAimedDir );
		}

		m_bSetAimedDir = false;

		if( pMissile && m_vSecondAttributeList.size() > 0 )
			pMissile->SetSecondAttribute( m_vSecondAttributeList );
	}
	else
	{
		if( m_FireState == FS_AIMED_FIRE && !m_AimedTarget.IsEmpty() )
		{
			vAimedDir = ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
			pWeapon->SetMoveDir( vAimedDir );
			m_bSetAimedDir = false;
		}

		if( pArrow || pMissile )
			SetFireEffect( pWeapon->GetOwner(), vAimedDir );

		if( m_bZeroAimState )
		{
			if( pMissile && m_vSecondZeroAttributeList.size() > 0 )
			{
				pMissile->SetSecondAttribute( m_vSecondZeroAttributeList );
			}
			else if( pMissile && m_vSecondZeroAttributeList.size() > 0 )
			{
				pMissile->SetSecondAttribute( m_vSecondZeroAttributeList );
			}
		}
		else
		{
			if( pMissile && m_vSecondAttributeList.size() > 0 )
			{
				pMissile->SetSecondAttribute( m_vSecondAttributeList );
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

ioWeaponItem::WeaponSubType ioWestGunExtendItem::GetSubType() const
{
	return WST_WESTGUN_EXTEND_ITEM;
}

void ioWestGunExtendItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	pOwner->SetTargetMarker( ioTargetMarker::MT_BOW_ITEM,
		m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker, "" );

	pOwner->SetTargetMarkerRange( m_fMarkerRange );

	m_dwCheckCoolTime = FRAMEGETTIME();
}

void ioWestGunExtendItem::OnReleased( ioBaseChar *pOwner )
{
	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "", "" );

	m_fReloadTimeModifyRate = FLOAT1;
	m_fFireTimeModifyRate = FLOAT1;

	ioWeaponItem::OnReleased( pOwner );
}

bool ioWestGunExtendItem::IsCanFire( const ioBaseChar* pOwner, int iFireCnt, bool bNormalAttack ) const
{
	if( m_iMaxBullet == 0 )
		return true;

	if( GetJumpType() != JT_AIM_JUMP && GetJumpType() != JT_AUTO_SHOT_JUMP )
		return true;

	if( bNormalAttack )
		return true;

	if( m_iCurBullet >= iFireCnt )
		return true;

	return false;
}

void ioWestGunExtendItem::ModifyFireTimeRate( float fRate )
{
	m_fFireTimeModifyRate *= fRate;
}

void ioWestGunExtendItem::WasteBullet()
{
	if( m_iMaxBullet == 0 ) return;

	m_iCurBullet--;
	m_iCurBullet = max( m_iCurBullet, 0 );
}

void ioWestGunExtendItem::SetAimedSkillUse( bool bSkillUsed )
{
	m_bSetSkillAimed = bSkillUsed;
}

void ioWestGunExtendItem::SetAimedDirSet( bool bAimedSet )
{
	m_bSetAimedDir = bAimedSet;
}

D3DXVECTOR3 ioWestGunExtendItem::ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon, bool bSkillTest )
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
	
	if( bSkillTest == false )
	{
		vAimedDir.x = m_vAimedDir.x;
		vAimedDir.z = m_vAimedDir.z;
	}

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	//LOG.PrintTimeAndLog( 0, "RecalDir %.2f  -  %.2f   - %.2f", vAimedDir.x, vAimedDir.y, vAimedDir.z );

	return vAimedDir;
}

bool ioWestGunExtendItem::CheckDownAimState( ioBaseChar *pTarget )
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

bool ioWestGunExtendItem::CheckAimState(bool bAimMove)
{
	if( !bAimMove )
	{
		switch( m_FireState )
		{
		case FS_AIMING:
		case FS_AIM_WAIT:
		case FS_AIMED:
		case FS_NORMAL_FIRE:
		case FS_COMBO_FIRE:
			return true;
		}
	}

	switch( m_FireState )
	{
	case FS_AIMING:
	case FS_AIM_WAIT:
	case FS_AIMED:
	case FS_NORMAL_FIRE:
	case FS_COMBO_FIRE:
		return true;
	}

	return false;
}

bool ioWestGunExtendItem::SetAimState( ioBaseChar *pChar )
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
		pChar->SetAttackMoveEnable( false );
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

void ioWestGunExtendItem::SetNoneState( ioBaseChar *pOwner, int eNewState /* = 0 */ )
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


bool ioWestGunExtendItem::SetChangeJumpFireMotion( ioBaseChar *pOwner )
{
	float fCurTime = (float)FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	
	// 에니메이션 처리
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	float fTimeRate = FLOAT1;
	DWORD dwPreDelay = 0;


	if( m_FireState == FS_AIMED )
	{
		float fWeight = GetAimedUpDownWeight( m_vAimedDir, 85.0f );

		if( m_vAimedDir.y >= 0.0f )
			AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true, false );
		else
			AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true, false );
	}

	return true;
}


bool ioWestGunExtendItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	float fCurTime = (float)FRAMEGETTIME();
	m_vAirJumpTimeList.clear();
	m_iCurAirJump = 0;

	AttackType eExtendType = eType;
	if( GetJumpType() == JT_AIM_JUMP )
		eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioWestGunExtendItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );

	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioWestGunExtendItem::SetJumpAttack - JumpAttack Animation Not Exist" );
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

	pGrp->GetAnimationEventTimeList_e( iAniID, "air_jump", m_vAirJumpTimeList );
	std::list< float >::iterator iter = m_vAirJumpTimeList.begin();
	while( iter != m_vAirJumpTimeList.end() )
	{
		(*iter) *= fTimeRate;
		(*iter) += fCurTime;
		iter++;
	}

	m_dwFireStateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStateEndTime += FRAMEGETTIME();

	m_bAimJumpAttack = true;

	// For AirJumpPower
	m_fAirJumpPowerRate = 0.0f;

	float fJumpPowerAmt = pOwner->GetCurJumpPowerAmt();
	if( fJumpPowerAmt > 0.0f )
	{
		m_fAirJumpPowerRate = fabs( fJumpPowerAmt - pOwner->GetGravityAmt() ) / fJumpPowerAmt;
		m_fAirJumpPowerRate = max( 0.0f, min( FLOAT1, m_fAirJumpPowerRate ) );
		m_fAirJumpPowerRate = FLOAT1 - m_fAirJumpPowerRate;
	}

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
				AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true, false );
			}
			else
			{
				AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true, false );
			}
		}
		else
		{
			if( m_vAimedDir.y >= 0.0f )
			{
				AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, false, false );
			}
			else
			{
				AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, false, false );
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

void ioWestGunExtendItem::CheckJumpAttack( ioBaseChar *pOwner )
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

bool ioWestGunExtendItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_FireState == FS_CHARGING ||
			m_FireState == FS_AIMING ||
			m_FireState == FS_AIM_WAIT ||
			m_FireState == FS_AIMED ||
			m_FireState == FS_AIMED_FIRE ||
			m_FireState == FS_NORMAL_FIRE ||
			m_FireState == FS_COMBO_FIRE 
			)
			return true;
	}

	return false;
}

void ioWestGunExtendItem::CheckAimMoveAniState( ioBaseChar *pOwner )
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

void ioWestGunExtendItem::OnEndExtendDefense( ioBaseChar *pOwner )
{
	if( !m_ReloadEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_ReloadEffect, false );
		pOwner->ShowWeaponItemMesh( true );
	}
}

void ioWestGunExtendItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
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

bool ioWestGunExtendItem::SetAimJumpState( ioBaseChar *pChar )
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

void ioWestGunExtendItem::SetAimJumpReady( ioBaseChar *pChar )
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

void ioWestGunExtendItem::SetAimJumpping( ioBaseChar *pChar, bool bFullTime )
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

void ioWestGunExtendItem::SetAimJumpCancel( ioBaseChar *pChar )
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

const ioHashString& ioWestGunExtendItem::GetJumppingAnimation() const
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

void ioWestGunExtendItem::CheckAimJumpState( ioBaseChar *pChar )
{
	switch( m_FireState )
	{
	case FS_AIMING:
		m_bFireReservedFromShotAndMove = false;
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


bool ioWestGunExtendItem::CheckLanding( ioBaseChar *pChar )
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

bool ioWestGunExtendItem::CheckZeroAim( ioBaseChar *pOwner )
{
	if( m_bZeroAimState ) return false;

	if( m_dwZeroAimStartTime == 0 ) return false;

	if( m_dwZeroAimStartTime+m_dwZeroAimedCheckTime > FRAMEGETTIME() )
		return false;

	SetZeroAim( pOwner, true );
	return true;
}

void ioWestGunExtendItem::SetZeroAim( ioBaseChar *pOwner, bool bSet )
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


void ioWestGunExtendItem::MoveEndRotate( ioBaseChar *pOwner )
{
	if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		pOwner->SetTargetRotToRotate( qtRotate, true );
	}
}


bool ioWestGunExtendItem::CheckDashInput( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;
	if( !pOwner->IsNeedProcess() )
		return false;

	if( pOwner->IsDirKeyDoubleClick() )
		return true;

	return false;
}


void ioWestGunExtendItem::CheckMoveKey( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;
	if( !pOwner->IsNeedProcess() )
		return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if(	eNewDirKey != ioUserKeyInput::DKI_NONE && eNewDirKey != m_CurDirKey )
	{
		m_CurDirKey = eNewDirKey;
	}
}


bool ioWestGunExtendItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;
	if( !pOwner->IsNeedProcess() )
		return false;

	//if( !m_bCanFireMove )
		//return false;

	if( !m_bCanFireMove )
		return false;

	bool bChangeDir = false;
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			bChangeDir = true;

		m_CurDirKey = eNewDirKey;
	}

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
	{
//		m_FirstDirKey = m_CurDirKey;
		return false;
	}

	/*if( m_bFirstBullet )
	{
		if( m_FirstDirKey != ioUserKeyInput::DKI_NONE && m_CurDirKey == m_FirstDirKey )
			return false;
	}*/

	//////////////////////////////////////////////////////////////////////////
	ChangeMove( pOwner );
	//////////////////////////////////////////////////////////////////////////

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << false;	//AimJump
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		kPacket << m_iChargeCombo;
		kPacket << (int)m_CurDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}


void ioWestGunExtendItem::ChangeMove( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRollingStartTime = dwCurTime;
	m_bRollingCancelAttack = false;

	//pOwner->SetForcePowerAmt( 0.0f );
	pOwner->InitForcePower();
	pOwner->SetJumpPower( 0 );
	pOwner->SetAirJumpPower( 0 );
	pOwner->SetCurMoveSpeed( 0 );

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if(	eDir != ioUserKeyInput::DKI_NONE && pOwner->IsNeedProcess() )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			//pOwner->SetTargetRot( qtRotate );
			pOwner->SetTargetRotToRotate( qtRotate, true );
		}
	}

	//DWORD dwCurTime = FRAMEGETTIME();

	D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	CheckFireAndMoveAni( pOwner, m_ShotAndMoveInfo.m_stShotAndMoveFr, m_ShotAndMoveInfo.m_fShotAndMoveRateFr, vMoveDir );

	// KeyDir
	/*ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();;
	float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
	D3DXVECTOR3 vSightDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vMoveDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iSide = (int)fAngleGap % 360;

	int iCnt = iSide / 45;
	int iRest = iSide % 45;

	switch( iCnt )
	{
	case 0:
	case 7:
		if( !m_ShotAndMoveInfo.m_stShotAndMoveFr.IsEmpty() )
			CheckFireAndMoveAni( pOwner, m_ShotAndMoveInfo.m_stShotAndMoveFr, m_ShotAndMoveInfo.m_fShotAndMoveRateFr, vMoveDir );
		else
			LOG.PrintTimeAndLog( 0, "No Front Ani" );
		break;
	case 1:
		if( iRest > 15 )
		{
			if( !m_ShotAndMoveInfo.m_stShotAndMoveRt.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_ShotAndMoveInfo.m_stShotAndMoveRt, m_ShotAndMoveInfo.m_fShotAndMoveRateRr, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Right Ani" );
		}
		else
		{
			if( !m_ShotAndMoveInfo.m_stShotAndMoveFr.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_ShotAndMoveInfo.m_stShotAndMoveFr, m_ShotAndMoveInfo.m_fShotAndMoveRateFr, vMoveDir );
		}
		break;
	case 2:
		if( iRest <= 40 )
		{
			if( !m_ShotAndMoveInfo.m_stShotAndMoveRt.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_ShotAndMoveInfo.m_stShotAndMoveRt, m_ShotAndMoveInfo.m_fShotAndMoveRateRr, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Right Ani" );
		}
		else
		{
			if( !m_ShotAndMoveInfo.m_stShotAndMoveBk.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_ShotAndMoveInfo.m_stShotAndMoveBk, m_ShotAndMoveInfo.m_fShotAndMoveRateBk, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Back Ani" );
		}
		break;
	case 3:
	case 4:
		if( !m_ShotAndMoveInfo.m_stShotAndMoveBk.IsEmpty() )
			CheckFireAndMoveAni( pOwner, m_ShotAndMoveInfo.m_stShotAndMoveBk, m_ShotAndMoveInfo.m_fShotAndMoveRateBk, vMoveDir );
		else
			LOG.PrintTimeAndLog( 0, "No Back Ani" );
		break;
	case 5:
		if( iRest > 5 )
		{
			if( !m_ShotAndMoveInfo.m_stShotAndMoveLt.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_ShotAndMoveInfo.m_stShotAndMoveLt, m_ShotAndMoveInfo.m_fShotAndMoveRateLr, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Left Ani" );
		}
		else
		{
			if( !m_ShotAndMoveInfo.m_stShotAndMoveBk.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_ShotAndMoveInfo.m_stShotAndMoveBk, m_ShotAndMoveInfo.m_fShotAndMoveRateBk, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Back Ani" );
		}
		break;
	case 6:
		if( iRest <= 30 )
		{
			if( !m_ShotAndMoveInfo.m_stShotAndMoveLt.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_ShotAndMoveInfo.m_stShotAndMoveLt, m_ShotAndMoveInfo.m_fShotAndMoveRateLr, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Left Ani" );
		}
		else
		{
			if( !m_ShotAndMoveInfo.m_stShotAndMoveFr.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_ShotAndMoveInfo.m_stShotAndMoveFr, m_ShotAndMoveInfo.m_fShotAndMoveRateFr, vMoveDir );
		}
		break;
	}*/
	
	if( m_FireState == FS_AIMING || m_FireState == FS_AIMED )
		m_bAiming_AimedMove = true;
	else
		m_bAiming_AimedMove = false;

	m_FireState = FS_MOVE;
	m_bCanFireMove = false;

	pOwner->ClearDirDoubleClick();

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << false;	//AimJump
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		kPacket << m_iChargeCombo;
		//kPacket << (int)m_CurDirKey;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_vMoveForceDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioWestGunExtendItem::CheckFireAndMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	//m_dwKeyReserveTime = 0;
	float fTimeRate = fAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	int iAniID = pGrp->GetAnimationIdx( szAni );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwFullAniTime = pGrp->GetAnimationFullTime(iAniID) * fTimeRate;
	m_dwFireMoveMotionTime = dwCurTime + dwFullAniTime;

	if( m_dwComboAttackTimeEnd )
		m_dwComboAttackTimeEnd += dwFullAniTime;

	DWORD dwTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" ) * fTimeRate;
	m_dwMoveForceTime = dwCurTime + dwTime;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	m_dwKeyReserveTime += dwCurTime;

	m_vMoveForceDir = vDir;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, 0 );
}


bool ioWestGunExtendItem::CheckExtendDefenseEnable( ioBaseChar *pChar )
{
	if( m_iCurBullet == m_iMaxBullet)
		return false;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ReloadAnimation );
	if( iAniID == -1 )	return false;

	return true;
}


void ioWestGunExtendItem::CheckDefenseMoveAniState( ioBaseChar *pOwner )
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


float ioWestGunExtendItem::GetAttackSpeedRate() const
{
	if( m_bZeroAimState && m_fZeroAimSpeedRate > 0.0f )
		return m_fZeroAimSpeedRate;
	else
		return m_fAttackSpeedRate;
}


bool ioWestGunExtendItem::IsAimedState()
{
	if( m_FireState == FS_AIMED )
		return true;

	return false;
}


bool ioWestGunExtendItem::CompareAimedTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) 
		return false;

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


void ioWestGunExtendItem::CheckChargeRate()
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


void ioWestGunExtendItem::CheckTargetMarker( ioBaseChar *pOwner )
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


float ioWestGunExtendItem::GetChargeAimRange()
{
	if( m_fCurChargeRate > 0.0f )
	{
		float fCurAimRangeRate = FLOAT1;
		float fCurMaxAimRange = max( 0.0f, (m_fMaxAimRangeRate - FLOAT1) );

		fCurAimRangeRate += fCurMaxAimRange * m_fCurChargeRate;

		float fCurRange = m_fAimRange * fCurAimRangeRate;
		if( m_FireState == FS_COMBO_FIRE || m_FireState == FS_MOVE )
			fCurRange = m_fAimRangeInFight * fCurAimRangeRate;

		return fCurRange;
	}

	if( m_FireState == FS_COMBO_FIRE || m_FireState == FS_MOVE )
		return m_fAimRangeInFight;

	return m_fAimRange;
}

float ioWestGunExtendItem::GetChargeAimAngle()
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

void ioWestGunExtendItem::SetFireEffect( ioBaseChar *pOwner, const D3DXVECTOR3 &vDir )
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

bool ioWestGunExtendItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	/*switch( m_FireState )
	{
	case FS_AIMING:
	case FS_AIM_WAIT:
	case FS_AIMED:
		return true;
	}

	return false;*/

	//return true;
	return false;
}


bool ioWestGunExtendItem::IsEnableItemDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	return true;
}

int ioWestGunExtendItem::GetMaxBullet()
{
	return m_iMaxBullet;
}

int ioWestGunExtendItem::GetNeedBullet()
{
	return (int)m_dwNeedCoolTime;
}

int ioWestGunExtendItem::GetCurBullet()
{
	return m_iCurBullet;
}

void ioWestGunExtendItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	int iBullet = min( iCurBullet, m_iMaxBullet );
	iBullet = max( 0, iBullet );

	m_iCurBullet = iBullet;
}

void ioWestGunExtendItem::InitCoolTime()
{
	m_dwCurCoolTime = 0;
	m_dwCheckCoolTime = FRAMEGETTIME();
}

void ioWestGunExtendItem::MaxCoolTime()
{
	m_dwCurCoolTime = m_dwMaxCoolTime;
	m_dwCheckCoolTime = FRAMEGETTIME();
}

void ioWestGunExtendItem::DecreaseCoolTime()
{
	if( m_dwCurCoolTime < m_dwNeedCoolTime )
		m_dwCurCoolTime = 0;
	else
		m_dwCurCoolTime -= m_dwNeedCoolTime;
}


bool ioWestGunExtendItem::IsChargeBulletType()
{
	return true;
}


void ioWestGunExtendItem::CheckReserve( ioBaseChar *pOwner )
{
	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput = iSkillNum;
		bReserveSkill = true;
	}

	if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState && !bReserveSkill )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		m_bEnableAttackState = true;
		m_bEnableDefenseState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_iSkillInput = -1;
	}
}


AttackAttribute* ioWestGunExtendItem::GetCorrectDirAttibute( ioBaseChar *pOwner )
{
	//LOG.PrintTimeAndLog( 0 ,"GetCorrectDirAttibute   %s", m_AimedTarget  );

	if( !m_AimedTarget.IsEmpty() )
	{
		bool bUp = false; //타겟이 나보다 위에 있다.

		D3DXVECTOR3 vPosSrc = pOwner->GetWorldPosition();
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
		if( pTarget == NULL && m_iChargeCombo < (int)m_vChargeComboAttListCenter.size() && m_iChargeCombo >= 0)
			return &m_vChargeComboAttListCenter[m_iChargeCombo];

		D3DXVECTOR3 vPosDest = pTarget->GetWorldPosition();

		if( (vPosDest.y - vPosSrc.y) > 0 )
			bUp = true;

		D3DXVECTOR3 v1 = vPosDest - vPosSrc;
		D3DXVECTOR3 v2 = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );

		D3DXVec3Normalize( &v1, &v1);
		D3DXVec3Normalize( &v2, &v2);

		float fDotValue = (float)D3DXVec3Dot( &v1, &v2 );
		float fAngle = RADtoDEG( acosf( fDotValue ) );

		if( fAngle > m_fChargeComboAttAngle )
		{
			if( m_bZeroAimState )
			{
				if( bUp )
				{
					if(m_iChargeCombo < (int)m_vChargeComboZeroAttListUp.size() && m_iChargeCombo >= 0)
						return &m_vChargeComboZeroAttListUp[m_iChargeCombo];		
				}
				else
				{
					if(m_iChargeCombo < (int)m_vChargeComboZeroAttListDown.size() && m_iChargeCombo >= 0)
						return &m_vChargeComboZeroAttListDown[m_iChargeCombo];		
				}
			}
			else
			{
				if( bUp )
				{
					if(m_iChargeCombo < (int)m_vChargeComboAttListUp.size() && m_iChargeCombo >= 0)
						return &m_vChargeComboAttListUp[m_iChargeCombo];		
				}
				else
				{
					if(m_iChargeCombo < (int)m_vChargeComboAttListDown.size() && m_iChargeCombo >= 0)
						return &m_vChargeComboAttListDown[m_iChargeCombo];		
				}
			}
		}

	}
	
	if( m_bZeroAimState )
	{
		if(m_iChargeCombo < (int)m_vChargeComboZeroAttListCenter.size() && m_iChargeCombo >= 0)
			return &m_vChargeComboZeroAttListCenter[m_iChargeCombo];
	}

	if(m_iChargeCombo < (int)m_vChargeComboAttListCenter.size() && m_iChargeCombo >= 0)
		return &m_vChargeComboAttListCenter[m_iChargeCombo];
	return NULL;
}



bool ioWestGunExtendItem::ProcessCancel( ioBaseChar *pOwner )
{
	return false;
}