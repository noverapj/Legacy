#include "StdAfx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioRangeTargetShotSkill.h"

ioRangeTargetShotSkill::ioRangeTargetShotSkill(void)
{
	ClearData();
}

ioRangeTargetShotSkill::ioRangeTargetShotSkill( const ioRangeTargetShotSkill &rhs )
	: ioNormalSkill( rhs ),
	m_LoopDelayAnimation( rhs.m_LoopDelayAnimation ),
	m_LoopMoveAnimation( rhs.m_LoopMoveAnimation ),
	m_szRedAimMarker( rhs.m_szRedAimMarker ),
	m_szBlueAimMarker( rhs.m_szBlueAimMarker ),
	m_TargetWoundType( rhs.m_TargetWoundType ),
	m_fNTargetAngle( rhs.m_fNTargetAngle ),
	m_fNTargetRange( rhs.m_fNTargetRange ),
	m_fNTargetMinRange( rhs.m_fNTargetMinRange ),
	m_fFTargetAngle( rhs.m_fFTargetAngle ),
	m_fFTargetRange( rhs.m_fFTargetRange ),
	m_fFTargetMinRange( rhs.m_fFTargetMinRange ),
	m_NearAimAttribute( rhs.m_NearAimAttribute ),
	m_FarAimAttribute( rhs.m_FarAimAttribute ),
	m_dwSkillDuration( rhs.m_dwSkillDuration ),
	m_NearUpAnimation( rhs.m_NearUpAnimation ),
	m_NearDownAnimation( rhs.m_NearDownAnimation ),
	m_FarUpAnimation( rhs.m_FarUpAnimation ),
	m_FarDownAnimation( rhs.m_FarDownAnimation ),
	m_bEnableDownAim( rhs.m_bEnableDownAim ),
	m_fDownTargetAimRate( rhs.m_fDownTargetAimRate )
{
	ClearData();
}


ioRangeTargetShotSkill::~ioRangeTargetShotSkill(void)
{
}

void ioRangeTargetShotSkill::ClearData()
{
	m_szTargetName.Clear();
	m_szNearTargetName.Clear();
	m_szFarTargetName.Clear();
	m_szPreTargetName.c_str();

	m_SkillState = SS_PRE;
	m_bSetHandMesh = false;
	m_bSetMoveAni = false;
	
	//maker
	m_dwAimMarkerID = -1;
	
	m_bFindNear = true;
	m_bFindFar = true;

	m_dwSkillStartTime = 0;
}

void ioRangeTargetShotSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );
	char szBuf[MAX_PATH];

	wsprintf_e( szBuf, "near_aim_attack");
	LoadAttackAttribute( szBuf, m_NearAimAttribute, rkLoader );
	rkLoader.LoadString_e( "near_aim_attack_up", "", szBuf, MAX_PATH );	
	m_NearUpAnimation = szBuf;
	rkLoader.LoadString_e( "near_aim_attack_down", "", szBuf, MAX_PATH );
	m_NearDownAnimation	= szBuf;

	wsprintf_e( szBuf, "far_aim_attack");
	LoadAttackAttribute( szBuf, m_FarAimAttribute, rkLoader );
	rkLoader.LoadString_e( "far_aim_attack_up", "", szBuf, MAX_PATH );	
	m_FarUpAnimation = szBuf;
	rkLoader.LoadString_e( "far_aim_attack_down", "", szBuf, MAX_PATH );
	m_FarDownAnimation	= szBuf;

	//loop motion
	rkLoader.LoadString_e( "delay_animation", "", szBuf, MAX_PATH );	
	m_LoopDelayAnimation = szBuf;
	rkLoader.LoadString_e( "move_animation", "", szBuf, MAX_PATH );
	m_LoopMoveAnimation	= szBuf;

	// aim mark
	rkLoader.LoadString_e( "red_aim_marker", "", szBuf, MAX_PATH );
	m_szRedAimMarker = szBuf;
	rkLoader.LoadString_e( "blue_aim_marker", "", szBuf, MAX_PATH );
	m_szBlueAimMarker = szBuf;

	// 타겟 검색 범위
	m_TargetWoundType	= (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	m_fNTargetAngle		= rkLoader.LoadFloat_e( "near_target_angle", 0.0f );
	m_fNTargetRange		= rkLoader.LoadFloat_e( "near_target_max_range", 0.0f );
	m_fNTargetMinRange	= rkLoader.LoadFloat_e( "near_target_min_range", 0.0f );

	m_fFTargetAngle		= rkLoader.LoadFloat_e( "far_target_angle", 0.0f );
	m_fFTargetRange		= rkLoader.LoadFloat_e( "far_target_max_range", 0.0f );
	m_fFTargetMinRange	= rkLoader.LoadFloat_e( "far_target_min_range", 0.0f );

	m_dwSkillDuration = (DWORD)rkLoader.LoadInt_e( "skill_duration", 0 );

	m_bEnableDownAim = rkLoader.LoadBool_e( "enable_down_aim", false );
	m_fDownTargetAimRate = rkLoader.LoadFloat_e( "down_target_aim_rate", 0.0f );
}

ioSkill* ioRangeTargetShotSkill::Clone()
{
	return new ioRangeTargetShotSkill( *this );
}

bool ioRangeTargetShotSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	ioNormalSkill::OnSkillStart( pChar );

	ClearData();

	pChar->AttachEffect( m_SkillEffectName );
	m_dwSkillStartTime = FRAMEGETTIME();

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
	
	pChar->SetSkillMoveEnable( false, false );

	SetCameraBuff( pChar );
	
	if( !m_bReduceGauge )
	{
		float fCurGauge = pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	ioPlayStage* pStage = g_SkillMaker.GetPlayStage();
	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{	
		SetLoopState( pChar, pStage );
	}	

	return true;
}

void ioRangeTargetShotSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );
	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	EndCameraBuff( pChar );

	pChar->SetSkillMoveEnable( false, false );

	if( !m_SkillSoundName.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_SkillSoundName, m_SkillSoundID );		
	}

	DestroyAimMarker( pChar );
	ClearData();
}

void ioRangeTargetShotSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
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
			if ( m_dwSkillStartTime > 0 && m_dwSkillStartTime + m_dwSkillDuration < dwCurTime )
			{
				SetEndState( pChar );
			}
			FindNearTarget( pChar, pStage );
			FindFarTarget( pChar, pStage );
			int iTarget = CheckTarget( pChar, pStage );
			CheckNormalMoveAni( pChar );
			UpdateAimMarker( pChar, pStage );
			if( pChar->IsAttackKey() && pChar->IsNeedProcess() )
			{
				if ( iTarget == TA_NEAR )
				{
					CheckAttackDir( pChar, pStage );
					SetNearAimFireState( pChar, pStage );
				}
				else if ( iTarget == TA_FAR )
				{
					CheckAttackDir( pChar, pStage );
					SetFarAimFireState( pChar, pStage );
				}
				else if ( iTarget == TA_NONE )
				{
					SetNearAimFireState( pChar, pStage );
				}

				if( pChar->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
					kPacket << pChar->GetCharName();
					kPacket << GetName();
					kPacket << iTarget;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
		break;
	case SS_NEAR_AIM_FIRE:
		{
			UpdateAimMarker( pChar, pStage );
			if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
			{
				SetLoopState( pChar, pStage );
				DestroyAimMarker( pChar );
			}
		}
		break;
	case SS_FAR_AIM_FIRE:
		{
			UpdateAimMarker( pChar, pStage );
			if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
			{
				SetLoopState( pChar, pStage );
				DestroyAimMarker( pChar );
			}
		}
		break;
	case SS_END:
		{

		}
		break;
	}
}


void ioRangeTargetShotSkill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_SkillState = SS_LOOP;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx( m_LoopDelayAnimation );
	if( iAniID == -1 )
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100 );
	m_dwMotionEndTime = 0;

	pChar->SetSkillMoveEnable( true, true );

	m_bSetMoveAni = false;
	m_szTargetName.c_str();
}

void ioRangeTargetShotSkill::CheckNormalMoveAni( ioBaseChar* pChar )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = 0;

	if( pChar->IsSettedDirection() )
	{	
		if( !m_bSetMoveAni )
		{	
			iAniID = pGrp->GetAnimationIdx( m_LoopMoveAnimation );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100 );	
			m_bSetMoveAni = true;
		}
	}
	else
	{
		if( m_bSetMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_LoopDelayAnimation );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100 );
			m_bSetMoveAni = false;
		}
	}
}

bool ioRangeTargetShotSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_PRE:
	case SS_LOOP:
		return false;
	case SS_END:
		return true;
	}

	return false;
}

void ioRangeTargetShotSkill::FindNearTarget( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;
	
	ioHashString	szTarget;
	float fNPreLengthSq = m_fNTargetRange * m_fNTargetRange;

	D3DXQUATERNION qtRot = pChar->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	const BaseCharList &rkCharList = pStage->GetBaseCharList();

	auto Iter = rkCharList.begin();
	for ( ; Iter != rkCharList.end(); ++Iter )
	{
		ioBaseChar *pCurChar = *Iter;

		if ( pCurChar == pChar )
			continue;

		if ( pCurChar->GetTeam() == pChar->GetTeam() )
			continue;
		
		if( !IsEnableTargetState( pCurChar, true ) )
			continue;

		D3DXVECTOR3 vDiff = pCurChar->GetMidPositionByRate() - pChar->GetMidPositionByRate();
		float fLengthSq = D3DXVec3LengthSq( &vDiff );

		float fMaxRangeSq = m_fNTargetRange * m_fNTargetRange;
		float fMinRangeSq = m_fNTargetMinRange * m_fNTargetMinRange;
		if( !COMPARE( fLengthSq, fMinRangeSq, fMaxRangeSq ) )
			continue;

		D3DXVec3Normalize( &vDiff, &vDiff );
		float fValue = D3DXVec3Dot( &vDir, &vDiff );
		float fAngleGap = RADtoDEG( acosf(fValue) );
		if( fAngleGap >= m_fNTargetAngle )
			continue;

		if( fLengthSq < fNPreLengthSq )
		{
			fNPreLengthSq = fLengthSq;
			szTarget = pCurChar->GetCharName();
		}
	}

	m_szNearTargetName = szTarget;
}

void ioRangeTargetShotSkill::FindFarTarget( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	ioHashString	szTarget;
	float	fFPreLengthSq = m_fFTargetRange * m_fFTargetRange;

	D3DXQUATERNION qtRot = pChar->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	const BaseCharList &rkCharList = pStage->GetBaseCharList();

	auto Iter = rkCharList.begin();
	for ( ; Iter != rkCharList.end(); ++Iter )
	{
		ioBaseChar *pCurChar = *Iter;

		if ( pCurChar == pChar )
			continue;

		if ( pCurChar->GetTeam() == pChar->GetTeam() )
			continue;

		if( !IsEnableTargetState( pCurChar, true ) )
			continue;

		D3DXVECTOR3 vDiff = pCurChar->GetMidPositionByRate() - pChar->GetMidPositionByRate();
		float fLengthSq = D3DXVec3LengthSq( &vDiff );

		float fMaxRangeSq = m_fFTargetRange * m_fFTargetRange;
		float fMinRangeSq = m_fFTargetMinRange * m_fFTargetMinRange;
		if( !COMPARE( fLengthSq, fMinRangeSq, fMaxRangeSq ) )
			continue;

		D3DXVec3Normalize( &vDiff, &vDiff );
		float fValue = D3DXVec3Dot( &vDir, &vDiff );
		float fAngleGap = RADtoDEG( acosf(fValue) );
		if( fAngleGap >= m_fFTargetAngle )
			continue;

		if( fLengthSq < fFPreLengthSq )
		{
			fFPreLengthSq = fLengthSq;
			szTarget = pCurChar->GetCharName();
		}
	}

	m_szFarTargetName = szTarget;
}

int ioRangeTargetShotSkill::CheckTarget( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( m_szNearTargetName.IsEmpty() && m_szFarTargetName.IsEmpty() )
	{
		m_szTargetName.Clear();
		m_bFindNear = true;
		m_bFindFar = true;
		DestroyAimMarker( pChar );
		return TA_NONE;
	}
	else if ( !m_szNearTargetName.IsEmpty() && !m_szFarTargetName.IsEmpty() )
	{
		if ( m_szPreTargetName == m_szNearTargetName || ( !m_bFindNear && m_bFindFar ) )
		{
			m_szTargetName = m_szFarTargetName;
			m_szFarTargetName.Clear();
			m_szPreTargetName.Clear();
			CreateAimMarker( pChar, pStage );
			return TA_FAR;
		}
		else
		{
			m_szTargetName = m_szNearTargetName;
			m_szNearTargetName.Clear();
			CreateAimMarker( pChar, pStage );
			return TA_NEAR;
		}		
	}
	else if ( !m_szNearTargetName.IsEmpty() && m_szFarTargetName.IsEmpty() )
	{
		m_szTargetName = m_szNearTargetName;
		m_szNearTargetName.Clear();
		m_bFindFar = true;
		CreateAimMarker( pChar, pStage );
		return TA_NEAR;
	}
	else if ( m_szNearTargetName.IsEmpty() && !m_szFarTargetName.IsEmpty() )
	{
		m_szTargetName = m_szFarTargetName;
		m_szFarTargetName.Clear();
		m_bFindNear = true;
		CreateAimMarker( pChar, pStage );
		return TA_FAR;
	}

	return TA_NONE;
}

void ioRangeTargetShotSkill::CreateAimMarker( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar ) return;
	if ( !pStage ) return;

	if( m_szTargetName.IsEmpty() ) return;
	
	if( m_dwAimMarkerID != -1 )	return;

	ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetName );
	if( pTarget )
	{
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
		D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
		vTargetPos.y = pTarget->GetMidHeightByRate();
		ioMapEffect *pMapEffect = NULL;
		if( pChar->IsNeedProcess() ) 
		{
			pMapEffect = pStage->CreateMapEffect( m_szBlueAimMarker, vTargetPos, vScale );
		}
		else
		{
			if( pChar->GetTeam() == TEAM_BLUE )
				pMapEffect = pStage->CreateMapEffect( m_szBlueAimMarker, vTargetPos, vScale );
			else
				pMapEffect = pStage->CreateMapEffect( m_szRedAimMarker, vTargetPos, vScale );
		}

		if( pMapEffect )
		{	
			m_dwAimMarkerID = pMapEffect->GetUniqueID();
		}
	}
}


void ioRangeTargetShotSkill::UpdateAimMarker( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar ) return;
	if ( !pStage ) return;
		
	if( m_szTargetName.IsEmpty() )
	{			
		DestroyAimMarker( pChar );
		return;
	}

	if( m_dwAimMarkerID == -1 && !m_szTargetName.IsEmpty() )
	{
		CreateAimMarker( pChar, pStage );
		return;
	}

	ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetName );
	if ( pTarget )
	{
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
		D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
		vTargetPos.y = pTarget->GetMidHeightByRate();
		
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwAimMarkerID );
		if( pMapEffect )
		{
			pMapEffect->SetWorldPosition( vTargetPos );
		}
	}
}

void ioRangeTargetShotSkill::DestroyAimMarker( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_dwAimMarkerID != -1 )
	{
		pChar->EndMapEffect( m_dwAimMarkerID, false );
		m_dwAimMarkerID = -1;
	}
}

bool ioRangeTargetShotSkill::IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	ioPlayStage *pStage = pTarget->GetCreator();
	if( pStage )
	{
		ioPlayMode *pMode = pStage->GetPlayMode();
		if( pMode && pMode->GetModeType() == MT_FOOTBALL )
			return false;
	}

	if( bCheckTargetWound )
	{
		if( !pTarget->CheckTargetWoundType( m_TargetWoundType ) )
			return false;
	}

	return true;
}

void ioRangeTargetShotSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{	
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
	
	D3DXVECTOR3 vAimedDir = ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
	pWeapon->SetMoveDir( vAimedDir );
}

void ioRangeTargetShotSkill::SetNearAimFireState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar ) return;
	if ( !pStage ) return;

	m_SkillState = SS_NEAR_AIM_FIRE;
	
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_NearAimAttribute.m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fTimeRate  = m_NearAimAttribute.m_fAttackAniRate * m_fExtraAniRate;
	if ( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_NearAimAttribute.m_dwPreDelay;
	
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iAniID,
										m_NearAimAttribute.m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										dwPreDelay );
	pChar->IncreaseWeaponIndexBase();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	
	float fWeight = GetAimedUpDownWeight( m_vAttackDir, 85.0f );
	if( m_vAttackDir.y >= 0.0f )
	{
		AimedFireToUp( pGrp, m_NearAimAttribute.m_AttackAnimation, m_NearUpAnimation, fWeight, fTimeRate, dwPreDelay, true );
	}
	else
	{
		AimedFireToDown( pGrp, m_NearAimAttribute.m_AttackAnimation, m_NearDownAnimation, fWeight, fTimeRate, dwPreDelay, true );
	}

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + m_NearAimAttribute.m_dwEndDelay;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwFireStartTime;
	dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pChar->SetSkillMoveEnable( false, false );
	m_bFindNear = false;
}

void ioRangeTargetShotSkill::SetFarAimFireState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar ) return;
	if ( !pStage ) return;
	
	m_SkillState = SS_FAR_AIM_FIRE;
	
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_FarAimAttribute.m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fTimeRate  = m_FarAimAttribute.m_fAttackAniRate * m_fExtraAniRate;
	if ( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_FarAimAttribute.m_dwPreDelay;

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iAniID,
										m_FarAimAttribute.m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										dwPreDelay );
	pChar->IncreaseWeaponIndexBase();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	
	float fWeight = GetAimedUpDownWeight( m_vAttackDir, 85.0f );
	if( m_vAttackDir.y >= 0.0f )
	{
		AimedFireToUp( pGrp, m_FarAimAttribute.m_AttackAnimation, m_FarUpAnimation, fWeight, fTimeRate, dwPreDelay, true );
	}
	else
	{
		AimedFireToDown( pGrp, m_FarAimAttribute.m_AttackAnimation, m_FarDownAnimation, fWeight, fTimeRate, dwPreDelay, true );
	}
	
	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + m_FarAimAttribute.m_dwEndDelay;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwFireStartTime;
	dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pChar->SetSkillMoveEnable( false, false );
	m_bFindFar = false;
}

void ioRangeTargetShotSkill::SetEndState( ioBaseChar *pChar )
{
	if ( !pChar ) return;
	if ( m_SkillState == SS_END ) return;
		
	m_SkillState = SS_END;
}

void ioRangeTargetShotSkill::CheckAttackDir( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage) return;

	ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetName );
	if ( !pTarget )
	{
		m_vAttackDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		return;
	}	
	m_szPreTargetName = m_szTargetName;
	
	D3DXVECTOR3 vTargetPos;
	if( CheckDownAimState( pTarget ) )
		vTargetPos = pTarget->GetMidPositionByRate( m_fDownTargetAimRate );
	else
		vTargetPos = pTarget->GetMidPositionByRate();

	pChar->SetTargetRotToTargetPos( vTargetPos, true, false );

	m_vAttackDir = vTargetPos - pChar->GetMidPositionByRate();
	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );
}

float ioRangeTargetShotSkill::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
{
	D3DXVECTOR3 vXZDir( vAimDir.x, 0.0f, vAimDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	float fAngle = D3DXVec3Dot( &vXZDir, &vAimDir );
	fAngle = max( -FLOAT1, min( fAngle, FLOAT1 ) );
	fAngle = acosf( fAngle );
	fAngle = RADtoDEG( fAngle );
	fAngle = max( 0.0f, min( fAngle, fMaxAngle ) );

	float fWeight = fAngle / fMaxAngle;

	return fWeight;
}

void ioRangeTargetShotSkill::AimedFireToUp( ioEntityGroup *pGrp,
	ioHashString szCenterAni,
	ioHashString szUpAni,
	float fWeight,
	float fTimeRate,
	DWORD dwPreDelay,
	bool bHold )
{
	float fFadeOut = FLOAT100;
	if( !bHold )
		fFadeOut = 0.0f;

	bool bAniFx1 = false;
	bool bAniFx2 = true;

	if( fWeight > FLOAT05 )
	{
		bAniFx1 = true;
		bAniFx2 = false;
	}

	ioHashString szFireUp, szFireCenter;
	szFireUp = szUpAni;
	szFireCenter = szCenterAni;
	
	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( szFireUp,
			FLOAT100, fFadeOut,
			fWeight, FLOAT1/fTimeRate,
			0.0f, dwPreDelay,
			bHold, bAniFx1 );
	}

	pGrp->SetActionAni( szFireCenter,
		FLOAT100, fFadeOut,
		1.0f - fWeight, FLOAT1/fTimeRate,
		0.0f, dwPreDelay,
		bHold, bAniFx2 );
}

void ioRangeTargetShotSkill::AimedFireToDown( ioEntityGroup *pGrp,
	ioHashString szCenterAni,
	ioHashString szDownAni,
	float fWeight,
	float fTimeRate,
	DWORD dwPreDelay,
	bool bHold )
{
	float fFadeOut = FLOAT100;
	if( !bHold )
		fFadeOut = 0.0f;
	bool bAniFx1 = true;
	bool bAniFx2 = false;

	if( fWeight > FLOAT05 )
	{
		bAniFx1 = false;
		bAniFx2 = true;
	}

	ioHashString szFireDown, szFireCenter;
	szFireDown = szDownAni;
	szFireCenter = szCenterAni;

	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( szFireDown,
			FLOAT100, fFadeOut,
			fWeight, FLOAT1/fTimeRate,
			0.0f, dwPreDelay,
			bHold, bAniFx2 );
	}

	pGrp->SetActionAni( szFireCenter,
		FLOAT100, fFadeOut,
		1.0f - fWeight, FLOAT1/fTimeRate,
		0.0f, dwPreDelay,
		bHold, bAniFx1 );
}

bool ioRangeTargetShotSkill::CheckDownAimState( ioBaseChar *pTarget )
{
	if( !pTarget ) return false;

	CharState eState = pTarget->GetState();

	if( eState == CS_FROZEN && pTarget->GetBlowFrozenState() && m_bEnableDownAim )
		return true;

	if( eState == CS_BLOW_WOUNDED && !pTarget->IsFloatingState() && m_bEnableDownAim )
		return true;

	if( eState == CS_BOUND_BLOW_WOUNDED && !pTarget->IsFloatingState() && m_bEnableDownAim )
		return true;

	if( eState == CS_ETC_STATE && pTarget->IsApplyDownState(false) && m_bEnableDownAim )
		return true;

	if( eState == CS_USING_SKILL && pTarget->IsApplyDownState(false) && m_bEnableDownAim )
		return true;

	if( eState == CS_STOP_MOTION && pTarget->GetBlowStopMotionState() && m_bEnableDownAim )
		return true;

	return false;
}

D3DXVECTOR3 ioRangeTargetShotSkill::ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon )
{
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
	D3DXVECTOR3 vTargetPos;
	D3DXVECTOR3 vWeaponPos = pWeapon->GetPosition();

	if( pTarget )
	{
		if( CheckDownAimState( pTarget ) )
			vTargetPos = pTarget->GetMidPositionByRate( m_fDownTargetAimRate );
		else
			vTargetPos = pTarget->GetMidPositionByRate();
	

		D3DXVECTOR3 vAimedDir =  vTargetPos - vWeaponPos;
		D3DXVec3Normalize( &vAimedDir, &vAimedDir );
		vAimedDir.x = m_vAttackDir.x;
		vAimedDir.z = m_vAttackDir.z;

		D3DXVec3Normalize( &vAimedDir, &vAimedDir );

		m_vAttackDir = vAimedDir;
		
		return vAimedDir;
	}
	else
	{
		D3DXVECTOR3 vAimedDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		return vAimedDir;
	}
}

void ioRangeTargetShotSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	switch( iState )
	{
	case TA_NEAR:
		{
			CheckAttackDir( pChar, pStage );
			SetNearAimFireState( pChar, pStage );
		}
		break;
	case TA_FAR:
		{
			CheckAttackDir( pChar, pStage );
			SetFarAimFireState( pChar, pStage );
		}
		break;
	case TA_NONE:
		{
			SetNearAimFireState( pChar, pStage );
		}
		break;
	}
}


bool ioRangeTargetShotSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	return __super::IsCanUseSkill( pChar, bHelpMsg );
	//return true;
}