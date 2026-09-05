
#include "stdafx.h"

#include "ioLinearComboSkill3.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioLinearComboSkill3::ioLinearComboSkill3()
{
	m_SkillState = SS_None;
	
	m_bSetHandMesh = false;
	m_dwCurSkillProtectTime = 0;
	m_dwAttackReserveTime = 0;
	m_nCurCombo = 0;
	m_bAttackInput = false;
	m_bDefenceInput = false;
	m_bReserveEnd = false;
}

ioLinearComboSkill3::ioLinearComboSkill3( const ioLinearComboSkill3 &rhs )
	: ioAttackSkill( rhs ),
	  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	  m_vAttackInfo( rhs.m_vAttackInfo ),
	  m_vDefenceInfo( rhs.m_vDefenceInfo ),
	  m_bAutoAttack( rhs.m_bAutoAttack )
{
	m_SkillState = SS_None;
	
	m_bSetHandMesh = false;
	m_dwCurSkillProtectTime = 0;
	m_dwAttackReserveTime = 0;
	m_nCurCombo = 0;
	m_bAttackInput = false;
	m_bDefenceInput = false;
	m_bReserveEnd = false;
}

ioLinearComboSkill3::~ioLinearComboSkill3()
{
}

void ioLinearComboSkill3::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );
	
	m_vAttackInfo.clear();
	int nMaxList = rkLoader.LoadInt_e( "max_attack_cnt", 0 );
	for ( int i=0; i<nMaxList; i++ )
	{
		char szBuf[MAX_PATH] = "";
		wsprintf( szBuf, "attack%d_info", i+1 );

		AttackAttribute sAttackInfo;
		LoadAttackAttribute( szBuf, sAttackInfo, rkLoader );
		m_vAttackInfo.push_back( sAttackInfo );
	}

	m_vDefenceInfo.clear();
	nMaxList = rkLoader.LoadInt_e( "max_defence_cnt", 0 );
	for ( int i=0; i<nMaxList; i++ )
	{
		char szBuf[MAX_PATH] = "";
		wsprintf( szBuf, "defence%d_info", i+1 );

		AttackAttribute sDefenceInfo;
		LoadAttackAttribute( szBuf, sDefenceInfo, rkLoader );
		m_vDefenceInfo.push_back( sDefenceInfo );
	}

	m_bAutoAttack = rkLoader.LoadBool_e( "auto_attack", false );
}

ioSkill* ioLinearComboSkill3::Clone()
{
	return new ioLinearComboSkill3( *this );
}

bool ioLinearComboSkill3::OnSkillStart( ioBaseChar *pChar )
{	
	m_SkillState = SS_None;
	
	m_bSetHandMesh = false;
	m_dwCurSkillProtectTime = 0;
	m_dwAttackReserveTime = 0;
	m_nCurCombo = 0;
	m_bAttackInput = false;
	m_bDefenceInput = false;
	m_bReserveEnd = false;

	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;
		
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
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	return true;
}

void ioLinearComboSkill3::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_None:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );

				if ( COMPARE( m_nCurCombo, 0, (int)m_vAttackInfo.size() ) )
					SetState( pChar, m_vAttackInfo[m_nCurCombo], true );
			}
		}
		break;
	case SS_Combo:
		{			
			if ( m_dwMotionEndTime < dwCurTime )
			{
				if ( m_bDefenceInput && COMPARE( m_nCurCombo, 0, (int)m_vDefenceInfo.size() ) )
					SetState( pChar, m_vDefenceInfo[m_nCurCombo], false );
				else if ( !m_bAutoAttack && m_bAttackInput && COMPARE( m_nCurCombo, 0, (int)m_vAttackInfo.size() ) )
					SetState( pChar, m_vAttackInfo[m_nCurCombo], true );
				else if ( !m_bReserveEnd && m_bAutoAttack && COMPARE( m_nCurCombo+1, 0, (int)m_vAttackInfo.size() ) )
				{
					m_nCurCombo++;
					SetState( pChar, m_vAttackInfo[m_nCurCombo], true );
				}
				else
					SetEndState( pChar );
				return;
			}

			CheckKeyInput( pChar );
		}
		break;
	}
}

void ioLinearComboSkill3::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	EndCameraBuff( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioLinearComboSkill3::IsProtected( int iDefenseBreakType ) const
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

	return false;
}

bool ioLinearComboSkill3::IsSkillMotionEnd( DWORD dwActionStop ) const
{	
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_None:
	case SS_Combo:
		return false;
	case SS_End:
		if( m_fSkillEndNextJumpAmt == 0.0f )
			return true;
		else
			return false;
	}

	if( m_dwMotionEndTime + dwActionStop >= FRAMEGETTIME() )
		return false;

	return true;
}

bool ioLinearComboSkill3::IsAttackEndState() const
{
	if( m_SkillState == SS_End )
		return true;

	return false;
}

void ioLinearComboSkill3::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );	
}

void ioLinearComboSkill3::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage ) 
		return;

	int nState;
	rkPacket >> nState;

	switch( nState )
	{
	case SS_Combo:
		{
			rkPacket >> m_nCurCombo;
			rkPacket >> m_bAttackInput;
			rkPacket >> m_bDefenceInput;
		}
		break;
	case SS_End:
		m_SkillState = SS_End;
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioLinearComboSkill3::CheckKeyInput( ioBaseChar *pChar )
{
	if( !pChar || !pChar->IsNeedProcess() || m_bReserveEnd ) 
		return false;

	if ( m_dwAttackReserveTime > FRAMEGETTIME() )
		return false;

	bool bSend = false;
	if ( !m_bAutoAttack && !m_bAttackInput && pChar->IsAttackKey() )
	{
		m_nCurCombo++;
		m_bAttackInput = true;
		m_bDefenceInput = false;
		bSend = true;
	}
	else if ( !m_bDefenceInput && pChar->IsDefenseKey() )
	{
		m_bAttackInput = false;
		m_bDefenceInput = true;
		m_bReserveEnd = true;
		bSend = true;
	}

	if ( bSend && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_Combo;
		kPacket << m_nCurCombo;
		kPacket << m_bAttackInput;
		kPacket << m_bDefenceInput;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioLinearComboSkill3::SetState( ioBaseChar *pChar, const AttackAttribute &sAttribute, bool bAttack )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int nAniID = pGrp->GetAnimationIdx( sAttribute.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = sAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = sAttribute.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	
	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pChar->RefreshFireTimeList( nAniID, sAttribute.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	pChar->ClearReservedSliding();
	pChar->SetReservedSliding( sAttribute.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	m_dwAttackReserveTime = dwCurTime;
	m_dwAttackReserveTime += pGrp->GetAnimationEventTime_e( nAniID, "key_reserve" ) * fTimeRate;
	m_dwAttackReserveTime += dwPreDelay;
	
	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( nAniID, "tracking_e" ) * fTimeRate;

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTarget( ATT_SKILL );
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	m_SkillState = SS_Combo;
	m_bAttackInput = false;
	m_bDefenceInput = false;
}

void ioLinearComboSkill3::SetEndState( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;

	m_SkillState = SS_End;

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_End;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}