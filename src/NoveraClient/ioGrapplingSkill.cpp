

#include "stdafx.h"

#include "ioNpcChar.h"
#include "ioPlayStage.h"

#include "ioGrapplingSkill.h"

#include "WeaponDefine.h"

ioGrapplingSkill::ioGrapplingSkill()
{
	ClearData();
}

ioGrapplingSkill::ioGrapplingSkill( const ioGrapplingSkill &rhs )
: ioAttackSkill( rhs ),
 m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
 m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
 m_dwSkillLoopDuration( rhs.m_dwSkillLoopDuration ),
 m_fTargetLoopGapRange( rhs.m_fTargetLoopGapRange ),
 m_bEnableLoopStop( rhs.m_bEnableLoopStop ),
 m_EndAttackAttribute( rhs.m_EndAttackAttribute ),
 m_dwLoopRotateTime( rhs.m_dwLoopRotateTime ),
 m_dwEnableSwingTime( rhs.m_dwEnableSwingTime ),
 m_dwSkillLoopProtectTime( rhs.m_dwSkillLoopProtectTime ),
 m_fCharHeightRate( rhs.m_fCharHeightRate ),
 m_fCharJumpPower( rhs.m_fCharJumpPower ),
 m_bEnableSwingMove( rhs.m_bEnableSwingMove ),
 m_fMoveSpeed( rhs.m_fMoveSpeed ),
 m_dwMoveRotateTime( rhs.m_dwMoveRotateTime )
{
	ClearData();
}

ioGrapplingSkill::~ioGrapplingSkill()
{
}

void ioGrapplingSkill::ClearData()
{
	m_SkillState = SS_NONE;

	m_fCurChargeRate		= 0.0f;
	
	m_dwSkillLoopStartTime	= 0;
	m_dwSkillLoopEndTime	= 0;

	m_bTargetRot	= false;
	m_CurDirKey		= ioUserKeyInput::DKI_NONE;
	m_vCurMoveDir	= ioMath::VEC3_ZERO;

	m_GrapplingTargetName.Clear();

	m_bSetHandMesh	= false;
}

void ioGrapplingSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	// 잡고 있는 동작
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;
	m_fSkillLoopMotionRate	= rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	m_dwSkillLoopDuration	= rkLoader.LoadInt_e( "skill_loop_duration", 0 );

	m_fTargetLoopGapRange	= rkLoader.LoadFloat_e( "skill_target_loop_gap_range", 0.0f );

	m_bEnableLoopStop = rkLoader.LoadBool_e( "enable_loop_stop", false );

	// 종료모션
	m_EndAttackAttribute.Init();
	LoadAttackAttribute_e( "skill_end_attack", m_EndAttackAttribute, rkLoader );
	
	m_dwLoopRotateTime		= rkLoader.LoadInt_e( "skill_loop_rotate_speed", 0 );
	m_dwEnableSwingTime		= rkLoader.LoadInt_e( "skill_loop_swing_enable_time", 0 );
	m_dwSkillLoopProtectTime= rkLoader.LoadInt_e( "skill_loop_protect_time", 0 );

	m_fCharHeightRate	= rkLoader.LoadFloat_e( "char_height_rate", FLOAT05 );
	m_fCharJumpPower	= rkLoader.LoadFloat_e( "skill_jump_power", 0.0f );

	m_bEnableSwingMove	= rkLoader.LoadBool_e( "enable_swing_move", false );
	m_fMoveSpeed		= rkLoader.LoadFloat_e( "swing_move_speed", 0.0f );
	m_dwMoveRotateTime	= (DWORD)rkLoader.LoadInt_e( "swing_move_rotate_speed", 0 );

	int i;
	int iBuffCnt = rkLoader.LoadInt_e( "swing_buff_cnt", 0 );
	for( i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "swing_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SkillGrapplingInfo.m_SwingBuff.push_back( szBuf );
	}

	iBuffCnt = rkLoader.LoadInt_e( "swing_wound_buff_cnt", 0 );
	for( i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "swing_wound_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SkillGrapplingInfo.m_SwingWoundBuff.push_back( szBuf );
	}

	iBuffCnt = rkLoader.LoadInt_e( "swing_cancel_buff_cnt", 0 );
	for( i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "swing_cancel_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SkillGrapplingInfo.m_SwingCancelBuff.push_back( szBuf );
	}
}

ioSkill* ioGrapplingSkill::Clone()
{
	return new ioGrapplingSkill( *this );
}

bool ioGrapplingSkill::OnSkillStart( ioBaseChar *pChar )
{
	ClearData();

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}
	
	if( pChar )
	{
		m_vCurMoveDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	}

	return true;
}

void ioGrapplingSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	int iSlot = GetOwnerItem()->GetType() - 1;

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				AttackSkillFire( pChar );
				EndPreDelayEffect( pChar );
			}
		}
		break;
	case SS_ACTION:
		break;
	case SS_LOOP:
		{
			// Target 유효 체크
			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget )
			{
				if( m_dwSkillLoopStartTime+g_App.GetGrapplingCheckDelay() < dwCurTime &&
					pTarget->GetState() != CS_GRAPPLING_WOUNDED_SKILL )
				{
					SetSkillEndState( pChar, true );
					return;
				}
			}
			else
			{
				SetSkillEndState( pChar, true );
				return;
			}

			if( m_fCharJumpPower > 0.0f )
			{
				if( CheckLanding( pChar ) )
				{
					SetSkillEndState( pChar, true );
					return;
				}
			}
			else if( m_dwSkillLoopEndTime < dwCurTime )
			{
				// SwingBuff 유무에 따라 분기를 나눈다.
				if( pChar->IsNeedProcess() && m_SkillGrapplingInfo.m_SwingBuff.size() > 0 )
				{
					SetSkillEndState( pChar, true );
					return;
				}
				else if( m_SkillGrapplingInfo.m_SwingBuff.size() == 0 )
				{
					SetSkillEndState( pChar, true );
					return;
				}
			}

			// 액션키 체크
			if( m_bEnableLoopStop )
			{
				if( pChar->IsNeedProcess() && m_dwSkillLoopStartTime + m_dwEnableSwingTime < dwCurTime )
				{
					if( pChar->IsAttackKey() ||
						pChar->IsDefenseKey() ||
						pChar->IsJumpKey() ||
						pChar->GetSkillKeyInput() == iSlot )
					{
						SetSkillEndState( pChar, true );
						return;
					}
				}
			}

			if( m_bEnableSwingMove )
			{
				CheckKeyInput( pChar );
				ProcessRotate( pChar );
				pChar->SetCurMoveSpeed( m_fMoveSpeed );
			}

			ProcessLoopDir( pChar );
		}
		break;
	case SS_END:
		break;
	}
}

void ioGrapplingSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	ClearData();

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	// 스킬 중간 동작에 사용된 메쉬들 제거
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
}

void ioGrapplingSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioAttackSkill::AttackSkillFire( pChar );

	m_SkillState = SS_ACTION;

	if( !pChar ) return;
	
	m_GrapplingTargetName.Clear();

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioGrapplingSkill::SetSkillLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_SkillState != SS_ACTION )
		return;
	
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	m_dwSkillLoopStartTime = FRAMEGETTIME();
	m_SkillState = SS_LOOP;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_fSkillLoopMotionRate > 0.0f )
		fAniRate = m_fSkillLoopMotionRate;

	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );

	if( m_fCharJumpPower > 0.0f )
	{
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );
		pChar->SetJumpPower( m_fCharJumpPower );
	}
	else if( m_dwSkillLoopDuration != 0 )
	{
		// m_dwSkillLoopDuration 값이 설정되어 있는 경우 SetLoopAni로 셋팅.
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

		m_dwSkillLoopEndTime = m_dwSkillLoopStartTime;
		m_dwSkillLoopEndTime += m_dwSkillLoopDuration;
	}
	else
	{
		// m_dwSkillLoopDuration 값이 설정되지 않은 경우 SetActionAni로 셋팅.
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

		m_dwSkillLoopEndTime = m_dwSkillLoopStartTime;
		m_dwSkillLoopEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;
	}
}

void ioGrapplingSkill::SetSkillEndState( ioBaseChar *pChar, bool bSendNet )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	//
	// 차징 체크
	// 프로레슬러의 스킬로 셋팅 하던 중이라 활용이 적절치 않음.
	// 현재 m_dwSkillLoopDuration, m_bEnableLoopStop가 있는 경우에만 차징이 작동하게끔 되어 있음.
	// 단, 데미지의 형태가 버프를 통할 경우 차징이 적용되지 않음. (ex. 자이언트스윙)
	// ※ 현재 네트워크 동기화까지 완료된 상태임.
	//

	/*
	DWORD dwGapTime = dwCurTime - m_dwSkillLoopStartTime;
	if( m_dwSkillLoopStartTime > 0 && m_dwSkillLoopDuration > 0 && m_bEnableLoopStop)
	{
		if( m_dwSkillLoopDuration > dwGapTime )
			m_fCurChargeRate = (float)dwGapTime / m_dwSkillLoopDuration;
		else
			m_fCurChargeRate = FLOAT1;
	}
	*/

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;

	if( m_EndAttackAttribute.m_fAttackAniRate > 0.0f )
		fAniRate = m_EndAttackAttribute.m_fAttackAniRate;
	
	int iAniID = pGrp->GetAnimationIdx( m_EndAttackAttribute.m_AttackAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	m_SkillState = SS_END;

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fAniRate;

	// Key Reserve
	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" );

	if( m_SkillGrapplingInfo.m_SwingBuff.size() > 0 )
	{
		// SwingBuff가 있는 경우만 처리.
		if( bSendNet && pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SSC_END;
			kPacket << pChar->GetTargetRot();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
		if( !pTarget ) return;

		// for NPC
		bool bNPC = false;
		bool bAttackerNPC = false;
		if( ToNpcChar( pTarget ) )
		{
			bNPC = true;
		}

		if( ToNpcChar( pChar ) )
		{
			bAttackerNPC = true;
		}

		// local or 훈련소 or NPC
		if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || (bNPC || bAttackerNPC) )
		{
			if( pTarget->GetState() == CS_GRAPPLING_WOUNDED_SKILL && pTarget->IsNeedProcess() )
			{
				pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
				pTarget->SetGrapplingWoundSwing( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), "", FLOAT1 );
			}
		}
	}
	else
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
											iAniID,
											m_EndAttackAttribute.m_vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fAniRate,
											m_EndAttackAttribute.m_dwPreDelay );
	}

}

void ioGrapplingSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING && !m_GrapplingTargetName.IsEmpty() )
	{
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	}
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING_ALL && !m_GrapplingTargetName.IsEmpty() )
	{
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	}

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, true );
		}
	}
}

bool ioGrapplingSkill::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) return false;
	if( m_SkillState != SS_ACTION )		return false;

	// 충돌처리 후 체크된 타겟을 m_GrapplingTargetName로 지정한다.
	m_GrapplingTargetName = szTargetName;
	
	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return false;

	SetSkillLoopState( pChar );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_LOOP;
		kPacket << pChar->GetTargetRot();
		kPacket << szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// for NPC
	bool bNPC = false;
	bool bAttackerNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	if( ToNpcChar( pChar ) )
	{
		bAttackerNPC = true;
	}

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || (bNPC || bAttackerNPC) )
	{
		pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
		pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
	}

	return true;
}

D3DXVECTOR3 ioGrapplingSkill::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_fTargetLoopGapRange > 0.0f )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fCharHeightRate );
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		if( m_fTargetLoopGapRange > 0.0f )
			vPos += m_fTargetLoopGapRange * vDir;

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

bool ioGrapplingSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_LOOP:
		return false;
	case SS_ACTION:
	case SS_END:
		if( m_dwMotionEndTime + dwActionStop < dwCurTime )
		{
			return true;
		}
		return false;
	}

	return false;
}

void ioGrapplingSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;
	if( !pStage ) return;

	int iState;
	rkPacket >> iState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pChar->SetTargetRotToRotate( qtRot, true, false );

	switch( iState )
	{
	case SSC_LOOP:
		{
			ioHashString szTargetName;
			rkPacket >> m_GrapplingTargetName;

			SetSkillLoopState( pChar );

			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget && pTarget->IsNeedProcess() )
			{
				pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
			}
		}
		break;
	case SSC_END:
		{
			if( m_SkillState != SS_END )
			{
				SetSkillEndState( pChar, false );
				ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );

				if( pTarget && pTarget->IsNeedProcess() && pTarget->GetState() == CS_GRAPPLING_WOUNDED_SKILL )
				{
					pTarget->SetGrapplingWoundSwing( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), "", FLOAT1 );
				}
			}
		}
		break;
	case SSC_KEY:
		{
			int iDir;
			rkPacket >> iDir;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;

			rkPacket >> m_vCurMoveDir;

			D3DXQUATERNION qtNewRot;
			ioMath::CalcDirectionQuaternion( qtNewRot, -m_vCurMoveDir );
			pChar->SetMoveDirByRotate( qtNewRot );
		}
		break;
	}

}

void ioGrapplingSkill::ProcessLoopDir( ioBaseChar *pChar )
{
	if( m_dwLoopRotateTime == 0 )
		return;

	// 캐릭터 회전
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	float fRate = (fTimePerSec*1000) / m_dwLoopRotateTime;
	float fRotateAngle = 360.0f * fRate;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

	D3DXQUATERNION	qtNewRot;
	D3DXQuaternionRotationYawPitchRoll( &qtNewRot, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

	D3DXQUATERNION qtCharRot = pChar->GetTargetRot();
	D3DXVECTOR3 vDir = qtCharRot * D3DXVECTOR3( 0.0, 0.0, -FLOAT1 );
	vDir = qtNewRot * vDir;

	ioMath::CalcDirectionQuaternion( qtNewRot, -vDir );
	pChar->SetTargetRotToRotate( qtNewRot, true, true );
}

bool ioGrapplingSkill::IsProtected( int iDefenseBreakType ) const
{
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

	switch( m_SkillState )
	{
	case SS_ACTION:
		return true;
	case SS_LOOP:
	case SS_END:
		if( m_dwSkillLoopStartTime+m_dwSkillLoopProtectTime < dwCurTime )
			return false;

		return true;
	}

	return false;
}

bool ioGrapplingSkill::CheckLanding( ioBaseChar *pChar )
{
	if( !pChar ) return true;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pChar->CheckGhostState() )
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetPreHeightForGhostState();
	else
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();

	if( fHeightGap > 0.0f || !pChar->IsDownState() )
	{
		return false;
	}

	return true;
}

bool ioGrapplingSkill::IsEnableReserve() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_END:
		if( m_dwEnableReserveTime == 0 || m_dwMotionStartTime >= m_dwEnableReserveTime )
			return false;

		if( m_dwEnableReserveTime < FRAMEGETTIME() )
			return true;
		break;
	}

	return false;
}

void ioGrapplingSkill::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		m_bTargetRot = false;
		m_CurDirKey = eNewDirKey;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << SSC_KEY;
			kPacket << pOwner->GetTargetRot();
			kPacket << (int)m_CurDirKey;
			kPacket << m_vCurMoveDir;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioGrapplingSkill::ProcessRotate( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_dwMoveRotateTime <= 0 ) return;

	if( m_vCurMoveDir == ioMath::VEC3_ZERO )
		return;
	if( m_bTargetRot ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwMoveRotateTime;
	float fRotateAngle = 360.0f * fRate;

	D3DXVECTOR3 vCurMoveDir = m_vCurMoveDir;
	D3DXVec3Normalize( &vCurMoveDir, &vCurMoveDir );


	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	float fDotValue = D3DXVec3Dot( &vCurMoveDir, &vTargetDir );
	float fGapAngle = RADtoDEG( acosf(fDotValue) );

	if( fDotValue >= FLOAT1 )
	{
		m_vCurMoveDir = vTargetDir;
		pOwner->SetMoveDirByRotate( qtRotate );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -FLOAT1 )
	{
		vAxis = ioMath::UNIT_Y;
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vTargetDir, &vCurMoveDir );
		D3DXVec3Normalize( &vAxis, &-vAxis );
	}

	D3DXQUATERNION qtNewRot;
	if( fGapAngle < fRotateAngle )
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fGapAngle) );
	else
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle) );

	D3DXVECTOR3 vNewDir = qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	m_vCurMoveDir = vNewDir;

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );
	pOwner->SetMoveDirByRotate( qtNewRot );
}

bool ioGrapplingSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}