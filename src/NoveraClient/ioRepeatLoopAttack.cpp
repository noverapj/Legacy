
#include "stdafx.h"

#include "ioRepeatLoopAttack.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioRepeatLoopAttack::ioRepeatLoopAttack()
{
	m_SkillState = SS_NONE;
	
	m_bSetHandMesh = false;
	m_dwCurSkillProtectTime = 0;
	m_dwCheckLoopTime = 0;
	m_dwAttackReserveTime = 0;
	m_nCurRepeatCnt = 0;
	m_bUseActiveCnt = false;
}

ioRepeatLoopAttack::ioRepeatLoopAttack( const ioRepeatLoopAttack &rhs )
	: ioAttackSkill( rhs ),
	  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	  m_nMaxRepeatCnt( rhs.m_nMaxRepeatCnt ),
	  m_vAttackList( rhs.m_vAttackList ),
	  m_szLoopAni( rhs.m_szLoopAni ),
	  m_fLoopAniRate( rhs.m_fLoopAniRate ),
	  m_dwLoopTime( rhs.m_dwLoopTime )
{
	m_SkillState = SS_NONE;
	
	m_bSetHandMesh = false;
	m_dwCurSkillProtectTime = 0;
	m_dwCheckLoopTime = 0;
	m_dwAttackReserveTime = 0;
	m_nCurRepeatCnt = 0;
	m_bUseActiveCnt = false;
}

ioRepeatLoopAttack::~ioRepeatLoopAttack()
{
	m_vAttackList.clear();
}

void ioRepeatLoopAttack::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	ioAttackSkill::LoadProperty( rkLoader );
		
	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );
	m_dwLoopTime = (DWORD)rkLoader.LoadInt_e( "loop_time", 0 );

	rkLoader.LoadString_e( "loop_ani", "", szBuf, MAX_PATH );
	m_szLoopAni = szBuf;

	m_fLoopAniRate = rkLoader.LoadFloat_e( "loop_ani_rate", FLOAT1 );

	m_vAttackList.clear();
	m_nMaxRepeatCnt = rkLoader.LoadInt_e( "attack_info_cnt", 0 );
	for ( int i=0; i<m_nMaxRepeatCnt; i++ )
	{
		wsprintf( szBuf, "attack_info%d", i+1 );

		AttackAttribute sAttackInfo;
		LoadAttackAttribute( szBuf, sAttackInfo, rkLoader );
		m_vAttackList.push_back( sAttackInfo );
	}
}

ioSkill* ioRepeatLoopAttack::Clone()
{
	return new ioRepeatLoopAttack( *this );
}

bool ioRepeatLoopAttack::OnSkillStart( ioBaseChar *pChar )
{	
	m_SkillState = SS_NONE;
	
	m_bSetHandMesh = false;
	m_dwCurSkillProtectTime = 0;
	m_dwCheckLoopTime = 0;
	m_dwAttackReserveTime = 0;
	m_nCurRepeatCnt = 0;
	m_bUseActiveCnt = true;

	if ( m_dwLoopTime > 0 )
		m_dwCheckLoopTime = FRAMEGETTIME() + m_dwLoopTime;

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

void ioRepeatLoopAttack::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_dwCheckLoopTime < dwCurTime )
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
				//SetLoopState( pChar );
				if ( pChar->IsNeedProcess() )
					SetAttackState( pChar );
			}
		}
		break;
	case SS_LOOP:
		{
			if ( CheckKeyInput( pChar ) )
				return;
		}
		break;
	case SS_ATTACK:
		{			
			if ( m_dwMotionEndTime < dwCurTime )
			{
				if ( m_nCurRepeatCnt < m_nMaxRepeatCnt )
				{				
					SetLoopState( pChar );
					return;
				}
				else
				{
					SetEndState( pChar );
					return;
				}
			}
		}
		break;
	}
}

void ioRepeatLoopAttack::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	EndCameraBuff( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;
	m_bUseActiveCnt = false;
}

bool ioRepeatLoopAttack::IsProtected( int iDefenseBreakType ) const
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

bool ioRepeatLoopAttack::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{	
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_ATTACK:
	case SS_LOOP:
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

bool ioRepeatLoopAttack::IsAttackEndState() const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

void ioRepeatLoopAttack::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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

void ioRepeatLoopAttack::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage ) 
		return;

	int nState;
	rkPacket >> nState;

	switch( nState )
	{
	case SS_ATTACK:
		{
			rkPacket >> m_nCurRepeatCnt;
			SetAttackState( pChar );
		}
		break;
	case SS_END:
		m_SkillState = SS_END;
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioRepeatLoopAttack::CheckKeyInput( ioBaseChar *pChar )
{
	if( !pChar || !pChar->IsNeedProcess() || m_nCurRepeatCnt >= m_nMaxRepeatCnt ) 
		return false;

	if ( m_dwAttackReserveTime < FRAMEGETTIME() && pChar->IsAttackKey() )
	{
		SetAttackState( pChar );
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioRepeatLoopAttack::SetLoopState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	
	ioEntityGroup *pGrp = pChar->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( m_szLoopAni );
	if( nAniID == -1 )	
		return;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	
	pGrp->SetLoopAni( nAniID, FLOAT100, FLOAT1, FLOAT1/m_fLoopAniRate );

	m_SkillState = SS_LOOP;
}

void ioRepeatLoopAttack::SetAttackState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	if ( m_nCurRepeatCnt < 0 || (int)m_vAttackList.size() <= m_nCurRepeatCnt )
		return;

	const AttackAttribute &sAttackInfo = m_vAttackList[m_nCurRepeatCnt];

	ioEntityGroup *pGrp = pChar->GetGroup();
	int nAniID = pGrp->GetAnimationIdx( sAttackInfo.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	//빠르기 설정, 딜레이 설정
	float fTimeRate  = sAttackInfo.m_fAttackAniRate;
	DWORD dwPreDelay = sAttackInfo.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
		
	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pChar->ClearReservedSliding();

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex, nAniID, sAttackInfo.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	m_SkillState = SS_ATTACK;

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_ATTACK;
		kPacket << m_nCurRepeatCnt;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_nCurRepeatCnt++;
}

void ioRepeatLoopAttack::SetEndState( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;

	m_SkillState = SS_END;

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioRepeatLoopAttack::IsUseActiveCnt() const
{
	return m_bUseActiveCnt;
}

int ioRepeatLoopAttack::GetMaxActiveCnt( ioBaseChar *pChar ) const
{
	return m_nMaxRepeatCnt;
}

int ioRepeatLoopAttack::GetCurActiveCnt( ioBaseChar *pChar ) const
{
	return m_nCurRepeatCnt;
}