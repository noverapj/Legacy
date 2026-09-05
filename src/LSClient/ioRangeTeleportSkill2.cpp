

#include "stdafx.h"

#include "ioRangeTeleportSkill2.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"


#include "WeaponDefine.h"

ioRangeTeleportSkill2::ioRangeTeleportSkill2()
{
	m_iCurLinearCombo = 0;
	m_bUsedComboList = false;
	m_bSetAttackFire = false;

	m_dwAttackEndTime = 0;
	m_dwCurSkillProtectTime = 0;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

ioRangeTeleportSkill2::ioRangeTeleportSkill2( const ioRangeTeleportSkill2 &rhs )
: ioRangeSkill( rhs ),
 m_fBackRange( rhs.m_fBackRange ),
 m_fHeightRange( rhs.m_fHeightRange ),
 m_vComboList( rhs.m_vComboList ),
 m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
 m_bNoGravityCombo( rhs.m_bNoGravityCombo ),
 m_bSelectTeleportDirection( rhs.m_bSelectTeleportDirection ),
 m_fRangeEmptyTarget( rhs.m_fRangeEmptyTarget )
{
	m_iCurLinearCombo = 0;
	m_bUsedComboList = false;
	m_bSetAttackFire = false;

	m_dwAttackEndTime = 0;
	m_dwCurSkillProtectTime = 0;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

ioRangeTeleportSkill2::~ioRangeTeleportSkill2()
{
}

void ioRangeTeleportSkill2::LoadProperty( ioINILoader &rkLoader )
{
	ioRangeSkill::LoadProperty( rkLoader );

	char szKey[MAX_PATH];

    m_fBackRange = rkLoader.LoadFloat_e( "back_move_range", 0.0f );
	m_fHeightRange = rkLoader.LoadFloat_e( "back_move_height_range", 0.0f );

	m_bNoGravityCombo = rkLoader.LoadBool_e( "no_gravity_combo", false );
	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	int iComboCnt = rkLoader.LoadInt_e( "linear_combo_cnt", 0 );
	m_vComboList.clear();

	if( iComboCnt > 0 )
	{
		m_vComboList.reserve( iComboCnt );

		for( int i=0; i < iComboCnt; ++i )
		{
			AttackAttribute kComboAttack;

			wsprintf_e( szKey, "linear_combo%d", i+1 );
			LoadAttackAttribute( szKey, kComboAttack, rkLoader );

			m_vComboList.push_back( kComboAttack );
		}
	}

	m_bSelectTeleportDirection = rkLoader.LoadBool_e( "select_teleport_direction", false );
	m_fRangeEmptyTarget = rkLoader.LoadFloat_e( "range_empty_target", 0.0f );
}

ioSkill* ioRangeTeleportSkill2::Clone()
{
	return new ioRangeTeleportSkill2( *this );
}

bool ioRangeTeleportSkill2::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioRangeSkill::OnSkillStart( pChar ) )
		return false;

	m_iCurLinearCombo = 0;
	m_bUsedComboList = false;
	m_bSetAttackFire = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	m_dwAttackEndTime = 0;
	m_dwCurSkillProtectTime = 0;

	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	CheckChangeDirRot( pChar );

	return true;
}

void ioRangeTeleportSkill2::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelayTime = GetPreDelayTime();

	switch( m_RangeState )
	{
	case RS_NONE:
		if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
		{
			m_dwPreDelayStartTime = 0;
			SetTeleportList( pChar, pStage );
		}
		break;
	case RS_ACTION:
		{
			CheckLinearCombo( pChar );
		}
		break;
	case RS_END:
		break;
	}
}

void ioRangeTeleportSkill2::SetTeleportList( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( !pOwner )
	{
		LOG.PrintTimeAndLog( 0, "ioRangeTeleportSkill2::SetTeleportList() - Owner is NULL" );
		return;
	}

	if( !pStage )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	float fPrevDistSq = m_fSkillRange;
	float fHalfCosine = cosf( DEGtoRAD( m_fSkillAngle ) * FLOAT05 );

	ioBaseChar *pTargetChar = pOwner;
	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pChar = (*iter);

		if( !pChar || IsCollisioned( pChar ) )
			continue;

		if( !pStage->IsCollisionCheckByMode() )
			continue;

		if( pOwner->GetCharName() == pChar->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pChar->GetTeam() )
			continue;

		if( pChar->IsSystemState() )
			continue;
			
		if( !pChar->IsEnableTarget() )
			continue;

		if( pChar->IsPrisonerMode() )
			continue;

		if( pChar->IsApplyDownState(false) )
			continue;

		if( m_TargetColType == TCT_SPHERE )
		{
			D3DXVECTOR3 vSightDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vSightDir, &vSightDir );

			D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - pOwner->GetWorldPosition();
			D3DXVECTOR3 vTargetDir = vDiff;
			D3DXVec3Normalize( &vTargetDir, &vTargetDir );

			if( D3DXVec3Dot( &vSightDir, &vTargetDir ) < fHalfCosine )
				continue;

			float fDistSq = D3DXVec3LengthSq( &vDiff );
			if( fDistSq < fPrevDistSq )	// Enemy
			{
				pTargetChar = pChar;
				fPrevDistSq = fDistSq;
			}
		}
		else if( m_TargetColType == TCT_CYLINDER )
		{
			D3DXVECTOR3 vTop, vBottom, vCenter;
			vCenter = pOwner->GetMidPositionByRate();
			vTop = vBottom = vCenter;
			vTop.y += m_fUpHeight;
			vBottom.y -= m_fUnderHeight;
			ioCylinder kCylinder( vTop, vBottom, m_fSkillRange );
			vCenter = kCylinder.GetCenter();

			D3DXVECTOR3 vCollisionPoint;
			if( !pChar->CheckCylinderPoint( kCylinder, vCenter, vCollisionPoint ) )
				continue;
			
			D3DXVECTOR3 vSightDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vSightDir, &vSightDir );

			D3DXVECTOR3 vTargetPos = pChar->GetWorldPosition();
			D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
			vTargetPos.y = 0.0f;
			vOwnerPos.y = 0.0f;
			D3DXVECTOR3 vDiff = vTargetPos - vOwnerPos;
			D3DXVECTOR3 vTargetDir = vDiff;
			D3DXVec3Normalize( &vTargetDir, &vTargetDir );

			if( D3DXVec3Dot( &vSightDir, &vTargetDir ) < fHalfCosine )
				continue;

			float fDistSq = D3DXVec3LengthSq( &vDiff );
			if( fDistSq < fPrevDistSq )	// Enemy
			{
				pTargetChar = pChar;
				fPrevDistSq = fDistSq;
			}
		}
	}

	pOwner->ReduceNeedGaugeBySkill( this, GetNeedGauge(pOwner->GetCharName()) );
	m_bReduceGauge = true;

	if( pOwner && pTargetChar )
	{
		D3DXVECTOR3 vOffSet( 0.0f, m_fHeightRange, 0.0f );
		D3DXVECTOR3 vTargetPos = pTargetChar->GetWorldPosition() + vOffSet;
		D3DXQUATERNION qtTargetOrientation = pTargetChar->GetWorldOrientation();	
		D3DXVECTOR3 vNewPos;
		if( m_bSelectTeleportDirection )
		{
			if( pOwner->GetCharName() == pTargetChar->GetCharName() )
			{
				vNewPos = vTargetPos + ( pOwner->GetMoveDir() * m_fRangeEmptyTarget );
				float fMapHeight = pStage->GetMapHeight( vNewPos.x, vNewPos.z );
				if( fMapHeight > 0.0f && fMapHeight > vNewPos.y )
					vNewPos.y = fMapHeight;

				pOwner->SetWorldPosition( vNewPos );
			}
			else
			{
				D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
				D3DXVECTOR3 vDir = vTargetPos - vOwnerPos;
				D3DXVec3Normalize( &vDir, &vDir );
				vNewPos = vTargetPos + ( vDir * m_fBackRange );
				pOwner->SetWorldPosition( vNewPos );
				pOwner->SetTargetRotToTargetPos( vTargetPos, true );
			}
		}
		else
		{
			vNewPos = vTargetPos - ( ( qtTargetOrientation * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) ) * m_fBackRange );
			pOwner->SetWorldPosition( vNewPos );
			pOwner->SetTargetRotAndMoveDirChange( qtTargetOrientation );
		}
		
		if( pStage )
			pStage->SetCameraToOwnerExplicit( pOwner );

		SetSkillActionState( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << ST_TELEPORT;
			kPacket << pOwner->GetWorldPosition();
			kPacket << pOwner->GetWorldOrientation();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioRangeTeleportSkill2::ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case ST_TELEPORT:
		{
			D3DXVECTOR3 vTargetPos;
			D3DXQUATERNION qtTargetOrientation;
			rkPacket >> vTargetPos >> qtTargetOrientation;
			pOwner->SetWorldPosition( vTargetPos );
			pOwner->SetTargetRotAndMoveDirChange( qtTargetOrientation );

			SetSkillActionState( pOwner );
		}
		break;
	case ST_COMBO:
		{
			rkPacket >> m_iCurLinearCombo;

			int iCurKey;
			rkPacket >> iCurKey;
			pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)iCurKey );

			SetNextLinearCombo( pOwner, false );

			if( IsUseAutoTargetValue() )
			{
				pOwner->ApplyAutoTargetInfo( rkPacket );
			}
		}
		break;
	}
}

void ioRangeTeleportSkill2::SetSkillActionState( ioBaseChar *pChar )
{
	m_RangeState = RS_ACTION;

	m_iCurLinearCombo = 0;
	SetNextLinearCombo( pChar, false );
}

void ioRangeTeleportSkill2::CheckLinearCombo( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_dwPreDelayStartTime != 0 )
		return;

	if( !COMPARE( m_iCurLinearCombo, 0, (int)m_vComboList.size() ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwEnableReserveTime == 0 )
		return;

	if( m_dwEnableReserveTime < dwCurTime )
	{
		pChar->SetSkillMoveEnable( false, true );

		if( pChar->IsNeedProcess() )
		{
			bool bReserveSkill = false;
			int iSkillNum = pChar->GetSkillKeyInput();
			if( iSkillNum >= 0 )
			{
				bReserveSkill = true;
			}

			if( !bReserveSkill )
			{
				if( pChar->IsAttackKey() )
				{
					SetNextLinearCombo( pChar, true );
					return;
				}
			}
		}
	}
}

void ioRangeTeleportSkill2::SetNextLinearCombo( ioBaseChar *pChar, bool bSend )
{
	if( !pChar ) return;

	m_dwEnableReserveTime = 0;

	int iMaxCombo = m_vComboList.size();
	if( !COMPARE( m_iCurLinearCombo, 0, iMaxCombo ) )
		return;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( pChar->IsNeedProcess() && pChar->IsSettedDirection() && !m_bSelectTeleportDirection )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
		m_CurDirKey = eNewDirKey;
		pChar->SetTargetRotToDirKey( eNewDirKey );
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	AttackAttribute kAttack = m_vComboList[m_iCurLinearCombo];

	int iAniID = pGrp->GetAnimationIdx( kAttack.m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->InitActionStopDelay( true );
	m_bUsedComboList = true;

	if( m_bNoGravityCombo )
	{
		pChar->SetGravityAmt( 0.0f );
		pChar->SetForcePowerAmt( 0.0f );
		pChar->SetCurJumpPowerAmt( 0.0f );
	}

	float fTimeRate = kAttack.m_fAttackAniRate;
	DWORD dwPreDelay = kAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTarget( ATT_SKILL );
	}

	if( m_iCurLinearCombo == 0 )
		m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex+m_iCurLinearCombo,
										iAniID,
										kAttack.m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + kAttack.m_dwEndDelay;

	DWORD dwFireStartTime;
	dwFireStartTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( kAttack.m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		float fEndTimeRate = kAttack.m_fAttackEndAniRate;

		pGrp->SetActionAni( kAttack.m_AttackEndAnimation,
							0.0f, FLOAT100,
							1.0f, FLOAT1/fEndTimeRate,
							fWaitTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( kAttack.m_AttackEndAnimation );
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;
	}

	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	SetExtraAniJump( pChar, iAniID, fTimeRate );

	//
	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	//
	pChar->SetReservedSliding( kAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if( bSend && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << ST_COMBO;
		kPacket << m_iCurLinearCombo;
		kPacket << (int)m_CurDirKey;

		if( IsUseAutoTargetValue() )
		{
			pChar->FillAutoTargetInfo( kPacket );
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurLinearCombo++;
}

bool ioRangeTeleportSkill2::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	if( m_dwPreDelayStartTime == 0 )
	{
		if( m_dwCurSkillProtectTime > dwCurTime )
			return true;
	}

	return false;
}

bool ioRangeTeleportSkill2::IsEnableReserve() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	if( m_dwEnableReserveTime == 0 || m_dwMotionStartTime >= m_dwEnableReserveTime )
		return false;

	if( m_dwEnableReserveTime < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioRangeTeleportSkill2::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	if( m_dwMotionEndTime + dwActionStop >= FRAMEGETTIME() )
		return false;

	return true;
}

bool ioRangeTeleportSkill2::IsJumpState() const
{
	if( !m_bNoGravityCombo )
		return false;

	return m_bUsedComboList;
}

bool ioRangeTeleportSkill2::IsNoDropState() const
{
	if( !m_bNoGravityCombo )
		return false;

	return m_bUsedComboList;
}

bool ioRangeTeleportSkill2::IsCanSendControl() const
{
	if( m_bSetAttackFire )
		return false;

	return true;
}

bool ioRangeTeleportSkill2::IsAttackEndState() const
{
	if( m_dwAttackEndTime > 0 && m_dwAttackEndTime < FRAMEGETTIME() )
	{
		return true;
	}

	return false;
}


