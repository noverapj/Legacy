#include "StdAfx.h"
#include "ioBubbleFighterItem.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioArrowWeapon.h"
#include "ioMissileWeapon.h"
#include "ioBombMissileWeapon.h"
#include "ioZoneEffectWeapon.h"


ioBubbleFighterItem::ioBubbleFighterItem(void)
{
	m_iCurCombo = 0;
	m_iCurBullet = 0;

	m_ChargeState = CS_NONE;
	
	m_fReloadTimeModifyRate = FLOAT1;
	m_bSetReloadMoveAni = false;
	m_dwReloadEndTime = 0;
	m_dwReloadGapTime = 0;
	m_dwNextReloadTime = 0;
	m_AimStartDirKey = ioUserKeyInput::DKI_NONE;
	m_bAimMotionSetted = false;
	m_bFirstAimMoveMotion = true;
	m_vAimedDir = ioMath::VEC3_ZERO;
	m_dwRotateStartTime = 0;
	m_dwRotateEndTime = 0;
	m_fCurAngle = 0.0f;
	
	m_dwMoveForceTime = 0;
	m_vMoveForceDir = ioMath::VEC3_ZERO;

	m_bChargeRetreateMove = false;
	m_bAimedState = false;
}

ioBubbleFighterItem::ioBubbleFighterItem( const ioBubbleFighterItem &rhs ):
ioWeaponItem( rhs ),
m_iMaxBullet( rhs.m_iMaxBullet ),

//Attack
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
m_MotionInfoList( rhs.m_MotionInfoList ),
m_fAimMoveAniRate( rhs.m_fAimMoveAniRate ),
m_FireEffectList( rhs.m_FireEffectList ),

//Reload
m_ReloadAnimation( rhs.m_ReloadAnimation ),
m_ReloadMoveAnimation( rhs.m_ReloadMoveAnimation ),
m_fReloadTimeRate( rhs.m_fReloadTimeRate ),
m_LevelTimeRate( rhs.m_LevelTimeRate ),
m_ReloadEffect( rhs.m_ReloadEffect ),

//Aim
m_AimEffect( rhs.m_AimEffect ),
m_fAimAngle( rhs.m_fAimAngle ),
m_fAimRange( rhs.m_fAimRange ),
m_fMinRange( rhs.m_fMinRange ),
m_bEnableDownAim( rhs.m_bEnableDownAim ),
m_fMarkerRange( rhs.m_fMarkerRange ),

//Retreat
m_szRetreatMoveFr( rhs.m_szRetreatMoveFr ),
m_szRetreatMoveBk( rhs.m_szRetreatMoveBk ),
m_szRetreatMoveRt( rhs.m_szRetreatMoveRt ),
m_szRetreatMoveLt( rhs.m_szRetreatMoveLt ),
m_fRetreatMoveRateFr( rhs.m_fRetreatMoveRateFr ),
m_fRetreatMoveRateBk( rhs.m_fRetreatMoveRateBk ),
m_fRetreatMoveRateRt( rhs.m_fRetreatMoveRateRt ),
m_fRetreatMoveRateLt( rhs.m_fRetreatMoveRateLt ),
m_fRetreatMoveForce( rhs.m_fRetreatMoveForce ),
m_fRetreatMoveFriction( rhs.m_fRetreatMoveFriction ),

//Jump
m_JumpFireMotionList( rhs.m_JumpFireMotionList ),
m_vJumpAttributeList( rhs.m_vJumpAttributeList ),

//Crush Bubble Attack
m_CrushBubbleAttack( rhs.m_CrushBubbleAttack ),
m_vCheckCrushBubbleBuff( rhs.m_vCheckCrushBubbleBuff ),
m_fCrushBubbleBuffCheckRange( rhs.m_fCrushBubbleBuffCheckRange )
{
	m_iCurCombo = 0;
	if( m_iMaxBullet > 0 )
		m_iCurBullet = m_iMaxBullet;
	else
		m_iCurBullet = 0;

	m_ChargeState = CS_NONE;

	m_fReloadTimeModifyRate = FLOAT1;
	m_bSetReloadMoveAni = false;
	m_dwReloadEndTime = 0;
	m_dwReloadGapTime = 0;
	m_dwNextReloadTime = 0;
	m_AimStartDirKey = ioUserKeyInput::DKI_NONE;
	m_bAimMotionSetted = false;
	m_bFirstAimMoveMotion = true;
	m_vAimedDir = ioMath::VEC3_ZERO;
	m_dwRotateStartTime = 0;
	m_dwRotateEndTime = 0;
	m_fCurAngle = 0.0f;

	m_dwMoveForceTime = 0;
	m_vMoveForceDir = ioMath::VEC3_ZERO;

	m_bChargeRetreateMove = false;
	m_bAimedState = false;
}

ioBubbleFighterItem::~ioBubbleFighterItem(void)
{
}

ioWeaponItem::WeaponSubType ioBubbleFighterItem::GetSubType() const
{
	return WST_BUBBLE_FIGHTER;
}

ioItem* ioBubbleFighterItem::Clone()
{
	return new ioBubbleFighterItem( *this );
}

void ioBubbleFighterItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );
	char szBuf[MAX_PATH] = "";
	//Bullet
	m_iMaxBullet = rkLoader.LoadInt_e( "max_bullet", 0 );

	//Mark
	m_fMarkerRange = rkLoader.LoadFloat_e( "marker_range", 0.0f );
	rkLoader.LoadString_e( "aim_effect", "", szBuf, MAX_PATH );
	m_AimEffect = szBuf;

	//Target
	m_bEnableDownAim = rkLoader.LoadBool_e( "enable_down_aim", false );
	m_fAimAngle = rkLoader.LoadFloat_e( "aim_angle", 45.0f );
	m_fAimRange = rkLoader.LoadFloat_e( "aim_range", FLOAT1000 );
	m_fMinRange = rkLoader.LoadFloat_e( "aim_min_range", 0.0f );

	LoadChargeAttack( rkLoader );

	LoadReload( rkLoader );

	LoadRetreat( rkLoader );

	LoadJumpAttack( rkLoader );

	LoadCrushBubbleAttack( rkLoader );
}

void ioBubbleFighterItem::LoadChargeAttack( ioINILoader &rkLoader )
{
	int i=0;
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	//Attack Ready
	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	if ( iMaxCombo > 0 )
	{
		m_AttackReadyAniList.reserve( iMaxCombo );
		for( i=0; i < iMaxCombo; ++i )
		{
			wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
			rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
			m_AttackReadyAniList.push_back( ioHashString(szBuf) );
		}
	}

	//Charge
	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	m_vExtendAttributeList.clear();
	m_vExtendAttributeList.reserve( iExtendCnt );
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}

	m_MotionInfoList.clear();
	m_MotionInfoList.reserve( iExtendCnt );
	for( i=0; i < iExtendCnt; ++i )
	{
		MotionInfo kInfo;

		wsprintf_e( szKey, "fire_up_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FireMotion.m_FireUp = szBuf;

		wsprintf_e( szKey, "fire_center_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FireMotion.m_FireCenter = szBuf;

		wsprintf_e( szKey, "fire_down_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FireMotion.m_FireDown = szBuf;

		wsprintf_e( szKey, "aim_move_front%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_AimMoveFront = szBuf;

		wsprintf_e( szKey, "aim_move_back%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_AimMoveBack = szBuf;

		wsprintf_e( szKey, "aim_move_left%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_AimMoveLeft = szBuf;

		wsprintf_e( szKey, "aim_move_right%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_AimMoveRight = szBuf;

		m_MotionInfoList.push_back( kInfo );
	}

	m_fAimMoveAniRate = rkLoader.LoadFloat_e( "aim_move_ani_rate", FLOAT1 );

	m_FireEffectList.clear();
	m_FireEffectList.reserve( iExtendCnt );
	for( i=0; i < iExtendCnt; i++ )
	{
		wsprintf_e( szKey, "fire_effect%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_FireEffectList.push_back( ioHashString(szBuf) );
	}
}

void ioBubbleFighterItem::LoadRetreat( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "retreat_move_ani_fr", "", szBuf, MAX_PATH );
	m_szRetreatMoveFr = szBuf;
	rkLoader.LoadString_e( "retreat_move_ani_bk", "", szBuf, MAX_PATH );
	m_szRetreatMoveBk = szBuf;
	rkLoader.LoadString_e( "retreat_move_ani_rt", "", szBuf, MAX_PATH );
	m_szRetreatMoveRt = szBuf;
	rkLoader.LoadString_e( "retreat_move_ani_lt", "", szBuf, MAX_PATH );
	m_szRetreatMoveLt = szBuf;

	m_fRetreatMoveRateFr = rkLoader.LoadFloat_e( "retreat_move_ani_rate_fr", 0 );
	m_fRetreatMoveRateBk = rkLoader.LoadFloat_e( "retreat_move_ani_rate_bk", 0 );
	m_fRetreatMoveRateRt = rkLoader.LoadFloat_e( "retreat_move_ani_rate_rt", 0 );
	m_fRetreatMoveRateLt = rkLoader.LoadFloat_e( "retreat_move_ani_rate_lt", 0 );

	m_fRetreatMoveForce = rkLoader.LoadFloat_e( "retreat_move_force", 0 );
	m_fRetreatMoveFriction = rkLoader.LoadFloat_e( "retreat_move_friction", 0 );
}

void ioBubbleFighterItem::LoadReload( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	//Reload
	rkLoader.LoadString_e( "reload_animation", "", szBuf, MAX_PATH );
	m_ReloadAnimation = szBuf;
	rkLoader.LoadString_e( "reload_move_animation", "", szBuf, MAX_PATH );
	m_ReloadMoveAnimation = szBuf;
	rkLoader.LoadString_e( "reload_effect", "", szBuf, MAX_PATH );
	m_ReloadEffect = szBuf;
	m_fReloadTimeRate  = rkLoader.LoadFloat_e( "reload_time_rate", FLOAT1 );
	m_LevelTimeRate.clear();
	int iLevelCount = rkLoader.LoadInt_e( "level_count", 0 );
	for( int i=0; i<iLevelCount; i++ )
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
}

void ioBubbleFighterItem::LoadJumpAttack( ioINILoader &rkLoader )
{
	int i=0;
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	//Attack Ready
	int iMaxCombo = rkLoader.LoadFloat_e( "bubble_jump_attack_cnt", 0 );
	m_vJumpAttributeList.clear();
	m_vJumpAttributeList.reserve( iMaxCombo );
	for( i=0 ; i<iMaxCombo ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "bubble_jump_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vJumpAttributeList.push_back( kAttribute );
	}

	m_JumpFireMotionList.clear();
	m_JumpFireMotionList.reserve( iMaxCombo );
	for( i=0; i < iMaxCombo; ++i )
	{
		FireMotion kInfo;

		wsprintf_e( szKey, "bubble_jump_attack_fire_up_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FireUp = szBuf;

		wsprintf_e( szKey, "bubble_jump_attack_fire_center_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FireCenter = szBuf;

		wsprintf_e( szKey, "bubble_jump_attack_fire_down_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FireDown = szBuf;

		m_JumpFireMotionList.push_back( kInfo );
	}
}

void ioBubbleFighterItem::LoadCrushBubbleAttack( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	int iMaxBuffCount = rkLoader.LoadInt_e( "check_crush_bubble_max_buff_cnt", 0 );
	m_vCheckCrushBubbleBuff.clear();
	if ( iMaxBuffCount > 0 )
	{
		m_vCheckCrushBubbleBuff.reserve( iMaxBuffCount );
		for( int i=0; i < iMaxBuffCount; ++i )
		{
			wsprintf_e( szBuf, "check_crush_bubble_buff%d", i+1 );
			rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
			m_vCheckCrushBubbleBuff.push_back( ioHashString(szBuf) );
		}
	}

	LoadAttackAttribute( "crush_bubble_attack", m_CrushBubbleAttack, rkLoader );
	m_fCrushBubbleBuffCheckRange = rkLoader.LoadFloat_e( "crush_bubble_buff_check_range", 0.0f );
}

void ioBubbleFighterItem::ClearData()
{
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	m_bChargeRetreateMove = false;
	m_bAimedState = false;
}

void ioBubbleFighterItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	pOwner->SetTargetMarker( ioTargetMarker::MT_BOW_ITEM,
		m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker, "" );

	pOwner->SetTargetMarkerRange( m_fMarkerRange );
}

void ioBubbleFighterItem::OnReleased( ioBaseChar *pOwner )
{
	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "", "" );
	m_fReloadTimeModifyRate = FLOAT1;

	ioWeaponItem::OnReleased( pOwner );
}

void ioBubbleFighterItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	int iBullet = min( iCurBullet, m_iMaxBullet );
	iBullet = max( 0, iBullet );

	m_iCurBullet = iBullet;
}

void ioBubbleFighterItem::WasteBullet()
{
	if( m_iMaxBullet == 0 ) return;

	m_iCurBullet--;
	m_iCurBullet = max( m_iCurBullet, 0 );
}

void ioBubbleFighterItem::SetNoneState( ioBaseChar* pOwner, int eNewState /*= 0 */ )
{
	m_AimedTarget.Clear();
	m_bAimedBall = false;

	m_bAimedState = false;

	m_ChargeState = CS_NONE;
	m_bChargeRetreateMove = false;
}

void ioBubbleFighterItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	ClearCancelInfo();
	ClearData();

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_fCurAngle = 0.0f;
		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState = CS_CHARGING;
		m_dwMotionStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		if( m_bChargeAutoTarget )
		{
			DWORD dwTrackingTime = FRAMEGETTIME();
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
		else
			pOwner->StopAutoTargetTracking();
	}
}

void ioBubbleFighterItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		ProcessCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ProcessNormalAttack( pOwner );
		break;
	case CS_CHARGE_ATTACK:
		ProcessChargeAttack( pOwner );
		break;
	case CS_RETREAT_MOVE:
		ProcessRetreatMove( pOwner );
		break;
	case CS_CRUSH_BUBBLE_ATTACK:
		ProcessCrushBubbleAttack( pOwner );
		break;
	}

	UpdateAimDir( pOwner );
	CheckAimMoveAniState( pOwner );
}

void ioBubbleFighterItem::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;
	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwMotionStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if ( CheckEnableCrushBubbleAttack( pOwner ) )
			{
				ChangeToCrushBubbleAttack( pOwner );
				return;
			}
			else if( EnableBulletAttack() )
			{
				ChangeToChargeAttackState( pOwner );
				return;
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
		if( EnableBulletAttack() )
		{
			ChangeToNormalAttack( pOwner );
			return;
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

void ioBubbleFighterItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	ClearCancelInfo();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		if( !m_bChargeAutoTarget )
			pOwner->SetAutoTarget( ATT_NORMAL );

		pOwner->SetNormalAttackByAttribute( *pAttribute );

		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( ST_NORMAL_ATTACK );
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBubbleFighterItem::ProcessNormalAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{
		const DWORD& dwKeyReserveTime = pOwner->GetCharKeyReserveEnableTime();
		CheckCancelReserve( pOwner );
		if( ProcessCancel(pOwner) )
			return;
		else if ( dwKeyReserveTime && dwKeyReserveTime < FRAMEGETTIME() )
		{
			//대쉬
			if( pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
			{
				ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
				ChangeToRetreatMoveState( pOwner, eDir, false );
				return;
			}
		}
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

void ioBubbleFighterItem::ChangeToChargeAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ClearData();
	m_iCurCombo = 0;
	
	m_ChargeState = CS_CHARGE_ATTACK;
	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->StopAutoTargetTracking();
	pOwner->SetAttackMoveEnable( true );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	
	ClearCancelInfo();
	m_bFirstAimMoveMotion = true;
	m_bAimMotionSetted = true;

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	m_vStartDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	m_bAimedState = false;
	if( !m_bAimEffect )
	{
		pOwner->AttachEffect( m_AimEffect );
		m_bAimEffect = true;
	}

	pGrp->ClearAllLoopAni( FLOAT10, true );
	if ( pOwner->IsNeedProcess() )
	{
		m_AimStartDirKey = pOwner->GetCurDirKey();
		m_AimedTarget.Clear();
		m_bAimedBall = false;
		// 타겟 찾기
		ioHashString szTarget;
		if( pOwner->CheckAimedTargetByBow( m_fAimAngle, m_fMinRange, m_fAimRange, szTarget, m_bEnableDownAim ) )
			m_AimedTarget = szTarget;
		// 타겟 비교
		if( CompareAimedTarget( pOwner ) )
		{
			m_AimedTarget.Clear();
			m_bAimedBall = true;
		}
	}
	
	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

	if( !m_AimedTarget.IsEmpty() || m_bAimedBall )
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
	else
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );

	SetChangeFireMotion( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( ST_CHARGE_ATTACK );
		kPacket << pOwner->GetTargetRot();
		kPacket << (int)m_AimStartDirKey;
		kPacket << m_AimedTarget;
		kPacket << m_bAimedBall;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBubbleFighterItem::ProcessChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	// 시간체크...
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMotionEndTime < dwCurTime )
	{
		// 버튼 뗌 : 종료동작으로...
		if( !pOwner->IsAttackKeyDown() )
		{
			pOwner->NormalAttackOver();
			SetNoneState( pOwner );
			return;
		}

		// 다음 발사
		if( IsCanFire( pOwner, 1 ) )
		{
			// 타겟 체크
			if( !CheckCurrentTargetValid( pOwner, true ) )
			{
				// 타겟 찾기
				ioHashString szTarget;

				m_AimedTarget.Clear();
				m_bAimedBall = false;

				if( pOwner->CheckAimedTargetByBow( m_fAimAngle, m_fMinRange, m_fAimRange, szTarget, m_bEnableDownAim ) )
				{
					m_AimedTarget = szTarget;
				}

				// 타겟 비교
				if( CompareAimedTarget( pOwner ) )
				{
					m_AimedTarget.Clear();
					m_bAimedBall = true;
				}

				pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

				if( !m_AimedTarget.IsEmpty() || m_bAimedBall )
					pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
				else
					pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			}

			m_iCurCombo++;
			if ( m_iCurCombo >= (int)m_MotionInfoList.size() )
				m_iCurCombo = 0;
			SetChangeFireMotion( pOwner );

			
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << static_cast<BYTE>( ST_CHARGE_ATTACK_COMBO );
				kPacket << m_AimedTarget;
				kPacket << m_bAimedBall;
				kPacket << m_iCurCombo;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			
			return;
		}
		else
		{
			pOwner->NormalAttackOver();

			if( SetExtendDefenseState( pOwner ) )
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
			return;
		}
	}
	else if ( m_dwKeyReserveTime && m_dwKeyReserveTime < dwCurTime )
	{
		// 점프
		if( pOwner->IsJumpKeyDown() && !pOwner->IsBuffLimitJump() )
		{
			pOwner->SetJumpState();
			return;
		}

		//대쉬
		if( pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
		{
			ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
			ChangeToRetreatMoveState( pOwner, eDir, true );
			
			return;
		}
	}
}

//Reload
bool ioBubbleFighterItem::CheckExtendDefenseEnable( ioBaseChar *pChar )
{
	if( m_iCurBullet == m_iMaxBullet)
		return false;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return false;
	int iAniID = pGrp->GetAnimationIdx( m_ReloadAnimation );
	if( iAniID == -1 )
		return false;

	return true;
}

bool ioBubbleFighterItem::SetExtendDefenseState( ioBaseChar *pOwner )
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

		std::vector<float>::iterator iter = m_LevelTimeRate.begin();
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
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/m_fReloadTimeModifyRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwReloadEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate - 100;
	m_dwReloadGapTime = ( m_dwReloadEndTime - dwCurTime ) / m_iMaxBullet;
	m_dwNextReloadTime = dwCurTime + m_dwReloadGapTime;

	pOwner->SetState( CS_EXTEND_DEFENSE );
	pOwner->SetReloadMoveEnable( true );

	if( !m_ReloadEffect.IsEmpty() )
	{
		pOwner->ShowWeaponItemMesh( false );
		pOwner->AttachEffect( m_ReloadEffect );
	}

	return true;
}

void ioBubbleFighterItem::CheckExtendDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwReloadEndTime + pOwner->GetActionStopTotalDelay() > dwCurTime )
	{
		// Check Skill ( Reload 일때만 )
		int iSkillNum = pOwner->GetSkillKeyInput();
		if( iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(0, SUT_NORMAL))
		{
			m_bSetReloadMoveAni = false;
			m_ChargeState = CS_NONE;
			pOwner->SetReloadMoveEnable( false );
			/*
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << false;	//AimJump
			kPacket << (int)m_FireState;
			kPacket << m_iCurCombo;
			kPacket << false;
			P2PNetwork::SendToAllPlayingUser( kPacket );
			*/
			return;
		}

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
		m_ChargeState = CS_NONE;
		pOwner->SetReloadMoveEnable( false );
		pOwner->SetState( CS_DELAY );

		/*
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << false;	//AimJump
			kPacket << (int)m_FireState;
			kPacket << m_iCurCombo;
			kPacket << true;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		*/
	}

	g_TutorialMgr.ActionWeaponCharge();
}

void ioBubbleFighterItem::CheckDefenseMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsCanReloadMove() )
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

void ioBubbleFighterItem::OnEndExtendDefense( ioBaseChar *pOwner )
{
	if( !m_ReloadEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_ReloadEffect, false );
		pOwner->ShowWeaponItemMesh( true );
	}
}

void ioBubbleFighterItem::CheckAimMoveAniState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsCanAttackMove() )
		return;
	if( !COMPARE( m_iCurCombo, 0, (int)m_MotionInfoList.size() ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		if( pOwner->IsChangeDirection() || m_bFirstAimMoveMotion )
		{
			pGrp->ClearAllLoopAni( FLOAT10 );

			m_bAimMotionSetted = false;
			m_bFirstAimMoveMotion = false;

			ioUserKeyInput::DirKeyInput eXformDir;
			eXformDir = pOwner->GetXformDir( m_AimStartDirKey );

			switch( eXformDir )
			{
			case ioUserKeyInput::DKI_UP:
				pGrp->SetLoopAni( m_MotionInfoList[m_iCurCombo].m_AimMoveFront, FLOAT100, FLOAT1, m_fAimMoveAniRate );
				break;
			case ioUserKeyInput::DKI_RIGHTUP:
			case ioUserKeyInput::DKI_RIGHT:
			case ioUserKeyInput::DKI_RIGHTDOWN:
				pGrp->SetLoopAni( m_MotionInfoList[m_iCurCombo].m_AimMoveRight, FLOAT100, FLOAT1, m_fAimMoveAniRate );
				break;
			case ioUserKeyInput::DKI_DOWN:
				pGrp->SetLoopAni( m_MotionInfoList[m_iCurCombo].m_AimMoveBack, FLOAT100, FLOAT1, m_fAimMoveAniRate );
				break;
			case ioUserKeyInput::DKI_LEFTDOWN:
			case ioUserKeyInput::DKI_LEFT:
			case ioUserKeyInput::DKI_LEFTUP:
				pGrp->SetLoopAni( m_MotionInfoList[m_iCurCombo].m_AimMoveLeft, FLOAT100, FLOAT1, m_fAimMoveAniRate );
				break;
			}
			g_TutorialMgr.ActionAimingMove();
		}
	}
	else
	{
		if( !m_bAimMotionSetted )
		{
			pGrp->ClearAllLoopAni( FLOAT10 );

			m_bAimMotionSetted = true;
		}
	}
}

bool ioBubbleFighterItem::CompareAimedTarget( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall )
		return false;

	ioBaseChar *pChar = pOwner->GetBaseChar( m_AimedTarget );

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vCharDir  = pOwner->GetMoveDir();

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

bool ioBubbleFighterItem::SetChangeFireMotion( ioBaseChar *pOwner )
{
	if( !COMPARE( m_iCurCombo, 0, (int)m_vExtendAttributeList.size() ) )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	// FireDir 처리
	// 발사 모션 처리
	int	  iAniID = -1;
	float fTimeRate = FLOAT1;
	float fWeight = 0.0f;

	pGrp->ClearAllActionAni( 0.0f, true );

	const AttackAttribute *pAttr = &m_vExtendAttributeList[m_iCurCombo];
	if( pAttr )
	{
		iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
		fTimeRate = pAttr->m_fAttackAniRate;

		pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
		pOwner->IncreaseWeaponIndexBase();
	}

	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( pTarget )
	{
		D3DXVECTOR3 vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		D3DXVec3Normalize( &vAimedDir, &vAimedDir );

		fWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
		m_vAimedDir = vAimedDir;

		if( m_vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, 0, false, true );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, 0, false, true );
		}

		m_bAimedState = true;
	}
	else if( m_bAimedBall && pBall )
	{
		D3DXVECTOR3 vAimedDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		D3DXVec3Normalize( &vAimedDir, &vAimedDir );

		fWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
		m_vAimedDir = vAimedDir;

		if( m_vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, 0, false, true );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, 0, false, true );
		}

		m_bAimedState = true;
	}
	else
	{
		ioHashString szFireUp, szFireCenter;
		if( COMPARE( m_iCurCombo, 0, (int)m_MotionInfoList.size() ) )
		{
			szFireCenter = m_MotionInfoList[m_iCurCombo].m_FireMotion.m_FireCenter;
			pGrp->SetActionAni( szFireCenter, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0, 0, true );
		}

		m_bAimedState = false;
	}


	// 시간값들 갱신
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwRotateStartTime = dwCurTime;

	m_dwRotateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if ( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwCurTime;

	return true;
}

float ioBubbleFighterItem::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
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

void ioBubbleFighterItem::AimedFireToUp( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay,bool bJump, bool bHold )
{
	float fFadeOut = 0.0f;

	bool bAniFx1 = false;
	bool bAniFx2 = true;

	if( fWeight > FLOAT05 )
	{
		bAniFx1 = true;
		bAniFx2 = false;
	}
	
	ioHashString szFireUp, szFireCenter;
	if( bJump )
	{
		if( COMPARE( m_iCurCombo, 0, (int)m_JumpFireMotionList.size() ) )
		{
			szFireUp = m_JumpFireMotionList[m_iCurCombo].m_FireUp;
			szFireCenter = m_JumpFireMotionList[m_iCurCombo].m_FireCenter;
		}
	}
	else
	{
		if( COMPARE( m_iCurCombo, 0, (int)m_MotionInfoList.size() ) )
		{
			szFireUp = m_MotionInfoList[m_iCurCombo].m_FireMotion.m_FireUp;
			szFireCenter = m_MotionInfoList[m_iCurCombo].m_FireMotion.m_FireCenter;
		}
	}
	

	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( szFireUp,
			0.0f, fFadeOut,
			fWeight, FLOAT1/fTimeRate,
			0.0f, dwPreDelay,
			bHold, bAniFx1 );
	}

	pGrp->SetActionAni( szFireCenter,
		0.0f, fFadeOut,
		1.0f - fWeight, FLOAT1/fTimeRate,
		0.0f, dwPreDelay,
		bHold, bAniFx2 );
}

void ioBubbleFighterItem::AimedFireToDown( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bJump, bool bHold )
{
	float fFadeOut = 0.0f;

	bool bAniFx1 = true;
	bool bAniFx2 = false;

	if( fWeight > FLOAT05 )
	{
		bAniFx1 = false;
		bAniFx2 = true;
	}

	ioHashString szFireDown, szFireCenter;
	if( bJump )
	{
		if( COMPARE( m_iCurCombo, 0, (int)m_JumpFireMotionList.size() ) )
		{
			szFireDown = m_JumpFireMotionList[m_iCurCombo].m_FireDown;
			szFireCenter = m_JumpFireMotionList[m_iCurCombo].m_FireCenter;
		}
	}
	else
	{
		if( COMPARE( m_iCurCombo, 0, (int)m_MotionInfoList.size() ) )
		{
			szFireDown = m_MotionInfoList[m_iCurCombo].m_FireMotion.m_FireDown;
			szFireCenter = m_MotionInfoList[m_iCurCombo].m_FireMotion.m_FireCenter;
		}
	}
	

	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( szFireCenter,
			0.0f, fFadeOut,
			1.0f - fWeight, FLOAT1/fTimeRate,
			0.0f, dwPreDelay,
			bHold, bAniFx1 );
	}

	pGrp->SetActionAni( szFireDown,
		0.0f, fFadeOut,
		fWeight, FLOAT1/fTimeRate,
		0.0f, dwPreDelay,
		bHold, bAniFx2 );
}

bool ioBubbleFighterItem::IsCanFire( const ioBaseChar* pOwner, int iFireCnt /*= 0*/, bool bNormalAttack /*= false */ ) const
{
	if( m_iMaxBullet == 0 )
		return true;

	if( bNormalAttack )
		return true;

	if( m_iCurBullet >= iFireCnt )
		return true;

	return false;
}

bool ioBubbleFighterItem::CheckCurrentTargetValid( ioBaseChar *pOwner, bool bAngleAlsoCheck )
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

		float fHalfCosine = cosf( DEGtoRAD( m_fAimAngle * FLOAT05 ) );
		if( D3DXVec3Dot( &m_vStartDir, &vDiff ) < fHalfCosine )
			return false;
	}

	return true;
}

void ioBubbleFighterItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioArrowWeapon *pArrow = ToArrowWeapon( pWeapon );
	ioMissileWeapon *pMissile = ToMissileWeapon( pWeapon );
	ioBombMissileWeapon *pBombMissile = ToBombMissileWeapon( pWeapon );
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );

	// m_bSetAimedDir 해제하기 전에 해야함
	if( pArrow || pMissile || pBombMissile )
	{
		WasteBullet();
	}

	D3DXVECTOR3 vAimedDir = m_vAimedDir;
	if( bJumpAttack )
	{
		if( m_bAimedState )
		{
			vAimedDir = ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
			pWeapon->SetMoveDir( vAimedDir );
		}

		if( pArrow || pMissile || pBombMissile )
			SetFireEffect( pWeapon->GetOwner(), vAimedDir );
	}
	else
	{
		if( m_bAimedState && m_ChargeState == CS_CHARGE_ATTACK )
		{
			vAimedDir = ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
			pWeapon->SetMoveDir( vAimedDir );
		}

		if( pArrow || pMissile || pBombMissile )
			SetFireEffect( pWeapon->GetOwner(), vAimedDir );
	}

	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, false );
		}
	}
}

D3DXVECTOR3 ioBubbleFighterItem::ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon )
{
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !pTarget && !m_bAimedBall )
		return m_vAimedDir;

	ioBall *pBall = pOwner->FindBall();
	if( !pTarget && !pBall ) return m_vAimedDir;

	D3DXVECTOR3 vTargetPos;
	D3DXVECTOR3 vWeaponPos = pWeapon->GetPosition();

	if( pTarget )
		vTargetPos = pTarget->GetMidPositionByRate( FLOAT08);
	else if( m_bAimedBall && pBall )
		vTargetPos = pBall->GetMidPositionByRate();

	D3DXVECTOR3 vAimedDir =  vTargetPos - vWeaponPos;
	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	vAimedDir.x = m_vAimedDir.x;
	vAimedDir.z = m_vAimedDir.z;

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	return vAimedDir;
}

void ioBubbleFighterItem::SetFireEffect( ioBaseChar *pOwner, const D3DXVECTOR3 &vDir )
{
	if( !pOwner ) return;

	int iSize = m_FireEffectList.size();
	if( iSize == 0 ) return;

	if( !COMPARE( m_iCurCombo, 0, iSize ) )
		return;

	ioHashString szCurEffect = m_FireEffectList[m_iCurCombo];
	if( m_bAimedState )
	{
		pOwner->CreateMapEffectWithDir( szCurEffect, vDir );
	}
	else
	{
		D3DXVECTOR3 vNormalDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		pOwner->CreateMapEffectWithDir( szCurEffect, vNormalDir );
	}
}

void ioBubbleFighterItem::ChangeToRetreatMoveState( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eDir, bool bChargeMove )
{
	if ( !pOwner )
		return;

	if ( eDir == ioUserKeyInput::DKI_NONE )
		return;

	if( pOwner->GetState() != CS_ATTACK )
		pOwner->SetState( CS_ATTACK );

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	m_bChargeRetreateMove = bChargeMove;

	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	m_bFirstAimMoveMotion = true;

	// KeyDir
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
		if( !m_szRetreatMoveFr.IsEmpty() )
			CheckFireAndMoveAni( pOwner, m_szRetreatMoveFr, m_fRetreatMoveRateFr, vMoveDir );
		else
			LOG.PrintTimeAndLog( 0, "No Front Ani" );
		break;
	case 1:
		if( iRest > 15 )
		{
			if( !m_szRetreatMoveRt.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_szRetreatMoveRt, m_fRetreatMoveRateRt, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Right Ani" );
		}
		else
		{
			if( !m_szRetreatMoveFr.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_szRetreatMoveFr, m_fRetreatMoveRateFr, vMoveDir );
		}
		break;
	case 2:
		if( iRest <= 40 )
		{
			if( !m_szRetreatMoveRt.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_szRetreatMoveRt, m_fRetreatMoveRateRt, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Right Ani" );
		}
		else
		{
			if( !m_szRetreatMoveBk.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_szRetreatMoveBk, m_fRetreatMoveRateBk, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Back Ani" );
		}
		break;
	case 3:
	case 4:
		if( !m_szRetreatMoveBk.IsEmpty() )
			CheckFireAndMoveAni( pOwner, m_szRetreatMoveBk, m_fRetreatMoveRateBk, vMoveDir );
		else
			LOG.PrintTimeAndLog( 0, "No Back Ani" );
		break;
	case 5:
		if( iRest > 5 )
		{
			if( !m_szRetreatMoveLt.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_szRetreatMoveLt, m_fRetreatMoveRateLt, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Left Ani" );
		}
		else
		{
			if( !m_szRetreatMoveBk.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_szRetreatMoveBk, m_fRetreatMoveRateBk, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Back Ani" );
		}
		break;
	case 6:
		if( iRest <= 30 )
		{
			if( !m_szRetreatMoveLt.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_szRetreatMoveLt, m_fRetreatMoveRateLt, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Left Ani" );
		}
		else
		{
			if( !m_szRetreatMoveFr.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_szRetreatMoveFr, m_fRetreatMoveRateFr, vMoveDir );
		}
		break;
	}

	m_ChargeState = CS_RETREAT_MOVE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( ST_RETREAT_MOVE );
		kPacket << pOwner->GetTargetRot();
		kPacket << (int)eDir;
		kPacket << m_bChargeRetreateMove;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBubbleFighterItem::CheckFireAndMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
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
	m_dwMotionEndTime = dwCurTime + dwFullAniTime;

	DWORD dwTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" ) * fTimeRate;
	m_dwMoveForceTime = dwCurTime + dwTime;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	m_dwKeyReserveTime += dwCurTime;

	m_vMoveForceDir = vDir;
}

void ioBubbleFighterItem::ProcessRetreatMove( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwMotionEndTime < dwCurTime )
	{
		if ( m_bChargeRetreateMove )
		{
			m_ChargeState = CS_CHARGE_ATTACK;
			float fYawD = 180.0f + 45.0f * ( m_AimStartDirKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );
			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );
			pOwner->SetTargetRot( qtRotate );
			return;
		}
		else
		{
			ProcessReserveKeyInput( pOwner );
		}
	}
	else if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		// 예약체크
		CheckReserve( pOwner );
	}


	if( m_dwMoveForceTime > 0 && m_dwMoveForceTime < dwCurTime )
	{
		m_dwMoveForceTime = 0;
		pOwner->SetForcePower( m_vMoveForceDir, m_fRetreatMoveForce, m_fRetreatMoveFriction );
	}
}

bool ioBubbleFighterItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;
	if( !bCheck ) return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_NORMAL_ATTACK:
		case CS_CHARGE_ATTACK:
			return true;
		}
	}

	return false;
}

bool ioBubbleFighterItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	if ( pOwner->GetState() != CS_ATTACK && pOwner->GetState() != CS_EXTEND_DASH )
		return false;
	
	if ( pOwner->GetState() == CS_ATTACK && m_ChargeState == CS_RETREAT_MOVE )
		return false;

	return true;
}

void ioBubbleFighterItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if ( !pOwner )
		return;

	if ( pOwner->IsNeedProcess() )
		pOwner->SetCurNormalAttackItem( this );
	ChangeToRetreatMoveState( pOwner, eCurrDir, false );
}

bool ioBubbleFighterItem::EnableBulletAttack()
{
	if ( m_iCurBullet >= 1 || m_iMaxBullet == 0 )
		return true;

	return false;
}

void ioBubbleFighterItem::SetBubbleFighterJumpAttack( ioBaseChar *pOwner, OUT DWORD& dwInput_Dash_S_StartTime, OUT ioHashString& szTarget, OUT bool& bAimedBall )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->StopAutoTargetTracking();

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ChangeDirectionExplicitIfDirChanged( true );
	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	m_vStartDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	if ( pOwner->IsNeedProcess() )
	{
		m_AimedTarget.Clear();
		m_bAimedBall = false;

		// 타겟 찾기
		ioHashString szTarget;
		if( pOwner->CheckAimedTargetByBow( m_fAimAngle, m_fMinRange, m_fAimRange, szTarget, m_bEnableDownAim ) )
			m_AimedTarget = szTarget;

		// 타겟 비교
		if( CompareAimedTarget( pOwner ) )
		{
			m_bAimedBall = true;
			m_AimedTarget.Clear();
		}
	}

	m_bAimedState = false;
	m_iCurCombo = 0;
	m_fCurAngle = 0.0f;
	if( !m_bAimEffect )
	{
		pOwner->AttachEffect( m_AimEffect );
		m_bAimEffect = true;
	}

	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->ClearAllLoopAni( FLOAT10, true );

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

	if( !m_AimedTarget.IsEmpty() || m_bAimedBall )
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
	else
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );

	SetChangeJumpFireMotion( pOwner, dwInput_Dash_S_StartTime );
	szTarget = m_AimedTarget;
	bAimedBall = m_bAimedBall;
	
}

void ioBubbleFighterItem::ApplyBubbleFighterJumpAttack( ioBaseChar *pOwner, OUT DWORD& dwInput_Dash_S_StartTime, IN const ioHashString& szTarget, IN const bool& bAimedBall )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->StopAutoTargetTracking();

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ChangeDirectionExplicitIfDirChanged( true );
	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	m_vStartDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	m_AimedTarget = szTarget;
	m_bAimedBall = bAimedBall;

	m_bAimedState = false;
	m_iCurCombo = 0;
	m_fCurAngle = 0.0f;
	if( !m_bAimEffect )
	{
		pOwner->AttachEffect( m_AimEffect );
		m_bAimEffect = true;
	}

	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->ClearAllLoopAni( FLOAT10, true );

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

	if( !m_AimedTarget.IsEmpty() || m_bAimedBall )
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
	else
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );

	SetChangeJumpFireMotion( pOwner, dwInput_Dash_S_StartTime );
}

bool ioBubbleFighterItem::SetChangeJumpFireMotion( ioBaseChar *pOwner, DWORD& dwInput_Dash_S_StartTime )
{
	if( !COMPARE( m_iCurCombo, 0, (int)m_vJumpAttributeList.size() ) )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return false;

	// FireDir 처리
	// 발사 모션 처리
	int	  iAniID = -1;
	float fTimeRate = FLOAT1;
	float fWeight = 0.0f;

	pGrp->ClearAllActionAni( 0.0f, true );

	const AttackAttribute *pAttr = &m_vJumpAttributeList[m_iCurCombo];
	if( pAttr )
	{
		iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
		fTimeRate = pAttr->m_fAttackAniRate;

		pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, 0 );
		pOwner->IncreaseWeaponIndexBase();

		// AirJumpPower
		SetAirJump( pOwner, iAniID, fTimeRate );
	}

	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( pTarget )
	{
		D3DXVECTOR3 vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		D3DXVec3Normalize( &vAimedDir, &vAimedDir );

		fWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
		m_vAimedDir = vAimedDir;

		if( m_vAimedDir.y >= 0.0f )
			AimedFireToUp( pGrp, fWeight, fTimeRate, 0, true, true );
		else
			AimedFireToDown( pGrp, fWeight, fTimeRate, 0, true, true );

		m_bAimedState = true;
	}
	else if( m_bAimedBall && pBall )
	{
		D3DXVECTOR3 vAimedDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		D3DXVec3Normalize( &vAimedDir, &vAimedDir );

		fWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
		m_vAimedDir = vAimedDir;

		if( m_vAimedDir.y >= 0.0f )
			AimedFireToUp( pGrp, fWeight, fTimeRate, 0, true, true );
		else
			AimedFireToDown( pGrp, fWeight, fTimeRate, 0, true, true );

		m_bAimedState = true;
	}
	else
	{
		ioHashString szFireUp, szFireCenter;
		if( COMPARE( m_iCurCombo, 0, (int)m_JumpFireMotionList.size() ) )
		{
			szFireCenter = m_JumpFireMotionList[m_iCurCombo].m_FireCenter;
			pGrp->SetActionAni( szFireCenter, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0, 0, true );
		}

		m_bAimedState = false;
	}


	// 시간값들 갱신
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwRotateStartTime = dwCurTime;

	m_dwRotateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if ( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwCurTime;

	DWORD dwInputDashS = pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" ) * fTimeRate;
	if( 0 < dwInputDashS )
		dwInput_Dash_S_StartTime = dwCurTime + dwInputDashS;

	return true;
}

void ioBubbleFighterItem::ProcessBubbleFighterJumpAttack( ioBaseChar *pOwner, DWORD& dwInput_Dash_S_StartTime )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMotionEndTime < dwCurTime )
	{
		if ( !pOwner->IsNeedProcess() )
			return;

		// 버튼 뗌 : 
		if( !pOwner->IsAttackKeyDown() )
		{
			SetNoneState( pOwner );
			return;
		}

		// 다음 발사
		if( IsCanFire( pOwner, 1 ) )
		{
			if( !CheckCurrentTargetValid( pOwner, true ) )
			{
				// 타겟 찾기
				ioHashString szTarget;

				m_AimedTarget.Clear();
				m_bAimedBall = false;

				if( pOwner->CheckAimedTargetByBow( m_fAimAngle, m_fMinRange, m_fAimRange, szTarget, m_bEnableDownAim ) )
				{
					m_AimedTarget = szTarget;
				}

				// 타겟 비교
				if( CompareAimedTarget( pOwner ) )
				{
					m_AimedTarget.Clear();
					m_bAimedBall = true;
				}

				pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

				if( !m_AimedTarget.IsEmpty() && m_bAimedBall )
					pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
				else
					pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			}

			m_iCurCombo++;
			if ( m_iCurCombo >= (int)m_vJumpAttributeList.size() )
				m_iCurCombo = 0;
			SetChangeJumpFireMotion( pOwner, dwInput_Dash_S_StartTime );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << static_cast<BYTE>(ST_JUMP_ATTACK);
				kPacket << m_iCurCombo;
				kPacket << m_AimedTarget;
				kPacket << m_bAimedBall;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
		else
		{
			SetNoneState( pOwner );
			return;
		}
	}
	else
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
	}

	UpdateAimDir( pOwner );
}

void ioBubbleFighterItem::UpdateAimDir( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;
	ioBiped *pBiped = pGrp->GetBiped("Bip01 Spine1");
	if(!pBiped)
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	bool bStateCheck = false;
	if ( ( pOwner->GetState() == CS_ATTACK && m_ChargeState == CS_CHARGE_ATTACK ) ||
		pOwner->GetState() == CS_JUMP )
		bStateCheck = true;

	if ( !bStateCheck )
		return;

	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !bStateCheck || (!pTarget && !m_bAimedBall) || !CheckCurrentTargetValid( pOwner, true ) )
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
	DWORD dwTimeGap = dwCurTime - m_dwRotateStartTime;
	if( dwTimeGap < m_dwRotateEndTime )
	{
		fAimTimeRate = (float)dwTimeGap / (float)m_dwRotateEndTime;
	}

	fRotAngle = m_fCurAngle * fAimTimeRate;


	D3DXQUATERNION qtExtraRot;
	D3DXQuaternionRotationYawPitchRoll( &qtExtraRot, 0.0f, fRotAngle, 0.0f );
	pBiped->SetExtraRotation( qtExtraRot, true );
	
}

void ioBubbleFighterItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if ( !pOwner )
		return;

	BYTE btState;
	rkPacket >> btState;

	if ( btState == ST_NORMAL_ATTACK )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		rkPacket >> m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeToNormalAttack( pOwner );
	}
	else if ( btState == ST_CHARGE_ATTACK )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );
		int iDir;
		rkPacket >> iDir;
		m_AimStartDirKey = (ioUserKeyInput::DirKeyInput)iDir;
		rkPacket >> m_AimedTarget;
		rkPacket >> m_bAimedBall;
		ChangeToChargeAttackState( pOwner );
	}
	else if ( btState == ST_CHARGE_ATTACK_COMBO )
	{
		rkPacket >> m_AimedTarget;
		rkPacket >> m_bAimedBall;
		rkPacket >> m_iCurCombo;

		pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
		if( !m_AimedTarget.IsEmpty() || m_bAimedBall )
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
		else
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
		SetChangeFireMotion( pOwner );
	}
	else if ( btState == ST_RETREAT_MOVE )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );
		int iDir;
		rkPacket >> iDir;
		rkPacket >> m_bChargeRetreateMove;
		ChangeToRetreatMoveState( pOwner, (ioUserKeyInput::DirKeyInput)iDir, m_bChargeRetreateMove );
	}
	else if ( btState == ST_CRUSH_BUBBLE_ATTACK )
	{
		rkPacket >> m_CrushBubbleAttackTargetDir;
		ChangeToCrushBubbleAttack( pOwner );
	}
}

void ioBubbleFighterItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if ( !pOwner )
		return;

	BYTE btState;
	rkPacket >> btState;

	if ( btState == ST_JUMP_ATTACK )
	{
		rkPacket >> m_iCurCombo;
		rkPacket >> m_AimedTarget;
		rkPacket >> m_bAimedBall;

		pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
		if( !m_AimedTarget.IsEmpty() || m_bAimedBall )
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
		else
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
		DWORD dwInput_Dash_s;
		SetChangeJumpFireMotion( pOwner, dwInput_Dash_s );
	}
	
}

bool ioBubbleFighterItem::CheckEnableCrushBubbleAttack( ioBaseChar *pOwner )
{
	if ( m_vCheckCrushBubbleBuff.empty())
		return false;

	if ( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;

	int iBuffSize = m_vCheckCrushBubbleBuff.size();
	ioHashString szOwnerName = pOwner->GetCharName();
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	m_CrushBubbleAttackTargetDir = ioMath::VEC3_ZERO;
	float fMaxRangeSq = m_fCrushBubbleBuffCheckRange * m_fCrushBubbleBuffCheckRange;
	float fMinRange = fMaxRangeSq;

	ioBaseChar* pAttackTarget = NULL;

	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;

		if( pTarget == pOwner )
			continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;

		if( !pTarget->IsEnableTarget() )
			continue;

		bool bPass = false;
		for ( int i=0; i < iBuffSize ; ++i )
		{
			if ( pTarget->HasBuff( m_vCheckCrushBubbleBuff[i] ) )
				bPass = true;
		}

		if ( !bPass )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		float fLengthSq = D3DXVec3LengthSq( &vDiff );
		bool bRangeAvailable = false;
		if( COMPARE( fLengthSq, 0.0f, fMaxRangeSq ) )
			bRangeAvailable = true;
		if( bRangeAvailable )
		{	
			if ( fMinRange > fLengthSq )
			{
				fMinRange = fLengthSq;
				vDiff.y = 0;
				D3DXVec3Normalize( &vDiff, &vDiff );
				m_CrushBubbleAttackTargetDir = vDiff;
			}
		}
	}

	if ( m_CrushBubbleAttackTargetDir == ioMath::VEC3_ZERO )
		return false;

	return true;
}

void ioBubbleFighterItem::ChangeToCrushBubbleAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if ( m_CrushBubbleAttackTargetDir == ioMath::VEC3_ZERO )
		return;

	ClearData();

	m_ChargeState = CS_CRUSH_BUBBLE_ATTACK;
	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->StopAutoTargetTracking();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();

	ClearCancelInfo();

	int iAniID = pGrp->GetAnimationIdx( m_CrushBubbleAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	float fTimeRate = m_CrushBubbleAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_CrushBubbleAttack.m_dwPreDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;
	
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_CrushBubbleAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();

	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_CrushBubbleAttack.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	m_dwMotionEndTime = dwStartTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += m_CrushBubbleAttack.m_dwEndDelay;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwStartTime;

	SetCurAttackAniForInputJump( m_CrushBubbleAttack.m_AttackAnimation, fTimeRate );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->SetTargetRotToDir( m_CrushBubbleAttackTargetDir, true );
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( ST_CRUSH_BUBBLE_ATTACK );
		kPacket << m_CrushBubbleAttackTargetDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBubbleFighterItem::ProcessCrushBubbleAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTIme = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTIme )
	{
		ProcessReserveKeyInput( pOwner );
		return;
	}
	else if ( m_dwKeyReserveTime && m_dwKeyReserveTime < dwCurTIme )
		CheckReserve( pOwner );
}
