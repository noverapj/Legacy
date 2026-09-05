
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioCountDashAttackSkill.h"

ioCountDashAttackSkill::ioCountDashAttackSkill()
{
	m_iCurUseCount = 0;
	m_dwMotionEndTime = 0;
	m_SkillState = SS_None;
	m_bSetHandMesh = false;
}

ioCountDashAttackSkill::ioCountDashAttackSkill( const ioCountDashAttackSkill &rhs )
	: ioCountSkill( rhs ),
	  m_vCreateBuffList( rhs.m_vCreateBuffList ),
	  m_vRemoveBuffList( rhs.m_vRemoveBuffList ),
	  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	  m_NormalAttack( rhs.m_NormalAttack ),
	  m_JumpAttack( rhs.m_JumpAttack ),
	  m_Move( rhs.m_Move )
{
	m_iCurUseCount = 0;
	m_dwMotionEndTime = 0;
	m_SkillState = SS_None;
	m_bSetHandMesh = false;
}

ioCountDashAttackSkill::~ioCountDashAttackSkill()
{
}

void ioCountDashAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioCountSkill::LoadProperty( rkLoader );

	LoadBuffInfo( rkLoader );
		
	m_dwSkillProtectTime = rkLoader.LoadInt_e( "skill_protect_time", 0 );

	LoadAttackAttribute_e( "normal_attack", m_NormalAttack, rkLoader );
	LoadAttackAttribute_e( "jump_attack", m_JumpAttack, rkLoader );
	LoadAttackAttribute_e( "move_info", m_Move, rkLoader );
}

ioSkill* ioCountDashAttackSkill::Clone()
{
	return new ioCountDashAttackSkill( *this );
}

//////////////////////////////////////////////////////////////////////////////////////

bool ioCountDashAttackSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	ioAttackableItem *pAttackable = ToAttackableItem( GetOwnerItem() );
	if( pAttackable )
	{
		if( !pAttackable->IsCanFire( pChar ) )
			return false;
	}

	if( m_iCurUseCount < m_iNeedCount )
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////

bool ioCountDashAttackSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioCountSkill::OnSkillStart(pChar) )
		return false;

	m_SkillState = SS_None;
	m_dwCurSkillProtectTime = 0;
	m_bSetHandMesh = false;

	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
	
	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;
	
	if ( m_SkillUseType == SUT_NORMAL )
		m_CurAttack = m_NormalAttack;
	else if ( m_SkillUseType == SUT_JUMP )
		m_CurAttack = m_JumpAttack;

	return true;
}

void ioCountDashAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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

				SetActionState( pChar );
			}
		}
		break;
	case SS_Action:
		{
			if ( m_dwMotionEndTime < dwCurTime )
				SetMoveState( pChar );
		}
		break;
	case SS_Move:
		{
			if ( m_dwMotionEndTime < dwCurTime )
				m_SkillState = SS_End;
		}
		break;
	}
}

void ioCountDashAttackSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioCountSkill::OnSkillEnd( pChar );
	
	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );

	RemoveBuff( pChar );

	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
}

void ioCountDashAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage )
		return;

	int nState;
	rkPacket >> nState;

	switch ( (SkillState)nState )
	{
	case SS_Move:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pChar->SetTargetRotAndMoveDirChange( qtRot );
			pChar->SetWorldPosition( vPos );

			SetMoveState( pChar );
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////

bool ioCountDashAttackSkill::IsProtected( int iDefenseBreakType ) const
{
	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCurSkillProtectTime > dwCurTime )
		return true;

	return false;
}

bool ioCountDashAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if ( m_SkillState == SS_End )
		return true;

	return false;
}

bool ioCountDashAttackSkill::IsEndState() const
{
	if ( m_SkillState == SS_End )
	{
		return true;
	}
	return false;
}

bool ioCountDashAttackSkill::IsEnableReserve() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_Move:
		if( m_dwEnableReserveTime > 0 && m_dwEnableReserveTime < FRAMEGETTIME() )
			return true;
		break;
	}

	return false;
}

bool ioCountDashAttackSkill::IsCanSendControl() const
{
	switch ( m_SkillState )
	{
	case SS_Action:
	case SS_Move:
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////

void ioCountDashAttackSkill::LoadBuffInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int nCreateCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_vCreateBuffList.clear();
	for ( int i=0; i<nCreateCnt; i++ )
	{
		wsprintf_e( szKey, "owner_buff%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vCreateBuffList.push_back( szBuf );
	}
	
	int nRemoveCnt = rkLoader.LoadInt_e( "remove_buff_cnt", 0 );
	m_vRemoveBuffList.clear();
	for ( int i=0; i<nRemoveCnt; i++ )
	{
		wsprintf_e( szKey, "remove_buff%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vRemoveBuffList.push_back( szBuf );
	}
}

//////////////////////////////////////////////////////////////////////////////////////

void ioCountDashAttackSkill::SetActionState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_CurAttack.m_AttackAnimation );
	if ( iAniID == -1)
		return;

	float fTimeRate  = m_CurAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_CurAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );	

	pChar->ClearReservedSliding();
	pChar->SetForcePowerAmt( 0.0f );

	pChar->SetReservedSliding( m_CurAttack.m_vForceInfoList, iAniID, fTimeRate, 0 );
	pChar->RefreshFireTimeList( iAniID, m_CurAttack.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, 0 );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	
	if( IsUseAutoTargetValue() )
	{
		DWORD dwTrackingTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate + dwPreDelay;
		pChar->SetAutoTarget( ATT_SKILL );
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	m_SkillState = SS_Action;	
}

void ioCountDashAttackSkill::SetMoveState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_Move.m_AttackAnimation );
	if ( iAniID == -1)
		return;

	float fTimeRate  = m_Move.m_fAttackAniRate;
	DWORD dwPreDelay = m_Move.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );	

	pChar->ClearReservedSliding();
	pChar->SetForcePowerAmt( 0.0f );

	pChar->SetReservedSliding( m_Move.m_vForceInfoList, iAniID, fTimeRate, 0 );
	pChar->RefreshFireTimeList( iAniID, m_Move.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, 0 );
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwEnableReserveTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_Move.m_fAttackAniRate;

	m_SkillState = SS_Move;
			
	if ( pChar->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( pChar->IsSettedDirection() )
		{
			eNewDirKey = pChar->CheckCurDirKey();
			pChar->SetTargetRotToDirKey( eNewDirKey );
			pChar->SetWorldOrientation( pChar->GetTargetRot() );
		}

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SS_Move;
			kPacket << pChar->GetWorldOrientation();
			kPacket << pChar->GetWorldPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}	
	}
}

//////////////////////////////////////////////////////////////////////////////////////

void ioCountDashAttackSkill::CreateBuff( ioBaseChar *pChar )
{
	if ( !pChar || m_vCreateBuffList.empty() )
		return;

	int nCreateCnt = (int)m_vCreateBuffList.size();
	for ( int i=0; i<nCreateCnt; i++ )
	{
		//해당 이름의 버프가 없을때
		if ( !pChar->HasBuff( m_vCreateBuffList[i] ) )
			pChar->AddNewBuff( m_vCreateBuffList[i], pChar->GetCharName(), "", NULL, false );
	}
}

void ioCountDashAttackSkill::RemoveBuff( ioBaseChar *pChar )
{
	if ( !pChar || m_vRemoveBuffList.empty() )
		return;

	int nRemoveCnt = (int)m_vRemoveBuffList.size();
	for ( int i=0; i<nRemoveCnt; i++ )
	{
		//해당 이름의 버프가 있을때
		if ( pChar->HasBuff( m_vRemoveBuffList[i] ) )
			pChar->RemoveBuff( m_vRemoveBuffList[i], true );
	}
}

int ioCountDashAttackSkill::GetCurUseCount() 
{ 
	return m_iCurUseCount; 
}

int ioCountDashAttackSkill::GetMaxUseCount()
{ 
	return m_iMaxUseCount;
}

int ioCountDashAttackSkill::GetNeedCount()
{
	return m_iNeedCount;
}