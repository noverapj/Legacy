
#include "stdafx.h"

#include "ioShieldWeaponItem3.h"

#include "ioBaseChar.h"
#include "WeaponDefine.h"

ioShieldWeaponItem3::ioShieldWeaponItem3()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;

	m_bDefenseFirstMoveMotion = true;
	m_bDefenseMotionSetted = false;

	m_bCharged = false;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;
	m_vAniRotateInfoList.clear();

	m_DefenceCounterState = DCS_None;
}

ioShieldWeaponItem3::ioShieldWeaponItem3( const ioShieldWeaponItem3 &rhs )
	: ioWeaponItem( rhs ),  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	  m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
	  m_AniDefenseFrontMove( rhs.m_AniDefenseFrontMove ),
	  m_AniDefenseBackMove( rhs.m_AniDefenseBackMove ),
	  m_AniDefenseRightMove( rhs.m_AniDefenseRightMove ),
	  m_AniDefenseLeftMove( rhs.m_AniDefenseLeftMove ),
	  m_szRetreatFr( rhs.m_szRetreatFr ),
	  m_szRetreatBack( rhs.m_szRetreatBack ),
	  m_szRetreatLt( rhs.m_szRetreatLt ),
	  m_szRetreatRt( rhs.m_szRetreatRt ),
	  m_fRetreatFrRate( rhs.m_fRetreatFrRate ),
	  m_fRetreatBackRate( rhs.m_fRetreatBackRate ),
	  m_fRetreatLtRate( rhs.m_fRetreatLtRate ),
	  m_fRetreatRtRate( rhs.m_fRetreatRtRate ),
	  m_fRetreatForceAmt( rhs.m_fRetreatForceAmt ),
	  m_fRetreatForceFric( rhs.m_fRetreatForceFric ),
	  m_bUseDefenceCounterAttack( rhs.m_bUseDefenceCounterAttack ),
	  m_bUseDefenceCounterAttackEndDash( rhs.m_bUseDefenceCounterAttackEndDash ),
	  m_bUseSpecialDash( rhs.m_bUseSpecialDash ),
	  m_SpecialDash( rhs.m_SpecialDash ),
	  m_bUseJumpEndSpecialDash( rhs.m_bUseJumpEndSpecialDash )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_bDefenseFirstMoveMotion = true;
	m_bDefenseMotionSetted = false;

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;

	m_bCharged = false;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;
	m_vAniRotateInfoList.clear();

	m_DefenceCounterState = DCS_None;
}

ioShieldWeaponItem3::~ioShieldWeaponItem3()
{
}

void ioShieldWeaponItem3::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "ani_defense_front_move", "victory_defense_move_fr.ani", szBuf, MAX_PATH );
	m_AniDefenseFrontMove = szBuf;
	rkLoader.LoadString_e( "ani_defense_back_move", "victory_defense_move_bk.ani", szBuf, MAX_PATH );
	m_AniDefenseBackMove = szBuf;
	rkLoader.LoadString_e( "ani_defense_right_move", "victory_defense_move_rt.ani", szBuf, MAX_PATH );
	m_AniDefenseRightMove = szBuf;
	rkLoader.LoadString_e( "ani_defense_left_move", "victory_defense_move_lt.ani", szBuf, MAX_PATH );
	m_AniDefenseLeftMove = szBuf;

	//
	rkLoader.LoadString_e( "retreat_animation_front", "", szBuf, MAX_PATH );
	m_szRetreatFr = szBuf;
	m_fRetreatFrRate = rkLoader.LoadFloat_e( "retreat_animation_front_rate", FLOAT1 );
	rkLoader.LoadString_e( "retreat_animation_back", "", szBuf, MAX_PATH );
	m_szRetreatBack = szBuf;
	m_fRetreatBackRate = rkLoader.LoadFloat_e( "retreat_animation_back_rate", FLOAT1 );
	rkLoader.LoadString_e( "retreat_animation_right", "", szBuf, MAX_PATH );
	m_szRetreatRt = szBuf;
	m_fRetreatRtRate = rkLoader.LoadFloat_e( "retreat_animation_right_rate", FLOAT1 );
	rkLoader.LoadString_e( "retreat_animation_left", "", szBuf, MAX_PATH );
	m_szRetreatLt = szBuf;
	m_fRetreatLtRate = rkLoader.LoadFloat_e( "retreat_animation_left_rate", FLOAT1 );

	m_fRetreatForceAmt = rkLoader.LoadFloat_e( "retreat_force_amt", 0.0f );
	m_fRetreatForceFric = rkLoader.LoadFloat_e( "retreat_force_friction", FLOAT1 );
	
	int i=0;
	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	m_vExtendAttributeList.clear();
	m_vExtendAttributeList.reserve( iExtendCnt );
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}

	m_AttackReadyAniList.clear();
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	// 진화 스파르타쿠스 기능 추가
	m_bUseDefenceCounterAttack = rkLoader.LoadBool_e( "enable_defence_counter_attack", false );
	m_bUseDefenceCounterAttackEndDash = rkLoader.LoadBool_e( "enable_defence_counter_attack_end_dash", false );
	m_bUseSpecialDash = rkLoader.LoadBool_e( "enable_special_dash", false );
	m_bUseJumpEndSpecialDash = rkLoader.LoadBool_e( "enable_jump_end_special_dash", false );
	LoadAttackAttribute_e( "special_dash", m_SpecialDash, rkLoader );
}

ioItem* ioShieldWeaponItem3::Clone()
{
	return new ioShieldWeaponItem3( *this );
}

ioWeaponItem::WeaponSubType ioShieldWeaponItem3::GetSubType() const
{
	return WST_SHIELD3;
}

void ioShieldWeaponItem3::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	m_bCharged = false;

	ClearCancelInfo();

	int iExtendCnt = m_vExtendAttributeList.size();
	if( !COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );
		
		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;
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
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );

		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioShieldWeaponItem3::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_ATTACK_FIRE:
		{
			if( m_bCharged )
				pOwner->CheckExtraAniJump();

			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserveByConditionalSpecialDash( pOwner );
				if( ProcessCancelBySpeicalDash( pOwner ) )
					return;

				CheckCancelReserve( pOwner );
				if( ProcessCancel(pOwner) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	case CS_SPECIAL_DASH:
		{
			OnConditionSpecialDash( pOwner );
		}
		break;
	}
}

void ioShieldWeaponItem3::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToAttackFire( pOwner, true );
			g_TutorialMgr.ActionChargeAttack();
		}
	}
	else
	{
		ChangeToAttackFire( pOwner, false );
	}
}

void ioShieldWeaponItem3::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
{
	DWORD dwCurTime = FRAMEGETTIME();
	m_bCharged = bCharged;

	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

	if( m_bCharged )
	{
		int iExtendCnt = m_vExtendAttributeList.size();
		if( COMPARE( m_iCurCombo, 0, iExtendCnt ) )
		{
			m_szCurAni = m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation;
			m_fCurAniRate = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;

			m_dwInputCancelCheckTime = dwCurTime;
			m_dwCancelCheckTime = dwCurTime;

			pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[m_iCurCombo] );

			ioEntityGroup *pGrp = pOwner->GetGroup();
			int iAniID	     = pGrp->GetAnimationIdx( m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation );
			float fTimeRate  = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;
			DWORD dwPreDelay = m_vExtendAttributeList[m_iCurCombo].m_dwPreDelay;

			pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
		}

		if( iExtendCnt <= m_iCurCombo + 1 )
			pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
		else
			pOwner->ApplyNormalAttackResult( m_iCurCombo );
	}
	else
	{
		const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
		if( pAttribute )
		{
			pOwner->SetNormalAttackByAttribute( *pAttribute );

			m_szCurAni					= pAttribute->m_AttackAnimation;
			m_fCurAniRate				= pAttribute->m_fAttackAniRate;
			m_dwInputCancelCheckTime	= dwCurTime;
			m_dwCancelCheckTime			= dwCurTime;
		}

		pOwner->ApplyNormalAttackResult( m_iCurCombo );
	}

	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << CS_ATTACK_FIRE;
		kPacket << m_iCurCombo;
		kPacket << bCharged;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioShieldWeaponItem3::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if ( !pOwner )
		return;

	int nState;
	rkPacket >> nState; 

	switch( nState )
	{
	case CS_ATTACK_FIRE:
		{
			bool bCharged;
			rkPacket >> m_iCurCombo;
			rkPacket >> bCharged;
			ChangeToAttackFire( pOwner, bCharged );
		}
		break;
	case CS_DEFENCE_COUNTER_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );
			DefenceAttack( pOwner );
		}
		break;
	case CS_SPECIAL_DASH:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			
			ChangeConditionSpecialDash( pOwner );
		}
		break;
	}
}

void ioShieldWeaponItem3::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
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

bool ioShieldWeaponItem3::SetExtendDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	if( pOwner->IsPrisonerMode() ) return false;
	if( pOwner->IsCatchMode() ) return false;
	if( pOwner->IsHasCrown() ) return false;

	pOwner->SetState( CS_EXTEND_DEFENSE );
	pOwner->SetChargingState( true );
	pOwner->SetDefenseMoveEnable( m_bDefenseMove, m_bDefenseRoate );

	m_bDefenseFirstMoveMotion = true;
	m_bDefenseMotionSetted = true;
	
	m_bDefenseEnableSet = false;
	m_dwDefenseStartTime = FRAMEGETTIME();
	m_dwDefenseEndTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_iCurCombo = 0;

	return true;
}

void ioShieldWeaponItem3::CheckExtendDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() && pOwner->IsDirKeyDoubleClick() )
	{
		EndDefense( pOwner );

		if( !CheckRetreat( pOwner, true ) )
			LOG.PrintTimeAndLog( 0, "ioShieldWeaponItem3::CheckExtendDefenseState - Retreat Fail" );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_ChargeState;
			kPacket << m_iCurCombo;
			kPacket << true;
			kPacket << m_szCurRetreatAni;
			kPacket << m_fCurRetreatAniRate;
			kPacket << m_vForceDir;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		return;
	}

	if( !m_bDefenseEnableSet && m_dwDefenseStartTime+m_dwDefensePreDuration <= dwCurTime )
	{
		m_bDefenseEnableSet = true;
		pOwner->SetDefenseEnable( true );
	}

	if( m_dwDefenseEndTime > 0 )
	{
		if( m_dwDefenseEndTime + m_dwDefenseEndDuration < dwCurTime )
		{
			if( pOwner->IsNeedProcess() )
			{
				if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
				{
					m_iSkillInput = -1;
				}
				else if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
				{
					pOwner->SetDefenseState();
				}
				else if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
				{
					pOwner->SetJumpState();
				}
				else if( m_bEnableAttackState )
				{
					if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
					{
						pOwner->SetNormalAttack( m_iCurCombo );
					}
				}
				else
				{
					pOwner->SetState( CS_DELAY );
				}
			}
			else
			{
				pOwner->SetState( CS_DELAY );
			}
		}
		else
		{
			if( pOwner->IsNeedProcess() )
			{
				bool bReserveSkill = false;
				int iSkillNum = pOwner->GetSkillKeyInput();
				if( iSkillNum >= 0 && !pOwner->IsChatModeState( true ) )
				{
					m_iSkillInput = iSkillNum;
					bReserveSkill = true;
				}

				if( pOwner->IsDefenseKeyDown() && !bReserveSkill && !m_bEnableDefenseState )
				{
					m_bEnableDefenseState = true;
					m_bEnableAttackState = false;
					m_bEnableJumpState = false;
				}
				else if( pOwner->IsAttackKey() && !bReserveSkill && !m_bEnableAttackState )
				{
					m_bEnableAttackState = true;
					m_bEnableDefenseState = false;
					m_bEnableJumpState = false;
				}
				else if( pOwner->IsJumpKey() && !bReserveSkill && !m_bEnableJumpState )
				{
					m_bEnableJumpState = true;
					m_bEnableAttackState = false;
					m_bEnableDefenseState = false;
				}
			}
		}

		return;
	}

	if( !pOwner->IsNeedProcess() ) return;

	if( !m_bDefenseEnableSet )
		return;

	if( m_dwDefenseStartTime+m_dwDefenseDuration > dwCurTime )
		return;

	if( m_dwExtendDefenseWoundedTime > 0 )
	{
		if( m_dwExtendDefenseWoundedTime > dwCurTime )
			return;
		else
			m_dwExtendDefenseWoundedTime = 0;
	}

	if( !pOwner->IsDefenseKeyDown() )
	{
		EndDefense( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_ChargeState;
			kPacket << m_iCurCombo;
			kPacket << false;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioShieldWeaponItem3::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iChargeState;
	rkPacket >> iChargeState;
	rkPacket >> m_iCurCombo;

	bool bRetreat;
	rkPacket >> bRetreat;

	if( bRetreat )
	{
		float fAniRate;
		D3DXVECTOR3 vDir;
		ioHashString szAni;

		rkPacket >> szAni >> fAniRate >> vDir;

		EndDefense( pOwner );
		ChangeToRetreatState( pOwner, szAni, fAniRate, vDir );
	}
	else
	{
		if( iChargeState == CS_NONE )
			EndDefense( pOwner );
	}
}

void ioShieldWeaponItem3::EndDefense( ioBaseChar *pOwner )
{
	pOwner->SetDefenseMoveEnable( false, false );
	pOwner->SetDefenseEnable( false );

	m_ChargeState = CS_NONE;
	m_dwDefenseEndTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		int iAniID = pOwner->GetDelayAnimationIdx();

		pGrp->ClearAllLoopAniExceptOne( iAniID, FLOAT100 );
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->SetLoopAni( iAniID, (float)m_dwDefenseEndDuration );
	}
}

bool ioShieldWeaponItem3::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

void ioShieldWeaponItem3::CheckDefenseMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanDefenseMove( false ) )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		if( pOwner->IsChangeDirection() || m_bDefenseFirstMoveMotion )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			m_bDefenseMotionSetted = false;
			m_bDefenseFirstMoveMotion = false;

			if( m_bDefenseRoate )
			{
				pGrp->SetLoopAni( m_AniDefenseFrontMove, FLOAT100, FLOAT1 );
			}
			else
			{
				ioUserKeyInput::DirKeyInput eXformDir;
				eXformDir = pOwner->GetXformDir( pOwner->GetDefenseDirKey() );

				switch( eXformDir )
				{
				case ioUserKeyInput::DKI_UP:
					pGrp->SetLoopAni( m_AniDefenseFrontMove, FLOAT100, FLOAT1 );
					break;
				case ioUserKeyInput::DKI_RIGHTUP:
				case ioUserKeyInput::DKI_RIGHT:
				case ioUserKeyInput::DKI_RIGHTDOWN:
					pGrp->SetLoopAni( m_AniDefenseRightMove, FLOAT100, FLOAT1 );
					break;
				case ioUserKeyInput::DKI_DOWN:
					pGrp->SetLoopAni( m_AniDefenseBackMove, FLOAT100, FLOAT1 );
					break;
				case ioUserKeyInput::DKI_LEFTDOWN:
				case ioUserKeyInput::DKI_LEFT:
				case ioUserKeyInput::DKI_LEFTUP:
					pGrp->SetLoopAni( m_AniDefenseLeftMove, FLOAT100, FLOAT1 );
					break;
				}
			}

			g_TutorialMgr.ActionDefenseMove();
		}
	}
	else
	{
		if( !m_bDefenseMotionSetted )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			
			int iAniID = pOwner->GetDefenseAnimationIdx();
			pGrp->SetLoopAni( iAniID, 50 );

			m_bDefenseMotionSetted = true;
		}
	}
}

void ioShieldWeaponItem3::CheckRetreatState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	CheckAniRotate( pOwner );

	if( m_dwRetreatEndTime > 0 && m_dwRetreatEndTime < dwCurTime )
	{
		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			ClearState();

			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
			return;
		}

		// 필살기 예약되었으면 필살기 발동
		if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
		{
			ClearState();
			return;
		}

		// 방어
		if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
		{
			ClearState();
			pOwner->SetDefenseState();
			return;
		}

		// 점프
		if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			ClearState();
			pOwner->SetJumpState();
			return;
		}

		// 공격
		if( m_bEnableAttackState )
		{
			ClearState();
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
			return;
		}

		ClearState();
		pOwner->SetState( CS_DELAY );
		return;
	}
	else
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			CheckReserve( pOwner );
		}

		if( m_dwRetreatForceTime > 0 && m_dwRetreatForceTime < dwCurTime )
		{
			m_dwRetreatForceTime = 0;
			pOwner->SetForcePower( m_vForceDir, m_fRetreatForceAmt, m_fRetreatForceFric );
		}
	}
}

bool ioShieldWeaponItem3::CheckRetreat( ioBaseChar *pOwner, bool bFront )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	// KeyDir
	ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
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
		if( !m_szRetreatFr.IsEmpty() )
		{
			if( bFront )
			{
				ChangeToRetreatState( pOwner, m_szRetreatFr, m_fRetreatFrRate, vMoveDir );
				return true;			
			}
		}
		break;
	case 7:
		if( !m_szRetreatFr.IsEmpty() )
		{
			if( bFront )
			{
				ChangeToRetreatState( pOwner, m_szRetreatFr, m_fRetreatFrRate, vMoveDir );
				return true;
			}
		}
		break;
	case 1:
		if( iRest > 15 )
		{
			if( !m_szRetreatRt.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatRt, m_fRetreatRtRate, vMoveDir );
				return true;
			}
		}
		else
		{
			if( bFront && !m_szRetreatFr.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatFr, m_fRetreatFrRate, vMoveDir );
				return true;
			}
		}
		break;
	case 2:
		if( iRest <= 40 )
		{
			if( !m_szRetreatRt.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatRt, m_fRetreatRtRate, vMoveDir );
				return true;
			}
		}
		else
		{
			if( !m_szRetreatBack.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatBack, m_fRetreatBackRate, vMoveDir );
				return true;
			}
		}
		break;
	case 3:
	case 4:
		if( !m_szRetreatBack.IsEmpty() )
		{
			ChangeToRetreatState( pOwner, m_szRetreatBack, m_fRetreatBackRate, vMoveDir );
			return true;
		}
		break;
	case 5:
		if( iRest > 5 )
		{
			if( !m_szRetreatLt.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatLt, m_fRetreatLtRate, vMoveDir );
				return true;
			}
		}
		else
		{
			if( !m_szRetreatBack.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatBack, m_fRetreatBackRate, vMoveDir );
				return true;
			}
		}
		break;
	case 6:
		if( iRest <= 30 )
		{
			if( !m_szRetreatLt.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatLt, m_fRetreatLtRate, vMoveDir );
				return true;
			}
		}
		else
		{
			if( bFront && !m_szRetreatFr.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatFr, m_fRetreatFrRate, vMoveDir );
				return true;
			}
		}
		break;
	}

	return false;
}

void ioShieldWeaponItem3::ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearDirDoubleClick();
	pOwner->ClearReservedSliding();

	pOwner->SetForcePowerAmt( 0.0f );

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;

	float fCurRate = fAniRate;
	if( fCurRate <= 0.0f )
		fCurRate = FLOAT1;

	m_ChargeState = CS_NONE;

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vDir );

	pOwner->SetMoveDirByRotate( qtTargetRot );

	int iAniID = pGrp->GetAnimationIdx( szAni );
	pGrp->ClearAllActionAni( 0.0f );
	pGrp->ClearAllLoopAni( 0.0f );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	m_dwRetreatEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fCurRate;

	DWORD dwTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" );
	m_dwRetreatForceTime = dwCurTime + dwTime;

	m_szCurRetreatAni = szAni;
	m_fCurRetreatAniRate = fCurRate;
	m_vForceDir = vDir;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fCurRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime = dwCurTime + dwKeyReserve;

	pOwner->CheckCharColSkipTime( iAniID, fCurRate, 0 );

	pOwner->SetState( CS_RETREAT_STATE );

	SetAniRotate( pOwner, iAniID, fCurRate );
}

void ioShieldWeaponItem3::ClearState()
{
	m_ChargeState = CS_NONE;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
}

bool ioShieldWeaponItem3::ProcessCancel( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( !pOwner ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );

	std::list<float> vTimeList;
	pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );

	if( vTimeList.empty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	std::list< float >::iterator iter = vTimeList.begin();
	while( iter != vTimeList.end() )
	{
		DWORD dwTime = (DWORD)*iter;
		dwTime *= m_fCurAniRate;

		dwTime += m_dwInputCancelCheckTime;

		if( m_dwCancelCheckTime > 0 && COMPARE( dwTime, m_dwCancelCheckTime, dwCurTime ) )
		{
			m_dwCancelCheckTime = dwTime+1;

			if( pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
			{
				pOwner->ChangeDirectionByInputDir( false );
				pOwner->SetDashState( false );
				pOwner->SendDashState( false );

				return true;
			}		
			else if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
			{
				pOwner->SetJumpState();
				return true;
			}
			return false;
		}

		if( dwTime > dwCurTime )
			return false;

		iter++;
	}

	return false;
}

bool ioShieldWeaponItem3::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if ( !m_bUseSpecialDash )
	{
		if( !IsChargeDash() ) 
			return false;

		if( !m_bCharged ) 
			return false;
	}

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_ATTACK_FIRE:
			if( !bCheck )
				return false;

			return true;
		}
	}

	return false;
}

bool ioShieldWeaponItem3::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioShieldWeaponItem3::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioShieldWeaponItem3::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioShieldWeaponItem3::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

// 진화 스파르타쿠스 기능 추가
bool ioShieldWeaponItem3::IsUseDefenceCounterAttack()
{
	return m_bUseDefenceCounterAttack;
}

void ioShieldWeaponItem3::SetDefenceCounterState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_stDefenceCounterDelayAni );
	if ( iAniID == -1 )
		return;

	float fTimeRate = m_fDefenceCounterDelayAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	pOwner->ClearDirDoubleClick();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwDefenceDelayEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_DefenceCounterState = DCS_Delay;
}

void ioShieldWeaponItem3::DefenceAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_DefenceCounterAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate = m_DefenceCounterAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_DefenceCounterAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	pOwner->RefreshFireTimeList( iAniID, m_DefenceCounterAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	pOwner->SetReservedSliding( m_DefenceCounterAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwDefenceAttackEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_DefenceCounterState = DCS_Attack;

	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << CS_DEFENCE_COUNTER_ATTACK;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioShieldWeaponItem3::ProcessDefenceCounterAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	switch ( m_DefenceCounterState )
	{
	case DCS_Delay:
		{
			if( dwCurTime > m_dwDefenceDelayEndTime )
			{
				pOwner->SetState( CS_DELAY, true );
				return;
			}

			if( pOwner->IsNeedProcess() )
			{
				if( pOwner->IsAttackKey() )
					DefenceAttack( pOwner );
			}
		}
		break;
	case DCS_Attack:
		{
			if( m_dwDefenceAttackEndTime < dwCurTime )
			{
				pOwner->SetState( CS_DELAY, true );
				return;
			}

			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserveByConditionalSpecialDash( pOwner );
				if( ProcessCancelBySpeicalDash( pOwner ) )
					return;
			}
		}
		break;
	}
}

bool ioShieldWeaponItem3::IsUseDefenceCounterAttackEndDash()
{
	return m_bUseDefenceCounterAttackEndDash;
}

bool ioShieldWeaponItem3::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	return m_bUseSpecialDash;
}

void ioShieldWeaponItem3::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner )
		return;

	if( eCurrDir == ioUserKeyInput::DKI_NONE )
		return;

	pOwner->SetTargetRotToDirKey( eCurrDir );

	ChangeConditionSpecialDash( pOwner );
}

void ioShieldWeaponItem3::ChangeConditionSpecialDash( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_SpecialDash.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate  = m_SpecialDash.m_fAttackAniRate;
	DWORD dwPreDelay = m_SpecialDash.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetReservedSliding( m_SpecialDash.m_vForceInfoList, iAniID, fTimeRate, 0 );

	pOwner->RefreshFireTimeList( iAniID, m_SpecialDash.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwSpecialDashEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwKeyReserveTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_NORMAL );
	DWORD dwTrackingTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	ClearCancelInfo();

	pOwner->ClearDirDoubleClick();
	pOwner->ClearSkillReserveKey();
	pOwner->ClearReserveKeyInput();

	pOwner->SetCurNormalAttackItem( this );
	pOwner->SetState( CS_ATTACK );

	m_ChargeState = CS_SPECIAL_DASH;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << CS_SPECIAL_DASH;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioShieldWeaponItem3::OnConditionSpecialDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwSpecialDashEndTime > 0 && m_dwSpecialDashEndTime < dwCurTime )
	{
		if( pOwner->IsNeedProcess() )
			ProcessReserveKeyInput( pOwner );
		else
			pOwner->SetState( CS_DELAY );
	}
	else
	{
		if( pOwner->IsNeedProcess() )
		{
			if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
				CheckReserve( pOwner );
		}
	}
}

bool ioShieldWeaponItem3::IsJumpEndSpecialDash()
{
	return m_bUseJumpEndSpecialDash;
}