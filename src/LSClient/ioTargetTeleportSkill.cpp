

#include "stdafx.h"

#include "ioTargetTeleportSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"


ioTargetTeleportSkill::ioTargetTeleportSkill()
{
	m_SkillState = SS_NONE;

	m_dwWaitStartTime = 0;

	m_vBasePos = ioMath::VEC3_ZERO;
	m_vTargetPos = ioMath::VEC3_ZERO;

	m_CurTargetName.Clear();
	m_TargetCharList.clear();
	m_iCurTargetIndex = -1;

	m_dwAttackEndTime = 0;
	m_dwTargetMapEffect = 0;
}

ioTargetTeleportSkill::ioTargetTeleportSkill( const ioTargetTeleportSkill &rhs )
: ioNormalSkill( rhs ),
 m_dwWaitDuration( rhs.m_dwWaitDuration ),
 m_SkillEndMotion( rhs.m_SkillEndMotion ),
 m_fEndMotionRate( rhs.m_fEndMotionRate ),
 m_TeleportCameraBuff( rhs.m_TeleportCameraBuff ),
 m_fTargetOffSetHeight( rhs.m_fTargetOffSetHeight ),
 m_TargetMapEffect( rhs.m_TargetMapEffect )
{
	m_SkillState = SS_NONE;

	m_dwWaitStartTime = 0;

	m_vBasePos = ioMath::VEC3_ZERO;
	m_vTargetPos = ioMath::VEC3_ZERO;

	m_CurTargetName.Clear();
	m_TargetCharList.clear();
	m_iCurTargetIndex = -1;

	m_dwAttackEndTime = 0;
	m_dwTargetMapEffect = 0;
}

ioTargetTeleportSkill::~ioTargetTeleportSkill()
{
}

void ioTargetTeleportSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_dwWaitDuration = (DWORD)rkLoader.LoadInt_e( "wait_duration", 1000 );

	rkLoader.LoadString_e( "skill_end_motion", "", szBuf, MAX_PATH );
	m_SkillEndMotion = szBuf;
	m_fEndMotionRate = rkLoader.LoadFloat_e( "end_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "teleport_camera_buff", "", szBuf, MAX_PATH );
	m_TeleportCameraBuff = szBuf;

	m_fTargetOffSetHeight = rkLoader.LoadFloat_e( "teleport_offset_height", 0.0f );

	rkLoader.LoadString_e( "target_map_effect", "", szBuf, MAX_PATH );
	m_TargetMapEffect = szBuf;
}

ioSkill* ioTargetTeleportSkill::Clone()
{
	return new ioTargetTeleportSkill( *this );
}

bool ioTargetTeleportSkill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	m_dwWaitStartTime = 0;
	m_dwAttackEndTime = 0;
	m_dwTargetMapEffect = 0;
	m_SkillState = SS_NONE;

	m_vBasePos = ioMath::VEC3_ZERO;
	m_vTargetPos = ioMath::VEC3_ZERO;

	m_CurTargetName.Clear();
	m_TargetCharList.clear();

	m_iCurTargetIndex = -1;

	pChar->AttachEffect( m_SkillEffectName );

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
		return false;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	pChar->SetSkillProtection();

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	SetWaitState( pChar, pStage );

	if( !m_TeleportCameraBuff.IsEmpty() )
		pChar->AddNewBuff( m_TeleportCameraBuff, "", "", this );

	CheckSkillDesc( pChar );

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioTargetTeleportSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	pChar->RemoveBuff( m_TeleportCameraBuff );

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	
	m_SkillState = SS_NONE;

	DestroyTargetMapEffect( pChar );
}

void ioTargetTeleportSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iSlot = GetOwnerItem()->GetType() - 1;

	switch( m_SkillState )
	{
	case SS_WAIT:
		if( pChar->IsNeedProcess() )
		{
			if( !CheckValidateTarget( pChar, pStage ) )
			{
				SelectNextTarget( pChar, pStage );
			}

			if( m_dwWaitStartTime+m_dwWaitDuration < dwCurTime )
			{
				SetTeleportState( pChar, pStage );
				CheckTargetPos( pChar, pStage );
				UpdateTargetMapEffect( pStage );
				return;
			}

			if( pChar->IsAttackKey() )
			{
				SetTeleportState( pChar, pStage );
				CheckTargetPos( pChar, pStage );
				UpdateTargetMapEffect( pStage );
				return;
			}
			else if( pChar->IsDefenseKey() )
			{
				SelectNextTarget( pChar, pStage );
			}

			CheckTargetPos( pChar, pStage );
			UpdateTargetMapEffect( pStage );
		}
		break;
	case SS_TELEPORT:
		if( m_dwMotionEndTime < dwCurTime )
			SetEndState( pChar );
		break;
	case SS_END:
		break;
	}
}

bool ioTargetTeleportSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioTargetTeleportSkill::SetNormalSkill( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = GetSkillMotionRate();

	float fTimeRate = fAniRate * m_fExtraAniRate;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	
	m_dwMotionStartTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioTargetTeleportSkill::SetWaitState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_SkillState = SS_WAIT;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwWaitStartTime = dwCurTime;

	SetTargetList( pChar, pStage );

	if( pChar && pChar->IsNeedProcess() )
		SetTargetMapEffect( pChar );
}

void ioTargetTeleportSkill::SetTeleportState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_SkillState != SS_WAIT )
		return;

	m_SkillState = SS_TELEPORT;

	m_dwPreDelayStartTime = 0;
	EndPreDelayEffect( pChar );
	SetNormalSkill( pChar );
	CheckTargetPos( pChar, pStage );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << m_SkillState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTeleportSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SS_TELEPORT:
		SetTeleportState( pChar, pStage );
		break;
	}
}

bool ioTargetTeleportSkill::IsAttackEndState() const
{
	if( m_SkillState == SS_END && m_dwAttackEndTime > 0 && m_dwAttackEndTime < FRAMEGETTIME() )
		return true;

	return false;
}

void ioTargetTeleportSkill::SetEndState( ioBaseChar *pChar )
{
	m_SkillState = SS_END;

	if( !pChar )
		return;

	D3DXVECTOR3 vTargetPos = m_vTargetPos;
	vTargetPos.y += m_fTargetOffSetHeight;
	pChar->SetWorldPosition( vTargetPos );

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_SkillEndMotion );
	if( iCurSkillAniID == -1 )	return;

	float fTimeRate = m_fEndMotionRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1/fTimeRate );
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionStartTime = dwCurTime;
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iCurSkillAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;
}

bool ioTargetTeleportSkill::IsProtected( int iDefenseBreakType ) const
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

	switch( m_SkillState )
	{
	case SS_WAIT:
		return false;
	case SS_TELEPORT:
	case SS_END:
		return true;
	}

	return false;
}

bool ioTargetTeleportSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_SkillState == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

void ioTargetTeleportSkill::SetTargetList( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage )
		return;

	if( !pChar->IsNeedProcess() )
		return;

	ioPlayMode *pMode = pStage->GetPlayMode();
	if( !pMode )
		return;

	m_iCurTargetIndex = -1;
	m_CurTargetName.Clear();
	m_TargetCharList.clear();
	m_vBasePos = pMode->GetRevivalPos( pChar );

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();

	BaseCharDistanceSqList kDistanceSqList;

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarChar = (*iter);

		if( !pTarChar ) continue;
		if( pTarChar->GetTeam() != pChar->GetTeam() )
			continue;
		if( pTarChar->GetCharName() == pChar->GetCharName() )
			continue;

		BaseCharDistanceSq kInfo;

		D3DXVECTOR3 vDiff = vPos - pTarChar->GetWorldPosition();
		kInfo.m_Name = pTarChar->GetCharName();
		kInfo.m_fDistanceSq = D3DXVec3LengthSq( &vDiff );

		kDistanceSqList.push_back( kInfo );
	}

	if( !kDistanceSqList.empty() )
	{
		std::sort( kDistanceSqList.begin(), kDistanceSqList.end(), BaseCharDistanceSqSort() );

		int iListCnt = kDistanceSqList.size();
		for( int i=0; i < iListCnt; ++i )
		{
			m_TargetCharList.push_back( kDistanceSqList[i].m_Name );
		}
	}

	if( !m_TargetCharList.empty() )
		SelectNextTarget( pChar, pStage );
}

void ioTargetTeleportSkill::SelectNextTarget( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage )
		return;

	if( m_TargetCharList.empty() )
		return;

	int iNewIndex = m_iCurTargetIndex + 1;
	if( !COMPARE( iNewIndex, 0, (int)m_TargetCharList.size() ) )
	{
		m_iCurTargetIndex = -1;
		m_CurTargetName.Clear();
		CheckTargetPos( pChar, pStage );
		return;
	}

	ioBaseChar *pTarget = NULL;

	while( !pTarget && COMPARE(iNewIndex, 0, (int)m_TargetCharList.size()) )
	{
		pTarget = pStage->GetBaseChar( m_TargetCharList[iNewIndex] );

		if( pTarget && pTarget->IsSystemState() )
		{	
			pTarget = NULL;
			iNewIndex++;
			continue;			
		}
		else
		{
			iNewIndex++;
		}
	}

	if( pTarget )
	{
		m_CurTargetName = pTarget->GetCharName();
		m_iCurTargetIndex = iNewIndex;
	}

	CheckTargetPos( pChar, pStage );
}

bool ioTargetTeleportSkill::CheckValidateTarget( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage )
		return false;

	if( m_iCurTargetIndex == -1 || m_CurTargetName.IsEmpty() )		// Base
		return true;

	ioBaseChar *pTarget = pStage->GetBaseChar( m_CurTargetName );
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	return true;
}

void ioTargetTeleportSkill::CheckTargetPos( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage )
	{
		m_vTargetPos = m_vBasePos;
		return;
	}

	if( m_iCurTargetIndex == -1 || m_CurTargetName.IsEmpty() )
	{
		m_vTargetPos = m_vBasePos;
		return;
	}

	ioBaseChar *pTarget = pStage->GetBaseChar( m_CurTargetName );
	if( pTarget )
	{
		m_vTargetPos = pTarget->GetWorldPosition();
		return;
	}

	m_vTargetPos = m_vBasePos;
	return;
}

bool ioTargetTeleportSkill::IsViewPositionState()
{
	switch( m_SkillState )
	{
	case SS_WAIT:
	case SS_TELEPORT:
	case SS_END:
		return true;
	}

	return false;
}

const D3DXVECTOR3& ioTargetTeleportSkill::GetViewPosition()
{
	return m_vTargetPos;
}

void ioTargetTeleportSkill::SetTargetMapEffect( ioBaseChar *pChar )
{
	DestroyTargetMapEffect( pChar );

	if( pChar && !m_TargetMapEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos = m_vTargetPos;
		ioMapEffect *pMapEffect = pChar->CreateMapEffectBySkill( m_TargetMapEffect, vPos );
		if( pMapEffect )
			m_dwTargetMapEffect = pMapEffect->GetUniqueID();
	}
}

void ioTargetTeleportSkill::UpdateTargetMapEffect( ioPlayStage *pStage )
{
	if( !pStage )
		return;

	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwTargetMapEffect );
	if( pMapEffect )
	{
		D3DXVECTOR3 vPos = m_vTargetPos;
		vPos.y += m_fTargetOffSetHeight;

		pMapEffect->SetWorldPosition( vPos );
	}
}

void ioTargetTeleportSkill::DestroyTargetMapEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwTargetMapEffect != 0 )
	{
		pChar->DestroyMapEffect( m_dwTargetMapEffect );
		m_dwTargetMapEffect = 0;
	}
}






