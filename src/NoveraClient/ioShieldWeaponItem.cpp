

#include "stdafx.h"

#include "ioShieldWeaponItem.h"

#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioShieldWeaponItem::ioShieldWeaponItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iSkillInput = -1;
	m_bDefenseFirstMoveMotion = true;
	m_bDefenseMotionSetted = false;
	m_bEnableDefenseState = false;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
}

ioShieldWeaponItem::ioShieldWeaponItem( const ioShieldWeaponItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
  m_AniDefenseFrontMove( rhs.m_AniDefenseFrontMove ),
  m_AniDefenseBackMove( rhs.m_AniDefenseBackMove ),
  m_AniDefenseRightMove( rhs.m_AniDefenseRightMove ),
  m_AniDefenseLeftMove( rhs.m_AniDefenseLeftMove )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_bDefenseFirstMoveMotion = true;
	m_bDefenseMotionSetted = false;
	m_bEnableDefenseState = false;

	m_iSkillInput = -1;
}

ioShieldWeaponItem::~ioShieldWeaponItem()
{
}

void ioShieldWeaponItem::LoadProperty( ioINILoader &rkLoader )
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
}

ioItem* ioShieldWeaponItem::Clone()
{
	return new ioShieldWeaponItem( *this );
}

ioWeaponItem::WeaponSubType ioShieldWeaponItem::GetSubType() const
{
	return WST_SHIELD;
}

void ioShieldWeaponItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
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

void ioShieldWeaponItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_ATTACK_FIRE:
		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	}
}

void ioShieldWeaponItem::OnCharging( ioBaseChar *pOwner )
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

void ioShieldWeaponItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
{
	if( bCharged )
	{
		int iExtendCnt = m_vExtendAttributeList.size();
		if( COMPARE( m_iCurCombo, 0, iExtendCnt ) )
		{
			pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[m_iCurCombo] );
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
		}

		pOwner->ApplyNormalAttackResult( m_iCurCombo );
	}

	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_iCurCombo;
		kPacket << bCharged;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioShieldWeaponItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	bool bCharged;
	rkPacket >> m_iCurCombo;
	rkPacket >> bCharged;
	ChangeToAttackFire( pOwner, bCharged );
}

void ioShieldWeaponItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );
}

bool ioShieldWeaponItem::SetExtendDefenseState( ioBaseChar *pOwner )
{
	if( pOwner->IsPrisonerMode() ) return false;
	if( pOwner->IsCatchMode() ) return false;
	if( pOwner->IsHasCrown() ) return false;

	pOwner->SetState( CS_EXTEND_DEFENSE );
	pOwner->SetChargingState( true );
	pOwner->SetDefenseMoveEnable( true, false );

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

void ioShieldWeaponItem::CheckExtendDefenseState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

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
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else if( pOwner->IsAttackKeyDown() && pOwner->GetUseEnableSkill( 0, SUT_NORMAL ) )
	{
		int iSkillNum = pOwner->GetSkillKeyInput();
		if( iSkillNum == 0 && !pOwner->IsBuffLimitSkill() )
		{
			EndDefense( pOwner );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_ChargeState;
				kPacket << m_iCurCombo;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}

			if( pOwner->SetUseSkill(0, SUT_NORMAL) )
			{
				return;
			}
		}
	}
}

void ioShieldWeaponItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iChargeState;
	rkPacket >> iChargeState;
	rkPacket >> m_iCurCombo;

	if( iChargeState == CS_NONE )
	{
		EndDefense( pOwner );
	}
}

void ioShieldWeaponItem::EndDefense( ioBaseChar *pOwner )
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

bool ioShieldWeaponItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

void ioShieldWeaponItem::CheckDefenseMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanDefenseMove( false ) )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		if( pOwner->IsChangeDirection() || m_bDefenseFirstMoveMotion )
		{
			m_bDefenseMotionSetted = false;
			m_bDefenseFirstMoveMotion = false;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			CheckDefenseMoveAniDir( pOwner );

			/*
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
			*/

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

bool ioShieldWeaponItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioShieldWeaponItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioShieldWeaponItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioShieldWeaponItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioShieldWeaponItem::CheckDefenseMoveAniDir( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	// KeyDir
	D3DXVECTOR3 vDefenseDir = pOwner->GetDefenseDir();
	D3DXVec3Normalize( &vDefenseDir, &vDefenseDir );

	ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
	float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
	D3DXVECTOR3 vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vMoveDir.y = 0.0f;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	D3DXQUATERNION qtDefenseRot;
	ioMath::CalcDirectionQuaternion( qtDefenseRot, -vDefenseDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );

	float fCurYaw, fTargetYaw;
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtDefenseRot ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iSide = (int)fAngleGap % 360;

	int iCnt = iSide / 45;
	int iRest = iSide % 45;

	ioHashString szCurMoveAni;

	switch( iCnt )
	{
	case 0:
		if( !m_AniDefenseFrontMove.IsEmpty() )
			szCurMoveAni = m_AniDefenseFrontMove;
		break;
	case 7:
		if( !m_AniDefenseFrontMove.IsEmpty() )
			szCurMoveAni = m_AniDefenseFrontMove;
		break;
	case 1:
		if( iRest > 15 )
		{
			if( !m_AniDefenseRightMove.IsEmpty() )
				szCurMoveAni = m_AniDefenseRightMove;
		}
		else
		{
			if( !m_AniDefenseFrontMove.IsEmpty() )
				szCurMoveAni = m_AniDefenseFrontMove;
		}
		break;
	case 2:
		if( iRest <= 40 )
		{
			if( !m_AniDefenseRightMove.IsEmpty() )
				szCurMoveAni = m_AniDefenseRightMove;
		}
		else
		{
			if( !m_AniDefenseBackMove.IsEmpty() )
				szCurMoveAni = m_AniDefenseBackMove;
		}
		break;
	case 3:
	case 4:
		if( !m_AniDefenseBackMove.IsEmpty() )
			szCurMoveAni = m_AniDefenseBackMove;
		break;
	case 5:
		if( iRest > 5 )
		{
			if( !m_AniDefenseLeftMove.IsEmpty() )
				szCurMoveAni = m_AniDefenseLeftMove;
		}
		else
		{
			if( !m_AniDefenseBackMove.IsEmpty() )
				szCurMoveAni = m_AniDefenseBackMove;
		}
		break;
	case 6:
		if( iRest <= 30 )
		{
			if( !m_AniDefenseLeftMove.IsEmpty() )
				szCurMoveAni = m_AniDefenseLeftMove;
		}
		else
		{
			if( !m_AniDefenseFrontMove.IsEmpty() )
				szCurMoveAni = m_AniDefenseFrontMove;
		}
		break;
	}

	if( !szCurMoveAni.IsEmpty() )
	{
		pGrp->SetLoopAni( szCurMoveAni, FLOAT100, FLOAT1 );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioShieldWeaponItem::CheckDefenseMoveAniDir MoveAni Fail!" );
	}
}




