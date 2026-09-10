#include "StdAfx.h"
#include "ioGrapplingAttack5Skill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioNpcChar.h"

#include "ioZoneEffectWeapon.h"
#include "ioGuideWeapon.h"

ioGrapplingAttack5Skill::ioGrapplingAttack5Skill()
{
	ClearData();
}

ioGrapplingAttack5Skill::ioGrapplingAttack5Skill( const ioGrapplingAttack5Skill &rhs )
	: ioAttackSkill( rhs ),
	m_dwEnableKeyTime( rhs.m_dwEnableKeyTime ),
	m_dwDelayKeyTime( rhs. m_dwDelayKeyTime ),
	m_iMaxCombo( rhs.m_iMaxCombo ),
	m_GrapplingAttackList( rhs.m_GrapplingAttackList ),
	m_szLoopMotion( rhs.m_szLoopMotion ),
	m_fLoopTimeRate( rhs.m_fLoopTimeRate ),
	m_iTargetCnt( rhs.m_iTargetCnt ),
	m_fTargetAngle( rhs.m_fTargetAngle ),
	m_fTargetRange( rhs.m_fTargetRange ),
	m_GuideEffect( rhs.m_GuideEffect ),
	m_iMaxEffectCount( rhs.m_iMaxEffectCount ),
	m_dwProtectTime( rhs.m_dwProtectTime )
{
	ClearData();
}

ioGrapplingAttack5Skill::~ioGrapplingAttack5Skill()
{
}

void ioGrapplingAttack5Skill::ClearData()
{
	m_SkillState        = SS_NONE;

	m_dwFireStartTime   = 0;
	m_dwCheckKeyTime    = 0;
	m_dwMotionStartTime = 0;

	m_iCurCombo = 0;

	m_GrapplingTargetList.clear();
	m_TargetList.clear();
}

void ioGrapplingAttack5Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	// 기립잡기 타격정보
	m_iMaxCombo = rkLoader.LoadInt_e( "grappling_attack_type_count", 0 );

	m_GrapplingAttackList.clear();
	m_GrapplingAttackList.reserve( m_iMaxCombo );

	for( int i=0 ; i<m_iMaxCombo; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "grappling_attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "grappling_attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "grappling_attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "grappling_attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "grappling_attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_GrapplingAttackList.push_back( kInfo );
	}

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_szLoopMotion = szBuf;
	m_fLoopTimeRate = rkLoader.LoadFloat_e( "skill_loop_time_rate", FLOAT1 );

	// 키 연타.
	m_dwEnableKeyTime = (DWORD)rkLoader.LoadInt_e( "enable_key_time", 0 );
	m_dwDelayKeyTime = (DWORD)rkLoader.LoadInt_e( "delay_key_time", 0 );

	m_iTargetCnt   = rkLoader.LoadInt_e( "target_count", 0 );
	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );

	// 가이드 이펙트
	rkLoader.LoadString_e( "guide_effect", "", szBuf, MAX_PATH );
	m_GuideEffect = szBuf;
	m_iMaxEffectCount = rkLoader.LoadInt_e( "guide_max_effect", 1 );

	m_dwProtectTime = rkLoader.LoadInt_e( "skill_protect_time", 0 );
}

ioSkill* ioGrapplingAttack5Skill::Clone()
{
	return new ioGrapplingAttack5Skill( *this );
}

bool ioGrapplingAttack5Skill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	ClearData();

	m_GrapplingTargetName.Clear();

	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	SetCameraBuff( pChar );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	DestroyArrow();

	return true;
}

void ioGrapplingAttack5Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	ioSkill::OnProcessState( pChar, pStage );

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				EndPreDelayEffect( pChar );
				AttackSkillFire( pChar );
			}
		}
		break;
	case SS_ACTION:
		ProcessAction( pChar );
		break;
	case SS_GRAPPLING:
		{
			if( !CheckTarget( pChar ) )
			{
				SetSkillEndState( pChar );
				return;
			}

			if( pChar->IsNeedProcess() )
			{
				if( m_iCurCombo < m_iMaxCombo )
				{
					if( m_dwCheckKeyTime + m_dwDelayKeyTime <= dwCurTime &&
						COMPARE( dwCurTime, m_dwCheckKeyTime + m_dwDelayKeyTime, m_dwCheckKeyTime + m_dwEnableKeyTime ) &&
						pChar->IsAttackKey() )
					{
						m_dwCheckKeyTime = dwCurTime;
						GrapplingAttackFire( pChar );
					}
					else if( m_dwCheckKeyTime + m_dwEnableKeyTime < dwCurTime )
					{
						SetSkillEndState( pChar, true );
					}
				}
				else
				{
					SetSkillEndState( pChar, true );
				}
			}

			ProcessGuideEffect( pChar );
		}
		break;
	case SS_END:
		break;
	}
}

void ioGrapplingAttack5Skill::ProcessAction( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( GetAttribute().m_vWeaponInfoList.empty() )
		return;

	if( m_dwFireStartTime > 0 && m_dwFireStartTime <= FRAMEGETTIME() )
	{
		int iCnt = m_TargetList.size();
		if( iCnt == 0 )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
			D3DXVECTOR3 vCurPos = pChar->GetMidPositionByRate();
			pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList[0], GetName(), m_dwWeaponBaseIndex, vCurPos, vDir );
			m_dwFireStartTime = 0;

			return;
		}

		for( int i=0; i<iCnt; ++i )
		{
			ioBaseChar *pTarget = pChar->GetBaseChar( m_TargetList[i] );
			if( !pTarget )
				continue;

			D3DXVECTOR3 vDir = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
			D3DXVec3Normalize( &vDir, &vDir );
			m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
			D3DXVECTOR3 vCurPos = pChar->GetMidPositionByRate();
			m_GrapplingTargetName = m_TargetList[i];
			pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList[0], GetName(), m_dwWeaponBaseIndex, vCurPos, vDir );
			m_dwFireStartTime = 0;
		}
	}
}

void ioGrapplingAttack5Skill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_GrapplingTargetName.Clear();

	EndCameraBuff( pChar );

	DestroyArrow();
	ClearData();
}

void ioGrapplingAttack5Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage )	
		return;

	int iState;
	rkPacket >> iState;

	D3DXQUATERNION qtCharRot;
	rkPacket >> qtCharRot;

	pChar->SetTargetRotToRotate( qtCharRot, true );
	switch( iState )
	{
	case SSC_GRAPPLING:
		{
			ioHashString szTargetName;
			rkPacket >> szTargetName;

			ioBaseChar *pTarget = pChar->GetBaseChar( szTargetName );
			AddTargetInfo( pChar, pTarget );

			if( m_SkillState != SS_GRAPPLING )
			{
				m_SkillState = SS_GRAPPLING;

				ioEntityGroup *pGrp = pChar->GetGroup();
				if( pGrp )
				{
					pGrp->ClearAllLoopAni( FLOAT100, true );
					pGrp->ClearAllActionAni( FLOAT100, true );
					pGrp->SetLoopAni( m_szLoopMotion, FLOAT100, FLOAT100, FLOAT1/m_fLoopTimeRate );
				}
			}

			if( pTarget && pTarget->IsNeedProcess() )
				pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
		}
		break;
	case SSC_ATTACK:
		{
			GrapplingAttackFire( pChar );

			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget && pTarget->IsNeedProcess() )
				pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
		}
		break;
	case SSC_SKILL_END:
		SetSkillEndState( pChar );
		break;
	}
}

void ioGrapplingAttack5Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	if( !pWeapon || !pStage )
		return;

	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING && !m_GrapplingTargetName.IsEmpty() )
	{
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	}
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING_ALL && !m_GrapplingTargetName.IsEmpty() )
	{
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	}
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_ATTACK && !m_GrapplingTargetName.IsEmpty() )
	{
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	}

	//
	ioGuideWeapon *pGuideWeapon = ToGuideWeapon( pWeapon );
	if( pGuideWeapon )
	{
		pGuideWeapon->SetTraceTarget( m_GrapplingTargetName );
	}

	ioZoneEffectWeapon *pZoneEffect = ToZoneEffectWeapon( pWeapon );
	if( pZoneEffect )
	{
		ioBaseChar *pChar = pWeapon->GetOwner();
		ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
		if( pChar && pTarget )
		{
			D3DXVECTOR3 vDir = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
			D3DXVec3Normalize( &vDir, &vDir );
			pZoneEffect->SetCollisionDir( vDir );
			pZoneEffect->SetZoneValue( pTarget->GetMidPositionByRate(), true );
		}
	}
}

void ioGrapplingAttack5Skill::FindTarget( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	// target
	m_TargetList.clear();

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	float fPreDistSq = m_fTargetRange * m_fTargetRange;
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	BaseCharDistanceSqList kList;
	kList.clear();
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pCurChar = *iter;
		if( !pCurChar )
			continue;

		if( pCurChar == pOwner )
			continue;

		if( pCurChar->GetTeam() == pOwner->GetTeam() )
			continue;

		if( !IsEnableTargetState( pCurChar ) )
			continue;

		D3DXVECTOR3 vDiff = pCurChar->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		float fDistSq = D3DXVec3LengthSq( &vDiff );

		if( fDistSq <= fPreDistSq )
		{
			D3DXVec3Normalize( &vDiff, &vDiff );
			float fValue = D3DXVec3Dot( &vDir, &vDiff );
			float fAngleGap = RADtoDEG( acosf(fValue) );
			if( fAngleGap < m_fTargetAngle * FLOAT05 )
			{
				BaseCharDistanceSq kCharInfo;
				kCharInfo.m_Name = pCurChar->GetCharName();
				kCharInfo.m_fDistanceSq = fDistSq;
				kCharInfo.m_vPos = pCurChar->GetWorldPosition();
				kList.push_back( kCharInfo );
			}
		}
	}

	std::sort( kList.begin(), kList.end(), BaseCharDistanceSqSort() );

	int iCnt = kList.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( i >= m_iTargetCnt )
			break;

		m_TargetList.push_back( kList[i].m_Name );
	}
}

bool ioGrapplingAttack5Skill::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if ( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	return true;
}

bool ioGrapplingAttack5Skill::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar )
		return false;
	if( m_SkillState == SS_END )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	ioBaseChar *pTarget = pChar->GetBaseChar( szTargetName );
	AddTargetInfo( pChar, pTarget );

	if( m_SkillState != SS_GRAPPLING )
	{
		m_SkillState = SS_GRAPPLING;

		m_dwCheckKeyTime = dwCurTime;

		ioEntityGroup *pGrp = pChar->GetGroup();
		if( pGrp )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->SetLoopAni( m_szLoopMotion, FLOAT100, FLOAT100, FLOAT1/m_fLoopTimeRate );
		}
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_GRAPPLING;
		kPacket << pChar->GetTargetRot();
		kPacket << szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || ( ToNpcChar(pTarget) || ToNpcChar(pChar) ) )
	{
		D3DXQUATERNION qtCharRot = pChar->GetTargetRot();
		pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
	}

	return true;
}

D3DXVECTOR3 ioGrapplingAttack5Skill::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	/*if( m_vTargetPosition != ioMath::VEC3_ZERO )
		return m_vTargetPosition;*/

	return ioMath::VEC3_ZERO;
}

bool ioGrapplingAttack5Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_GRAPPLING:
		return false;
	case SS_ACTION:
		if( m_dwMotionEndTime + 50 > dwCurTime )
			return false;
		return true;
	case SS_END:
		if( m_dwMotionEndTime < dwCurTime )
			return true;
		return false;
	}

	return false;
}

bool ioGrapplingAttack5Skill::IsProtected( int iDefenseBreakType ) const
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
		if( m_dwFireMotionEndTime > dwCurTime )
			return true;
	}

	if( m_dwProtectTime > 0 && m_dwSkillStartTime + m_dwProtectTime > dwCurTime )
		return true;

	return false;
}

bool ioGrapplingAttack5Skill::IsEnableReserve() const
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

bool ioGrapplingAttack5Skill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioGrapplingAttack5Skill::AttackSkillFire( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )
		return;

	m_SkillState = SS_ACTION;

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

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

	m_dwFireStartTime = FRAMEGETTIME();
	m_dwFireStartTime += dwPreDelay;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	FindTarget( pChar );
}

void ioGrapplingAttack5Skill::SetSkillEndState( ioBaseChar *pChar, bool bSend /* = false */ )
{
	if( !pChar )
		return;

	if( m_SkillState == SS_END )
		return;

	DestroyArrow();
	m_SkillState = SS_END;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackEndAnimation );
	if( iAniID == -1 )
		return;

	float fTimeRate  = GetAttribute().m_fAttackEndAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f );

	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if(  pChar->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_SKILL_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGrapplingAttack5Skill::GrapplingAttackFire( ioBaseChar *pChar )
{
	if( !pChar )
		return;
	if( m_SkillState == SS_END )
		return;
	if( !COMPARE( m_iCurCombo, 0, m_iMaxCombo ) )
		return;

	int iCnt = m_GrapplingTargetList.size();
	for( int i=0; i<iCnt; ++i )
	{
		ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetList[i].name );
		if( !pTarget )
			continue;

		D3DXVECTOR3 vDir = pChar->GetWorldPosition() - pTarget->GetWorldPosition();
		D3DXVec3Normalize( &vDir, &vDir );
		m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
		D3DXVECTOR3 vCurPos = pChar->GetMidPositionByRate();
		m_GrapplingTargetName = m_GrapplingTargetList[i].name;
		pChar->SkillFireExplicit( m_GrapplingAttackList[m_iCurCombo], GetName(), m_dwWeaponBaseIndex, vCurPos, vDir );
	}

	m_iCurCombo++;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGrapplingAttack5Skill::ProcessGuideEffect( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	if( !CheckArrowCreated( pOwner ) )
		return;

	ArrangeArrows( pOwner );
}

void ioGrapplingAttack5Skill::DestroyArrow()
{
	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( !pStage )
		return;

	for( size_t i=0; i<m_GrapplingTargetList.size(); ++i )
	{
		ArrowList::iterator guide_iter = m_GrapplingTargetList[i].guide.begin();
		for( ; guide_iter!=m_GrapplingTargetList[i].guide.end(); ++guide_iter )
		{
			ioMapEffect *pMapEffect = pStage->FindMapEffect( *guide_iter );
			if( pMapEffect )
				pMapEffect->EndEffectForce();
		}
		m_GrapplingTargetList[i].guide.clear();
	}
}

bool ioGrapplingAttack5Skill::CheckArrowCreated( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	bool bCreated = false;
	int iCnt = m_GrapplingTargetList.size();
	for( int i=0; i<iCnt; ++i )
	{
		GrapplingTargetInfo &rkInfo = m_GrapplingTargetList[i];

		if( !rkInfo.guide.empty() )
		{
			bCreated = true;
			continue;
		}

		rkInfo.guide.reserve( m_iMaxEffectCount );
		for( int j=0 ; j<m_iMaxEffectCount ; ++j )
		{
			ioMapEffect *pEffect = pStage->CreateMapEffect( m_GuideEffect, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
			if( pEffect )
			{
				rkInfo.guide.push_back( pEffect->GetUniqueID() );
			}
			bCreated = true;
		}
	}

	return bCreated;
}

void ioGrapplingAttack5Skill::ArrangeArrows( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	if( m_iMaxEffectCount <= 0 )
		return;

	int iCnt = m_GrapplingTargetList.size();
	for( int i=0; i<iCnt; ++i )
	{
		GrapplingTargetInfo kInfo =  m_GrapplingTargetList[i];
		ioBaseChar *pTarget = pOwner->GetBaseChar( kInfo.name );
		if( !pTarget )
			continue;

		// Update Arrow Pos and Rotation
		D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
		float fGapLength = kInfo.distance / m_iMaxEffectCount;

		ArrowList::iterator iter = kInfo.guide.begin();
		for( ; iter!=kInfo.guide.end() ; ++iter )
		{
			vCurPos += kInfo.direction * fGapLength;

			ioMapEffect *pMapEff = pStage->FindMapEffect( *iter );
			if( pMapEff )
			{
				pMapEff->SetWorldPosition( vCurPos );

				ioEffect *pEff = pMapEff->GetEffect();
				if( pEff )
				{
					D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
					pEff->GetParentNode()->SetScale( vScale );

					D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( kInfo.direction );
					pEff->GetParentNode()->SetOrientation( qtRot );
				}
			}
		}
	}
}

void ioGrapplingAttack5Skill::AddTargetInfo( ioBaseChar *pChar, ioBaseChar *pTarget )
{
	if( !pChar || !pTarget )
		return;

	int iCnt = m_GrapplingTargetList.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( m_GrapplingTargetList[i].name == pTarget->GetCharName() )
		{
			m_GrapplingTargetList[i].grappling_time = FRAMEGETTIME();
			return;
		}
	}

	GrapplingTargetInfo kInfo;

	kInfo.name = pTarget->GetCharName();

	D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
	kInfo.distance = D3DXVec3Length( &vDiff );

	D3DXVec3Normalize( &vDiff, &vDiff );
	kInfo.direction = vDiff;

	kInfo.grappling_time = FRAMEGETTIME();

	m_GrapplingTargetList.push_back( kInfo );
}

bool ioGrapplingAttack5Skill::CheckAlreadyTarget( const ioHashString &szName )
{
	GrapplingTargetList::iterator iter = m_GrapplingTargetList.begin();
	for( ; iter != m_GrapplingTargetList.end(); ++iter )
	{
		if( (*iter).name == szName )
			return true;
	}
	
	return false;
}

bool ioGrapplingAttack5Skill::CheckTarget( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	GrapplingTargetList::iterator iter = m_GrapplingTargetList.begin();
	while ( iter != m_GrapplingTargetList.end() )
	{
		ioBaseChar *pTarget = pChar->GetBaseChar( (*iter).name );
		if( !pTarget )
		{
			ioPlayStage *pStage = pChar->GetCreator();
			if( pStage )
			{
				for( size_t i=0; i<(*iter).guide.size(); ++i )
				{
					ioMapEffect *pMapEffect = pStage->FindMapEffect( (*iter).guide[i] );
					if( pMapEffect )
						pMapEffect->EndEffectForce();
				}
			}

			iter = m_GrapplingTargetList.erase( iter );
		}
		else
		{
			if( (*iter).grappling_time + g_App.GetGrapplingCheckDelay() < FRAMEGETTIME() &&
				pTarget->GetState() != CS_GRAPPLING_WOUNDED_SKILL )
			{
				ioPlayStage *pStage = pChar->GetCreator();
				if( pStage )
				{
					for( size_t i=0; i<(*iter).guide.size(); ++i )
					{
						ioMapEffect *pMapEffect = pStage->FindMapEffect( (*iter).guide[i] );
						if( pMapEffect )
							pMapEffect->EndEffectForce();
					}
				}

				iter = m_GrapplingTargetList.erase( iter );
			}
			else
			{
				++iter;
			}
		}
	}

	if( m_GrapplingTargetList.empty() )
		return false;

	return true;
}