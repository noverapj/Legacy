

#include "stdafx.h"

#include "ioMultiDummyCharSkill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


ioMultiDummyCharSkill::ioMultiDummyCharSkill()
{
	m_State = SS_NONE;

	m_bSetHandMesh = false;
	m_bReduceGauge = false;

	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwLoopStartTime = 0;

	m_iCurDummyCharCnt = 0;
	m_CurCreateKey = CDK_NONE;
}

ioMultiDummyCharSkill::ioMultiDummyCharSkill( const ioMultiDummyCharSkill &rhs )
: ioNormalSkill( rhs ),
 m_SkillEndMotion( rhs.m_SkillEndMotion ),
 m_fEndMotionRate( rhs.m_fEndMotionRate ),
 m_StartEffect( rhs.m_StartEffect ),
 m_EndEffect( rhs.m_EndEffect ),
 m_fDummyCharOffset( rhs.m_fDummyCharOffset ),
 m_LoopAnimation( rhs.m_LoopAnimation ),
 m_AttackKeyDummy( rhs.m_AttackKeyDummy ),
 m_DefenseKeyDummy( rhs.m_DefenseKeyDummy ),
 m_JumpKeyDummy( rhs.m_JumpKeyDummy ),
 m_iMaxDummyCharCnt( rhs.m_iMaxDummyCharCnt )
{
	m_State = SS_NONE;

	m_bSetHandMesh = false;
	m_bReduceGauge = false;

	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwLoopStartTime = 0;

	m_iCurDummyCharCnt = 0;
	m_CurCreateKey = CDK_NONE;
}

ioMultiDummyCharSkill::~ioMultiDummyCharSkill()
{
}

void ioMultiDummyCharSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_LoopAnimation = szBuf;

	rkLoader.LoadString_e( "skill_end_motion", "", szBuf, MAX_PATH );
	m_SkillEndMotion = szBuf;
	m_fEndMotionRate = rkLoader.LoadFloat_e( "end_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "start_effect", "", szBuf, MAX_PATH );
	m_StartEffect = szBuf;
	rkLoader.LoadString_e( "end_effect", "", szBuf, MAX_PATH );
	m_EndEffect = szBuf;

	m_iMaxDummyCharCnt = rkLoader.LoadInt_e( "max_dummy_char_cnt", 0 );

	rkLoader.LoadString_e( "attack_key_dummy_name", "", szBuf, MAX_PATH );
	m_AttackKeyDummy = szBuf;

	rkLoader.LoadString_e( "defense_key_dummy_name", "", szBuf, MAX_PATH );
	m_DefenseKeyDummy = szBuf;

	rkLoader.LoadString_e( "jump_key_dummy_name", "", szBuf, MAX_PATH );
	m_JumpKeyDummy = szBuf;

	m_fDummyCharOffset = rkLoader.LoadFloat_e( "dummy_char_offset", 0.0f );
}

ioSkill* ioMultiDummyCharSkill::Clone()
{
	return new ioMultiDummyCharSkill( *this );
}

bool ioMultiDummyCharSkill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	m_bReduceGauge = false;
	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	pChar->AttachEffect( m_SkillEffectName );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwLoopStartTime = 0;

	m_iCurDummyCharCnt = 0;
	m_CurCreateKey = CDK_NONE;

	m_State = SS_NONE;

	pChar->SetSkillProtection();
	pChar->SetSkillMoveEnable( false, false );

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetLoopState( pChar );
	}

	SetCameraBuff( pChar );

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioMultiDummyCharSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

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

	pChar->SetMagicCircleTarget( "" );

	pChar->DestroyMagicCircle();
	pChar->SetSkillMoveEnable( false, false );

	m_bReduceGauge = false;
	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_iCurDummyCharCnt = 0;
	m_State = SS_NONE;
	m_CurCreateKey = CDK_NONE;
}

void ioMultiDummyCharSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				SetLoopState( pChar );
			}
		}
		break;
	case SS_LOOP:
		{
			if( pChar->IsNeedProcess() )
			{
				if( pChar->IsChangeDirection() )
				{
					pChar->ChangeDirectionByInputDir( false );
				}

				if( pChar->IsAttackKey() )
					SetActionState( pChar, pStage, CDK_ATTACK );
				else if( pChar->IsDefenseKey() )
					SetActionState( pChar, pStage, CDK_DEFENSE );
				else if( pChar->IsJumpKey() )
					SetActionState( pChar, pStage, CDK_JUMP );
			}
		}
		break;
	case SS_ACTION:
		if( m_dwMotionEndTime < dwCurTime )
		{
			if( m_iCurDummyCharCnt < m_iMaxDummyCharCnt )
				SetLoopState( pChar );
			else
				SetEndState( pChar );
		}
		else
		{
			if( m_dwFireStartTime < dwCurTime )
			{
				CheckCreateDummyChar( pChar, pStage );
			}
		}
		break;
	case SS_END:
		break;
	}
}

bool ioMultiDummyCharSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioMultiDummyCharSkill::SetNormalSkill( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	float fAniRate = GetSkillMotionRate();
	float fTimeRate = fAniRate * m_fExtraAniRate;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	
	m_dwMotionStartTime = dwCurTime;
	DWORD dwDuration = pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	m_dwMotionEndTime = m_dwMotionStartTime + dwDuration;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	m_dwFireStartTime = m_dwFireMotionEndTime = m_dwActionStartTime = dwCurTime;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	pChar->AttachEffect( m_StartEffect );

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioMultiDummyCharSkill::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage, CreateDummyKey eKey )
{
	m_State = SS_ACTION;
	m_CurCreateKey = eKey;

	SetNormalSkill( pChar );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSS_ACTION;
		kPacket << eKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMultiDummyCharSkill::SetLoopState( ioBaseChar *pChar )
{
	if( m_LoopAnimation.IsEmpty() || !pChar )
	{
		m_dwLoopStartTime = 0;
		return;
	}

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( m_LoopAnimation, FLOAT100 );

	m_State = SS_LOOP;
	m_dwLoopStartTime = FRAMEGETTIME();

	EndPreDelayEffect( pChar );

	pChar->SetSkillMoveEnable( false, true );
}


void ioMultiDummyCharSkill::SetEndState( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();

	m_State = SS_END;

	if( m_SkillEndMotion.IsEmpty() )
	{
		m_dwMotionStartTime = dwCurTime;
		m_dwMotionEndTime = m_dwMotionStartTime;
		return;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_SkillEndMotion );
	if( iCurSkillAniID == -1 )	return;

	float fCurEndRate = m_fEndMotionRate;
	if( fCurEndRate <= 0.0f )
		fCurEndRate = FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1/fCurEndRate );

	m_dwMotionStartTime = dwCurTime;
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fCurEndRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	pChar->AttachEffect( m_EndEffect );
	pChar->SetSkillMoveEnable( false, false );
}

void ioMultiDummyCharSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iSyncState;
	rkPacket >> iSyncState;

	if( iSyncState == SSS_ACTION )
	{
		int iKey;
		rkPacket >> iKey;

		SetActionState( pChar, pStage, (CreateDummyKey)iKey );
	}
}

void ioMultiDummyCharSkill::CheckCreateDummyChar( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !COMPARE( m_iCurDummyCharCnt, 0, m_iMaxDummyCharCnt ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
	{
		ioHashString szCurDummyCharName;

		switch( m_CurCreateKey )
		{
		case CDK_ATTACK:
			szCurDummyCharName = m_AttackKeyDummy;
			break;
		case CDK_DEFENSE:
			szCurDummyCharName = m_DefenseKeyDummy;
			break;
		case CDK_JUMP:
			szCurDummyCharName = m_JumpKeyDummy;
			break;
		}

		if( szCurDummyCharName.IsEmpty() )
			return;

		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVECTOR3 vTargetPos = pChar->GetWorldPosition() + (m_fDummyCharOffset * vDir);
		
		int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();

		ioDummyChar *pDummy = pStage->CreateDummyChar( szCurDummyCharName, iCurIndex,
													   pChar->GetCharName(),
													   vTargetPos,
													   0.0f,
													   0, true );

		m_iCurDummyCharCnt++;
		m_dwFireStartTime = 0;
	}
}

bool ioMultiDummyCharSkill::IsEndState() const
{
	if( m_State == SS_END )
		return true;

	return false;
}

bool ioMultiDummyCharSkill::IsProtected( int iDefenseBreakType ) const
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

	switch( m_State )
	{
	case SS_NONE:
		return false;
	case SS_LOOP:
		return false;
	case SS_ACTION:
		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;
		break;
	case SS_END:
		return false;
	}

	return false;
}

bool ioMultiDummyCharSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

bool ioMultiDummyCharSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioMultiDummyCharSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}





