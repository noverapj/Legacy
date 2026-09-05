
#include "stdafx.h"

#include "ioTeleportAttackSkill3.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "FindPredicateImpl.h"

#include "WeaponDefine.h"

ioTeleportAttackSkill3::ioTeleportAttackSkill3()
{
	m_dwFireStartTime = 0;
	m_dwAttackEndTime = 0;
}

ioTeleportAttackSkill3::ioTeleportAttackSkill3( const ioTeleportAttackSkill3 &rhs )
	: ioNormalSkill( rhs ),
	m_fHeightRange( rhs.m_fHeightRange ),
	m_SkillWoundType( rhs.m_SkillWoundType ),
	m_fSetHeightGap( rhs.m_fSetHeightGap ),
	m_fJumpHeight( rhs.m_fJumpHeight ),
	m_LandAttackAttribute( rhs.m_LandAttackAttribute ),
	m_JumpAttackAttribute( rhs.m_JumpAttackAttribute ),
	m_EndAttackAttribute( rhs.m_EndAttackAttribute ),
	m_fSkillAngle( rhs.m_fSkillAngle ),
	m_fSkillRange( rhs.m_fSkillRange ),
	m_fTeleportOffset( rhs.m_fTeleportOffset ),
	m_fBackRange( rhs.m_fBackRange ),
	m_vLinearAttackAttribute( rhs.m_vLinearAttackAttribute ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime )
{
}

ioTeleportAttackSkill3::~ioTeleportAttackSkill3()
{
}

void ioTeleportAttackSkill3::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szKey[MAX_PATH];

	m_fHeightRange = rkLoader.LoadFloat_e( "back_move_height_range", 0.0f );

	m_SkillWoundType = (TargetWoundType)rkLoader.LoadInt_e( "skill_wound_type", 0 );

	m_fJumpHeight = rkLoader.LoadFloat_e( "last_height", 0.0f );

	m_fSetHeightGap = rkLoader.LoadFloat_e( "height_gap", 0.0f );

	LoadAttackAttribute( "last_attack", m_LandAttackAttribute, rkLoader );

	LoadAttackAttribute( "last_jump", m_JumpAttackAttribute, rkLoader );

	LoadAttackAttribute( "end_attack", m_EndAttackAttribute, rkLoader );
	//////////////////////////////////////////////////////////////////////////
	m_fSkillAngle = rkLoader.LoadFloat_e( "target_angle", 0.f );
	m_fSkillRange = rkLoader.LoadFloat_e( "target_range", 0.f );

	m_fTeleportOffset = rkLoader.LoadFloat_e( "teleport_offset", 0.f );
	m_fBackRange = rkLoader.LoadFloat_e( "back_range", 0.f );

	int nMaxLinear = rkLoader.LoadInt_e( "linear_combo_cnt", 0 );
	for( int i = 0; i < nMaxLinear; i++ )
	{
		AttackAttribute kComboAttack;
		wsprintf_e( szKey, "linear_combo%d", i+1 );
		LoadAttackAttribute( szKey, kComboAttack, rkLoader );

		m_vLinearAttackAttribute.push_back( kComboAttack );
	}

	m_dwSkillProtectTime = rkLoader.LoadInt_e( "skill_protect_time", 0 );
}

ioSkill* ioTeleportAttackSkill3::Clone()
{
	return new ioTeleportAttackSkill3( *this );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioTeleportAttackSkill3::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioNormalSkill::OnSkillStart( pChar ) )
		return false;

	m_dwFireStartTime = 0;
	m_dwAttackEndTime = 0;
	m_bLinerAttackReserved = false;
	m_bBackTeleportReserved = false;
	m_nCurLinearComboIndex = 0;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;


	m_dwFireStartTime = 0;
	m_dwAttackEndTime = 0;

	m_bSetHandMesh = false;

	m_SkillState = SS_NONE;
	m_AttackState = AS_NONE;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	pChar->AttachEffect( m_SkillEffectName );

	m_vCurMoveDir = ioMath::VEC3_ZERO;

	return true;
}

void ioTeleportAttackSkill3::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelayTime = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				FindTeleportPosition( pStage, pChar );
			}
		}
		break;
	case SS_JUMP:
		{
			if( pChar->IsNeedProcess() )
			{
				if( dwCurTime > m_dwMotionEndTime )
				{
					SetEndState( pChar );
					return;
				}
				if ( CheckLanding( pStage, pChar ) )
					SetLandAttack( pChar );
			}
		}
		break;
	case SS_LAND_ATTACK:
		{
			if( pChar->IsNeedProcess() )
			{
				ProcessLandAttack( pChar );
				
				if( m_dwMotionEndTime <= dwCurTime )
				{
					if( m_bLinerAttackReserved )
						SetLinearAttack( pChar, 0 );
					else
						SetEndState( pChar );
				}
			}
		}
		break;
	case SS_LINEAR_ATTACK:
		//if( pChar->IsNeedProcess() )
		{
			CheckExtraAniJump( pChar );
			CheckAniRotate( pChar );
			ProcessLinearAttackState( pChar, pStage );
			break;
		}
		break;
	case SS_END_ATTACK:
		CheckExtraAniJump( pChar );
		CheckAniRotate( pChar );
		ProcessEndAttackState(pChar, pStage );
		break;
	case SS_BACK_MOVE:
		break;
	case SS_END:
		break;
	}
}


void ioTeleportAttackSkill3::ProcessLandAttack( ioBaseChar *pChar )
{
	if( pChar->IsAttackKey() )
		m_bLinerAttackReserved = true;
}


void ioTeleportAttackSkill3::ProcessLinearAttackState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pChar->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	/*if( dwCurTime > m_dwEnableReserveTime && m_bReserveLinearCombo )
	{
		SetLinearComboState( pChar, m_nCurLinearComboIndex + 1 );
		return;
	}*/

	if( dwCurTime > m_dwMotionEndTime )
	{
		if( m_bBackTeleportReserved )
		{
			SetEndAttack( pChar );
			return;
		}
		if( !m_bLinerAttackReserved )
			SetEndState( pChar );
		else
			SetLinearAttack( pChar, m_nCurLinearComboIndex + 1 );

		return;
	}

	if( !m_bLinerAttackReserved && dwCurTime > m_dwEnableReserveTime && pChar->IsAttackKey() )
		m_bLinerAttackReserved = true;

	if( !m_bBackTeleportReserved && dwCurTime > m_dwEnableReserveTime && pChar->IsDefenseKey() )
		m_bBackTeleportReserved = true;
}


void ioTeleportAttackSkill3::ProcessEndAttackState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pChar->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( dwCurTime > m_dwMotionEndTime )
	{
		SetBackMoveState( pChar, pStage );
		return;
	}
}


void ioTeleportAttackSkill3::OnSkillEnd( ioBaseChar *pChar )
{
	m_dwFireStartTime = 0;

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;

	pChar->EndEffect( m_SkillEffectName, false );

	ioNormalSkill::OnSkillEnd( pChar );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioTeleportAttackSkill3::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioZoneEffectWeapon *pZoneEffect = ToZoneEffectWeapon( pWeapon );
	if( pZoneEffect )
	{
		pZoneEffect->SetCollisionDir( m_vCurMoveDir );
		pZoneEffect->SetZoneValue( true, true );
	}
}

void ioTeleportAttackSkill3::ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pOwner || !pStage ) 
		return;

	int nState;
	rkPacket >> nState;

	int iCurKey;
	D3DXVECTOR3 vPos;
	D3DXQUATERNION qtRot;

	switch( nState )
	{
	case SS_JUMP:
		rkPacket >> vPos;
		SetLastJump( pOwner, vPos );
		break;
	case SS_LAND_ATTACK:
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		SetLandAttack( pOwner );
		break;
	case SS_LINEAR_ATTACK:
		rkPacket >> m_nCurLinearComboIndex;
		rkPacket >> iCurKey;
		rkPacket >> vPos;
		rkPacket >> qtRot;

		pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)iCurKey );
		pOwner->SetWorldPosition( vPos );
		pOwner->SetTargetRotToRotate( qtRot, true, false );

		SetLinearAttack( pOwner, m_nCurLinearComboIndex );

		if( IsUseAutoTargetValue() )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		
		break;
	case SS_END_ATTACK:
		rkPacket >> iCurKey;
		pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)iCurKey );

		SetEndAttack( pOwner );

		if( IsUseAutoTargetValue() )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		
		break;
	case SS_BACK_MOVE:
		SetBackMoveState( pOwner, pStage );
		break;
	case SS_END:
		SetEndState( pOwner );
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioTeleportAttackSkill3::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_JUMP:
	case SS_LAND_ATTACK:
	case SS_LINEAR_ATTACK:
	case SS_END_ATTACK:
	case SS_BACK_MOVE:
		return false;
	case SS_END:
		return true;
	}

	return true;
}

bool ioTeleportAttackSkill3::IsAttackEndState() const
{
	if( m_dwAttackEndTime > 0 && m_dwAttackEndTime < FRAMEGETTIME() )
	{
		if( m_AttackState == AS_FLOAT )
			return true;
		else
			return false;
	}

	if( m_SkillState == SS_END || m_SkillState == SS_BACK_MOVE )
		return true;

	return false;
}

bool ioTeleportAttackSkill3::IsNoDropState() const
{
	if( m_SkillState == SS_BACK_MOVE )
		return true;

	return false;
}


void ioTeleportAttackSkill3::FindTeleportPosition( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	if( !pStage )	return;
	if( !pOwner )	return;

	if( !pOwner->IsNeedProcess() )
		return;

	const D3DXVECTOR3& vCurPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fSkillAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fSkillRange * m_fSkillRange;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	//ioHashString szTarget;
	ioBaseChar *pFindChar = pOwner;
	float fCurGap = -FLOAT1;
	float fMinFindLength = 999999999.f;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget )
			continue;

		if( !pStage->IsCollisionCheckByMode() )
			continue;

		if( pOwner->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;
			
		if( !pTarget->IsEnableTarget() )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
		{
			if( fMinFindLength > fDiffRangeSq )
			{
				pFindChar = pTarget;
				fMinFindLength = fDiffRangeSq;
			}
		}
	}

	if( pFindChar )
	{
		if( pFindChar == pOwner )
		{
			D3DXVECTOR3 vTargetPos = pFindChar->GetWorldPosition();
			SetLastJump( pOwner, vTargetPos );
		}
		else
		{
			D3DXVECTOR3 vTargetPos = pFindChar->GetWorldPosition();
			D3DXVECTOR3 vDir = vTargetPos - vCurPos;
			vDir.y = 0.f;
			D3DXVec3Normalize( &vDir, &vDir );
			vTargetPos = vTargetPos - (vDir*m_fTeleportOffset);
			SetLastJump( pOwner, vTargetPos );
		}
	}
	else
		SetEndState( pOwner );
}


void ioTeleportAttackSkill3::SetLastJump( ioBaseChar *pChar, const D3DXVECTOR3& vStartPos )
{
	if ( !pChar )
		return;

	D3DXVECTOR3 vPos = vStartPos;
	vPos.y += m_fJumpHeight;
	pChar->SetWorldPosition( vPos );

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_JumpAttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )	
	{
		SetEndState( pChar );
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimeRate  = m_JumpAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_JumpAttackAttribute.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex, iAniID, m_JumpAttackAttribute.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, m_JumpAttackAttribute.m_dwPreDelay );

	//캐릭터 충돌 해제
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	//슬라이딩 설정
	pChar->ClearReservedSliding();
	pChar->SetReservedSliding( m_JumpAttackAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_SkillState = SS_JUMP;

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_JUMP;
		kPacket << vStartPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTeleportAttackSkill3::SetLandAttack( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_LandAttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )	
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimeRate  = m_LandAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_LandAttackAttribute.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex, iAniID, m_LandAttackAttribute.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, m_LandAttackAttribute.m_dwPreDelay );

	//캐릭터 충돌 해제
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	//슬라이딩 설정
	pChar->ClearReservedSliding();
	pChar->SetReservedSliding( m_LandAttackAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_SkillState = SS_LAND_ATTACK;

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_LAND_ATTACK;
		kPacket << pChar->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioTeleportAttackSkill3::SetLinearAttack( ioBaseChar *pChar, int nComboIndex )
{
	if( !pChar || (int)m_vLinearAttackAttribute.size() <= nComboIndex )
	{
		SetEndState( pChar );
		return;
	}

	m_SkillState = SS_LINEAR_ATTACK;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
	{
		SetEndState( pChar );
		return;
	}

	m_bLinerAttackReserved = false;

	m_nCurLinearComboIndex = nComboIndex;

	pChar->ClearSkillReserveKey();
	const AttackAttribute &kAttack = m_vLinearAttackAttribute[nComboIndex];

	if( pChar->IsNeedProcess() && pChar->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
		m_CurDirKey = eNewDirKey;
		pChar->SetTargetRotToDirKey( eNewDirKey );
	}

	int iAniID = pGrp->GetAnimationIdx( kAttack.m_AttackAnimation );
	if( iAniID == -1 )	
		return;

	pChar->InitActionStopDelay( true );
	float fTimeRate = kAttack.m_fAttackAniRate;
	DWORD dwPreDelay = kAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pChar->SetAutoTarget( ATT_SKILL );
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex + nComboIndex + 1,
		iAniID,
		kAttack.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fTimeRate,
		dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + kAttack.m_dwEndDelay;

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	SetExtraAniJump( pChar, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	//if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	pChar->SetReservedSliding( kAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_vCurMoveDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f , -FLOAT1);

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << m_SkillState;
		kPacket << m_nCurLinearComboIndex;
		kPacket << (int)m_CurDirKey;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetTargetRot();

		if( IsUseAutoTargetValue() )
		{
			pChar->FillAutoTargetInfo( kPacket );
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioTeleportAttackSkill3::SetEndAttack( ioBaseChar *pChar )
{
	m_SkillState = SS_END_ATTACK;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
	{
		SetEndState( pChar );
		return;
	}

	pChar->ClearSkillReserveKey();
	if( pChar->IsNeedProcess() && pChar->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
		m_CurDirKey = eNewDirKey;
		pChar->SetTargetRotToDirKey( eNewDirKey );
	}


	int iAniID = pGrp->GetAnimationIdx( m_EndAttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )	
		return;

	pChar->InitActionStopDelay( true );
	float fTimeRate = m_EndAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_EndAttackAttribute.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pChar->SetAutoTarget( ATT_SKILL );
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
		iAniID,
		m_EndAttackAttribute.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fTimeRate,
		dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + m_EndAttackAttribute.m_dwEndDelay;

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	SetExtraAniJump( pChar, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	//if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	pChar->SetReservedSliding( m_EndAttackAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_vCurMoveDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f , -FLOAT1);

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << m_SkillState;
		kPacket << (int)m_CurDirKey;

		if( IsUseAutoTargetValue() )
		{
			pChar->FillAutoTargetInfo( kPacket );
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioTeleportAttackSkill3::SetBackMoveState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_SkillState == SS_END )
		return;

	m_SkillState = SS_BACK_MOVE;
	

	D3DXVECTOR3 vTargetDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	vTargetDir = vTargetDir * -FLOAT1;

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	vPos = vPos + vTargetDir*m_fBackRange;

	float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z );
	if( fMapHeight > 0.0f && fMapHeight > vPos.y )
		vPos.y = fMapHeight;

	pChar->SetWorldPosition( vPos );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << m_SkillState;
		kPacket << vPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioTeleportAttackSkill3::SetEndState( ioBaseChar *pChar )
{
	if( m_SkillState == SS_END )
		return;

	m_SkillState = SS_END;


	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioTeleportAttackSkill3::IsEnableTarget( ioBaseChar *pChar )
{
	if( !pChar ) return false;

	// 쓰러짐 관련 체크
	if( m_SkillWoundType == TWT_AIR_WOUND_EXCEPT ) // 쓰러짐중 공중상태는 패스
	{
		if( pChar->IsApplyDownState() && !pChar->IsApplyDownState(false) )
			return false;
	}
	else if( m_SkillWoundType == TWT_DOWN_WOUND_EXCEPT ) // 쓰러짐중 바닥상태는 패스
	{
		if( pChar->IsApplyDownState(false) )
			return false;
	}
	else if( m_SkillWoundType == TWT_ALL_WOUND_EXCEPT )
	{
		if( pChar->IsApplyDownState() ||
			( pChar->GetState() == CS_GRAPPLING_WOUNDED ||
			pChar->GetState() == CS_GRAPPLING_PUSHED ||
			pChar->GetState() == CS_GRAPPLING_WOUNDED_SKILL ||
			pChar->GetState() == CS_GRAPPLING_WOUNDED_SWING ) )
		{
			return false;
		}
	}
	else if( m_SkillWoundType == TWT_FLOATING_STATE )	// 공중에 뜬 상태가 아니면 패스
	{
		if( !pChar->IsFloating() )
			return false;
	}
	else if( m_SkillWoundType == TWT_DOWN_WOUNDED ) //바닥에 쓰러져 있지 않거나 공중에 뜬 상태면 패스
	{	
		if( !pChar->IsApplyDownState() || pChar->IsFloating() )
		{	
			return false;
		}
	}

	return true;
}

bool ioTeleportAttackSkill3::CheckLanding( ioPlayStage *pStage, ioBaseChar *pChar )
{
	if ( !pStage || !pChar || !pChar->IsNeedProcess() )
		return false;

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	float fHeightGap = 0.0f;
	float fHeight = vPos.y;
	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );

	fHeightGap = fHeight - fMapHeight;

	if( abs( fHeightGap ) <= m_fSetHeightGap )
		return true;

	return false;
}

bool ioTeleportAttackSkill3::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	// 스킬 무적 파괴
	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{		
		if( iDefenseBreakType == m_ProtectCancelType )
			return false;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( 0 < m_dwSkillProtectTime && dwCurTime < (m_dwSkillStartTime+m_dwSkillProtectTime) )
		return true;

	return false;
}