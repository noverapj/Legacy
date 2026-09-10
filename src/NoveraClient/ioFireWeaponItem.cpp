

#include "stdafx.h"

#include "ioFireWeaponItem.h"
#include "WeaponDefine.h"

ioFireWeaponItem::ioFireWeaponItem()
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
	m_AimJumpState = AJS_NONE;
	m_LevelTimeRate.clear();

	m_bSetSkillAimed = false;
	m_bSetAimedDir = false;
	m_bAimedState = false;
	m_bSetReloadMoveAni = false;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_fCurAngle = 0.0f;
	m_dwCheckAimCancelTime = 0;
	m_dwTrackingEndTime = 0;
	m_dwFireStateEndTime = 0;
	m_bAimJumpAttack = false;

	m_bZeroAimState = false;
	m_dwZeroAimStartTime = 0;

	m_bAimMove = false;
	m_bFirstAimMoveMotion = true;
	m_bAimMotionSetted = false;
	m_bChangeDir = false;
	m_AimStartDirKey = ioUserKeyInput::DKI_NONE;

	m_bDefenseDash = false;
}

ioFireWeaponItem::ioFireWeaponItem( const ioFireWeaponItem &rhs )
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
 m_dwZeroAimedCheckTime( rhs.m_dwZeroAimedCheckTime ),
 m_ZeroAimedEffect( rhs.m_ZeroAimedEffect ),
 m_AimJumpReadyAnimation( rhs.m_AimJumpReadyAnimation ),
 m_AimJumppingAnimation( rhs.m_AimJumppingAnimation ),
 m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
 m_bEnableZeroAiming( rhs.m_bEnableZeroAiming ),
 m_bEnableDownAim( rhs.m_bEnableDownAim ),
 m_vSecondAttributeList( rhs.m_vSecondAttributeList ),
 m_vSecondZeroAttributeList( rhs.m_vSecondZeroAttributeList ),
 m_fZeroAimSpeedRate( rhs.m_fZeroAimSpeedRate ),
 m_fTargetAimRate( rhs.m_fTargetAimRate ),
 m_fDownTargetAimRate( rhs.m_fDownTargetAimRate ),
 m_iMaxWeaponCnt( rhs.m_iMaxWeaponCnt ),
 m_CurAimUp( rhs.m_CurAimUp ),
 m_CurAimCenter( rhs.m_CurAimCenter ),
 m_CurAimDown( rhs.m_CurAimDown ),
 m_AimMoveFront( rhs.m_AimMoveFront ),
 m_AimMoveBack( rhs.m_AimMoveBack ),
 m_AimMoveLeft( rhs.m_AimMoveLeft ),
 m_AimMoveRight( rhs.m_AimMoveRight ),
 m_fAimMoveAniRate( rhs.m_fAimMoveAniRate ),
 m_fAimMoveSpeedRate( rhs.m_fAimMoveSpeedRate ),
 m_bAimMove( rhs.m_bAimMove ),
 m_bDefenseDash( rhs.m_bDefenseDash ),
 m_ProneLoopAni( rhs.m_ProneLoopAni ),
 m_ProneUpLoopAni( rhs.m_ProneUpLoopAni ),
 m_ProneDownLoopAni( rhs.m_ProneDownLoopAni ),
 m_ProneUpFireAni( rhs.m_ProneUpFireAni ),
 m_ProneDownFireAni( rhs.m_ProneDownFireAni ),
 m_ProneStartAni( rhs.m_ProneStartAni ),
 m_ProneEndAni( rhs.m_ProneEndAni ),
 m_fProneStartAniRate( rhs.m_fProneStartAniRate ),
 m_fProneEndAniRate( rhs.m_fProneEndAniRate ),
 m_fProneAimAngle( rhs.m_fProneAimAngle ),
 m_fProneAimRange( rhs.m_fProneAimRange ),
 m_szProneAimMark( rhs.m_szProneAimMark ),
 m_vProneFireBufList( rhs.m_vProneFireBufList ),
 m_vProneStateBufList( rhs.m_vProneStateBufList ),
 m_ProneAttack( rhs.m_ProneAttack ),
 m_vProneAttackOffset( rhs.m_vProneAttackOffset )
{
	m_iMaxBullet = rhs.m_iMaxBullet;
	m_iCurBullet = rhs.m_iMaxBullet;
	m_iNeedBullet = rhs.m_iNeedBullet;

	m_fReloadTimeRate = rhs.m_fReloadTimeRate;

	m_dwAimTime = rhs.m_dwAimTime;
	m_fAimAngle = rhs.m_fAimAngle;
	m_fAimRange = rhs.m_fAimRange;
	m_fMinRange = rhs.m_fMinRange;

	m_fMarkerRange = rhs.m_fMarkerRange;

	m_fReloadTimeModifyRate = FLOAT1;
	m_fFireTimeModifyRate = FLOAT1;

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
	m_AimJumpState = AJS_NONE;
	m_bSetSkillAimed = false;
	m_bSetAimedDir = false;
	m_bAimedState = false;
	m_bSetReloadMoveAni = false;
	m_LevelTimeRate = rhs.m_LevelTimeRate;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_fCurAngle = 0.0f;
	m_dwCheckAimCancelTime = 0;
	m_dwFireStateEndTime = 0;
	m_dwTrackingEndTime = 0;
	m_bAimJumpAttack = false;

	m_bZeroAimState = false;
	m_dwZeroAimStartTime = 0;

	m_bFirstAimMoveMotion = true;
	m_bAimMotionSetted = false;
	m_bChangeDir = false;
	m_AimStartDirKey = ioUserKeyInput::DKI_NONE;
}

ioFireWeaponItem::~ioFireWeaponItem()
{
	m_LevelTimeRate.clear();

	m_vSecondAttributeList.clear();
	m_vSecondZeroAttributeList.clear();
}

void ioFireWeaponItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

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

	rkLoader.LoadString_e( "aim_jump_ready_animation", "", szBuf, MAX_PATH );
	m_AimJumpReadyAnimation = szBuf;
	rkLoader.LoadString_e( "aim_jumpping_animation", "", szBuf, MAX_PATH );
	m_AimJumppingAnimation = szBuf;

	m_iMaxBullet = rkLoader.LoadInt_e( "max_bullet", 0 );
	m_iNeedBullet = rkLoader.LoadInt_e( "need_bullet", 1 );
	m_fReloadTimeRate  = rkLoader.LoadFloat_e( "reload_time_rate", FLOAT1 );

	m_bEnableZeroAiming = rkLoader.LoadBool_e( "enable_zero_aiming", false );
	m_bEnableDownAim = rkLoader.LoadBool_e( "enable_down_aim", false );

	m_dwZeroAimedCheckTime = rkLoader.LoadInt_e( "zero_aimed_check_time", 1 );
	rkLoader.LoadString_e( "zero_aimed_effect", "", szBuf, MAX_PATH );
	m_ZeroAimedEffect = szBuf;

	m_fZeroAimSpeedRate = rkLoader.LoadFloat_e( "zero_aim_speed_rate", 0.0f );

	int iLevelCount = rkLoader.LoadInt_e( "level_count", 0 );
	m_LevelTimeRate.clear();
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

	m_dwAimTime = rkLoader.LoadInt_e( "aim_time", 1000 );
	m_fAimAngle = rkLoader.LoadFloat_e( "aim_angle", 45.0f );
	m_fAimRange = rkLoader.LoadFloat_e( "aim_range", FLOAT1000 );
	m_fMinRange = rkLoader.LoadFloat_e( "aim_min_range", 0.0f );

	m_fMarkerRange = rkLoader.LoadFloat_e( "marker_range", 0.0f );

	m_fTargetAimRate = rkLoader.LoadFloat_e( "target_aim_rate", FLOAT05 );
	m_fDownTargetAimRate = rkLoader.LoadFloat_e( "down_target_aim_rate", 0.0f );

	rkLoader.LoadString_e( "aim_move_front", "", szBuf, MAX_PATH );
	m_AimMoveFront = szBuf;
	rkLoader.LoadString_e( "aim_move_back", "", szBuf, MAX_PATH );
	m_AimMoveBack = szBuf;
	rkLoader.LoadString_e( "aim_move_left", "", szBuf, MAX_PATH );
	m_AimMoveLeft = szBuf;
	rkLoader.LoadString_e( "aim_move_right", "", szBuf, MAX_PATH );
	m_AimMoveRight = szBuf;

	m_fAimMoveAniRate = rkLoader.LoadFloat_e( "aim_move_ani_rate", FLOAT1 );
	m_fAimMoveSpeedRate = rkLoader.LoadFloat_e( "aim_move_speed_rate", 0.0f );
	m_bAimMove = rkLoader.LoadBool_e( "enable_aim_move", false );

	rkLoader.LoadString_e( "prone_up_fire_ani", "", szBuf, MAX_PATH );
	m_ProneUpFireAni = szBuf;
	rkLoader.LoadString_e( "prone_down_fire_ani", "", szBuf, MAX_PATH );
	m_ProneDownFireAni = szBuf;	
	rkLoader.LoadString_e( "prone_loop_ani", "", szBuf, MAX_PATH );
	m_ProneLoopAni = szBuf;
	rkLoader.LoadString_e( "prone_up_loop_ani", "", szBuf, MAX_PATH );
	m_ProneUpLoopAni = szBuf;
	rkLoader.LoadString_e( "prone_down_loop_ani", "", szBuf, MAX_PATH );
	m_ProneDownLoopAni = szBuf;
	rkLoader.LoadString_e( "prone_start_ani", "", szBuf, MAX_PATH );
	m_ProneStartAni = szBuf;
	rkLoader.LoadString_e( "prone_end_ani", "", szBuf, MAX_PATH );
	m_ProneEndAni = szBuf;
	rkLoader.LoadString_e( "prone_aim_mark_effect", "", szBuf, MAX_PATH );
	m_szProneAimMark = szBuf;
	m_fProneStartAniRate = rkLoader.LoadFloat_e( "prone_start_ani_rate", FLOAT1 );
	m_fProneEndAniRate = rkLoader.LoadFloat_e( "prone_end_ani_rate", FLOAT1 );
	m_fProneAimAngle = rkLoader.LoadFloat_e( "prone_aim_angle", FLOAT1 );
	m_fProneAimRange = rkLoader.LoadFloat_e( "prone_aim_range", FLOAT1 );
	m_bDefenseDash = rkLoader.LoadBool_e( "enable_defense_dash", false );

	ioHashString szBuffName;
	int iCnt = rkLoader.LoadInt_e( "prone_fire_buff_cnt", 0 );
	m_vProneFireBufList.clear();
	if( iCnt > 0 )
	{
		m_vProneFireBufList.reserve( iCnt );
		for( int i=0; i < iCnt; ++i )
		{
			wsprintf_e( szKey, "prone_fire_buff%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			szBuffName = szBuf;

			m_vProneFireBufList.push_back( szBuf );
		}
	}

	iCnt = rkLoader.LoadInt_e( "prone_state_buff_cnt", 0 );
	m_vProneStateBufList.clear();
	if( iCnt > 0 )
	{
		m_vProneStateBufList.reserve( iCnt );
		for( int i=0; i < iCnt; ++i )
		{
			wsprintf_e( szKey, "prone_state_buff%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			szBuffName = szBuf;

			m_vProneStateBufList.push_back( szBuf );
		}
	}

	LoadAttackAttribute( "prone_attack", m_ProneAttack, rkLoader );

	m_vProneAttackOffset.x = rkLoader.LoadFloat_e( "prone_attack_offset_x", 0.0f );
	m_vProneAttackOffset.y = rkLoader.LoadFloat_e( "prone_attack_offset_y", 0.0f );
	m_vProneAttackOffset.z = rkLoader.LoadFloat_e( "prone_attack_offset_z", 0.0f );
}

ioItem* ioFireWeaponItem::Clone()
{
	return new ioFireWeaponItem( *this );
}

bool ioFireWeaponItem::SetExtendDefenseState( ioBaseChar *pOwner )
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

void ioFireWeaponItem::CheckExtendDefenseState( ioBaseChar *pOwner )
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

void ioFireWeaponItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	m_CurAimUp.Clear();
	m_CurAimCenter.Clear();
	m_CurAimDown.Clear();

	int iExtendCnt = m_vExtendAttributeList.size();
	if( COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		m_FireState = FS_CHARGING;
		m_fCurAngle = 0.0f;
		m_dwCheckAimCancelTime = 0;
		m_dwChargeStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		m_bAimJumpAttack = false;
		m_bSetAimedDir = false;
		m_bAimedState = false;

		m_bZeroAimState = false;
		m_dwZeroAimStartTime = 0;
		m_bChangeDir = false;

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

void ioFireWeaponItem::ChangeToAiming( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->StopAutoTargetTracking();

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_bEnableZeroAiming && m_bZeroAimState )
	{
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_ZERO_AIMED );
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
	{
		pGrp->SetLoopAni( m_JumpAimCenter, FLOAT100 );

		m_CurAimUp.Clear();
		m_CurAimDown.Clear();
		m_CurAimCenter = m_JumpAimCenter;
	}
	else
	{
		pGrp->SetLoopAni( m_AimCenter, FLOAT100 );

		m_CurAimUp.Clear();
		m_CurAimDown.Clear();
		m_CurAimCenter = m_AimCenter;
	}

	m_FireState = FS_AIMING;
	if( m_bAimMove )
		pOwner->SetAttackMoveEnable( true );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );
	
	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	m_vStartDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	m_AimedTarget.Clear();
	m_bAimedBall = false;

	m_bSetAimedDir = false;
	m_bAimedState = false;

	m_bFirstAimMoveMotion = true;
	m_bAimMotionSetted = true;
	m_AimStartDirKey = pOwner->GetCurDirKey();

	g_TutorialMgr.ActionChangeToAiming();
}

void ioFireWeaponItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_FireState )
	{
	case FS_CHARGING:
		OnCharging( pOwner );
		break;
	case FS_NORMAL_ATTACK:
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
		pOwner->EndEffect( m_ZeroAimedEffect );
	if( m_bAimMove )
		CheckAimMoveAniState( pOwner );
}

void ioFireWeaponItem::CheckAimJumpState( ioBaseChar *pChar )
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
		m_dwZeroAimStartTime = 0;
	}

	if( m_bZeroAimState )
		pChar->EndEffect( m_ZeroAimedEffect );

	UpdateAimDir( pChar );
}

bool ioFireWeaponItem::SetAimJumpState( ioBaseChar *pChar )
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

	if( pChar->IsJumpKeyDown() )
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

void ioFireWeaponItem::SetAimJumpReady( ioBaseChar *pChar )
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

void ioFireWeaponItem::SetAimJumpping( ioBaseChar *pChar, bool bFullTime )
{
	m_dwCheckAimCancelTime = 0;
	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetLoopAni( m_JumpAimCenter, FLOAT100, FLOAT1 );
	pGrp->SetLoopAni( m_JumpAimUp, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_JumpAimDown, 0.0f, 0.0f );

	m_CurAimUp = m_JumpAimUp;
	m_CurAimDown = m_JumpAimDown;
	m_CurAimCenter = m_JumpAimCenter;

	if( m_bZeroAimState )
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
	pChar->SetAimJumpState( bFullTime );

	m_AimJumpState = AJS_JUMPPING;
	m_dwTrackingEndTime = 0;

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

void ioFireWeaponItem::SetAimJumpCancel( ioBaseChar *pChar )
{
	m_dwCheckAimCancelTime = 0;
	m_FireState = FS_NONE;

	pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	pChar->SetState( CS_DELAY );
	pChar->EndEffect( m_ZeroAimedEffect );

	m_bSetAimedDir = false;
	m_bAimedState  = false;
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

bool ioFireWeaponItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	ClearAirJump();
	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioFireWeaponItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioFireWeaponItem::SetJumpAttack - JumpAttack Animation Not Exist" );
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

	m_dwFireStateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate + 30;
	m_dwFireStateEndTime += FRAMEGETTIME();

	m_bAimJumpAttack = true;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	// 에니메이션 처리
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_CurAimUp.Clear();
	m_CurAimCenter.Clear();
	m_CurAimDown.Clear();

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

			pGrp->SetActionAni( pAttr->m_AttackEndAnimation,
								0.0f, FLOAT100,
								1.0f, FLOAT1/pAttr->m_fAttackEndAniRate,
								m_dwFireEndTime, 0.0f, true );

			int iEndAniID = pGrp->GetAnimationIdx( pAttr->m_AttackEndAnimation );
			m_dwFireDelayTime += pGrp->GetAnimationFullTime( iEndAniID ) * pAttr->m_fAttackEndAniRate;
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

			pGrp->SetActionAni( pAttr->m_AttackEndAnimation,
								0.0f, FLOAT100,
								1.0f, FLOAT1/pAttr->m_fAttackEndAniRate,
								m_dwFireEndTime, 0.0f, true );

			int iEndAniID = pGrp->GetAnimationIdx( pAttr->m_AttackEndAnimation );
			m_dwFireDelayTime += pGrp->GetAnimationFullTime( iEndAniID ) * pAttr->m_fAttackEndAniRate;
		}
	}

	if( m_FireState == FS_AIMED )
		m_FireState = FS_AIMED_FIRE;
	else
		m_FireState = FS_NORMAL_FIRE;

	pOwner->SetChargingState( false );
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	pOwner->SetAttackMoveEnable( false );
	pOwner->EndEffect( m_ZeroAimedEffect );
	return true;
}

void ioFireWeaponItem::CheckJumpAttack( ioBaseChar *pOwner )
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
			
			++iter;
		}

		if( dwCurAirJumpTime > 0 && dwCurAirJumpTime < FRAMEGETTIME() )
		{
			if( m_fAirJumpPower >= 0.0f )
			{
				pOwner->SetAirJumpPower( m_fAirJumpPower*m_fAirJumpPowerRate );

				D3DXVECTOR3 vAttDir =  pOwner->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
				if( m_fAirForcePower < 0.0f )
					pOwner->SetForcePower( -vAttDir, fabs(m_fAirForcePower), FLOAT1, true );
				else
					pOwner->SetForcePower( vAttDir, fabs(m_fAirForcePower), FLOAT1, true );
			}

			m_iCurAirJump++;
		}
	}

	UpdateAimDir( pOwner );
}

void ioFireWeaponItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargeStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if( m_iCurBullet >= m_iNeedBullet )
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

				if( m_iCurBullet < m_iNeedBullet && SetExtendDefenseState( pOwner ) )
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

void ioFireWeaponItem::OnAiming( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;
	
	if( !pOwner->IsAttackKeyDown() && pOwner->GetState() != CS_JUMP )	// 버튼 뗌, 일반 공격으로 전환
	{
		ChangeToNormalFire( pOwner );
		return;
	}

	if( !SetAimJumpState( pOwner ) )
		return;

	if( pOwner->GetState() != CS_JUMP && pOwner->IsDefenseKeyDown() )
	{
		if(IsCanFire(0))
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

	if( pOwner->CheckAimedTarget( m_fAimAngle, m_fMinRange, m_fAimRange, szTarget, m_bEnableDownAim ) )
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

void ioFireWeaponItem::OnAimWait( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() && pOwner->GetState() != CS_JUMP )	// 버튼 뗌, 일반 공격으로 전환
	{
		ChangeToNormalFire( pOwner );
		return;
	}

	if( !SetAimJumpState( pOwner ) )
		return;

	if( pOwner->GetState() != CS_JUMP && pOwner->IsDefenseKeyDown() )
	{
		if(IsCanFire(0))
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

void ioFireWeaponItem::OnAimed( ioBaseChar *pOwner )
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
				kPacket << m_bAimedState;

				P2PNetwork::SendToAllPlayingUser( kPacket );
			}

			g_TutorialMgr.ActionAimedFireDeveloperK(m_AimedTarget);
		}
		else
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
							pOwner->SetChargingState( false );
							m_bSetSkillAimed = true;
							m_FireState = FS_NONE;
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
								kPacket << m_bAimedState;
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

void ioFireWeaponItem::OnFire( ioBaseChar *pOwner )
{
	if( pOwner->CheckDefaultNormalAttack( m_dwFireStartTime,
										  m_dwFireEndTime,
										  m_dwFireDelayTime ) )
	{
		m_FireState = FS_NONE;
	}
}

void ioFireWeaponItem::OnJumpFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwAttackOverTime = m_dwFireStartTime + m_dwFireEndTime + m_dwFireDelayTime;

	if( dwCurTime >= dwAttackOverTime )
	{
		m_FireState = FS_NONE;
		pOwner->SetMoveDirByRotate( pOwner->GetTargetRot() );
	}
}

void ioFireWeaponItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );
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

void ioFireWeaponItem::ChangeToAimWaitState( ioBaseChar *pOwner, const ioHashString &szTarget )
{
	m_dwAimStartTime = FRAMEGETTIME();
	m_FireState = FS_AIM_WAIT;

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMING );
}

void ioFireWeaponItem::ChangeToAimedState( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_bZeroAimState )
	{
		if( !pGrp->GetLoopInPlay( m_ZeroAimUp ) )
			pGrp->SetLoopAni( m_ZeroAimUp, 0.0f, 0.0f );
		if( !pGrp->GetLoopInPlay( m_ZeroAimDown ) )
			pGrp->SetLoopAni( m_ZeroAimDown, 0.0f, 0.0f );
		if( !pGrp->GetLoopInPlay( m_ZeroAimCenter ) )
			pGrp->SetLoopAni( m_ZeroAimCenter, 0.0f, FLOAT1 );

		m_CurAimUp = m_ZeroAimUp;
		m_CurAimCenter = m_ZeroAimCenter;
		m_CurAimDown = m_ZeroAimDown;
	}
	else if( pOwner->GetState() != CS_JUMP )
	{
		pGrp->SetLoopAni( m_AimUp, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_AimDown, 0.0f, 0.0f );
		if( !pGrp->GetLoopInPlay( m_AimCenter ) )
			pGrp->SetLoopAni( m_AimCenter, 0.0f, FLOAT1 );

		m_CurAimUp = m_AimUp;
		m_CurAimCenter = m_AimCenter;
		m_CurAimDown = m_AimDown;
	}
	else
	{
		pGrp->SetLoopAni( m_JumpAimUp, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_JumpAimDown, 0.0f, 0.0f );
		if( !pGrp->GetLoopInPlay( m_JumpAimCenter ) )
			pGrp->SetLoopAni( m_JumpAimCenter, 0.0f, FLOAT1 );

		m_CurAimUp = m_JumpAimUp;
		m_CurAimCenter = m_JumpAimCenter;
		m_CurAimDown = m_JumpAimDown;
	}

	m_FireState = FS_AIMED;
	m_dwZeroAimStartTime = FRAMEGETTIME();

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

	if( m_bZeroAimState )
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_ZERO_AIMED );
	else
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );

	if( CheckCurrentTargetValid( pOwner, false ) )
		TrackingTarget( pOwner );
}

void ioFireWeaponItem::ChangeToNormalFire( ioBaseChar *pOwner )
{
	int iExtendAttackCnt = m_vExtendAttributeList.size();
	if( iExtendAttackCnt <= m_iCurCombo )
		return;

	if( pOwner->GetState() == CS_JUMP )
		return;

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

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_CurAimUp.Clear();
	m_CurAimCenter.Clear();
	m_CurAimDown.Clear();

	m_dwFireStartTime = FRAMEGETTIME();
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
	if( pAttr->m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		pGrp->SetActionAni( pAttr->m_AttackEndAnimation,
							0.0f, FLOAT100,
							1.0f, FLOAT1/pAttr->m_fAttackEndAniRate,
							m_dwFireEndTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( pAttr->m_AttackEndAnimation );
		m_dwFireDelayTime += pGrp->GetAnimationFullTime( iEndAniID ) * pAttr->m_fAttackEndAniRate;
	}


	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );


	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	m_FireState = FS_NORMAL_FIRE;

	if( iExtendAttackCnt <= m_iCurCombo + 1 )
		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	else
		pOwner->ApplyNormalAttackResult( m_iCurCombo );

	pOwner->SetChargingState( false );

	DWORD dwComboTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" )*fTimeRate;
	pOwner->SetComboCheckTime( dwComboTime, true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << false;	//AimJump
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	g_TutorialMgr.ActionNormalFire();
}

bool ioFireWeaponItem::CheckCurrentTargetValid( ioBaseChar *pOwner, bool bAngleAlsoCheck )
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

	D3DXVECTOR3 vDiff = vTargetPos - pOwner->GetWorldPosition();
	if( D3DXVec3LengthSq( &vDiff ) > m_fAimRange * m_fAimRange )
		return false;

	if( D3DXVec3LengthSq( &vDiff ) < m_fMinRange * m_fMinRange )
		return false;

	if( bAngleAlsoCheck )
	{
		vDiff.y = 0.0f;
		D3DXVec3Normalize( &vDiff, &vDiff );

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

	return true;
}

void ioFireWeaponItem::UpdateAimDir( ioBaseChar *pOwner )
{
	if( !pOwner->IsJumpState() ) return;

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

	if( m_bAimJumpAttack )
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


void ioFireWeaponItem::TrackingTarget( ioBaseChar *pOwner )
{
	bool bJumpState = false;
	if( pOwner->IsJumpState() )
		bJumpState = true;

	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );

	D3DXVECTOR3 vTargetPos;
	if( pTarget )
	{
		if( CheckDownAimState( pTarget ) )
			vTargetPos = pTarget->GetMidPositionByRate( m_fDownTargetAimRate );
		else
			vTargetPos = pTarget->GetMidPositionByRate();
	}
	else if( m_bAimedBall && pBall )
		vTargetPos = pBall->GetMidPositionByRate();

	if( !bJumpState )
		pOwner->SetTargetRotToTargetPos( vTargetPos, true, false );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioAnimation *pUp, *pCenter, *pDown;

	pUp = pGrp->GetLoopInPlay( m_CurAimUp );
	pCenter = pGrp->GetLoopInPlay( m_CurAimCenter );
	pDown = pGrp->GetLoopInPlay( m_CurAimDown );

	D3DXVECTOR3 vAimedDir = vTargetPos - pOwner->GetMidPositionByRate();
	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	float fAniWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
	if( vAimedDir.y >= 0.0f )	// up, center
	{
		if( !pCenter || !pDown || !pUp )
		{
			LOG.PrintTimeAndLog( 0, "TrackingTarget - 1: %s, %s, %s", m_CurAimUp.c_str(),
															m_CurAimCenter.c_str(),
															m_CurAimDown.c_str() );
			return;
		}	
		pUp->ForceWeight( fAniWeight );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( 0.0f );
	}
	else	// center, down
	{
		if( !pCenter || !pDown || !pUp )
		{
			LOG.PrintTimeAndLog( 0, "TrackingTarget - 2: %s, %s, %s", m_CurAimUp.c_str(),
															m_CurAimCenter.c_str(),
															m_CurAimDown.c_str() );
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

void ioFireWeaponItem::ChangeToAimedFire( ioBaseChar *pOwner )
{
	int iExtendAttackCnt = m_vExtendAttributeList.size();
	if( iExtendAttackCnt <= m_iCurCombo )
		return;

	if( pOwner->GetState() == CS_JUMP )
		return;

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

	m_CurAimUp.Clear();
	m_CurAimCenter.Clear();
	m_CurAimDown.Clear();

	m_dwFireStartTime = FRAMEGETTIME();
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

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

		pGrp->SetActionAni( pAttr->m_AttackEndAnimation,
							0.0f, FLOAT100,
							1.0f, FLOAT1/pAttr->m_fAttackEndAniRate,
							m_dwFireEndTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( pAttr->m_AttackEndAnimation );
		m_dwFireDelayTime += pGrp->GetAnimationFullTime( iEndAniID ) * pAttr->m_fAttackEndAniRate;
	}


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
}

float ioFireWeaponItem::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
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

void ioFireWeaponItem::AimedFireToUp( ioEntityGroup *pGrp,
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

void ioFireWeaponItem::AimedFireToDown( ioEntityGroup *pGrp,
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
		pGrp->SetActionAni( szFireDown,
							FLOAT100, fFadeOut,
							fWeight, FLOAT1/fTimeRate,
							0.0f, dwPreDelay,
							bHold, bAniFx2 );
	}

	pGrp->SetActionAni( szFireCenter,
						FLOAT100, fFadeOut,
						1.0f - fWeight, FLOAT1/fTimeRate,
						0.0f, dwPreDelay,
						bHold, bAniFx1 );
}

void ioFireWeaponItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iFireState;
	bool bZeroAimState;
	rkPacket >> iFireState;
	if( iFireState == FS_AIMED_FIRE || iFireState == FS_AIMING || iFireState == FS_AIMED || iFireState == FS_NONE )
	{
		rkPacket >> m_AimedTarget;
		rkPacket >> m_bAimedBall;
		rkPacket >> m_iCurCombo;
	}
	else
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );

		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );
	}

	switch( iFireState )
	{
	case FS_AIMED_FIRE:
		rkPacket >> m_vAimedDir;
		rkPacket >> m_bSetAimedDir;
		rkPacket >> m_bAimedState;
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
		rkPacket >> m_bAimedState;
		rkPacket >> m_bSetSkillAimed;
		break;
	case FS_PRONE_START:
		SetProneStart( pOwner, false );
		break;
	case FS_PRONE:
		SetProne( pOwner, false );
		break;
	case FS_PRONE_FIRE:
		rkPacket >> m_vProneAttackDir;
		SetProneFireState( pOwner, false );
		break;
	case FS_PRONE_END:
		SetProneEndState( pOwner, false );
		break;
	case FS_END:
		SetEndState( pOwner, false );
		break;
	case CSS_CHNAGE_CHAR:
		rkPacket >> m_szProneTargetName;
		if( pOwner )
		{
			if( !m_szProneTargetName.IsEmpty() )
			{
				pOwner->SetTargetMarkerTarget( m_szProneTargetName, false );
				pOwner->SetTargetMarkerState( ioTargetMarker::MS_SPECIAL_AIMED );
			}
			else
			{
				pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
			}
		}
		break;
	}
}

void ioFireWeaponItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

void ioFireWeaponItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );
	
	ioMissileWeapon *pMissile = ToMissileWeapon( pWeapon );
	ioBombMissileWeapon *pBombMissile = ToBombMissileWeapon( pWeapon );
	ioReturnPingpongWeapon *pReturnPingpong = ToReturnPingpongWeapon( pWeapon );
	if( pReturnPingpong )
	{
		g_WeaponMgr.CheckMaxWeaponCnt( pWeapon, m_iMaxWeaponCnt );
	}

	if( pMissile || pBombMissile || pReturnPingpong )
		WasteBullet();

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
		if( m_FireState == FS_AIMED_FIRE )
		{
			D3DXVECTOR3 vAimedDir = ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
			pWeapon->SetMoveDir( vAimedDir );
			m_bSetAimedDir = false;
		}

		if( m_bZeroAimState )
		{
			if( pMissile && m_vSecondZeroAttributeList.size() > 0 )
				pMissile->SetSecondAttribute( m_vSecondZeroAttributeList );
		}
		else
		{
			if( pMissile && m_vSecondAttributeList.size() > 0 )
				pMissile->SetSecondAttribute( m_vSecondAttributeList );
		}
	}
}

ioWeaponItem::WeaponSubType ioFireWeaponItem::GetSubType() const
{
	return WST_FIRE;
}

void ioFireWeaponItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	pOwner->SetTargetMarker( ioTargetMarker::MT_FIRE_ITEM,
							 m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker, "" );
	pOwner->SetSpecialMarker( m_szProneAimMark );
	pOwner->SetTargetMarkerRange( m_fMarkerRange );
}

void ioFireWeaponItem::OnReleased( ioBaseChar *pOwner )
{
	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "","" );

	m_fReloadTimeModifyRate = FLOAT1;
	m_fFireTimeModifyRate = FLOAT1;

	ioWeaponItem::OnReleased( pOwner );
}

bool ioFireWeaponItem::IsCanFire( const ioBaseChar* pOwner, int iFireCnt, bool bNormalAttack ) const
{
	if( bNormalAttack )
		return true;

	if( m_iCurBullet >= iFireCnt )
		return true;

	return false;
}

void ioFireWeaponItem::ModifyFireTimeRate( float fRate )
{
	m_fFireTimeModifyRate *= fRate;
}

void ioFireWeaponItem::WasteBullet()
{
	m_iCurBullet--;
	m_iCurBullet = max( m_iCurBullet, 0 );
}

void ioFireWeaponItem::SetAimedSkillUse( bool bSkillUsed )
{
	m_bSetSkillAimed = bSkillUsed;
}

void ioFireWeaponItem::SetAimedDirSet( bool bAimedSet )
{
	m_bSetAimedDir = bAimedSet;
}

D3DXVECTOR3 ioFireWeaponItem::ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon )
{
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !m_bAimedBall && (!pTarget || !m_bSetAimedDir) )
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

	m_vAimedDir = vAimedDir;

	return vAimedDir;
}

bool ioFireWeaponItem::CheckDownAimState( ioBaseChar *pTarget )
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

void ioFireWeaponItem::CheckAimMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanAttackMove() )
		return;

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
				pGrp->SetLoopAni( m_AimMoveFront, FLOAT100, FLOAT1, m_fAimMoveAniRate );
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

bool ioFireWeaponItem::CheckAimState(bool bAimMove)
{
	switch( m_FireState )
	{
	case FS_AIMING:
	case FS_AIM_WAIT:
	case FS_AIMED:
		return true;
	}

	return false;
}

bool ioFireWeaponItem::SetAimState( ioBaseChar *pChar )
{
	if( pChar->IsNeedProcess() && m_iCurBullet < m_iNeedBullet )
		return false;

	if( pChar->GetState() == CS_JUMP && CheckAimState() )
	{
		m_dwCheckAimCancelTime = 0;
		pChar->GetGroup()->ClearAllActionAni( FLOAT100, true );
		pChar->SetState( CS_ATTACK );
		if( m_bAimMove )
			pChar->SetAttackMoveEnable( true );

		m_bFirstAimMoveMotion = true;
		m_bAimMotionSetted = true;

		if( m_FireState == FS_AIMED || m_FireState == FS_AIMING )
		{
			m_dwZeroAimStartTime = FRAMEGETTIME();
		}
	}
	else
	{
		ChangeToAiming( pChar );

		if( !pChar->IsJumpState() )
		{
			pChar->SetState( CS_ATTACK );
		}
	}

	pChar->SetChargingState( true );
	m_bAimJumpAttack = false;

	return true;
}

void ioFireWeaponItem::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	m_AimedTarget.Clear();
	m_bAimedBall = false;

	m_bSetSkillAimed = false;
	m_bSetAimedDir = false;
	m_bAimedState = false;

	m_bZeroAimState = false;
	m_dwZeroAimStartTime = 0;

	m_FireState = FS_NONE;

	m_CurAimUp.Clear();
	m_CurAimCenter.Clear();
	m_CurAimDown.Clear();
}

bool ioFireWeaponItem::CheckExtendDefenseEnable( ioBaseChar *pChar )
{
	if( m_iCurBullet == m_iMaxBullet)
		return false;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ReloadAnimation );
	if( iAniID == -1 )	return false;

	return true;
}

void ioFireWeaponItem::CheckDefenseMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanReloadMove() ) return;

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

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
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

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/m_fReloadTimeModifyRate );
			m_bSetReloadMoveAni = false;
		}
	}
}

bool ioFireWeaponItem::IsChargeAttackState( ioBaseChar *pOwner )
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

void ioFireWeaponItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	int iBullet = min( iCurBullet, m_iMaxBullet );
	iBullet = max( 0, iBullet );

	m_iCurBullet = iBullet;
}

void ioFireWeaponItem::OnEndExtendDefense( ioBaseChar *pOwner )
{
	if( !m_ReloadEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_ReloadEffect, false );
		pOwner->ShowWeaponItemMesh( true );
	}
}

void ioFireWeaponItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->EndEffect( m_ZeroAimedEffect );
}

const ioHashString& ioFireWeaponItem::GetJumppingAnimation() const
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

bool ioFireWeaponItem::CheckLanding( ioBaseChar *pChar )
{
	if( !CheckAimState() )
	{
		pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE );
		pChar->GetGroup()->ReleaseAllExtraRotate();
		SetNoneState( pChar );
		return true;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( 0.0f, true );

	pGrp->SetLoopAni( m_AimCenter, 0.0f );
	pGrp->SetLoopAni( m_AimUp, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_AimDown, 0.0f, 0.0f );
	pChar->GetGroup()->ReleaseAllExtraRotate();

	m_CurAimUp = m_AimUp;
	m_CurAimCenter = m_AimCenter;
	m_CurAimDown = m_AimDown;

	return false;
}

bool ioFireWeaponItem::CheckZeroAim( ioBaseChar *pOwner )
{
	if( m_bZeroAimState ) return false;

	if( m_dwZeroAimStartTime == 0 ) return false;

	if( m_dwZeroAimStartTime+m_dwZeroAimedCheckTime > FRAMEGETTIME() )
		return false;

	SetZeroAim( pOwner, true );
	return true;
}

void ioFireWeaponItem::SetZeroAim( ioBaseChar *pOwner, bool bSet )
{
	if( bSet )
	{
		m_bZeroAimState = true;
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_ZERO_AIMED );
		pOwner->AttachEffect( m_ZeroAimedEffect );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );

		pGrp->SetLoopAni( m_ZeroAimUp, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_ZeroAimDown, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_ZeroAimCenter, 0.0f, FLOAT1 );

		m_CurAimUp = m_ZeroAimUp;
		m_CurAimCenter = m_ZeroAimCenter;
		m_CurAimDown = m_ZeroAimDown;

		m_bFirstAimMoveMotion = true;
	}
	else
	{
		m_bZeroAimState = false;
		m_dwZeroAimStartTime = 0;
		pOwner->EndEffect( m_ZeroAimedEffect );
	}
}

float ioFireWeaponItem::GetAttackSpeedRate() const
{
	if( m_bZeroAimState && m_fZeroAimSpeedRate > 0.0f )
		return m_fZeroAimSpeedRate;
	else if( m_bAimMove && !m_bFirstAimMoveMotion && !m_bAimMotionSetted && m_fAimMoveSpeedRate > 0.f )
		return m_fAimMoveSpeedRate;
	else
		return m_fAttackSpeedRate;
}

bool ioFireWeaponItem::IsAimedState()
{
	if( m_FireState == FS_AIMED )
		return true;

	return false;
}

bool ioFireWeaponItem::CompareAimedTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) return false;

	ioBaseChar *pChar = pOwner->GetBaseChar( m_AimedTarget );

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
	
	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vCharDir  = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	float fHalfCosine = cosf( DEGtoRAD( m_fAimAngle ) * FLOAT05 );

	float fLimitDistSq = m_fAimRange * m_fAimRange;
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

	return false;
}

bool ioFireWeaponItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	switch( m_FireState )
	{
	case FS_AIMING:
	case FS_AIM_WAIT:
	case FS_AIMED:
		return true;
	}

	if( !IsChargeDash() ) return false;
	if( !pOwner ) return false;
	if( pOwner->GetState() != CS_ATTACK )
		return false;

	switch( m_FireState )
	{
	case FS_NORMAL_ATTACK:
	case FS_NORMAL_FIRE:
	case FS_AIMED_FIRE:
		if( pOwner->IsEnableReserveKeyInput() )
			return true;
		break;
	}

	return false;
}

bool ioFireWeaponItem::CheckDefenseSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner || !m_bDefenseDash )
		return false;
	if( m_iCurBullet <= 0 )
		return false;

	SetProneStart( pOwner, true );
	return true;
}

void ioFireWeaponItem::SetProneStart( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		pOwner->SetTargetRotToDirKey( eNewDirKey );
	}
	
	pOwner->SetState( CS_FIRE_SPECIAL );
	m_FireState = FS_PRONE_START;
	m_vProneStartDir = pOwner->GetMoveDir();
	m_bDefenseKey = false;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_ProneStartAni );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fProneStartAniRate, 0.0f, 0.0f, true );
	
	m_dwMotionEndTime = FRAMEGETTIME() + ( pGrp->GetAnimationFullTime( iAniID ) * m_fProneStartAniRate );

	int iCnt = m_vProneFireBufList.size();
	for( int i=0; i < iCnt; ++i )
		pOwner->AddNewBuff( m_vProneFireBufList[i], pOwner->GetCharName(), GetName(), NULL, 0 );

	iCnt = m_vProneStateBufList.size();
	for( int i=0; i < iCnt; ++i )
		pOwner->AddNewBuff( m_vProneStateBufList[i], pOwner->GetCharName(), GetName(), NULL, 0 );

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFireWeaponItem::SetProne( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_FireState = FS_PRONE;

	pOwner->ClearReserveKeyInput();
	pOwner->InitExtendAttackTagInfo();
	pOwner->SetAttackMoveEnable( false );

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	pGrp->SetLoopAni( m_ProneUpLoopAni, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_ProneDownLoopAni, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_ProneLoopAni, 0.0f, FLOAT1 );
	
	if(pOwner->IsNeedProcess() && bSendNet )
	{
		//Send
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFireWeaponItem::SetProneFireState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	AttackProneFire( pOwner );
	WasteBullet();

	m_FireState = FS_PRONE_FIRE;

	int iCnt = m_vProneFireBufList.size();
	for( int i=0; i < iCnt; ++i )
		pOwner->RemoveBuff( m_vProneFireBufList[i] );

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_vProneAttackDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFireWeaponItem::AttackProneFire( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ProneAttack.m_AttackAnimation );
	if( iAniID == -1 )	
		return;

	float fTimeRate  = m_ProneAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_ProneAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage )
		return;

	DWORD dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate() + m_vProneAttackOffset;

	int iAttackSize = m_ProneAttack.m_vWeaponInfoList.size();
	for ( int i = 0; i<iAttackSize ; ++i )
	{
		WeaponInfo sWeaponInfo = m_ProneAttack.m_vWeaponInfoList[i];

		FireTime kFireTime;
		kFireTime.dwStart = FRAMEGETTIME();

		kFireTime.iAttributeIdx = sWeaponInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = sWeaponInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni = sWeaponInfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = sWeaponInfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = sWeaponInfo.m_bWoundedLoopAni;

		kFireTime.dwWeaponIdx = dwWeaponBaseIndex++;
		kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;
			
		pChar->ExtendFireExplicit( kFireTime, vPos, m_vProneAttackDir, "" );
	}

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	if( m_vProneAttackDir.y == 0.0f || m_fCurProneMotionRate == 0.0f )		// center
	{
		pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	}
	else if( m_vProneAttackDir.y > 0.0f )	// center & up
	{
		pGrp->SetActionAni( m_ProneUpFireAni, 0.0f, 0.0f, m_fCurProneMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
		pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1 - m_fCurProneMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	}
	else								// center & down
	{
		pGrp->SetActionAni( m_ProneDownFireAni, 0.0f, 0.0f, m_fCurProneMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
		pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1 - m_fCurProneMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	}

	pChar->SetReservedSliding( m_ProneAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
}

void ioFireWeaponItem::SetProneEndState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_FireState = FS_PRONE_END;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_ProneEndAni );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fProneEndAniRate, 0.0f, 0.0f, true );
	
	m_dwMotionEndTime = FRAMEGETTIME() + ( pGrp->GetAnimationFullTime( iAniID ) * m_fProneEndAniRate );

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFireWeaponItem::SetEndState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ClearAttackState( pOwner );
	pOwner->SetState( CS_DELAY );
	pOwner->ClearDirDoubleClick();

	if( pOwner->IsNeedSendNetwork() && bSendNet )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)FS_END;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFireWeaponItem::ClearAttackState( ioBaseChar *pOwner )
{
	m_dwMotionEndTime = 0;
	m_szProneTargetName.Clear();
	
	if( pOwner )
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	int iCnt = m_vProneStateBufList.size();
	for( int i=0; i < iCnt; ++i )
		pOwner->RemoveBuff( m_vProneStateBufList[i] );

	iCnt = m_vProneFireBufList.size();
	for( int i=0; i < iCnt; ++i )
		pOwner->RemoveBuff( m_vProneFireBufList[i] );
}

void ioFireWeaponItem::ProcessSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_FireState )
	{
	case FS_PRONE:
		CheckKeyInput( pOwner );
		CheckTarget( pOwner, pStage );
		CheckVDir( pOwner );

		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->IsAttackKey() )
			{
				SetProneFireState( pOwner, true );
				return;
			}
			else if( pOwner->IsJumpKey() )
			{
				SetProneEndState( pOwner, true );
				return;
			}
		}
		break;
	case FS_PRONE_START:
		if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < FRAMEGETTIME() )
		{
			SetProne( pOwner, true );
		}
		break;
	case FS_PRONE_FIRE:
		if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < FRAMEGETTIME() )
		{
			if( m_iCurBullet == 0 )
				SetProneEndState( pOwner, true );
			else
				SetProne( pOwner, true );
		}
		break;
	case FS_PRONE_END:
		if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < FRAMEGETTIME() )
		{
			SetEndState( pOwner, true );
		}
		break;
	}
}

void ioFireWeaponItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	if( pOwner->IsDefenseKey() )
	{
		m_bDefenseKey = true;
	}
	else
	{
		m_bDefenseKey = false;
	}
}

void ioFireWeaponItem::CheckTarget( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pStage || !pChar )	
		return;
	if( !pChar->IsNeedProcess() )
		return;

	float fPrevDistSq = m_fProneAimRange * m_fProneAimRange;
	float fMaxHalfCosine  = cosf( DEGtoRAD( m_fProneAimAngle ) * FLOAT05  );
	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	
	m_vProneTargetInfoList.clear();
	int iSize = rkTargetList.size();
	m_vProneTargetInfoList.reserve(iSize);
	
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);
		if( pTarget == pChar )
			continue;

		if( pChar->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pChar->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;
		
		if( !pTarget->IsEnableTarget() ) 
			continue;
		
		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );
		if( fDistSq > fPrevDistSq )
			continue;

		D3DXVECTOR3 vOwnerDir = m_vProneStartDir;
		vDiff.y = 0.0f;
		D3DXVec3Normalize( &vDiff, &vDiff );

		float fHalfCosine = D3DXVec3Dot( &vOwnerDir, &vDiff );
		if( fHalfCosine < fMaxHalfCosine )
			continue;		

		TargetInfo	kInfo;
		kInfo.m_TargetName = pTarget->GetCharName();
		kInfo.m_fLenth = fDistSq;
		m_vProneTargetInfoList.push_back(kInfo);
	}

	if( m_szProneTargetName.IsEmpty() )
	{
		int iCnt = m_vProneTargetInfoList.size();
		for( int i=0; i<iCnt; ++i )
		{
			float fDistSq = m_vProneTargetInfoList[i].m_fLenth;
			if( fDistSq < fPrevDistSq )	// Enemy
			{
				fPrevDistSq = fDistSq;
				m_szProneTargetName = m_vProneTargetInfoList[i].m_TargetName;
			}
		}

		if( !m_szProneTargetName.IsEmpty() )
		{
			pChar->SetTargetMarkerTarget( m_szProneTargetName, false );
			pChar->SetTargetMarkerState( ioTargetMarker::MS_SPECIAL_AIMED );

			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pChar, kPacket );
				kPacket << (int)CSS_CHNAGE_CHAR;
				kPacket << pChar->GetWorldPosition();
				kPacket << pChar->GetTargetRot();
				kPacket << m_szProneTargetName;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else if( !m_vProneTargetInfoList.empty() )
	{
		int iCurTarget = -1;
		int iCnt = m_vProneTargetInfoList.size();
		for( int i=0; i<iCnt; ++i )
		{
			if( m_szProneTargetName == m_vProneTargetInfoList[i].m_TargetName )
				iCurTarget = i;
		}

		if( iCurTarget != -1 )
		{
			bool bTargetChange = false;
			if( m_bDefenseKey )
			{
				iCurTarget++;
				bTargetChange = true;
				if( iCurTarget >= iCnt )
					iCurTarget = 0;
			}

			m_szProneTargetName = m_vProneTargetInfoList[iCurTarget].m_TargetName;

			if( bTargetChange )
			{
				pChar->SetTargetMarkerTarget( m_szProneTargetName, false );
				pChar->SetTargetMarkerState( ioTargetMarker::MS_SPECIAL_AIMED );

				if( pChar->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pChar, kPacket );
					kPacket << (int)CSS_CHNAGE_CHAR;
					kPacket << pChar->GetWorldPosition();
					kPacket << pChar->GetTargetRot();
					kPacket << m_szProneTargetName;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
		else
		{
			m_szProneTargetName.Clear();
			if( pChar )
			{
				pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE );
				pChar->SetTargetRotToDir( m_vProneStartDir, true );

				m_vProneAttackDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				D3DXVec3Normalize( &m_vProneAttackDir, &m_vProneAttackDir );
			}

			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pChar, kPacket );
				kPacket << (int)CSS_CHNAGE_CHAR;
				kPacket << pChar->GetWorldPosition();
				kPacket << pChar->GetTargetRot();
				kPacket << m_szProneTargetName;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		m_szProneTargetName.Clear();
		if( pChar )
		{
			pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE );
			pChar->SetTargetRotToDir( m_vProneStartDir, true );

			m_vProneAttackDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &m_vProneAttackDir, &m_vProneAttackDir );
		}

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pChar, kPacket );
			kPacket << (int)CSS_CHNAGE_CHAR;
			kPacket << pChar->GetWorldPosition();
			kPacket << pChar->GetTargetRot();
			kPacket << m_szProneTargetName;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioFireWeaponItem::CheckVDir( ioBaseChar *pChar )
{
	ioBaseChar *pTarget = pChar->GetBaseChar( m_szProneTargetName );
	if( pTarget )
	{
		D3DXVECTOR3 vPos1 = pChar->GetWorldPosition();
		D3DXVECTOR3 vPos2 = pTarget->GetWorldPosition();

		pChar->SetTargetRotToTargetPos( vPos2, true );

		m_vProneAttackDir = vPos2 - vPos1;
		D3DXVec3Normalize( &m_vProneAttackDir, &m_vProneAttackDir );

		D3DXVECTOR3 vSightDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vSightDir, &vSightDir );

		float fAngle = 0.0f;
		if( m_vProneAttackDir != vSightDir )
		{
			float fDotValue = D3DXVec3Dot( &m_vProneAttackDir, &vSightDir );
			fAngle = RADtoDEG( acosf(fDotValue) );
		}
		m_fCurProneMotionRate = fAngle /FLOAT90;
	}
	else
	{
		m_fCurProneMotionRate = 0.0f;
		pChar->SetTargetRotToDir( m_vProneStartDir, true );

		m_vProneAttackDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &m_vProneAttackDir, &m_vProneAttackDir );
	}
	ioEntityGroup	*pGrp = pChar->GetGroup();
	ioAnimation		*pUp, *pCenter, *pDown;
	pUp = pCenter = pDown = NULL;

	pUp = pGrp->GetLoopInPlay( m_ProneUpLoopAni );
	pCenter = pGrp->GetLoopInPlay( m_ProneLoopAni );
	pDown = pGrp->GetLoopInPlay( m_ProneDownLoopAni );

	if( !pUp || !pCenter || !pDown )
		return;

	if( m_vProneAttackDir.y == 0.0f || m_fCurProneMotionRate == 0.0f )		// center
	{
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 );
		pDown->ForceWeight( 0.0f );
	}
	else if( m_vProneAttackDir.y > 0.0f )	// center & up
	{
		pUp->ForceWeight( m_fCurProneMotionRate );
		pCenter->ForceWeight( FLOAT1 - m_fCurProneMotionRate );
		pDown->ForceWeight( 0.0f );
	}
	else								// center & down
	{
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 - m_fCurProneMotionRate );
		pDown->ForceWeight( m_fCurProneMotionRate );
	}
}
//////////////////////////////////////////////////////////////////////////
ioFireSpecialState::ioFireSpecialState()
{
}

ioFireSpecialState::~ioFireSpecialState()
{
}

void ioFireSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioFireSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioFireWeaponItem *pFireWeapon = ToFireWeaponItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pFireWeapon )
		pFireWeapon->ClearAttackState( pOwner );
}

void ioFireSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioFireWeaponItem *pFireWeapon = ToFireWeaponItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pFireWeapon )
		pFireWeapon->ProcessSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}