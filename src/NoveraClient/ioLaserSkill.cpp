#include "StdAfx.h"
#include "ioLaserSkill.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "WeaponDefine.h"

ioLaserSkill::ioLaserSkill(void)
{
}

ioLaserSkill::ioLaserSkill( const ioLaserSkill &rhs ):
ioAttackSkill( rhs ),
m_szPreActionAni( rhs.m_szPreActionAni ),
m_fPreActionAniRate( rhs.m_fPreActionAniRate ),
m_fJumpAngle( rhs.m_fJumpAngle ),
m_fJumpPower( rhs.m_fJumpPower ),
m_PreAttack( rhs.m_PreAttack ),
m_fWeaponMoveSpeed( rhs.m_fWeaponMoveSpeed ),
m_dwWeaponRotateSpeed( rhs.m_dwWeaponRotateSpeed ),
m_fMaxMoveAngle( rhs.m_fMaxMoveAngle ),
m_EndWepaon( rhs.m_EndWepaon )
{
}


ioLaserSkill::~ioLaserSkill(void)
{
}

ioSkill* ioLaserSkill::Clone()
{
	return new ioLaserSkill( *this );
}

void ioLaserSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );
	char szBuf[MAX_PATH];
	//PreAction
	rkLoader.LoadString_e( "pre_action_ani", "", szBuf, MAX_PATH );
	m_szPreActionAni = szBuf;
	m_fPreActionAniRate = rkLoader.LoadFloat_e( "pre_action_ani_rate", FLOAT1 );
	m_fJumpAngle = rkLoader.LoadFloat_e( "jump_angle", 0.0f );
	m_fJumpPower = rkLoader.LoadFloat_e( "jump_power", 0.0f );
	LoadAttackAttribute_e( "pre_attack", m_PreAttack, rkLoader );

	m_fWeaponMoveSpeed = rkLoader.LoadFloat_e( "weapon_move_speed", 0.0f );
	LoadWeaponInfo( "end_weapon", m_EndWepaon, rkLoader );

	m_dwWeaponRotateSpeed = (DWORD)rkLoader.LoadInt_e( "weapon_rotate_speed", 0 );
	m_fMaxMoveAngle = rkLoader.LoadFloat_e( "max_move_angle", 0.0f );
}

void ioLaserSkill::ClearData()
{
	m_SkillState = SS_NONE;
	m_dwCurWeaponIdx = 0;
	m_vAttackDir = ioMath::VEC3_ZERO;
	m_fCurMoveAngle = 0.0f;
}

bool ioLaserSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	ClearData();

	pChar->AttachEffect( m_SkillEffectName );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;

	}
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	SetCameraBuff( pChar );

	CheckSkillDesc( pChar );	
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );


	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	m_vAttackDir = -ioMath::UNIT_Y;
	return true;
}

void ioLaserSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_SkillState )
	{
	case SS_NONE://처음 점프를 하기 전까지(PreDelay)
		{
			DWORD dwPreDelayTime = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				SetPreActionState( pChar );
			}
		}
		break;
	case SS_PRE_ACTION:
		{
			if( m_dwMotionEndTime < dwCurTime )
			{
				m_dwMotionEndTime = 0;
				SetPreAttackState( pChar );
			}
		}
		break;
	case SS_PRE_ATTACK:
		{
			if( m_dwMotionEndTime < dwCurTime )
			{
				m_dwMotionEndTime = 0;
				SetActionState( pChar );
				return;
			}

			MoveWeaponPosition( pChar );
		}
		break;
	case SS_ACTION:
		{
			if( m_dwMotionEndTime < dwCurTime )
			{
				m_dwMotionEndTime = 0;
				SetEndState( pChar );
				return;
			}
		}
		break;
	}
}

void ioLaserSkill::SetPreActionState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_szPreActionAni );
	if( iAniID == -1 )
		return;

	float fTimeRate = FLOAT1;
	if ( m_fPreActionAniRate > 0.0f )
		fTimeRate = m_fPreActionAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, 5.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	D3DXVECTOR3 vTargetDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	float fAngleH = ioMath::ArrangeHead( 0 );
	float fAngleV = ioMath::ArrangeHead( m_fJumpAngle );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	// vertical
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
	vTargetDir = qtRot * vTargetDir;

	// horizon
	D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
	vTargetDir = qtRot * vTargetDir;

	pChar->SetForcePower( vTargetDir, fabs(m_fJumpPower), FLOAT1, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_SkillState = SS_PRE_ACTION;
}

void ioLaserSkill::SetPreAttackState( ioBaseChar *pChar )
{
	if( !pChar )
		return;
	ioPlayStage *pState = pChar->GetCreator();
	if ( !pState )
		return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;
	int iAniID = pGrp->GetAnimationIdx( m_PreAttack.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	m_SkillState = SS_PRE_ATTACK;

	pChar->AttachEffect( m_SkillEffectName );

	float fTimeRate  = m_PreAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_PreAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_PreAttack.m_dwEndDelay;
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_bRefreshFireTimeList )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
			iAniID,
			m_PreAttack.m_vWeaponInfoList,
			FTT_SKILL_ATTACK,
			fTimeRate,
			dwPreDelay );
	}

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );	

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + dwEndDelay;

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
}

void ioLaserSkill::MoveWeaponPosition( ioBaseChar *pChar )
{
	if ( m_fMaxMoveAngle <= m_fCurMoveAngle )
		return;

	if ( !pChar )
		return;

	ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwCurWeaponIdx );
	ioSimpleBoxWeapon* pSimple = ToSimpleBoxWeapon( pWeapon );
	if( !pSimple )
		return;

	float fFrameGap = g_FrameTimer.GetSecPerFrame();
	float fRate = fFrameGap * m_dwWeaponRotateSpeed;
	m_fCurMoveAngle += fRate;
	fRate = DEGtoRAD(fRate);
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir );
	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vDir, &m_vAttackDir );
	D3DXVec3Normalize( &vAxis, &-vAxis );

	float fDotValue = D3DXVec3Dot( &m_vAttackDir, &vDir );
	float fGapAngle = RADtoDEG( acosf(fDotValue) );
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, fRate );
	m_vAttackDir = qtRot * m_vAttackDir;

	pSimple->SetMoveDir( m_vAttackDir );
}

void ioLaserSkill::SetActionState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	m_SkillState = SS_ACTION;
	RemoveWeapon( pChar );
	AttackSkillFire( pChar );
}

bool ioLaserSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_PRE_ACTION:
	case SS_PRE_ATTACK:
	case SS_ACTION:
		return false;
	case SS_END:
		if ( m_dwMotionEndTime < FRAMEGETTIME() && m_fSkillEndNextJumpAmt == 0.0f )
			return true;
		else
			return false;
	}

	return true;
}

bool ioLaserSkill::IsAttackEndState() const
{
	if( m_SkillState == SS_END && m_dwMotionEndTime < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioLaserSkill::IsProtected( int iDefenseBreakType ) const
{
	return ioAttackSkill::IsProtected( iDefenseBreakType );
}

bool ioLaserSkill::IsNoDropState() const
{
	switch( m_SkillState )
	{
	case SS_PRE_ATTACK:
	case SS_ACTION:
		return true;
	}

	return false;
}

void ioLaserSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );
	RemoveWeapon( pChar );
	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	EndCameraBuff( pChar );

	pChar->DestroyMagicCircle();

}

void ioLaserSkill::RemoveWeapon( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwCurWeaponIdx );
	if ( pWeapon && pWeapon->IsLive() )
		pWeapon->SetWeaponDead();
	
	m_dwCurWeaponIdx = 0;
}

void ioLaserSkill::SetEndState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	m_SkillState = SS_END;
	m_dwMotionEndTime = FRAMEGETTIME() + 100;
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	CallEndAttack( pChar );
	
}

void ioLaserSkill::CallEndAttack( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx = m_EndWepaon.m_iWeaponIdx;
	kFireTime.iResistanceIdx = m_EndWepaon.m_iResistanceIdx;
	kFireTime.szWoundedAni = m_EndWepaon.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration = m_EndWepaon.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni = m_EndWepaon.m_bWoundedLoopAni;
	kFireTime.dwWeaponIdx = m_dwWeaponBaseIndex++;
	kFireTime.eFireTimeType = FTT_SKILL_ATTACK;

	D3DXVECTOR3 vDir = pChar->GetWorldOrientation() * D3DXVECTOR3(0.0f,0.0f,-FLOAT1);
	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
	D3DXVec3Normalize( &vDir, &vDir );
	ioHashString szItemName;
	if ( GetOwnerItem() )
		szItemName = GetOwnerItem()->GetName();
	pChar->ExtendFireExplicit( kFireTime, vPos, vDir, szItemName );
}

void ioLaserSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	if( !pWeapon || !pStage )
		return;

	ioAttackSkill::ApplyAttributeToWeapon( pWeapon, pStage );
	m_dwCurWeaponIdx = pWeapon->GetWeaponIndex();
	ioSimpleBoxWeapon* pSimple = ToSimpleBoxWeapon( pWeapon );
	if( pSimple )
	{
		pSimple->SetOwnerAttach( true );
		pSimple->SetMoveDir( m_vAttackDir );
	}
}