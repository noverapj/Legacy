#include "StdAfx.h"
#include "ioTargetAimingAttackSkill.h"
#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"


ioTargetAimingAttackSkill::ioTargetAimingAttackSkill()
{
	InitData();
}

ioTargetAimingAttackSkill::ioTargetAimingAttackSkill( const ioTargetAimingAttackSkill& rhs )
	: ioAttackSkill( rhs ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_szAimedMarker( rhs.m_szAimedMarker ),
	m_fAimRangeMin( rhs.m_fAimRangeMin ),
	m_fAimRangeMax( rhs.m_fAimRangeMax ),
	m_fAimHeightMin( rhs.m_fAimHeightMin ),
	m_fAimHeightMax( rhs.m_fAimHeightMax ),
	m_dwAimedTime( rhs.m_dwAimedTime ),
	m_szAimLoopAni( rhs.m_szAimLoopAni ),
	m_fAimLoopAniRate( rhs.m_fAimLoopAniRate ),
	m_NormalAimedAttack( rhs.m_NormalAimedAttack ),
	m_bEnableDownAim( rhs.m_bEnableDownAim ),
	m_dwLoopTime( rhs.m_dwLoopTime ),
	m_dwAttackTime( rhs.m_dwAttackTime )
{
	InitData();
}

ioTargetAimingAttackSkill::~ioTargetAimingAttackSkill()
{
}

void ioTargetAimingAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	//스킬 무적 시간
	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	rkLoader.LoadString_e( "aimed_marker", "", szBuf, MAX_PATH );
	m_szAimedMarker = szBuf;
	m_dwAimedTime = rkLoader.LoadInt_e( "aimed_time", 0 );

	m_fAimRangeMin = rkLoader.LoadFloat_e( "aim_range_min", 0.0f );
	m_fAimRangeMax = rkLoader.LoadFloat_e( "aim_range_max", 0.0f );

	m_fAimHeightMin = rkLoader.LoadFloat_e( "aim_height_min", 0.0f );
	m_fAimHeightMax = rkLoader.LoadFloat_e( "aim_height_max", 0.0f );

	rkLoader.LoadString_e( "aim_loop_ani", "", szBuf, MAX_PATH );
	m_szAimLoopAni = szBuf;

	m_fAimLoopAniRate = rkLoader.LoadFloat_e( "aim_loop_ani_rate", FLOAT1 );

	LoadAttackAttribute( "normal_aimed_attack", m_NormalAimedAttack, rkLoader );

	m_bEnableDownAim = rkLoader.LoadBool_e( "enable_down_aim", false );

	m_dwLoopTime = rkLoader.LoadInt_e( "loop_time", 0 );

	m_dwAttackTime = rkLoader.LoadInt_e( "attack_time", 0 );
}

ioSkill* ioTargetAimingAttackSkill::Clone()
{
	return new ioTargetAimingAttackSkill( *this );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioTargetAimingAttackSkill::OnSkillStart( ioBaseChar *pChar )
{	
	if( m_SkillState == SS_AIM && m_szAimedTarget.IsEmpty() )
		return false;

	InitData();

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = dwCurTime + m_dwSkillProtectTime;

	if ( m_dwLoopTime > 0)
		m_dwCheckLoopTime = dwCurTime + m_dwLoopTime;

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	SetCameraBuff( pChar );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}	

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge( pChar->GetCharName() ) );
		m_bReduceGauge = true;
	}

	return true;
}

void ioTargetAimingAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_dwCheckLoopTime > 0 && m_dwCheckLoopTime < dwCurTime )
	{
		SetEndState( pChar );
		return;
	}

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				SetAimState( pChar );
			}
		}
		break;
	case SS_AIM:
		{
			GetTargetList( pChar, pStage );
			SetTargetRotation( pChar );
			UpdateMaker( pChar, pStage );

			if ( CheckInputChangeTarget( pChar ) )
			{
				DestroyEffect( pChar, pStage, m_dwMakerID );
				return;
			}

			if ( CheckInputAttack( pChar ) )
			{
				if( !m_szAimedTarget.IsEmpty() )
					SetAttackState( pChar, SS_AIM_ATTACK );
				else
					SetEndState( pChar );
				return;
			}
		}
		break;
	case SS_AIM_ATTACK:
		{
			if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
			{
				SetEndState( pChar );
				return;
			}
		}
		break;
	}
}

void ioTargetAimingAttackSkill::OnSkillEnd( ioBaseChar *pChar )
{
	pChar->ClearReservedSliding();

	ioPlayStage *pStage = pChar->GetCreator();
	if ( pStage )
	{
		DestroyEffect( pChar, pStage, m_dwMakerID );
	}

	ioAttackSkill::OnSkillEnd( pChar );

	EndCameraBuff( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioTargetAimingAttackSkill::IsProtected( int iDefenseBreakType ) const
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

	if( m_dwFireMotionEndTime > 0 && m_dwFireMotionEndTime > dwCurTime )
		return true;

	return false;
}

bool ioTargetAimingAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{	
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_AIM:
		return false;
	case SS_END:
		if( m_fSkillEndNextJumpAmt == 0.0f )
			return true;
		else
			return false;
	}

	if( m_dwMotionEndTime + dwActionStop >= FRAMEGETTIME() )
		return false;

	return true;
}

bool ioTargetAimingAttackSkill::IsAttackEndState() const
{	
	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioTargetAimingAttackSkill::IsNoDropState() const
{	
	switch( m_SkillState )
	{
	case SS_AIM_ATTACK:
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void ioTargetAimingAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );	

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{				
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, true );
		}		
	}
}

void ioTargetAimingAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage ) 
		return;

	int nState;
	rkPacket >> nState;

	switch ( nState )
	{		
	case SS_TARGET:
		{
			rkPacket >> m_szAimedTarget;
		}
		break;
	case SS_AIM_MARK:
		{
			DestroyEffect( pChar, pStage, m_dwMakerID );

			D3DXVECTOR3 vPos = pChar->GetMidPositionByRate() + m_vAimDir * m_fAimRangeMax;
			if ( !m_szAimedTarget.IsEmpty() )
			{
				ioBaseChar *pTarget = pStage->GetBaseChar( m_szAimedTarget );
				if ( pTarget )
					vPos = pTarget->GetMidPositionByRate();
			}

			m_dwMakerID = CreateEffect( pChar, pStage, m_szAimedMarker, vPos, D3DXVECTOR3(1.0f,1.0f,1.0f) );		
		}
		break;
	case SS_AIM_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pChar->SetTargetRotAndMoveDirChange( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pChar->SetWorldPosition( vPos );

			bool bFloating;
			rkPacket >> bFloating;

			m_CurAttack = m_NormalAimedAttack;

			SetAttackState( pChar, (SkillState)nState );
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void ioTargetAimingAttackSkill::InitData()
{
	m_SkillState = SS_NONE;	
	m_bSetHandMesh = false;
	m_dwCurSkillProtectTime = 0;
	m_dwCheckLoopTime = 0;

	m_szAimedTarget.Clear();
	m_dwMakerID = -1;
	m_dwCheckAimTime = 0;

	m_bChangeTargetPrev = false;
	m_bChangeTargetNext = false;

	m_vAimDir = ioMath::VEC3_ZERO;

	m_dwCheckAttackTime = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD ioTargetAimingAttackSkill::CreateEffect( ioBaseChar *pChar, ioPlayStage *pStage, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale )
{
	if ( !pChar || !pStage || szEffect.IsEmpty() )
		return -1;

	ioMapEffect *pMaker = pStage->CreateMapEffect( szEffect, vPos, vScale );
	if ( pMaker )
		return pMaker->GetUniqueID();

	return -1;
}

void ioTargetAimingAttackSkill::DestroyEffect( ioBaseChar *pChar, ioPlayStage *pStage, DWORD& dwEffectID )
{
	if ( !pChar || !pStage || dwEffectID == -1 )
		return;

	pStage->DestroyMapEffect( dwEffectID );
	dwEffectID = -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void ioTargetAimingAttackSkill::GetTargetList( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pChar->IsNeedProcess() || !pStage )
		return;

	m_vTargetInfoList.clear();

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();

	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( !pTarget ) 
			continue;

		if( pChar->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pChar->GetTeam() == pTarget->GetTeam() )	
			continue;

		if( !m_bEnableDownAim && pTarget->IsApplyDownState() )
			continue;

		if( pTarget->IsSystemState() )
			continue;

		if( !pTarget->IsEnableTarget() )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
		float fDiffHeight = pTarget->GetWorldPosition().y - pChar->GetWorldPosition().y;
		if( D3DXVec3LengthSq( &vDiff ) < m_fAimRangeMax * m_fAimRangeMax && D3DXVec3LengthSq( &vDiff ) > m_fAimRangeMin * m_fAimRangeMin 
			&& fDiffHeight > m_fAimHeightMin && fDiffHeight < m_fAimHeightMax )
		{
			BaseCharHP kInfo;
			kInfo.m_Name = pTarget->GetCharName();
			kInfo.m_fHP = pTarget->GetHP().m_fCurValue;
			kInfo.m_bPrisoner = pTarget->IsPrisonerMode();

			m_vTargetInfoList.push_back(kInfo);
		}
	}

	std::sort( m_vTargetInfoList.begin(), m_vTargetInfoList.end(), BaseCharHPSort() );	

	bool bSendPacket = false;
	if ( m_szAimedTarget.IsEmpty() )
	{
		if( !m_vTargetInfoList.empty() )
		{
			m_szAimedTarget = m_vTargetInfoList[0].m_Name;
			bSendPacket = true;
		}		
	}
	else if ( !m_vTargetInfoList.empty() )
	{
		int nCurTarget = -1;

		int nCnt = m_vTargetInfoList.size();
		for( int i=0; i<nCnt; ++i )
		{
			if( m_szAimedTarget == m_vTargetInfoList[i].m_Name )
				nCurTarget = i;
		}

		if ( nCurTarget != -1 )
		{
			if( m_bChangeTargetPrev )
			{
				nCurTarget--;
				m_bChangeTargetPrev = false;

				if( nCurTarget < 0 )
					nCurTarget = nCnt-1;
			}
			else if( m_bChangeTargetNext )
			{
				nCurTarget++;
				m_bChangeTargetNext = false;

				if( nCurTarget >= nCnt )
					nCurTarget = 0;
			}

			if( m_szAimedTarget != m_vTargetInfoList[nCurTarget].m_Name )
			{
				m_szAimedTarget = m_vTargetInfoList[nCurTarget].m_Name;
				bSendPacket = true;
			}
		}
		else if ( !m_szAimedTarget.IsEmpty() )
		{
			m_szAimedTarget.Clear();
			bSendPacket = true;
			m_bChangeTargetPrev = false;
			m_bChangeTargetNext = false;
		}

	}
	else
	{
		if ( !m_szAimedTarget.IsEmpty() )
		{
			m_szAimedTarget.Clear();
			bSendPacket = true;
			m_bChangeTargetPrev = false;
			m_bChangeTargetNext = false;
		}
	}

	if ( bSendPacket && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_TARGET;
		kPacket << m_szAimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetAimingAttackSkill::SetTargetRotation( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_szAimedTarget );
	D3DXVECTOR3 vAimedDir;
	if( pTarget )
	{
		vAimedDir = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();
		pChar->SetTargetRotToDir( vAimedDir, true );		
	}
	else
		vAimedDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	m_vAimDir = vAimedDir;
}

void ioTargetAimingAttackSkill::SetAimedMaker( ioBaseChar *pChar, ioPlayStage *pStage, D3DXVECTOR3 vPos )
{
	if ( !pChar || !pChar->IsNeedProcess() || !pStage )
		return;

	bool bSendNetwork = false;

	DWORD dwCurTime = FRAMEGETTIME();		
	if ( m_dwMakerID != -1 )
	{
		if ( m_dwCheckAimTime > 0 && m_dwCheckAimTime < dwCurTime )
		{
			bSendNetwork = true;
			DestroyEffect( pChar, pStage, m_dwMakerID );

			m_dwMakerID = CreateEffect( pChar, pStage, m_szAimedMarker, vPos, D3DXVECTOR3(1.0f,1.0f,1.0f) );
			m_dwCheckAimTime = m_dwAimedTime + dwCurTime;
		}
	}
	else if ( m_dwMakerID == -1 )
	{
		bSendNetwork = true;
		m_dwMakerID = CreateEffect( pChar, pStage, m_szAimedMarker, vPos, D3DXVECTOR3(1.0f,1.0f,1.0f) );
		m_dwCheckAimTime = m_dwAimedTime + dwCurTime;
	}

	if ( bSendNetwork && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_AIM_MARK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetAimingAttackSkill::UpdateMaker( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate() + m_vAimDir * m_fAimRangeMax;

	if ( !m_szAimedTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_szAimedTarget );
		if ( pTarget )
			vPos = pTarget->GetMidPositionByRate();
	}

	SetAimedMaker( pChar, pStage, vPos );

	ioMapEffect *pEffect = pStage->FindMapEffect( m_dwMakerID );
	if ( pEffect )		
		pEffect->SetWorldPosition( vPos );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void ioTargetAimingAttackSkill::SetAimState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( m_szAimLoopAni );
	if( nAniID == -1 )	
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( nAniID, FLOAT100, FLOAT1, FLOAT1/m_fAimLoopAniRate );

	m_SkillState = SS_AIM;

	m_dwCheckAttackTime = FRAMEGETTIME();
}

void ioTargetAimingAttackSkill::SetAttackState( ioBaseChar *pChar, SkillState eState )
{
	if ( !pChar )
		return;

	bool bFloating = false;
	if ( pChar->IsNeedProcess() )
	{
		m_CurAttack = m_NormalAimedAttack;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( m_CurAttack.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = m_CurAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_CurAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pChar->ClearReservedSliding();
	pChar->SetReservedSliding( m_CurAttack.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );
	pChar->CheckCharColSkipSkillTime( nAniID, fTimeRate, dwPreDelay );

	D3DXVECTOR3	vPos = ioMath::VEC3_ZERO;
	ioBaseChar *pTarget = pChar->GetBaseChar( m_szAimedTarget );
	if( pTarget )
		vPos = pTarget->GetWorldPosition();
	pChar->RefreshFireTimeList( nAniID, m_CurAttack.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay, false, vPos );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;	

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( nAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableKeyReserve = dwCurTime + pGrp->GetAnimationEventTime_e( nAniID, "key_reserve" ) * fTimeRate;

	m_SkillState = eState;

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << eState;
		kPacket << pChar->GetWorldOrientation();
		kPacket << pChar->GetWorldPosition();
		kPacket << bFloating;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetAimingAttackSkill::SetEndState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	m_SkillState = SS_END;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioTargetAimingAttackSkill::CheckInputChangeTarget( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return false;

	if ( pChar->IsDefenseKey() )
	{
		m_bChangeTargetNext = true;
		return true;
	}

	if ( pChar->IsJumpKey() )
	{
		m_bChangeTargetPrev = true;
		return true;
	}

	return false;
}

bool ioTargetAimingAttackSkill::CheckInputAttack( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return false;

	if ( m_dwCheckAttackTime + m_dwAttackTime > FRAMEGETTIME() )
		return false;

	if ( pChar->IsAttackKey() )
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioTargetAimingAttackSkill::CheckExceptionCamera()
{
	if( !m_szAimedTarget.IsEmpty() )
		return true;

	return false;
}

D3DXVECTOR3 ioTargetAimingAttackSkill::GetExceptionCameraPos( ioBaseChar *pChar )
{
	if( !pChar )
		return ioMath::VEC3_ZERO;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_szAimedTarget );
	if( pTarget )
		return pTarget->GetWorldPosition();

	return pChar->GetWorldPosition();
}