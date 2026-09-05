

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioTossSkill.h"

#include "WeaponDefine.h"

ioTossSkill::ioTossSkill()
{
	m_bSetHandMesh = false;
	m_dwAttackFireTime = 0;
	m_iCurTossCnt = 0;
	m_SkillState = SS_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

ioTossSkill::ioTossSkill( const ioTossSkill &rhs )
: ioAttackSkill( rhs ),
m_TossUpAni( rhs.m_TossUpAni ),
m_fUpJumpPowerAmt( rhs.m_fUpJumpPowerAmt ),
m_fTossUpAniRate( rhs.m_fTossUpAniRate ),
m_TossDownAttack( rhs.m_TossDownAttack ),
m_TossFinishAttack( rhs.m_TossFinishAttack ),
m_iMaxTossCnt( rhs.m_iMaxTossCnt ),
m_TossEndAni( rhs.m_TossEndAni ),
m_fTossEndAniRate( rhs.m_fTossEndAniRate ),
m_fMoveSpeed( rhs.m_fMoveSpeed )
{
	m_bSetHandMesh = false;
	m_dwAttackFireTime = 0;
	m_iCurTossCnt = 0;
	m_SkillState = SS_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

ioTossSkill::~ioTossSkill()
{
}

void ioTossSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	LoadAttackAttribute_e( "toss_down", m_TossDownAttack, rkLoader );

	LoadAttackAttribute_e( "toss_finish_down", m_TossFinishAttack, rkLoader );

	rkLoader.LoadString_e( "toss_up_ani", "", szBuf, MAX_PATH );
	m_TossUpAni = szBuf;

	m_fTossUpAniRate = rkLoader.LoadFloat_e( "toss_up_ani_rate", FLOAT1 );

	m_fUpJumpPowerAmt = rkLoader.LoadFloat_e( "toss_up_jump_power", 0.0f );
	m_iMaxTossCnt = rkLoader.LoadInt_e( "toss_max_cnt", 0 );

	rkLoader.LoadString_e( "toss_end_ani", "", szBuf, MAX_PATH );
	m_TossEndAni = szBuf;
	m_fTossEndAniRate = rkLoader.LoadFloat_e( "toss_end_ani_rate", FLOAT1 );

	m_fMoveSpeed = rkLoader.LoadFloat_e( "toss_move_speed", 0.0f );
}

ioSkill* ioTossSkill::Clone()
{
	return new ioTossSkill( *this );
}

bool ioTossSkill::OnSkillStart( ioBaseChar *pChar )
{
	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_dwEnableReserveTime = 0;
	m_dwFireMotionEndTime = 0;
	m_dwMotionEndTime = 0;

	InitExtraAniJump();

	m_dwAttackFireTime = 0;
	m_iCurTossCnt = 0;
	m_SkillState = SS_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	return true;
}

void ioTossSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioAttackSkill::AttackSkillFire( pChar );

	SetSkillActionState( pChar );
}

void ioTossSkill::SetSkillActionState( ioBaseChar *pChar )
{
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_SkillState = SS_ACTION;
}

void ioTossSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
		{
			CheckExtraAniJump( pChar );
			CheckAniRotate( pChar );

			if( CheckLanding( pChar ) )
				SetEndState( pChar );
		}
		break;
	case SS_UP:
		{
			CheckExtraAniJump( pChar );
			CheckAniRotate( pChar );

			CheckMoveDir( pChar );
			ProcessMove( pChar, pStage );
			OnTossUpState( pChar );
		}
		break;
	case SS_DOWN:
		{
			CheckExtraAniJump( pChar );
			CheckAniRotate( pChar );

			CheckMoveDir( pChar );
			ProcessMove( pChar, pStage );
			OnTossDownState( pChar );
		}
		break;
	case SS_FINISH_DOWN:
		{
			CheckExtraAniJump( pChar );
			CheckAniRotate( pChar );

			CheckMoveDir( pChar );
			ProcessMove( pChar, pStage );
			OnTossFinishState( pChar );
		}
		break;
	}
}

void ioTossSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );
	
	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_dwEnableReserveTime = 0;
	m_dwAttackFireTime = 0;
	m_SkillState = SS_NONE;
}

bool ioTossSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioTossSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			if( pChar )
			{
				D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				pZone->SetCollisionDir( vDir );
				pZone->SetZoneValue( true, true );
			}
		}
	}
}

void ioTossSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	if( iType == SCT_NORMAL )
	{
		int iState;
		rkPacket >> iState;

		switch( iState )
		{
		case SS_UP:
			{
				rkPacket >> m_iCurTossCnt;

				SetTossUpState( pChar );
			}
			break;
		}
	}
	else if( iType == SCT_DIR )
	{
		int iDir;
		rkPacket >> iDir;

		m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
	}
}

bool ioTossSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	if( m_SkillState != SS_END )
		return false;

	if( m_dwMotionEndTime + dwActionStop >= FRAMEGETTIME() )
		return false;

	return true;
}

bool ioTossSkill::IsEnableReserve() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	if( m_SkillState != SS_END )
		return false;

	if( m_dwEnableReserveTime == 0 || m_dwMotionStartTime >= m_dwEnableReserveTime )
		return false;

	if( m_dwEnableReserveTime < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioTossSkill::IsProtected( int iDefenseBreakType ) const
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

	if( m_SkillState == SS_ACTION )
		return true;

	return false;
}

bool ioTossSkill::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar )
		return false;

	switch( m_SkillState )
	{
	case SS_ACTION:
	case SS_DOWN:
		SetTossUpState( pChar );
		return true;
	case SS_FINISH_DOWN:
		return false;
	}

	return false;
}

void ioTossSkill::SetTossUpState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	int iAniID = pGrp->GetAnimationIdx( m_TossUpAni );
	float fTimeRate = m_fTossUpAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	// jump power
	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetJumpPower( m_fUpJumpPowerAmt );

	int iPreTossCnt = m_iCurTossCnt;
	m_iCurTossCnt++;
	m_SkillState = SS_UP;

	g_WeaponMgr.CheckSkillDependencyWeaponDead( pChar->GetCharName(), GetName() );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SCT_NORMAL;
		kPacket << m_SkillState;
		kPacket << iPreTossCnt;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTossSkill::SetTossDownState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	int iAniID = pGrp->GetAnimationIdx( m_TossDownAttack.m_AttackAnimation );
	float fTimeRate = m_TossDownAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwAttackFireTime = dwCurTime;
	m_dwAttackFireTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_SkillState = SS_DOWN;
}

void ioTossSkill::SetTossFinishState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	int iAniID = pGrp->GetAnimationIdx( m_TossFinishAttack.m_AttackAnimation );
	float fTimeRate = m_TossFinishAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwAttackFireTime = dwCurTime;
	m_dwAttackFireTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_SkillState = SS_FINISH_DOWN;
}

void ioTossSkill::SetEndState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	int iAniID = pGrp->GetAnimationIdx( m_TossEndAni );
	float fTimeRate = m_fTossEndAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	// Key Reserve
	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	m_SkillState = SS_END;

	g_WeaponMgr.CheckSkillDependencyWeaponDead( pChar->GetCharName(), GetName() );
}

void ioTossSkill::OnTossUpState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( CheckDown( pChar ) )
	{
		if( m_iCurTossCnt < m_iMaxTossCnt )
			SetTossDownState( pChar );
		else
			SetTossFinishState( pChar );

		return;
	}
}

void ioTossSkill::OnTossDownState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( CheckLanding( pChar ) )
	{
		SetEndState( pChar );
		return;
	}

	ProcessFire( pChar );
}

void ioTossSkill::OnTossFinishState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( CheckLanding( pChar ) )
	{
		SetEndState( pChar );
		return;
	}

	ProcessFire( pChar );
}

bool ioTossSkill::CheckDown( ioBaseChar *pChar )
{
	if( !pChar ) return true;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pChar->GetCurJumpPowerAmt() <= pChar->GetGravityAmt() )
		return true;

	return false;
}

bool ioTossSkill::CheckLanding( ioBaseChar *pChar )
{
	if( !pChar ) return true;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pChar->CheckGhostState() )
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetPreHeightForGhostState() - FLOAT1;
	else
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight() - FLOAT1;

	fHeightGap = max( 0.0f, fHeightGap );

	if( !pChar->CheckLandingInfo( fHeightGap ) )
		return false;

	return true;
}

void ioTossSkill::ProcessFire( ioBaseChar *pChar )
{
	if( m_dwAttackFireTime == 0 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwAttackFireTime < dwCurTime )
	{
		m_dwAttackFireTime = 0;

		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();

		m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
		if( m_SkillState == SS_DOWN )
		{
			int iCnt = m_TossDownAttack.m_vWeaponInfoList.size();
			for( int i=0; i < iCnt; ++i )
			{
				pChar->SkillFireExplicit( m_TossDownAttack.m_vWeaponInfoList[i],
										  GetName(),
										  m_dwWeaponBaseIndex+i,
										  vPos );
			}
		}
		else if( m_SkillState == SS_FINISH_DOWN )
		{
			int iCnt = m_TossFinishAttack.m_vWeaponInfoList.size();
			for( int i=0; i < iCnt; ++i )
			{
				pChar->SkillFireExplicit( m_TossFinishAttack.m_vWeaponInfoList[i],
										  GetName(),
										  m_dwWeaponBaseIndex+i,
										  vPos );
			}
		}
	}
}

void ioTossSkill::CheckMoveDir( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() )
		return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		m_CurDirKey = eNewDirKey;

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SCT_DIR;
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioTossSkill::ProcessMove( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	D3DXVECTOR3 vMoveAmt = vTargetDir * m_fMoveSpeed * fTimePerSec;

	bool bCol = false;
	if( pStage->ReCalculateMoveVectorByTerrain( pChar, &vMoveAmt, bCol ) )
		pChar->Translate( vMoveAmt );

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	if( pChar->CheckAxisLimit( &vPos ) )
		pChar->SetWorldPosition( vPos );

	return;
}









