

#include "stdafx.h"

#include "ioNpcChar.h"
#include "ioPlayStage.h"

#include "ioGrapplingSkill4.h"

#include "WeaponDefine.h"

ioGrapplingSkill4::ioGrapplingSkill4()
{
	ClearData();
}

ioGrapplingSkill4::ioGrapplingSkill4( const ioGrapplingSkill4 &rhs )
: ioAttackSkill( rhs ),
 m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
 m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
 m_dwSkillLoopDuration( rhs.m_dwSkillLoopDuration ),
 m_fTargetLoopGapRange( rhs.m_fTargetLoopGapRange ),
 m_dwLoopRotateTime( rhs.m_dwLoopRotateTime ),
 m_fCharHeightRate( rhs.m_fCharHeightRate ),
 m_fCharJumpPower( rhs.m_fCharJumpPower ),
 m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
 m_SuccessAttack( rhs.m_SuccessAttack ),
 m_FailAttack( rhs.m_FailAttack ),
 m_SkillLoopEffectName( rhs.m_SkillLoopEffectName )
{
	ClearData();
}

ioGrapplingSkill4::~ioGrapplingSkill4()
{
}

void ioGrapplingSkill4::ClearData()
{
	m_SkillState = SS_NONE;

	m_fCurChargeRate = 0.0f;
	
	m_dwSkillLoopStartTime	= 0;
	m_dwSkillLoopEndTime	= 0;

	m_GrapplingTargetName.Clear();

	m_bSetHandMesh	= false;

	m_iCurAttackCnt = -1;
	m_iCurWoundedCnt = -1;

	m_dwCheckCountTime = 0;
	m_dwCurSkillProtectTime = 0;

	m_bSendCountKey = false;
	m_dwSkillLoopEffect = -1;
}

void ioGrapplingSkill4::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	// 잡고 있는 동작
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;

	m_fSkillLoopMotionRate	= rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	m_dwSkillLoopDuration	= rkLoader.LoadInt_e( "skill_loop_duration", 0 );
	m_dwLoopRotateTime		= rkLoader.LoadInt_e( "skill_loop_rotate_speed", 0 );

	m_fTargetLoopGapRange	= rkLoader.LoadFloat_e( "skill_target_loop_gap_range", 0.0f );

	m_fCharHeightRate	= rkLoader.LoadFloat_e( "char_height_rate", FLOAT05 );
	m_fCharJumpPower	= rkLoader.LoadFloat_e( "skill_jump_power", 0.0f );

	// 
	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	//
	m_SuccessAttack.Init();
	LoadAttackAttribute_e( "success_attack", m_SuccessAttack, rkLoader );

	m_FailAttack.Init();
	LoadAttackAttribute_e( "fail_attack", m_FailAttack, rkLoader );

	rkLoader.LoadString_e( "skill_loop_effect_name", "", szBuf, MAX_PATH );
	m_SkillLoopEffectName = szBuf;
}

ioSkill* ioGrapplingSkill4::Clone()
{
	return new ioGrapplingSkill4( *this );
}

bool ioGrapplingSkill4::OnSkillStart( ioBaseChar *pChar )
{
	ClearData();

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_dwCurSkillProtectTime = 0;
	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	if( pChar->IsNeedProcess() )
	{
		pChar->SetShowPressButton( false );
	}
	
	return true;
}

void ioGrapplingSkill4::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

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
		CheckExtraAniJump( pChar );
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

			ProcessLoopDir( pChar );

			if( dwCurTime < m_dwSkillLoopEndTime - 200 )
				CheckCountKey( pChar );
			else if( !m_bSendCountKey )
				SendCountKey( pChar );

			if( pChar->IsNeedProcess() && m_dwSkillLoopEndTime < dwCurTime )
			{
				bool bSuccessAttack = false;
				if( m_iCurWoundedCnt >= 0 && m_iCurAttackCnt > m_iCurWoundedCnt )
					bSuccessAttack = true;

				// for NPC
				bool bNPC = false;
				if( ToNpcChar( pTarget ) )
					bNPC = true;

				// local or 훈련소 or NPC
				if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || bNPC )
				{
					if( m_iCurAttackCnt >= 0 && m_iCurAttackCnt > m_iCurWoundedCnt )
						bSuccessAttack = true;
				}

				SetSkillEndAttack( pChar, bSuccessAttack );
				return;
			}
		}
		break;
	case SS_END_ATTACK:
		break;
	case SS_END:
		break;
	}
}

void ioGrapplingSkill4::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	RemoveLoopEffect( pChar );
	ClearData();

	if( pChar->IsNeedProcess() )
	{
		pChar->SetShowPressButton( false );
	}

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	// 스킬 중간 동작에 사용된 메쉬들 제거
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
}

void ioGrapplingSkill4::AttackSkillFire( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->AttachEffect( m_SkillEffectName );

	DWORD dwCurTime = FRAMEGETTIME();

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iAniID,
										GetAttribute().m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										dwPreDelay );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;

	DWORD dwFireStartTime;
	dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	//
	SetExtraAniJump( pChar, iAniID, fTimeRate );

	//
	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;
	m_vAniRotateInfoList.clear();

	AniEventConstPtrList vAniRotateList;
	pGrp->GetAnimationEventConstPtrList( iAniID, "ani_rotate", vAniRotateList );

	AniEventConstPtrList::iterator iter_r = vAniRotateList.begin();
	while( iter_r != vAniRotateList.end() )
	{
		AniRotateInfo kInfo;

		kInfo.m_dwTime = (*iter_r)->fEventTime * fTimeRate;

		if( kInfo.m_dwTime > 0 )
			kInfo.m_dwTime += dwCurTime;
		else
			kInfo.m_dwTime = 0;

		if( !(*iter_r)->szSubInfo.IsEmpty() )
		{
			const StringVector &vParam = ioStringConverter::Split( (*iter_r)->szSubInfo.c_str(), " " );
			if( vParam.size() == 1 )
			{
				kInfo.m_fAngle = ioStringConverter::ParseFloat( vParam[0].c_str() );
			}
		}

		m_vAniRotateInfoList.push_back(kInfo);

		++iter_r;
	}

	if( !m_vAniRotateInfoList.empty() )
	{
		m_iCurAniRotate = 0;
		m_dwAniRotateTime = m_vAniRotateInfoList[m_iCurAniRotate].m_dwTime;
		m_fAniRotateAngle = m_vAniRotateInfoList[m_iCurAniRotate].m_fAngle;
	}
	//

	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( dwFireStartTime >= m_dwFireMotionEndTime )
	{
		m_dwFireMotionEndTime = m_dwMotionEndTime;
	}

	if( GetAttribute().m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		float fEndTimeRate = GetAttribute().m_fAttackEndAniRate;

		pGrp->SetActionAni( GetAttribute().m_AttackEndAnimation,
							0.0f, FLOAT100,
							1.0f, FLOAT1/fEndTimeRate,
							fWaitTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackEndAnimation );
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;
	}

	// 예외처리
	m_dwAttackEndTime = 0;

	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_SkillState = SS_ACTION;
	
	m_GrapplingTargetName.Clear();

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioGrapplingSkill4::SetSkillLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_SkillState != SS_ACTION )
		return;
	
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	m_dwSkillLoopStartTime = FRAMEGETTIME();
	m_SkillState = SS_LOOP;

	pChar->ClearReservedSliding();
	pChar->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_fSkillLoopMotionRate > 0.0f )
		fAniRate = m_fSkillLoopMotionRate;

	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	if( m_fCharJumpPower > 0.0f )
		pChar->SetJumpPower( m_fCharJumpPower );

	m_dwSkillLoopEndTime = m_dwSkillLoopStartTime;
	m_dwSkillLoopEndTime += m_dwSkillLoopDuration;

	AddLoopEffect( pChar );

	if( pChar->IsNeedProcess() )
	{
		pChar->SetShowPressButton( true );
	}
}

void ioGrapplingSkill4::SetSkillEndState( ioBaseChar *pChar, bool bSendNet )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime = dwCurTime;
	m_dwAttackEndTime = dwCurTime;
	m_dwMotionEndTime += 100;
	m_dwEnableReserveTime = 0;

	m_SkillState = SS_END;
	RemoveLoopEffect( pChar );

	if( pChar->IsNeedProcess() )
	{
		pChar->SetShowPressButton( false );
	}

	if( bSendNet && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_END;
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGrapplingSkill4::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING && !m_GrapplingTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING_ALL && !m_GrapplingTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_NO_GRAPPLING && !m_GrapplingTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );

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

bool ioGrapplingSkill4::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
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
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || bNPC )
	{
		pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
		pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );

		if( pTarget->GetState() == CS_GRAPPLING_WOUNDED_SKILL )
			pTarget->SetShowPressButton( true );
	}

	return true;
}

D3DXVECTOR3 ioGrapplingSkill4::GetGrapplingTargetPos( ioBaseChar *pChar )
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

bool ioGrapplingSkill4::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_LOOP:
		return false;
	case SS_ACTION:
	case SS_END_ATTACK:
	case SS_END:
		if( m_dwMotionEndTime + dwActionStop < dwCurTime )
		{
			return true;
		}
		return false;
	}

	return false;
}

void ioGrapplingSkill4::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
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

				if( pTarget->GetState() == CS_GRAPPLING_WOUNDED_SKILL )
					pTarget->SetShowPressButton( true );
			}
		}
		break;
	case SSC_END_ATTACK:
		{
			bool bSuccessAttack = false;
			rkPacket >> bSuccessAttack;

			SetSkillEndAttack( pChar, bSuccessAttack );
		}
		break;
	case SSC_END:
		if( m_SkillState != SS_END )
		{
			SetSkillEndState( pChar, false );
		}
		break;
	case SSC_COUNT_KEY:
		{
			rkPacket >> m_iCurWoundedCnt;
		}
		break;
	}
}

void ioGrapplingSkill4::ProcessLoopDir( ioBaseChar *pChar )
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

bool ioGrapplingSkill4::IsProtected( int iDefenseBreakType ) const
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
		if( m_dwCurSkillProtectTime < dwCurTime )
			return false;
		break;
	case SS_END_ATTACK:
		if( m_dwFireMotionEndTime < dwCurTime )
			return false;
		break;
	case SS_END:
		return false;
	}

	return true;
}

bool ioGrapplingSkill4::IsEnableReserve() const
{
	return false;
}

bool ioGrapplingSkill4::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioGrapplingSkill4::SetSkillEndAttack( ioBaseChar *pChar, bool bSuccessAttack )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	AttackAttribute kAttack = m_FailAttack;
	if( bSuccessAttack )
		kAttack = m_SuccessAttack;

	int iAniID = pGrp->GetAnimationIdx( kAttack.m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->InitActionStopDelay( true );

	float fTimeRate = kAttack.m_fAttackAniRate;
	DWORD dwPreDelay = kAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iAniID,
										kAttack.m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + kAttack.m_dwEndDelay;

	DWORD dwFireStartTime;
	dwFireStartTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	m_dwFireMotionEndTime = dwCurTime + dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	if( kAttack.m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		float fEndTimeRate = kAttack.m_fAttackEndAniRate;

		pGrp->SetActionAni( kAttack.m_AttackEndAnimation,
							0.0f, FLOAT100,
							1.0f, FLOAT1/fEndTimeRate,
							fWaitTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( kAttack.m_AttackEndAnimation );
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;
	}

	// 예외처리
	m_dwAttackEndTime = m_dwMotionEndTime;
	m_dwMotionEndTime += 100;

	//
	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	pChar->SetReservedSliding( kAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_SkillState = SS_END_ATTACK;
	RemoveLoopEffect( pChar );

	if( pChar->IsNeedProcess() )
	{
		pChar->SetShowPressButton( false );
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_END_ATTACK;
		kPacket << pChar->GetTargetRot();
		kPacket << bSuccessAttack;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGrapplingSkill4::CheckCountKey( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() )
	{
		if( m_iCurAttackCnt == -1 )
			m_iCurAttackCnt = 1;
		else
			m_iCurAttackCnt++;
		return;
	}

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_GrapplingTargetName );
	if( pTarget->IsNeedProcess() && pTarget->IsAttackKey() )
	{
		if( m_iCurWoundedCnt == -1 )
			m_iCurWoundedCnt = 1;
		else
			m_iCurWoundedCnt++;
		return;
	}
}

void ioGrapplingSkill4::SendCountKey( ioBaseChar *pOwner )
{
	m_bSendCountKey = true;

	if( !pOwner ) return;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return;

	if( pTarget->IsNeedSendNetwork() )
	{
		if( m_iCurWoundedCnt == -1 )
			m_iCurWoundedCnt = 0;

		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SSC_COUNT_KEY;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurWoundedCnt;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGrapplingSkill4::AddLoopEffect( ioBaseChar *pChar )
{
	if( !pChar ) return;

	if( m_SkillLoopEffectName.IsEmpty() )
		return;

	if( m_dwSkillLoopEffect != -1 )
		RemoveLoopEffect( pChar );

	ioEffect *pEffect = pChar->AttachEffect( m_SkillLoopEffectName );
	if( pEffect )
		m_dwSkillLoopEffect = pEffect->GetUniqueID();
}

void ioGrapplingSkill4::RemoveLoopEffect( ioBaseChar *pChar )
{
	if( !pChar ) return;

	if( m_dwSkillLoopEffect != -1 )
		pChar->EndEffect( m_dwSkillLoopEffect, false );

	m_dwSkillLoopEffect = -1;
}


