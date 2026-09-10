
#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioLaserControlSkill.h"

#include "WeaponDefine.h"

ioLaserControlSkill::ioLaserControlSkill()
{
	ClearData();
}

ioLaserControlSkill::ioLaserControlSkill( const ioLaserControlSkill &rhs )
	: ioAttackSkill( rhs ),
	m_fCircleRange( rhs.m_fCircleRange ),
	m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
	m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
	m_fCircleOffSet( rhs.m_fCircleOffSet ),
	m_ReadyCircle( rhs.m_ReadyCircle ),
	m_RedEffect( rhs.m_RedEffect ),
	m_BlueEffect( rhs.m_BlueEffect ),
	m_fCharJumpPower( rhs.m_fCharJumpPower ),
	m_AttackAttribute( rhs.m_AttackAttribute ),
	m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
	m_SkillLoopUpMotion( rhs.m_SkillLoopUpMotion ),
	m_SkillLoopDownMotion( rhs.m_SkillLoopDownMotion ),
	m_SkillActionLoopMotion( rhs.m_SkillActionLoopMotion ),
	m_SkillActionLoopUpMotion( rhs.m_SkillActionLoopUpMotion ),
	m_SkillActionLoopDownMotion( rhs.m_SkillActionLoopDownMotion ),
	m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
	m_SkillDownMotion( rhs.m_SkillDownMotion ),
	m_SkillUpMotion( rhs.m_SkillUpMotion ),
	m_dwProtectTime( rhs.m_dwProtectTime ),
	m_dwLoopDuration( rhs.m_dwLoopDuration ),
	m_dwActionLoopDuration( rhs.m_dwActionLoopDuration )
{
	ClearData();
}

ioLaserControlSkill::~ioLaserControlSkill()
{
}

void ioLaserControlSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueEffect = szBuf;

	m_fCharJumpPower = rkLoader.LoadFloat_e( "skill_jump_power", 0.0f );	
	m_dwProtectTime = rkLoader.LoadInt_e( "skill_protect_time", 0 );

	//loop
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;
	m_fSkillLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "skill_loop_up_motion", "", szBuf, MAX_PATH );
	m_SkillLoopUpMotion = szBuf;

	rkLoader.LoadString_e( "skill_loop_down_motion", "", szBuf, MAX_PATH );
	m_SkillLoopDownMotion = szBuf;

	//loop action
	rkLoader.LoadString_e( "skill_loop_action_motion", "", szBuf, MAX_PATH );
	m_SkillActionLoopMotion = szBuf;
	m_fSkillActionLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_action_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "skill_loop_action_up_motion", "", szBuf, MAX_PATH );
	m_SkillActionLoopUpMotion = szBuf;

	rkLoader.LoadString_e( "skill_loop_action_down_motion", "", szBuf, MAX_PATH );
	m_SkillActionLoopDownMotion = szBuf;

	//action
	rkLoader.LoadString_e( "skill_down_motion", "", szBuf, MAX_PATH );
	m_SkillDownMotion = szBuf;

	rkLoader.LoadString_e( "skill_up_motion", "", szBuf, MAX_PATH );
	m_SkillUpMotion = szBuf;

	m_dwLoopDuration = rkLoader.LoadInt_e( "skill_loop_duration", 0 );
	m_dwActionLoopDuration = rkLoader.LoadInt_e( "skill_loop_action_duration", 0 );
	
}

void ioLaserControlSkill::ClearData()
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_bSetHandMesh = false;

	m_dwMotionEndTime = 0;
	m_dwLoopDurationEnd = 0;
	m_dwActionLoopDurationEnd = 0;
	m_dwFireStartTime = 0;

	m_dwCurProtectTime = 0;
	m_fCurMotionRate = 0.0f;

	m_vAttackDir = ioMath::VEC3_ZERO;
}

ioSkill* ioLaserControlSkill::Clone()
{
	return new ioLaserControlSkill( *this );
}

bool ioLaserControlSkill::OnSkillStart( ioBaseChar *pChar )
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

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->SetSkillProtection();

	SetCameraBuff( pChar );

	CheckSkillDesc( pChar );	
	pChar->SetJumpPower( m_fCharJumpPower );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{	
		SetLoopState( pChar );
	}

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
	return true;
}

void ioLaserControlSkill::OnSkillEnd( ioBaseChar *pChar )
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

	ClearData();
}

void ioLaserControlSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				SetLoopState( pChar );
			}
		}
		break;
	case SS_LOOP:
		{
			CheckMotion( pChar );
			CheckMagicCircle( pChar );
			CheckLoopState( pChar );

			if( 0 < m_dwLoopDurationEnd && m_dwLoopDurationEnd < dwCurTime )
				SetEndState( pChar, false );
		}
		break;
	case SS_ACTION:
		{
			if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
			{
				pChar->ReduceNeedGaugeBySkill( this, GetOwnerItem()->GetMaxSkillGauge( pChar->GetCharName() ) );
				m_dwFireStartTime = 0;
			}

			if( pChar->IsNeedProcess() )
			{
				if( !pChar->IsAttackKeyDown() )
				{
					SetEndState( pChar, true );
				}

				if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
				{
					SetActionLoopState( pChar );
				}
			}
			else
			{
				if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
				{
					SetActionLoopState( pChar );
				}
			}
		}
		break;
	case SS_ACTION_LOOP:
		{
			if( pChar->IsNeedProcess() )
			{
				if( pChar->IsAttackKeyDown() )
				{
					CheckMotion( pChar );
					CheckWeapon( pChar );
					CheckMagicCircle( pChar );
				}
				else
				{
					SetEndState( pChar, true );
				}
			}
			else
			{
				CheckMotion( pChar );
				CheckWeapon( pChar );
				CheckMagicCircle( pChar );
			}

			if( 0 < m_dwActionLoopDurationEnd && m_dwActionLoopDurationEnd < dwCurTime )
				SetEndState( pChar, false );
		}
		break;	
	case SS_END:
		break;
	}
}

bool ioLaserControlSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetPreDelayAniName() );
	if( iAniID == -1 )
		return false;

	pChar->SetJumpPower( m_fCharJumpPower * m_fSkillGravityRate );
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1, 0.0f, GetPreDelayTime(), true );

	m_dwPreDelayStartTime = FRAMEGETTIME();

	return true;
}

void ioLaserControlSkill::SetActionState( ioBaseChar *pChar, bool bSend )
{
	if( !pChar )
		return;

	m_State = SS_ACTION;	
	m_vCirclePos = pChar->GetMagicCirclePos();

	//
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
	{
		SetEndState( pChar, true );
		return;
	}
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )
		return;

	pChar->AttachEffect( m_SkillEffectName );

	float fTimeRate  = GetAttribute().m_fAttackAniRate;
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;
	DWORD dwEndDelay = GetAttribute().m_dwEndDelay;
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_bRefreshFireTimeList )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
			iAniID,
			GetAttribute().m_vWeaponInfoList,
			FTT_SKILL_ATTACK,
			fTimeRate,
			dwPreDelay );
	}

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );	

	if( m_vAttackDir.y == 0.0f || m_fCurMotionRate == 0.0f )
	{
		// center
		pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	}
	else if( m_vAttackDir.y > 0.0f )	
	{
		// center & up
		pGrp->SetActionAni( m_SkillUpMotion, 0.0f, 0.0f, m_fCurMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
		pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1 - m_fCurMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	}
	else
	{
		// center & down
		pGrp->SetActionAni( m_SkillDownMotion, 0.0f, 0.0f, m_fCurMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
		pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1 - m_fCurMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	}

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + dwEndDelay;

	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	m_dwFireStartTime = dwCurTime;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	//
	ioMapEffect *pMapEffect = NULL;
	if( pChar->GetTeam() == TEAM_RED )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );
	}
	else if( pChar->GetTeam() == TEAM_BLUE )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
	}
	else
	{
		if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
		{
			pMapEffect = pChar->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
		}
		else
		{
			pMapEffect = pChar->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );
		}
	}

	if( pChar->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_ACTION;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioLaserControlSkill::SetLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_State = SS_LOOP;

	m_dwCurProtectTime = dwCurTime + m_dwProtectTime;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
	{
		SetEndState( pChar, true );
		return;
	}

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_fSkillLoopMotionRate > 0.0f )
		fAniRate = m_fSkillLoopMotionRate;

	pGrp->SetLoopAni( m_SkillLoopUpMotion,   0.0f, 0.0f, FLOAT1/fAniRate );
	pGrp->SetLoopAni( m_SkillLoopDownMotion, 0.0f, 0.0f, FLOAT1/fAniRate );
	pGrp->SetLoopAni( m_SkillLoopMotion,     0.0f, FLOAT1, FLOAT1/fAniRate );
		
	m_dwLoopDurationEnd = dwCurTime + m_dwLoopDuration;
	
	if( !pChar->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, m_fCircleOffSet );
	}
}

void ioLaserControlSkill::SetActionLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_State = SS_ACTION_LOOP;
	
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
	{
		SetEndState( pChar, true );
		return;
	}

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_fSkillActionLoopMotionRate > 0.0f )
		fAniRate = m_fSkillActionLoopMotionRate;

	pGrp->SetLoopAni( m_SkillActionLoopUpMotion,   0.0f, 0.0f, FLOAT1/fAniRate );
	pGrp->SetLoopAni( m_SkillActionLoopDownMotion, 0.0f, 0.0f, FLOAT1/fAniRate );
	pGrp->SetLoopAni( m_SkillActionLoopMotion,     0.0f, FLOAT1, FLOAT1/fAniRate );

	CheckMotion( pChar );

	m_dwActionLoopDurationEnd = dwCurTime + m_dwActionLoopDuration;
}

void ioLaserControlSkill::SetEndState( ioBaseChar* pChar, bool bSend )
{
	RemoveWeapon( pChar );
	m_State = SS_END;

	if( pChar->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioLaserControlSkill::CheckMotion( ioBaseChar* pChar )
{
	if( !pChar )
		return;

	m_vAttackDir = pChar->GetMagicCirclePos() - pChar->GetWorldPosition();
	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );

	//check sight direction
	D3DXVECTOR3 vSightDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	float fAngle = 0.0f;
	float fDotValue = D3DXVec3Dot( &m_vAttackDir, &vSightDir );
	fAngle = RADtoDEG( acosf(fDotValue) );

	//Check motion	
	m_fCurMotionRate = fAngle /FLOAT90;

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioAnimation *pCenter, *pDown ,*pUp;
	pUp = pCenter = pDown = NULL;

	if( m_State == SS_LOOP )
	{
		pUp = pGrp->GetLoopInPlay( m_SkillLoopUpMotion );
		pCenter = pGrp->GetLoopInPlay( m_SkillLoopMotion );
		pDown = pGrp->GetLoopInPlay( m_SkillLoopDownMotion );
	}
	else
	{
		pUp = pGrp->GetLoopInPlay( m_SkillActionLoopUpMotion );
		pCenter = pGrp->GetLoopInPlay( m_SkillActionLoopMotion );
		pDown = pGrp->GetLoopInPlay( m_SkillActionLoopDownMotion );
	}
	
	if( !pCenter || !pDown || !pUp )
	{
		#ifndef SHIPPING
		LOG.PrintTimeAndLog( 0, "ioLaserControlSkill::CheckMotion - Motion Error" );
		#endif
		return;
	}

	if( m_vAttackDir.y == 0.0f || m_fCurMotionRate == 0.0f )		// center
	{
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 );
		pDown->ForceWeight( 0.0f );
	}
	else if( m_vAttackDir.y > 0.0f )								// center & up
	{
		pUp->ForceWeight( m_fCurMotionRate );
		pCenter->ForceWeight( FLOAT1 - m_fCurMotionRate );
		pDown->ForceWeight( 0.0f );
	}
	else															// center & down
	{
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 - m_fCurMotionRate );
		pDown->ForceWeight( m_fCurMotionRate );
	}
}

void ioLaserControlSkill::CheckLoopState( ioBaseChar *pChar )
{
	if( !pChar )
		return;	
	
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

		if( pChar->IsAttackKeyDown() && !bReserveSkill )
		{	
			SetActionState( pChar, true );
		}
	}
}

void ioLaserControlSkill::CheckMagicCircle( ioBaseChar *pChar )
{
	// 뱡향 변화 체크
	pChar->CheckChangeDirectionForMagicCircle();

	// MagicCircle 위치 갱신
	pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, false );

	// MagicCircle 위치방향으로 캐릭터 회전
	D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
	pChar->SetTargetRotToTargetPos( vTargetPos, true, false );
}

void ioLaserControlSkill::CheckWeapon( ioBaseChar *pChar )
{
	ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwCurWeaponIdx );
	ioSimpleBoxWeapon* pSimple = ToSimpleBoxWeapon( pWeapon );
	if( pSimple )
	{
		pSimple->SetPointDir( pChar->GetMagicCirclePos() );		
	}
}

void ioLaserControlSkill::RemoveWeapon( ioBaseChar *pChar )
{
	ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwCurWeaponIdx );
	if( pWeapon && pWeapon->IsLive() )
	{
		pWeapon->SetWeaponDead();
	}
}

void ioLaserControlSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SSC_ACTION:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;
			rkPacket >> vPos >> qtRot;
			pChar->SetWorldPosition( vPos );
			pChar->SetTargetRotToRotate( qtRot, true, false );

			SetActionState( pChar, false );
		}
		break;
	case SSC_END:
		{
			SetEndState( pChar, false );
		}
		break;
	}
}

bool ioLaserControlSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{		
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	if( IsPreDelayProtect() )
	{	
		if( m_dwPreDelayStartTime != 0 )
			return true;	
	}

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_ACTION:
	case SS_LOOP:
	case SS_ACTION_LOOP:
		if( m_dwCurProtectTime < dwCurTime )
			return false;
		else
			return true;				
		break;
	}

	return false;
}

bool ioLaserControlSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_State )
	{
	case SS_NONE:
	case SS_LOOP:
	case SS_ACTION:
	case SS_ACTION_LOOP:
		return false;
	case SS_END:
		if( m_fSkillEndNextJumpAmt == 0.0f )
			return true;
		else
			return false;
	}

	return true;
}

bool ioLaserControlSkill::IsAttackEndState() const
{
	if( m_State == SS_END )
		return true;

	return false;
}

bool ioLaserControlSkill::IsNoDropState() const
{
	if( m_State == SS_LOOP || m_State == SS_ACTION_LOOP || m_State == SS_ACTION )
		return true;

	return false;
}

bool ioLaserControlSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioLaserControlSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	if( !pWeapon )
		return;

	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioBaseChar *pChar = pWeapon->GetOwner();
	if( pChar )
	{
		D3DXVECTOR3 vDiff = m_vCirclePos - pChar->GetMidPositionByRate();
		D3DXVec3Normalize( &vDiff, &vDiff );
		pWeapon->SetMoveDir( vDiff );

		m_dwCurWeaponIdx = pWeapon->GetWeaponIndex();

		ioSimpleBoxWeapon* pSimple = ToSimpleBoxWeapon( pWeapon );
		if( pSimple )
		{
			pSimple->SetOwnerAttach( true );
		}
	}
}