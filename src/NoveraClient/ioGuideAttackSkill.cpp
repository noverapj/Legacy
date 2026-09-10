

#include "stdafx.h"

#include "ioGuideAttackSkill.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

ioGuideAttackSkill::ioGuideAttackSkill()
{
	ClearData();
}

ioGuideAttackSkill::ioGuideAttackSkill( const ioGuideAttackSkill &rhs )
: ioAttackSkill( rhs ),
  m_fTargetAngle( rhs.m_fTargetAngle ),
  m_fTargetRange( rhs.m_fTargetRange ),
  m_TargetWoundType( rhs.m_TargetWoundType ),
  m_bDisableColGrap( rhs.m_bDisableColGrap ),
  m_vWeaponToAirList( rhs.m_vWeaponToAirList ),
  m_vEndAttackInfoList( rhs.m_vEndAttackInfoList ),
  m_fMoveSpeed( rhs.m_fMoveSpeed ),
  m_fTargetHeightOffset( rhs.m_fTargetHeightOffset ),
  m_fTargetOffset( rhs.m_fTargetOffset ),
  m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
  m_fSkillLoopAniRate( rhs.m_fSkillLoopAniRate ),
  m_EndAttackMotion( rhs.m_EndAttackMotion ),
  m_fEndAniRate( rhs.m_fEndAniRate )
{
	ClearData();
}

ioGuideAttackSkill::~ioGuideAttackSkill()
{
}

void ioGuideAttackSkill::ClearData()
{
	m_bSetHandMesh = false;
	m_TargetName.Clear();
	m_SkillState = SS_NONE;
	m_SearchTargetPosition = STP_NONE;
	m_fMaxMoveAmt = 0.0f;
	m_fCurMoveAmt = 0.0f;
	m_vMoveDir = ioMath::VEC3_ZERO;
}

void ioGuideAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	m_bDisableColGrap = rkLoader.LoadBool_e( "zone_collision_disable_grap", false );

	_ENCSTR("air_attack", szEncAirAttack);
	_ENCSTR("end_attack", szEncEndAttack);
	LoadWeaponInfo( szEncAirAttack, m_vWeaponToAirList, rkLoader );
	LoadWeaponInfo( szEncEndAttack, m_vEndAttackInfoList, rkLoader );

	m_fMoveSpeed = rkLoader.LoadFloat_e( "move_speed", 0.0f );

	m_fTargetHeightOffset = rkLoader.LoadFloat_e( "target_height_offset", 0.0f );
	m_fTargetOffset = rkLoader.LoadFloat_e( "target_offset", 0.0f );

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;
	m_fSkillLoopAniRate = rkLoader.LoadFloat_e( "skill_loop_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "end_attack_motion", "", szBuf, MAX_PATH );
	m_EndAttackMotion = szBuf;
	m_fEndAniRate = rkLoader.LoadFloat_e( "end_attack_ani_rate", FLOAT1 );
}

void ioGuideAttackSkill::LoadWeaponInfo( const char *szAttack, WeaponInfoList &WeaponInfoList, ioINILoader &rkLoader )
{
	char szKey[MAX_PATH], szBuf[MAX_PATH];

	wsprintf_e( szKey, "%s_type_count", szAttack );
	int iCnt = rkLoader.LoadInt( szKey, 0 );
	WeaponInfoList.reserve( iCnt );
	for( int i=0; i<iCnt; ++i )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "%s_type%d", szAttack, i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "%s_type%d_resist", szAttack, i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "%s_type%d_wounded", szAttack, i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "%s_type%d_loop_ani", szAttack, i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		wsprintf_e( szKey, "%s_type%d_wounded_time", szAttack, i+1 );
		kInfo.m_dwWoundedDuration = (DWORD)rkLoader.LoadInt( szKey, 0 );

		WeaponInfoList.push_back( kInfo );
	}
}

ioSkill* ioGuideAttackSkill::Clone()
{
	return new ioGuideAttackSkill( *this );
}

bool ioGuideAttackSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	ClearData();

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	FindTarget( pChar );
	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	return true;
}

void ioGuideAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		if( m_dwPreDelayStartTime + GetPreDelayTime() < FRAMEGETTIME() )
		{
			m_dwPreDelayStartTime = 0;

			AttackSkillFire( pChar );
			EndPreDelayEffect( pChar );
			CheckExtraAniJump( pChar );
			CheckAniRotate( pChar );
		}
		break;
	case SS_ACTION:
		break;
	case SS_MOVE:
		{
			if( m_fCurMoveAmt >= m_fMaxMoveAmt )
			{
				SetSkillEndState(pChar);
				return;
			}

			float fTimePerSec = g_FrameTimer.GetSecPerFrame();
			float fMoveDist = m_fMoveSpeed * fTimePerSec;

			if( m_vMoveDir == ioMath::VEC3_ZERO )
			{
				m_vMoveDir = pChar->GetMoveDir();
			}

			D3DXVECTOR3 vPos = pChar->GetWorldPosition();
			vPos += m_vMoveDir*fMoveDist;
			m_fCurMoveAmt += fMoveDist;

			pChar->SetWorldPosition( vPos );
		}
		break;
	case SS_END:
		break;
	}
}

void ioGuideAttackSkill::AttackSkillFire( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	m_SkillState = SS_ACTION;

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	WeaponInfoList vWeaponInfoList;
	vWeaponInfoList.clear();
	if( m_SearchTargetPosition == STP_STAND || m_SearchTargetPosition == STP_NONE )
		vWeaponInfoList = GetAttribute().m_vWeaponInfoList;
	else if( m_SearchTargetPosition == STP_AIR )
		vWeaponInfoList = m_vWeaponToAirList;
	else
		return;

	if( m_bRefreshFireTimeList )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
											iAniID,
											vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fTimeRate,
											dwPreDelay );
	}

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pChar->AttachEffect( m_SkillEffectName );

	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_dwMotionStartTime = m_dwMotionEndTime = m_dwFireMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;
	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	SetSkillActionState( pChar );
}

bool ioGuideAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_ACTION:
	case SS_MOVE:
		return false;
	case SS_END:
		if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
			return false;
	}

	return true;
}

bool ioGuideAttackSkill::IsAttackEndState() const
{
	if( m_SkillState == SS_END && m_dwAttackEndTime > 0 && m_dwAttackEndTime < FRAMEGETTIME() )
	{
		return true;
	}

	return false;
}

void ioGuideAttackSkill::FindTarget( ioBaseChar *pOwner )
{
	if( !pOwner )	return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )	return;

	// target
	m_TargetName.Clear();

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	float fPreDistSq = m_fTargetRange * m_fTargetRange;
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pCurChar = *iter;
		if( !pCurChar )	continue;

		if( pCurChar == pOwner )
			continue;

		if( pCurChar->GetTeam() == pOwner->GetTeam() )
			continue;

		if( !IsEnableTargetState( pCurChar ) )
			continue;

		D3DXVECTOR3 vDiff = pCurChar->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		float fLengthSq = D3DXVec3LengthSq( &vDiff );

		if( fLengthSq <= fPreDistSq )
		{
			D3DXVec3Normalize( &vDiff, &vDiff );
			float fValue = D3DXVec3Dot( &vDir, &vDiff );
			float fAngleGap = RADtoDEG( acosf(fValue) );
			if( fAngleGap < m_fTargetAngle * FLOAT05 )
			{
				m_TargetName = pCurChar->GetCharName();
				fPreDistSq = fLengthSq;
			}
		}
	}

	CheckTargetState( m_TargetName );

	if( !m_TargetName.IsEmpty() )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_TargetName );
		if( pTarget )
		{
			D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
			D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
			D3DXVECTOR3 vDir = vTargetPos - vOwnerPos;

			if( vDir != ioMath::VEC3_ZERO )	
			{
				vDir.y = 0.0f;
				D3DXVec3Normalize( &vDir, &vDir );
				D3DXQUATERNION qtRot;
				ioMath::CalcDirectionQuaternion( qtRot, -vDir );
				pOwner->SetTargetRotToRotate( qtRot, true, false );
			}
		}
	}
}

void ioGuideAttackSkill::CheckTargetState( ioHashString &szTargetName )
{
	if( szTargetName.IsEmpty() )	return;

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( !pStage )	return;

	ioBaseChar *pTarget = pStage->GetBaseChar( szTargetName );
	if( !pTarget )	return;

	if( pTarget->IsFloating() || pTarget->IsFloatingState() )
	{
		m_SearchTargetPosition = STP_AIR;
	}
	else
	{
		m_SearchTargetPosition = STP_STAND;
	}
}

bool ioGuideAttackSkill::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;
	
	if( pTarget->IsSystemState() )
		return false;
		
	if( !pTarget->IsEnableTarget() ) 
		return false;

	if( m_TargetWoundType == TWT_AIR_WOUND_EXCEPT ) // 쓰러짐중 공중상태는 패스
	{
		if( pTarget->IsApplyDownState() && !pTarget->IsApplyDownState(false) )
			return false;
	}
	else if( m_TargetWoundType == TWT_DOWN_WOUND_EXCEPT ) // 쓰러짐중 바닥상태는 패스
	{
		if( pTarget->IsApplyDownState(false) )
			return false;
	}
	else if( m_TargetWoundType == TWT_ALL_WOUND_EXCEPT )
	{
		if( m_bDisableColGrap && pTarget->IsApplyDownState() ||
			( pTarget->GetState() == CS_GRAPPLING_WOUNDED ||
			pTarget->GetState() == CS_GRAPPLING_PUSHED ||
			pTarget->GetState() == CS_GRAPPLING_WOUNDED_SKILL ||
			pTarget->GetState() == CS_GRAPPLING_WOUNDED_SWING ) )
		{
			return false;
		}
		else if( pTarget->IsApplyDownState() )
		{
			return false;
		}

	}
	else if( m_TargetWoundType == TWT_FLOATING_STATE )	// 공중에 뜬 상태가 아니면 패스
	{
		if( !pTarget->IsFloating() )
			return false;
	}
	else if( m_TargetWoundType == TWT_DOWN_WOUNDED ) //바닥에 쓰러져 있지 않거나 공중에 뜬 상태면 패스
	{
		if( !pTarget->IsApplyDownState() || pTarget->IsFloating() )
			return false;
	}

	return true;
}

void ioGuideAttackSkill::SetSkillActionState( ioBaseChar *pChar )
{
	if( !pChar )	return;

	m_SkillState = SS_ACTION;

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioGuideAttackSkill::SetSkillMoveState( ioBaseChar *pChar )
{
	if( !pChar )	return;

	m_SkillState = SS_MOVE;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )	return;
	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );
	if( iAniID == -1 )	return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fSkillLoopAniRate );
}

void ioGuideAttackSkill::SetSkillEndState( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_SkillState == SS_END )	return;

	m_SkillState = SS_END;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )	return;
	int iAniID = pGrp->GetAnimationIdx( m_EndAttackMotion );
	if( iAniID == -1 )	return;
	
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fEndAniRate );

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex+1,
										iAniID,
										m_vEndAttackInfoList,
										FTT_SKILL_ATTACK,
										m_fEndAniRate,
										0 );

	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime(iAniID);

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * m_fEndAniRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = FRAMEGETTIME() + fAttackEnd;
	else
		m_dwAttackEndTime = 0;
}

void ioGuideAttackSkill::OnSkillEnd( ioBaseChar *pChar )
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

	ClearData();
}

bool ioGuideAttackSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioGuideAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	SetChargingRateForWeapon( pWeapon );

	ioZoneEffectWeapon *pZoneEffect = ToZoneEffectWeapon( pWeapon );
	if( pZoneEffect )
	{
		if( pZoneEffect->GetOwner() )
		{
			D3DXVECTOR3 vDir = pZoneEffect->GetOwner()->GetMoveDir();
			pZoneEffect->SetCollisionDir( vDir );
			pZoneEffect->SetZoneValue( true, true );
		}
	}

	ioGuide2Weapon *pGuide2 = ToGuide2Weapon( pWeapon );
	if( pGuide2 )
	{
		D3DXVECTOR3 vDir = pGuide2->GetOwner()->GetMoveDir();
		pGuide2->SetMoveDir( vDir );
		pGuide2->SetTraceTarget( m_TargetName );
	}
}

void ioGuideAttackSkill::SetCreateWeaponDependency( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	SetSkillMoveState(pOwner);
}

void ioGuideAttackSkill::SetInfoByWeapon( ioBaseChar *pOwner, D3DXVECTOR3 vPos )
{
	if( !pOwner )	return;

	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vTargetPos = vPos;
	vTargetPos.y += m_fTargetHeightOffset;
	D3DXVECTOR3 vDiff = vTargetPos - vOwnerPos;
	m_fMaxMoveAmt = D3DXVec3Length( &vDiff );
	m_fMaxMoveAmt -= m_fTargetOffset;
	m_fMaxMoveAmt = max( 0.0f, m_fMaxMoveAmt );

	D3DXVec3Normalize( & vDiff, &vDiff );
	m_vMoveDir = vDiff;

	vDiff.y = 0.0f;
	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDiff );
	pOwner->SetTargetRot( qtRot );
}