

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioNpcChar.h"

#include "ioTeleportGrapplingSwingSkill.h"

ioTeleportGrapplingSwingSkill::ioTeleportGrapplingSwingSkill()
{
	m_SkillState = SS_NONE;

	m_dwSkillLoopStartTime = 0;

	m_dwSkillMapEffect = -1;

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_vCurMoveDir = ioMath::VEC3_ZERO;

	m_bSetHandMesh = false;
}

ioTeleportGrapplingSwingSkill::ioTeleportGrapplingSwingSkill( const ioTeleportGrapplingSwingSkill &rhs )
: ioAttackSkill( rhs ),
  m_fFindTargetRange( rhs.m_fFindTargetRange ),
  m_fFindTargetWidth( rhs.m_fFindTargetWidth ),
  m_fFindTargetHeight( rhs.m_fFindTargetHeight ),
  m_fFindTargetBackOffset( rhs.m_fFindTargetBackOffset ),
  m_fBackRange( rhs.m_fBackRange ),
  m_fHeightRange( rhs.m_fHeightRange ),
  m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
  m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
  m_dwSkillLoopDuration( rhs.m_dwSkillLoopDuration ),
  m_dwSkillLoopProtectTime( rhs.m_dwSkillLoopProtectTime ),
  m_SkillSwingMotion( rhs.m_SkillSwingMotion ),
  m_fSkillSwingMotionRate( rhs.m_fSkillSwingMotionRate ),
  m_SkillSwingedMotion( rhs.m_SkillSwingedMotion ),
  m_fSkillSwingedMotionRate( rhs.m_fSkillSwingedMotionRate ),
  m_bReverseLoopRotate( rhs.m_bReverseLoopRotate ),
  m_fCharHeightRate( rhs.m_fCharHeightRate ),
  m_fTargetLoopGapRange( rhs.m_fTargetLoopGapRange ),
  m_dwLoopRotateTime( rhs.m_dwLoopRotateTime ),
  m_dwEnableSwingTime( rhs.m_dwEnableSwingTime ),
  m_bEnableSwingMove( rhs.m_bEnableSwingMove ),
  m_fMoveSpeed( rhs.m_fMoveSpeed ),
  m_dwMoveRotateTime( rhs.m_dwMoveRotateTime ),
  m_TargetSearchState( rhs.m_TargetSearchState ),
  m_fFindTargetAngle( rhs.m_fFindTargetAngle ),
  m_TargetWoundType( rhs.m_TargetWoundType )
{
	m_SkillState = SS_NONE;

	m_dwSkillLoopStartTime = 0;

	m_dwSkillMapEffect = -1;

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_vCurMoveDir = ioMath::VEC3_ZERO;

	m_bSetHandMesh = false;
}

ioTeleportGrapplingSwingSkill::~ioTeleportGrapplingSwingSkill()
{
}

void ioTeleportGrapplingSwingSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	// 타겟 검색용 충돌체
	m_fFindTargetRange		= rkLoader.LoadFloat_e( "find_target_range", 0.0f );
	m_fFindTargetWidth		= rkLoader.LoadFloat_e( "find_target_width", 0.0f );
	m_fFindTargetHeight		= rkLoader.LoadFloat_e( "find_target_height", 0.0f );
	m_fFindTargetBackOffset	= rkLoader.LoadFloat_e( "find_target_back_offset", 0.0f );

	// 순간이동 보정
	m_fBackRange	= rkLoader.LoadFloat_e( "back_move_range", 0.0f );
	m_fHeightRange	= rkLoader.LoadFloat_e( "back_move_height_range", 0.0f );

	// 루프 애니메이션 관련
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion		= szBuf;
	m_fSkillLoopMotionRate	= rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	m_dwSkillLoopDuration	= rkLoader.LoadInt_e( "skill_loop_duration", 0 );
	m_dwSkillLoopProtectTime= rkLoader.LoadInt_e( "skill_loop_protect_time", 0 );

	// 던지기 애니메이션
	rkLoader.LoadString_e( "skill_swing_motion", "", szBuf, MAX_PATH );
	m_SkillSwingMotion		= szBuf;
	m_fSkillSwingMotionRate	= rkLoader.LoadFloat_e( "skill_swing_motion_rate", FLOAT1 );

	// Swinged??
	rkLoader.LoadString_e( "skill_swinged_motion", "", szBuf, MAX_PATH );
	m_SkillSwingedMotion		= szBuf;
	m_fSkillSwingedMotionRate	= rkLoader.LoadFloat_e( "skill_swinged_motion_rate", FLOAT1 );

	m_bReverseLoopRotate = rkLoader.LoadBool_e( "reverse_loop_rotate", false );

	// 잡힌 대상 위치
	m_fCharHeightRate		= rkLoader.LoadFloat_e( "char_height_rate", FLOAT05 );
	m_fTargetLoopGapRange	= rkLoader.LoadFloat_e( "skill_target_loop_gap_range", 0.0f );
	m_dwLoopRotateTime		= rkLoader.LoadInt_e( "skill_loop_rotate_speed", 0 );
	m_dwEnableSwingTime		= rkLoader.LoadInt_e( "skill_loop_swing_enable_time", 0 );

	m_bEnableSwingMove	= rkLoader.LoadBool_e( "enable_swing_move", false );
	m_fMoveSpeed		= rkLoader.LoadFloat_e( "swing_move_speed", 0.0f );
	m_dwMoveRotateTime	= (DWORD)rkLoader.LoadInt_e( "swing_move_rotate_speed", 0 );

	// 스윙 버프
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

	m_TargetSearchState = (TargetSearchState)rkLoader.LoadInt_e( "target_search_state", TSS_COLLISION );

	m_fFindTargetAngle		= rkLoader.LoadFloat_e( "find_target_angle", 0.0f );
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
}

ioSkill* ioTeleportGrapplingSwingSkill::Clone()
{
	return new ioTeleportGrapplingSwingSkill( *this );
}

bool ioTeleportGrapplingSwingSkill::OnSkillStart( ioBaseChar *pChar )
{
	m_SkillState = SS_NONE;
	m_GrapplingTargetName.Clear();

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( pChar )
	{
		m_vCurMoveDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	}

	ioPlayStage *pStage = pChar->GetCreator();
	if( pStage )
	{
		// 이펙트 처리
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );

		ioMapEffect *pMapEffect = NULL;
		pMapEffect = pStage->CreateMapEffect( m_SkillMapEffectName, pChar->GetWorldPosition(), vScale );
		if( pMapEffect )
		{
			m_dwSkillMapEffect = pMapEffect->GetUniqueID();
			pMapEffect->SetWorldOrientation( pChar->GetTargetRot() );
		}
	}

	return true;
}

void ioTeleportGrapplingSwingSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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

				EndPreDelayEffect( pChar );
				SetTeleport( pChar, pStage );//, m_fFindTargetRange, m_fFindTargetWidth, m_fFindTargetHeight );
			}
		}
		break;
	case SS_TELEPORT:
		AttackSkillFire( pChar );
		break;
	case SS_ACTION:
		break;
	case SS_LOOP:
		{
			//pChar->SetGravityAmt( 0.0f );

			// Target 유효 체크
			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			
			if( pTarget )
			{
				if( m_dwSkillLoopStartTime+g_App.GetGrapplingCheckDelay() < dwCurTime &&
					pTarget->GetState() != CS_GRAPPLING_WOUNDED_SKILL )
				{
					pChar->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
					SetSkillEndState( pChar, true );
					return;
				}
			}
			else
			{
				pChar->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
				SetSkillEndState( pChar, true );
				return;
			}

			// 시간만료 체크
			if( pChar->IsNeedProcess() && m_dwSkillLoopStartTime+m_dwSkillLoopDuration < dwCurTime )
			{
				pChar->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
				SetSkillEndState( pChar, true );
				return;
			}

			// 액션키 체크
			if( pChar->IsNeedProcess() && m_dwSkillLoopStartTime+m_dwEnableSwingTime < dwCurTime )
			{
				if( pChar->IsAttackKey() ||
					pChar->IsDefenseKey() ||
					pChar->IsJumpKey() ||
					pChar->GetSkillKeyInput() == iSlot )
				{
					pChar->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
					SetSkillEndState( pChar, true );
					return;
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

void ioTeleportGrapplingSwingSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}
/*
	if( m_dwSkillMapEffect != -1 )
	{
		pChar->EndMapEffect( m_dwSkillMapEffect, true );
		m_dwSkillMapEffect = -1;
	}
*/
	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );


	m_SkillState = SS_NONE;

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_vCurMoveDir = ioMath::VEC3_ZERO;
}

bool ioTeleportGrapplingSwingSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioTeleportGrapplingSwingSkill::AttackSkillFire( ioBaseChar *pChar )
{
	m_SkillState = SS_ACTION;

	if( m_GrapplingTargetName.IsEmpty() )
	{
		SetSkillEndState( pChar, true );
		return;
	}

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	ioAttackSkill::AttackSkillFire( pChar );


}

void ioTeleportGrapplingSwingSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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
}

bool ioTeleportGrapplingSwingSkill::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) return false;
	if( m_GrapplingTargetName != szTargetName )	return false;

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

	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return false;

	// for NPC
	bool bNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || bNPC )
	{
		pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
		pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
	}

	return true;
}

D3DXVECTOR3 ioTeleportGrapplingSwingSkill::GetGrapplingTargetPos( ioBaseChar *pChar )
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

bool ioTeleportGrapplingSwingSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_TELEPORT:
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

bool ioTeleportGrapplingSwingSkill::IsEndState() const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

void ioTeleportGrapplingSwingSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
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
				pTarget->SetTargetRotToRotate( qtRot, true, false );
				pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
			}
		}
		break;
	case SSC_TELEPORT:
		{
			D3DXVECTOR3 vTargetPos;
			rkPacket >> vTargetPos;
			rkPacket >> m_GrapplingTargetName;
			m_SkillState = SS_TELEPORT;
			pChar->SetWorldPosition( vTargetPos );
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
					pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
					pTarget->SetGrapplingWoundSwing( pChar->GetCharName(), GetSkillWoundGrapplingInfo(),
													 m_SkillSwingedMotion, m_fSkillSwingedMotionRate );
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

bool ioTeleportGrapplingSwingSkill::IsProtected( int iDefenseBreakType ) const
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

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_TELEPORT:
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

bool ioTeleportGrapplingSwingSkill::IsEnableReserve() const
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

void ioTeleportGrapplingSwingSkill::ProcessLoopDir( ioBaseChar *pChar )
{
	if( m_dwLoopRotateTime == 0 )
		return;

	// 캐릭터 회전
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	float fRate = (fTimePerSec*1000) / m_dwLoopRotateTime;
	float fRotateAngle = 360.0f * fRate;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

	D3DXQUATERNION	qtNewRot;
	D3DXVECTOR3 vAxis = ioMath::UNIT_Y;

	if( m_bReverseLoopRotate )
		vAxis = -ioMath::UNIT_Y;

	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle));

	D3DXQUATERNION qtCharRot = pChar->GetTargetRot();
	D3DXVECTOR3 vDir = qtCharRot * D3DXVECTOR3( 0.0, 0.0, -FLOAT1 );
	vDir = qtNewRot * vDir;

	ioMath::CalcDirectionQuaternion( qtNewRot, -vDir );
	pChar->SetTargetRotToRotate( qtNewRot, true, true );
}

void ioTeleportGrapplingSwingSkill::ProcessRotate( ioBaseChar *pOwner )
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

void ioTeleportGrapplingSwingSkill::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;
	if( m_dwMoveRotateTime <= 0 ) return;

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

void ioTeleportGrapplingSwingSkill::SetSkillLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_SkillState != SS_ACTION )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_fSkillLoopMotionRate > 0.0f )
		fAniRate = m_fSkillLoopMotionRate;

	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_SkillState = SS_LOOP;

	m_dwSkillLoopStartTime = FRAMEGETTIME();
}

void ioTeleportGrapplingSwingSkill::SetSkillEndState( ioBaseChar *pChar, bool bSendNet )
{
	if( m_SkillState == SS_END )	return;

	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_fSkillSwingMotionRate > 0.0f )
		fAniRate = m_fSkillSwingMotionRate;

	int iAniID = pGrp->GetAnimationIdx( m_SkillSwingMotion );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fAniRate;

	// Key Reserve
	//m_dwEnableReserveTime = dwCurTime;
	//m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" );

	m_SkillState = SS_END;

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
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || bNPC )
	{
		if( pTarget->GetState() == CS_GRAPPLING_WOUNDED_SKILL && pTarget->IsNeedProcess() )
		{
			pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
			pTarget->SetGrapplingWoundSwing( pChar->GetCharName(), GetSkillWoundGrapplingInfo(),
											 m_SkillSwingedMotion, m_fSkillSwingedMotionRate );
		}
	}
}

void ioTeleportGrapplingSwingSkill::SetTeleport( ioBaseChar *pOwner, ioPlayStage *pStage )//, float fRange, float fWidth, float fHeight )
{
	if( !pStage || !pOwner || !pOwner->IsNeedProcess() )
		return;

	m_SkillState = SS_TELEPORT;

	ioBaseChar* pTargetChar = NULL;
	if ( m_TargetSearchState == TSS_COLLISION )
		pTargetChar = FindTargetByWoundedCollision( pOwner, pStage );
	else if ( m_TargetSearchState == TSS_RANGE )
		pTargetChar = FindTargetByRange( pOwner, pStage );

	if ( pTargetChar == NULL )
		pTargetChar = pOwner;

	pOwner->ReduceNeedGaugeBySkill( this, GetNeedGauge(pOwner->GetCharName()) );
	m_bReduceGauge = true;

	if( pOwner && pTargetChar )
	{
		D3DXVECTOR3 vOffSet( 0.0f, 0.0f, 0.0f );
 		if( pOwner == pTargetChar )	
			vOffSet.y = m_fHeightRange;

		D3DXVECTOR3 vTargetPos = pTargetChar->GetWorldPosition() + vOffSet;
		D3DXQUATERNION qtTargetOrientation = pTargetChar->GetWorldOrientation();
		D3DXVECTOR3 vNewPos = vTargetPos - ( ( qtTargetOrientation * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) ) * m_fBackRange );
		
		pOwner->SetWorldPosition( vNewPos );
		pOwner->SetTargetRotAndMoveDirChange( qtTargetOrientation );

		if( pStage )
			pStage->SetCameraToOwnerExplicit( pOwner );

		if( pOwner != pTargetChar )
			m_GrapplingTargetName = pTargetChar->GetCharName();
		else						
			m_GrapplingTargetName.Clear();

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << SSC_TELEPORT;
			kPacket << pOwner->GetTargetRot();
			kPacket << pOwner->GetWorldPosition();
			kPacket << m_GrapplingTargetName;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

ioBaseChar* ioTeleportGrapplingSwingSkill::FindTargetByWoundedCollision( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( !pStage || !pOwner || !pOwner->IsNeedProcess() )
		return NULL;

	// 타겟 검색에 사용할 충돌영역을 설정.	
	D3DXVECTOR3	vMin, vMax, vCenter, vOffset;
	vCenter = pOwner->GetWorldPosition();
	vCenter.y = pOwner->GetMidHeightByRate();
	vCenter += ( m_vCurMoveDir * ( m_fFindTargetRange * FLOAT05 - m_fFindTargetBackOffset )  ); 

	vMin = vMax = vCenter;

	vMin.x -= m_fFindTargetWidth * FLOAT05;
	vMin.y -= m_fFindTargetHeight * FLOAT05;

	vMax.x += m_fFindTargetWidth * FLOAT05;
	vMax.y += m_fFindTargetHeight * FLOAT05;
	vMax.z += m_fFindTargetRange;

	ioOrientBox pOrientBox;
	pOrientBox.SetBoxByMinMax( vMin, vMax );
	pOrientBox.Transform( vCenter, ioMath::UNIT_ALL, pOwner->GetWorldOrientation() );
	pOrientBox.SetCenter( vCenter );

#ifndef SHIPPING
	pOrientBox.GetVertices( ioBaseChar::m_vRangeBoxArrayForTest );
#endif

	ioBaseChar* pTargetChar = NULL;
	float fPrevDistSq = m_fFindTargetRange;
	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pChar = (*iter);
		if( !pChar )
			continue;

		if( pOwner->GetCharName() == pChar->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pChar->GetTeam() )
			continue;

		if( pChar->IsSystemState() )
			continue;

		if( !pChar->IsEnableTarget() )
			continue;

		if( pChar->GetState() != CS_BOUND_BLOW_WOUNDED )
		{
			if( !pChar->IsFloating() )
				continue;

			if( pChar->IsApplyDownState(false) )
				continue;
		}

		if( pChar->IsWoundedCollision( pOrientBox ) )
		{
			D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - pOwner->GetWorldPosition();
			float fDistSq = D3DXVec3Length( &vDiff );
			if( fDistSq < fPrevDistSq )	// Enemy
			{
				pTargetChar = pChar;
				fPrevDistSq = fDistSq;
			}
		}
	}

	return pTargetChar;
}

ioBaseChar* ioTeleportGrapplingSwingSkill::FindTargetByRange( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( !pStage || !pOwner || !pOwner->IsNeedProcess() )
		return NULL;

	ioBaseChar *pTargetChar = NULL;
	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	float fPreDistSq = m_fFindTargetRange * m_fFindTargetRange;
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pChar = *iter;
		if( !pChar )
			continue;

		if( pOwner->GetCharName() == pChar->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pChar->GetTeam() )
			continue;

		if( pChar->IsSystemState() )
			continue;

		if( !pChar->IsEnableTarget() )
			continue;

		if( !pChar->CheckTargetWoundType( m_TargetWoundType ) )
			continue;

		D3DXVECTOR3 vDiff = pChar->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		float fDistSq = D3DXVec3LengthSq( &vDiff );

		if( fDistSq <= fPreDistSq )
		{
			D3DXVec3Normalize( &vDiff, &vDiff );
			float fValue = D3DXVec3Dot( &vDir, &vDiff );
			float fAngleGap = RADtoDEG( acosf(fValue) );
			if( fAngleGap < m_fFindTargetAngle * FLOAT05 )
			{
				pTargetChar = pChar;
				fPreDistSq = fDistSq;
			}
		}
	}

	return pTargetChar;
}