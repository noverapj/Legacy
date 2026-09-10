

#include "stdafx.h"

#include "ioAngleFire3Skill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

ioAngleFire3Skill::ioAngleFire3Skill()
{
	ClearData();
}

ioAngleFire3Skill::ioAngleFire3Skill( const ioAngleFire3Skill &rhs )
	: ioAttackSkill( rhs ),
	m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
	m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
	m_dwSkillLoopDuration( rhs.m_dwSkillLoopDuration ),
	m_dwSkillLoopProtectTime( rhs.m_dwSkillLoopProtectTime ),
	m_dwAngleTime( rhs.m_dwAngleTime ),
	m_fNormalMaxAngle( rhs.m_fNormalMaxAngle ),
	m_fNormalMinAngle( rhs.m_fNormalMinAngle ),
	m_fJumpMaxAngle( rhs.m_fJumpMaxAngle ),
	m_fJumpMinAngle( rhs.m_fJumpMinAngle ),
	m_AngleGuidEffect( rhs.m_AngleGuidEffect ),
	m_dwSkillLoopRotateSpeed( rhs.m_dwSkillLoopRotateSpeed )
{
	ClearData();
}

ioAngleFire3Skill::~ioAngleFire3Skill()
{
}

void ioAngleFire3Skill::ClearData()
{
	m_State = SS_PRE;

	m_dwLoopStartTime = 0;
	m_dwLoopEndTime = 0;

	m_bSetHandMesh = false;
	m_bReduceGauge = false;

	m_fCurAngle = 0;

	m_vAttackDir = ioMath::VEC3_ZERO;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwSkillLoopProtectEndTime = 0;

	m_dwInvisibleCloakID = 0;
}

void ioAngleFire3Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	
	//Loop
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;
	m_fSkillLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	m_dwSkillLoopDuration = rkLoader.LoadInt_e( "skill_loop_duration", 0 );
	m_dwSkillLoopProtectTime = rkLoader.LoadInt_e( "skill_loop_protect_time", 1000 );	

	//Angle
	m_dwAngleTime = (DWORD)rkLoader.LoadInt_e( "angle_time", 0 );

	m_fNormalMaxAngle = rkLoader.LoadFloat_e( "normal_max_angle", 0.0f );
	m_fNormalMinAngle = rkLoader.LoadFloat_e( "normal_min_angle", 0.0f );

	m_fJumpMaxAngle = rkLoader.LoadFloat_e( "jump_max_angle", 0.0f );
	m_fJumpMinAngle = rkLoader.LoadFloat_e( "jump_min_angle", 0.0f );

	rkLoader.LoadString_e( "angle_guid_effect", "", szBuf, MAX_PATH );
	m_AngleGuidEffect = szBuf;

	m_dwSkillLoopRotateSpeed = (DWORD)rkLoader.LoadInt_e( "skill_loop_rotate_speed", 0 );	
}

ioSkill* ioAngleFire3Skill::Clone()
{
	return new ioAngleFire3Skill( *this );
}

bool ioAngleFire3Skill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
		pChar->ShowEquipItemMesh( false, ES_CLOAK );
	}

	ClearData();

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge( pChar->GetCharName() ) );

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	return true;
}

void ioAngleFire3Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_PRE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				SetLoopState( pChar, pStage );
			}
		}
		break;
	case SS_LOOP:
		{

			CheckAngle( pChar, m_fCurMinAngle, m_fCurMaxAngle );
			CheckKeyInput( pChar );
			ProcessRotate( pChar, m_dwSkillLoopRotateSpeed );

			CheckDir( pChar );
			UpdateAngleGuidEffect( pChar, pStage );

			if( CheckLoopState( pChar, pStage ) )
			{
				SetActionState( pChar, pStage );
				return;
			}

			if( 0 < m_dwLoopEndTime && m_dwLoopEndTime < dwCurTime && pChar->IsNeedProcess() )
			{
				SetActionState( pChar, pStage );
			}
		}
		break;
	case SS_ACTION:
		{
			if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
			{
				SetEndState( pChar, pStage );
			}
		}
		break;
	}
}

void ioAngleFire3Skill::SetLoopState( ioBaseChar* pChar, ioPlayStage* pStage )
{
	if( !pChar )
		return;

	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_State = SS_LOOP;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );
	float fAniRate = FLOAT1;
	if( m_fSkillLoopMotionRate > 0.0f )
		fAniRate = m_fSkillLoopMotionRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwLoopStartTime = dwCurTime;
	m_dwLoopEndTime = dwCurTime;
	m_dwLoopEndTime += m_dwSkillLoopDuration;

	m_dwSkillLoopProtectEndTime = dwCurTime;
	m_dwSkillLoopProtectEndTime += m_dwSkillLoopProtectTime;

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	float fHieght = pStage->GetMapHeight( vPos.x, vPos.z );

	if( GetSkillUseType() == SUT_JUMP )
	{
		m_fCurAngle = m_fJumpMinAngle;
		m_fCurMaxAngle = m_fJumpMaxAngle;
		m_fCurMinAngle = m_fJumpMinAngle;
	}
	else
	{
		m_fCurAngle = m_fNormalMinAngle;
		m_fCurMaxAngle = m_fNormalMaxAngle;
		m_fCurMinAngle = m_fNormalMinAngle;
	}

	vPos = pChar->GetMidPositionByRate();
	ioMapEffect *pMapEffect = pChar->CreateMapEffectBySkill( m_AngleGuidEffect, vPos );
	if( pMapEffect )
	{
		m_dwAngleGuidEffect = pMapEffect->GetUniqueID();

		UpdateAngleGuidEffect( pChar, pStage );
	}
}

bool ioAngleFire3Skill::CheckLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( pChar->IsNeedProcess() )
	{
		//키입력 여부 검사
		bool bReserveSkill = false;
		int iSkillNum = pChar->GetSkillKeyInput();
		if( iSkillNum >= 0 )
		{
			bReserveSkill = true;
		}

		if( pChar->IsAttackKey() && !bReserveSkill )
		{
			return true;
		}		
	}

	return false;
}

void ioAngleFire3Skill::SetActionState( ioBaseChar *pChar, ioPlayStage* pStage, bool bSend )
{
	if( !pChar )
		return;

	if( !m_bReduceGauge )
	{
		pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(),
										   pChar->GetCurItemSkillGauge( GetOwnerItem()->GetName() ) );
		m_bReduceGauge = true;
	}	
	
	if( m_dwAngleGuidEffect != -1 )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwAngleGuidEffect );
		if( pMapEffect )
			pMapEffect->EndEffectForce();

		m_dwAngleGuidEffect = -1;
	}

	m_State = SS_ACTION;
	ioAttackSkill::AttackSkillFire( pChar );

	if( pChar->IsNeedProcess() && pChar->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_ACTION;
		kPacket << pChar->GetTargetRot();
		kPacket << m_vAttackDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAngleFire3Skill::SetEndState( ioBaseChar* pChar, ioPlayStage* pStage )
{
	if( !pChar )
		return;

	if( !pStage )
		return;

	m_State = SS_END;

	if( m_dwAngleGuidEffect != -1 )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwAngleGuidEffect );
		if( pMapEffect )
			pMapEffect->EndEffectForce();

		m_dwAngleGuidEffect = -1;
	}
}

void ioAngleFire3Skill::CheckKeyInput( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() ) return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{	
		m_CurDirKey = eNewDirKey;

		if( pChar->IsNeedSendNetwork() )
		{
			if( m_State == SS_LOOP )
			{		
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SSC_LOOP;				
				kPacket << pChar->GetTargetRot();
				kPacket << (int)m_CurDirKey;
				P2PNetwork::SendToAllPlayingUser( kPacket );

			}	
		}
	}
}

void ioAngleFire3Skill::ProcessRotate( ioBaseChar *pChar, float fRotateSpeed )
{
	if( !pChar ) return;
	if( fRotateSpeed <= 0 ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / fRotateSpeed;
	float fRotateAngle = 360.0f * fRate;

	D3DXQUATERNION qtCurRot = pChar->GetTargetRot();
	D3DXVECTOR3 vCurMoveDir = qtCurRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
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

	if( fDotValue >= 0.98f )
	{
		pChar->SetTargetRotToRotate( qtRotate, true );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -0.98f )
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

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );
	pChar->SetTargetRotToRotate( qtNewRot, true );
}

void ioAngleFire3Skill::CheckAngle( ioBaseChar* pChar, float fMinAngle, float fMaxAngle )
{
	if( !pChar )
		return;

	if( m_dwAngleTime == 0 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwLoopStartTime;

	dwGapTime = dwGapTime % m_dwAngleTime;

	float fGapAngle = fabsf(fMaxAngle - fMinAngle);
	float fHalfTime = (float)m_dwAngleTime * FLOAT05;

	if( dwGapTime > fHalfTime )
	{
		dwGapTime -= fHalfTime;

		float fRate = (float)dwGapTime / fHalfTime;
		m_fCurAngle = fMaxAngle - ( fGapAngle * fRate );
	}
	else if( dwGapTime < fHalfTime )
	{
		float fRate = (float)dwGapTime / fHalfTime;
		m_fCurAngle = fMinAngle + ( fGapAngle * fRate );
	}
	else
	{
		m_fCurAngle = fMaxAngle;
	}	
}

void ioAngleFire3Skill::CheckDir( ioBaseChar *pChar )
{	
	if( !pChar )
		return;

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();

	D3DXVECTOR3 vSightDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vSightDir, &ioMath::UNIT_Y );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_fCurAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	m_vAttackDir = qtNewRot * vSightDir;
	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );
}

void ioAngleFire3Skill::UpdateAngleGuidEffect( ioBaseChar *pChar, ioPlayStage *pStage )
{	
	if( m_dwAngleGuidEffect != -1 )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwAngleGuidEffect );
		if( pMapEffect )
		{
			pMapEffect->SetWorldPosition( pChar->GetMidPositionByRate() );

			ioEffect *pEffect = pMapEffect->GetEffect();
			if( pEffect )
			{
				D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( m_vAttackDir );
				pEffect->GetParentNode()->SetOrientation( qtRot );
			}
		}
	}
}

void ioAngleFire3Skill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}
	
	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}
	m_bSetHandMesh = false;

	ioPlayStage *pStage = pChar->GetCreator();
	if( m_dwAngleGuidEffect != -1 )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwAngleGuidEffect );
		if( pMapEffect )
			pMapEffect->EndEffectForce();

		m_dwAngleGuidEffect = -1;
	}

	pChar->ShowEquipItemMesh( true, ES_CLOAK );
}

bool ioAngleFire3Skill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}


void ioAngleFire3Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{

	ioPullWeapon2* pPull = ToPullWeapon2( pWeapon );

	if( pPull )
	{
		pPull->SetMoveDir( m_vAttackDir );
	}
}

void ioAngleFire3Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{	
	case SSC_LOOP:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			int iDirKey;
			rkPacket >> iDirKey;

			pChar->SetTargetRotToRotate( qtRot, true, true );
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDirKey;
		}
		break;
	case SSC_ACTION:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			rkPacket >> m_vAttackDir;
			pChar->SetTargetRotToRotate( qtRot, true, true );
			SetActionState( pChar, pStage, false);
		}
		break;
	}
}


bool ioAngleFire3Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_State )
	{
	case SS_PRE:	
	case SS_LOOP:	
	case SS_ACTION:
		return false;

	case SS_END:			
		return true;
	}

	return true;
}

bool ioAngleFire3Skill::IsNoDropState() const
{	
	if( m_State == SS_LOOP || m_State == SS_ACTION )
		return true;

	return false;
}

bool ioAngleFire3Skill::IsProtected( int iDefenseBreakType ) const
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

	switch( m_State )
	{
	case SS_PRE:
		return true;
		break;

	case SS_LOOP:
		if( 0 < m_dwSkillLoopProtectEndTime && dwCurTime < m_dwSkillLoopProtectEndTime )
			return true;
		break;

	case SS_ACTION:
		if( 0 < m_dwFireMotionEndTime && dwCurTime < m_dwFireMotionEndTime )
			return true;
	}

	return false;
}