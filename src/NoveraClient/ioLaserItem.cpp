

#include "stdafx.h"

#include "ioLaserItem.h"

#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioLaserItem::ioLaserItem()
{
	m_dwAimTime = 1000;
	m_fAimAngle = 45.0f;
	m_fAimRange = FLOAT1000;
	m_fMinRange = FLOAT100;

	m_fMarkerRange = 0.0f;
	m_fFireTimeModifyRate = FLOAT1;

	m_fAirJumpPower = 0;
	m_fAirForcePower = 0;

	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;
	m_dwFireDelayTime = 0;

	m_dwChargeStartTime = 0;
	m_dwAimStartTime = 0;
	m_iCurCombo = 0;
	m_FireState = FS_NONE;

	m_bSetSkillAimed = false;
	m_bSetAimedDir = false;
	m_bAimJumpAttack = false;

	m_fCurAngle = 0.0f;
	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_dwJumpMarkerEndTime = 0;

	m_dwCheckAimCancelTime = 0;
	m_dwTrackingEndTime = 0;
	m_dwFireStateEndTime = 0;

	m_bZeroAimState = false;
	m_dwZeroAimStartTime = 0;

	m_dwCurCoolTime = 0;
	m_dwCheckCoolTime = 0;
}

ioLaserItem::ioLaserItem( const ioLaserItem &rhs )
: ioWeaponItem( rhs ),
 m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
 m_ExtendZeroAimed( rhs.m_ExtendZeroAimed ),
 m_MissFire( rhs.m_MissFire ),
 m_fMissFireRate( rhs.m_fMissFireRate ),
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
 m_dwJumpMarkerDuration( rhs.m_dwJumpMarkerDuration ),
 m_AimJumpReadyAnimation( rhs.m_AimJumpReadyAnimation ),
 m_AimJumppingAnimation( rhs.m_AimJumppingAnimation ),
 m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
 m_bEnableZeroAiming( rhs.m_bEnableZeroAiming ),
 m_bEnableDownAim( rhs.m_bEnableDownAim ),
 m_vSecondAttributeList( rhs.m_vSecondAttributeList ),
 m_vSecondZeroAttributeList( rhs.m_vSecondZeroAttributeList ),
 m_fZeroAimSpeedRate( rhs.m_fZeroAimSpeedRate ),
 m_dwNeedCoolTime( rhs.m_dwNeedCoolTime )
{
	m_dwMaxCoolTime = rhs.m_dwMaxCoolTime;

	m_dwAimTime = rhs.m_dwAimTime;
	m_fAimAngle = rhs.m_fAimAngle;
	m_fAimRange = rhs.m_fAimRange;
	m_fMinRange = rhs.m_fMinRange;

	m_fMarkerRange = rhs.m_fMarkerRange;

	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;
	m_dwFireDelayTime = 0;
	m_fFireTimeModifyRate = FLOAT1;

	m_dwChargeStartTime = 0;
	m_dwAimStartTime = 0;
	m_iCurCombo = 0;
	m_FireState = FS_NONE;

	m_bSetSkillAimed = false;
	m_bSetAimedDir = false;
	m_bAimJumpAttack = false;

	m_fCurAngle = 0.0f;
	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_dwJumpMarkerEndTime = 0;

	m_dwCheckAimCancelTime = 0;
	m_dwTrackingEndTime = 0;
	m_dwFireStateEndTime = 0;

	m_bZeroAimState = false;
	m_dwZeroAimStartTime = 0;

	m_dwCurCoolTime = 0;
	m_dwCheckCoolTime = 0;
}

ioLaserItem::~ioLaserItem()
{
	m_vSecondAttributeList.clear();
	m_vSecondZeroAttributeList.clear();
}

void ioLaserItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

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

	char szKey[MAX_PATH];
	m_AttackReadyAniList.clear();
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szKey, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	rkLoader.LoadString_e( "miss_fire_animation", "", szBuf, MAX_PATH );
	m_MissFire = szBuf;
	m_fMissFireRate = rkLoader.LoadFloat_e( "miss_fire_ani_rate", FLOAT1 );

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

	m_dwMaxCoolTime = (DWORD)rkLoader.LoadInt_e( "max_cool_time", 0 );
	m_dwNeedCoolTime = (DWORD)rkLoader.LoadInt_e( "use_cool_time", 0 );
	m_bEnableZeroAiming = rkLoader.LoadBool_e( "enable_zero_aiming", false );
	m_bEnableDownAim = rkLoader.LoadBool_e( "enable_down_aim", false );

	m_dwZeroAimedCheckTime = rkLoader.LoadInt_e( "zero_aimed_check_time", 1 );
	rkLoader.LoadString_e( "zero_aimed_effect", "", szBuf, MAX_PATH );
	m_ZeroAimedEffect = szBuf;

	m_fZeroAimSpeedRate = rkLoader.LoadFloat_e( "zero_aim_speed_rate", 0.0f );

	m_dwJumpMarkerDuration = rkLoader.LoadFloat_e( "jump_marker_duration", 0 );

	m_dwAimTime = rkLoader.LoadInt_e( "aim_time", 1000 );
	m_fAimAngle = rkLoader.LoadFloat_e( "aim_angle", 45.0f );
	m_fAimRange = rkLoader.LoadFloat_e( "aim_range", FLOAT1000 );
	m_fMinRange = rkLoader.LoadFloat_e( "aim_min_range", 0.0f );

	m_fMarkerRange = rkLoader.LoadFloat_e( "marker_range", 0.0f );
}

ioItem* ioLaserItem::Clone()
{
	return new ioLaserItem( *this );
}

void ioLaserItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	int iExtendCnt = m_vExtendAttributeList.size();
	if( COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		m_FireState = FS_CHARGING;
		m_dwChargeStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;

		m_fCurAngle = 0.0f;
		m_dwCheckAimCancelTime = 0;

		m_dwZeroAimStartTime = 0;
		m_bZeroAimState = false;

		m_bAimJumpAttack = false;
		m_bSetAimedDir = false;
		m_bAimedState = false;

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

void ioLaserItem::ChangeToAiming( ioBaseChar *pOwner )
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
		{
			m_dwZeroAimStartTime = dwCurTime;
		}
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
	pOwner->ChangeDirectionExplicitIfDirChanged( true );
	
	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	m_vStartDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	m_AimedTarget.Clear();
	m_bAimedBall = false;

	m_bSetAimedDir = false;
	m_bAimedState = false;

	g_TutorialMgr.ActionChangeToAiming();
}

void ioLaserItem::CheckNormalAttack( ioBaseChar *pOwner )
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
		if( pOwner->ChangeDirectionExplicitIfKeyChanged( false ) )
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
					kPacket << false;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
	}

	if( m_FireState != FS_AIMED && m_FireState != FS_AIMING &&
		m_FireState != FS_AIM_WAIT && m_dwZeroAimStartTime > 0 )
	{
		m_dwZeroAimStartTime = 0;
	}

	if( !m_bZeroAimState )
		pOwner->EndEffect( m_ZeroAimedEffect );
}

void ioLaserItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargeStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToAiming( pOwner );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << false;	//AimJump
				kPacket << (int)m_FireState;
				kPacket << m_iCurCombo;
				kPacket << false;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		pOwner->SetChargingState( false );
		ChangeToNormalAttack( pOwner );
	}
}

void ioLaserItem::OnAiming( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() && pOwner->GetState() != CS_JUMP )	// 버튼 뗌, 일반 공격으로 전환
	{
		if( IsCanFire( pOwner, 1 ) )
		{
			ChangeToNormalFire( pOwner );
			return;
		}
		else
		{
			pOwner->SetMissFireState( m_MissFire, m_fMissFireRate );
			m_AimedTarget.Clear();
			m_bAimedBall = false;

			m_FireState = FS_NONE;
			
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
	}

	if( !SetAimJumpState( pOwner ) )
		return;

	if( pOwner->GetState() != CS_JUMP && pOwner->IsDefenseKeyDown() )
	{
		if(IsCanFire(0))
		{
			int iSkillNum = pOwner->GetSkillKeyInput();
			if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(0, SUT_NORMAL))
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

void ioLaserItem::OnAimWait( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() && pOwner->GetState() != CS_JUMP )	// 버튼 뗌, 일반 공격으로 전환
	{
		if( IsCanFire( pOwner, 1 ) )
		{
			ChangeToNormalFire( pOwner );
			return;
		}
		else
		{
			pOwner->SetMissFireState( m_MissFire, m_fMissFireRate );
			m_AimedTarget.Clear();
			m_bAimedBall = false;

			m_FireState = FS_NONE;
			
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
			return;
		}
	}

	if( !SetAimJumpState( pOwner ) )
		return;

	if( pOwner->GetState() != CS_JUMP && pOwner->IsDefenseKeyDown() )
	{
		if(IsCanFire(0))
		{
			int iSkillNum = pOwner->GetSkillKeyInput();
			if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(0, SUT_NORMAL))
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

void ioLaserItem::OnAimed( ioBaseChar *pOwner )
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
			if( IsCanFire( pOwner, 1 ) )
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
			else
			{
				pOwner->SetMissFireState( m_MissFire, m_fMissFireRate );
				m_AimedTarget.Clear();
				m_bAimedBall = false;

				m_FireState = FS_NONE;
				
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
				return;
			}
		}
		else	// AIMED FIRE..
		{
			if( !SetAimJumpState( pOwner ) )
				return;

			if( CheckCurrentTargetValid( pOwner, true ) )
			{
				pOwner->SetTargetMarkerEnable( IsCanFire( pOwner, 1 ) );
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
					kPacket << false;
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

void ioLaserItem::OnFire( ioBaseChar *pOwner )
{
	if( pOwner->CheckDefaultNormalAttack( m_dwFireStartTime,
										  m_dwFireEndTime,
										  m_dwFireDelayTime ) )
	{
		m_FireState = FS_NONE;
	}
}

void ioLaserItem::OnJumpFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwAttackOverTime = m_dwFireStartTime + m_dwFireEndTime + m_dwFireDelayTime;

	if( dwCurTime >= dwAttackOverTime )
	{
		m_FireState = FS_NONE;
		pOwner->SetMoveDirByRotate( pOwner->GetTargetRot() );
	}
}

void ioLaserItem::ChangeToNormalAttack( ioBaseChar *pOwner )
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
		kPacket << false;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioLaserItem::ChangeToAimWaitState( ioBaseChar *pOwner, const ioHashString &szTarget )
{
	m_dwAimStartTime = FRAMEGETTIME();
	m_FireState = FS_AIM_WAIT;
	m_AimedTarget = szTarget;

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMING );
}

void ioLaserItem::ChangeToAimedState( ioBaseChar *pOwner )
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
	m_dwZeroAimStartTime = FRAMEGETTIME();

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

	// 발사가능 여부를 먼저 설정해 둘 것
	pOwner->SetTargetMarkerEnable( IsCanFire(pOwner, 1 ) );

	if( m_bZeroAimState )
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_ZERO_AIMED );
	else
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );

	if( CheckCurrentTargetValid( pOwner, false ) )
	{
		TrackingTarget( pOwner );
	}
}

void ioLaserItem::ChangeToNormalFire( ioBaseChar *pOwner )
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

	m_dwFireStartTime = FRAMEGETTIME();
	DecreaseCoolTime();

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
		kPacket << false;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	g_TutorialMgr.ActionNormalFire();
}

bool ioLaserItem::CheckCurrentTargetValid( ioBaseChar *pOwner, bool bAngleAlsoCheck )
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

void ioLaserItem::TrackingTarget( ioBaseChar *pOwner )
{
	bool bJumpState = false;
	if( pOwner->IsJumpState() )
		bJumpState = true;

	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );

	D3DXVECTOR3 vTargetPos;
	if( pTarget )
		vTargetPos = pTarget->GetMidPositionByRate();
	else if( m_bAimedBall && pBall )
		vTargetPos = pBall->GetMidPositionByRate();

	if( !bJumpState )
		pOwner->SetTargetRotToTargetPos( vTargetPos, true, false );

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

	D3DXVECTOR3 vAimedDir = vTargetPos - pOwner->GetMidPositionByRate();
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

void ioLaserItem::ChangeToAimedFire( ioBaseChar *pOwner )
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

	m_dwFireStartTime = FRAMEGETTIME();
	DecreaseCoolTime();

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
	m_FireState = FS_AIMED_FIRE;

	if( iExtendAttackCnt <= m_iCurCombo + 1 )
		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	else
		pOwner->ApplyNormalAttackResult( m_iCurCombo );

	pOwner->SetChargingState( false );

	DWORD dwComboTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" )*fTimeRate;
	pOwner->SetComboCheckTime( dwComboTime, true );
}

float ioLaserItem::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir,
											  float fMaxAngle )
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

void ioLaserItem::AimedFireToUp( ioEntityGroup *pGrp,
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

void ioLaserItem::AimedFireToDown( ioEntityGroup *pGrp,
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

void ioLaserItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	}
}

void ioLaserItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

	bool bMissFire = false;

	switch( iFireState )
	{
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
	case FS_NONE:
		rkPacket >> bMissFire;

		m_FireState = FS_NONE;
		m_AimedTarget.Clear();
		m_bAimedBall = false;

		if( pOwner->GetState() != CS_JUMP )
		{
			if( !bMissFire )
				pOwner->SetState( CS_DELAY );
			else
				pOwner->SetMissFireState( m_MissFire, m_fMissFireRate );
		}
		break;
	}
}

void ioLaserItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );
	ioMissileWeapon *pMissile = ToMissileWeapon( pWeapon );

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
		if( m_FireState == FS_AIMED_FIRE && m_bSetAimedDir )
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

ioWeaponItem::WeaponSubType ioLaserItem::GetSubType() const
{
	return WST_LASER;
}

void ioLaserItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	pOwner->SetTargetMarker( ioTargetMarker::MT_FIRE_ITEM,
							 m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker,"" );

	pOwner->SetTargetMarkerRange( m_fMarkerRange );

	m_dwCheckCoolTime = FRAMEGETTIME();
}

void ioLaserItem::OnReleased( ioBaseChar *pOwner )
{
	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "","" );

	m_fFireTimeModifyRate = FLOAT1;

	ioWeaponItem::OnReleased( pOwner );
}

bool ioLaserItem::IsCanFire( const ioBaseChar* pOwner, int iFireCnt, bool bNormalAttack ) const
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

	return true;
}

void ioLaserItem::ModifyFireTimeRate( float fRate )
{
	m_fFireTimeModifyRate *= fRate;
}

void ioLaserItem::SetAimedSkillUse( bool bSkillUsed )
{
	m_bSetSkillAimed = bSkillUsed;
}

void ioLaserItem::SetAimedDirSet( bool bAimedSet )
{
	m_bSetAimedDir = bAimedSet;
}

D3DXVECTOR3 ioLaserItem::ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon )
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

	D3DXVECTOR3 vAimedDir = vTargetPos - pWeapon->GetPosition();
	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	
	vAimedDir.x = m_vAimedDir.x;
	vAimedDir.z = m_vAimedDir.z;

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	return vAimedDir;
}

void ioLaserItem::SetFireStartTime( DWORD dwFireTime )
{
	m_dwFireStartTime = dwFireTime;
}

bool ioLaserItem::CheckAimState(bool bAimMove)
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

bool ioLaserItem::SetAimState( ioBaseChar *pChar )
{
	if( pChar->GetState() == CS_JUMP && CheckAimState() )
	{
		m_dwCheckAimCancelTime = 0;
		pChar->GetGroup()->ClearAllActionAni( FLOAT100, true );
		pChar->SetState( CS_ATTACK );

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

void ioLaserItem::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	m_AimedTarget.Clear();
	m_bAimedBall = false;

	m_bSetSkillAimed = false;
	m_bSetAimedDir = false;
	m_bAimedState = false;

	m_bZeroAimState = false;
	m_dwZeroAimStartTime = 0;

	m_FireState = FS_NONE;
}

bool ioLaserItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	ClearAirJump();
	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioLaserItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioLaserItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate = pAttr->m_fAttackAniRate;
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

	m_dwFireStartTime = FRAMEGETTIME();
	DecreaseCoolTime();

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

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	pOwner->EndEffect( m_ZeroAimedEffect );

	return true;
}

void ioLaserItem::CheckJumpAttack( ioBaseChar *pOwner )
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

	UpdateAimDir( pOwner );
}

bool ioLaserItem::IsChargeAttackState( ioBaseChar *pOwner )
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

void ioLaserItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->EndEffect( m_ZeroAimedEffect );
}

bool ioLaserItem::SetAimJumpState( ioBaseChar *pChar )
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

void ioLaserItem::SetAimJumpReady( ioBaseChar *pChar )
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

void ioLaserItem::SetAimJumpping( ioBaseChar *pChar, bool bFullTime )
{
	m_dwCheckAimCancelTime = 0;

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetLoopAni( m_JumpAimCenter, FLOAT100, FLOAT1 );
	pGrp->SetLoopAni( m_JumpAimUp, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_JumpAimDown, 0.0f, 0.0f );

	if( m_bZeroAimState )
	{
		switch( m_FireState )
		{
		case FS_AIMING:
		case FS_AIM_WAIT:
			pChar->SetTargetMarkerState( ioTargetMarker::MS_READY );
			break;
		case FS_AIMED:
			// 발사가능 여부를 먼저 설정해 둘 것
			pChar->SetTargetMarkerEnable( IsCanFire( pChar, 1 ) );
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

void ioLaserItem::SetAimJumpCancel( ioBaseChar *pChar )
{
	m_dwCheckAimCancelTime = 0;
	m_FireState = FS_NONE;

	pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	pChar->SetState( CS_DELAY );
	pChar->EndEffect( m_ZeroAimedEffect );

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

void ioLaserItem::UpdateAimDir( ioBaseChar *pOwner )
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

const ioHashString& ioLaserItem::GetJumppingAnimation() const
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

void ioLaserItem::CheckAimJumpState( ioBaseChar *pChar )
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

	if( m_FireState == FS_AIM_WAIT || m_FireState == FS_AIMING || m_FireState == FS_AIMED )
	{
		if( pChar->IsNeedProcess() && pChar->IsDefenseKeyDown() )
		{
			if(IsCanFire(0))
			{
				bool bAimedDir = m_bSetAimedDir;
				ioHashString szTarget = m_AimedTarget;

				int iSkillNum = pChar->GetSkillKeyInput();
				if(iSkillNum == 0 && pChar->CheckJumpingSkill( 0 ) )
				{
					pChar->SetChargingState( false );
					
					m_bSetSkillAimed = true;
					m_bSetAimedDir = bAimedDir;
					m_AimedTarget = szTarget;

					m_FireState = FS_NONE;

					if( pChar->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
						FillItemGaugePacketHeader( pChar, kPacket );
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

	if( m_FireState != FS_AIMED && m_FireState != FS_AIMING &&
		m_FireState != FS_AIM_WAIT && m_dwZeroAimStartTime > 0 )
	{
		m_dwZeroAimStartTime = 0;
	}

	if( m_bZeroAimState )
		pChar->EndEffect( m_ZeroAimedEffect );

	UpdateAimDir( pChar );
}


bool ioLaserItem::CheckLanding( ioBaseChar *pChar )
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
	pChar->GetGroup()->ReleaseAllExtraRotate();
	return false;
}

bool ioLaserItem::CheckZeroAim( ioBaseChar *pOwner )
{
	if( m_bZeroAimState ) return false;

	if( m_dwZeroAimStartTime == 0 ) return false;

	if( m_dwZeroAimStartTime+m_dwZeroAimedCheckTime > FRAMEGETTIME() )
		return false;

	SetZeroAim( pOwner, true );
	return true;
}

void ioLaserItem::SetZeroAim( ioBaseChar *pOwner, bool bSet )
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
	}
	else
	{
		m_bZeroAimState = false;
		m_dwZeroAimStartTime = 0;
		pOwner->EndEffect( m_ZeroAimedEffect );
	}
}

float ioLaserItem::GetAttackSpeedRate() const
{
	if( m_bZeroAimState && m_fZeroAimSpeedRate > 0.0f )
		return m_fZeroAimSpeedRate;
	else
		return m_fAttackSpeedRate;
}

int ioLaserItem::GetMaxBullet()
{
	return (int)m_dwMaxCoolTime;
}

int ioLaserItem::GetNeedBullet()
{
	return (int)m_dwNeedCoolTime;
}

int ioLaserItem::GetCurBullet()
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwCheckCoolTime;

	m_dwCheckCoolTime = dwCurTime;

	m_dwCurCoolTime += dwGapTime;
	m_dwCurCoolTime = max( 0, min( m_dwCurCoolTime, m_dwMaxCoolTime ) );

	return (int)m_dwCurCoolTime;
}

void ioLaserItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	m_dwCheckCoolTime = FRAMEGETTIME();

	m_dwCurCoolTime = (DWORD)iCurBullet;
	m_dwCurCoolTime = max( 0, min( m_dwCurCoolTime, m_dwMaxCoolTime ) );
}

bool ioLaserItem::IsAimedState()
{
	if( m_FireState == FS_AIMED )
		return true;

	return false;
}

bool ioLaserItem::CompareAimedTarget( ioBaseChar *pOwner )
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

bool ioLaserItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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

void ioLaserItem::InitCoolTime()
{
	m_dwCurCoolTime = 0;
	m_dwCheckCoolTime = FRAMEGETTIME();
}

void ioLaserItem::MaxCoolTime()
{
	m_dwCurCoolTime = m_dwMaxCoolTime;
	m_dwCheckCoolTime = FRAMEGETTIME();
}

void ioLaserItem::DecreaseCoolTime()
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



