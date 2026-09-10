

#include "stdafx.h"


#include "ioBaseChar.h"

#include "ioMoveAttackSkill2.h"

#include "WeaponDefine.h"

ioMoveAttackSkill2::ioMoveAttackSkill2()
{
	m_bSetHandMesh = false;

	m_dwWeakAttackEndTime = 0;
	m_dwStrongAttackEndTime = 0;

	m_SkillState = SS_NONE;
	m_nCurRepeatCnt = 0;
	m_bUseActiveCnt = false;
}

ioMoveAttackSkill2::ioMoveAttackSkill2( const ioMoveAttackSkill2 &rhs )
: ioAttackSkill( rhs ),
 m_bDirection( rhs.m_bDirection ),
 m_dwSkillDuration( rhs.m_dwSkillDuration ),
 m_WaitAni( rhs.m_WaitAni ),
 m_MoveAni( rhs.m_MoveAni ),
 m_WeakAttack( rhs.m_WeakAttack ),
 m_StrongAttack( rhs.m_StrongAttack ),
 m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
 m_bSetUseCount( rhs.m_bSetUseCount ),
 m_nMaxRepeatCnt( rhs.m_nMaxRepeatCnt ),
 m_bSetNoDefenseKey( rhs.m_bSetNoDefenseKey ),
 m_bSetFirstAttack( rhs.m_bSetFirstAttack ),
 m_OwnerBuffList( rhs.m_OwnerBuffList ),
 m_bSetEnableRotate( rhs.m_bSetEnableRotate )
{
	m_bSetHandMesh = false;

	m_dwWeakAttackEndTime = 0;
	m_dwStrongAttackEndTime = 0;

	m_SkillState = SS_NONE;
	m_nCurRepeatCnt = 0;
	m_bUseActiveCnt = false;
}

ioMoveAttackSkill2::~ioMoveAttackSkill2()
{
}

void ioMoveAttackSkill2::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_bDirection = rkLoader.LoadBool_e( "direction", false );

	m_dwSkillDuration = (DWORD)rkLoader.LoadInt_e( "skill_duration", 0 );
	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	m_WaitAni = m_SkillMotionName;

	rkLoader.LoadString_e( "skill_move_ani", "", szBuf, MAX_PATH );
	m_MoveAni = szBuf;

	LoadAttackAttribute_e( "weak_attack", m_WeakAttack, rkLoader );

	LoadAttackAttribute_e( "strong_attack", m_StrongAttack, rkLoader );

	m_bSetUseCount = rkLoader.LoadBool_e( "set_use_count", false );
	m_nMaxRepeatCnt = rkLoader.LoadInt_e( "max_repeat_cnt", 0 );
	m_bSetNoDefenseKey = rkLoader.LoadBool_e( "set_no_defense_key", false );
	m_bSetFirstAttack = rkLoader.LoadBool_e( "set_first_attack", false );
	m_bSetEnableRotate = rkLoader.LoadBool_e( "set_enable_rotate", false );

	LoadOwnerBuffList( rkLoader );
}

ioSkill* ioMoveAttackSkill2::Clone()
{
	return new ioMoveAttackSkill2( *this );
}

bool ioMoveAttackSkill2::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	pChar->SetSkillProtection();

	m_bReduceGauge = false;
	m_dwAttackEndTime = 0;

	InitExtraAniJump();

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = dwCurTime + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = dwCurTime + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	m_nCurRepeatCnt = 0;
	m_bUseActiveCnt = m_bSetUseCount;

	// 기본 무기 손에서 제거
	m_bSetHandMesh = false;
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_dwWeakAttackEndTime = 0;
	m_dwStrongAttackEndTime = 0;
	m_dwPreDelayStartTime = 0;

	m_SkillState = SS_NONE;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetWaitState( pChar );
	}
	else
	{
		DWORD dwTrackingTime = 0;
		ioEntityGroup *pGrp = pChar->GetGroup();
		if( pGrp )
		{
			ioHashString szPreDelay = GetPreDelayAniName();

			int iAniID = pGrp->GetAnimationIdx( szPreDelay );
			if( iAniID != -1 )
				dwTrackingTime = pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" );
		}

		DWORD dwPreDelay = GetPreDelayTime();

		if( dwTrackingTime > 0 )
			dwTrackingTime += dwCurTime;
		else
			dwTrackingTime = dwCurTime + dwPreDelay;

		if( IsUseAutoTargetValue() )
		{
			pChar->SetAutoTarget( ATT_SKILL );
			pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
	}

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	SetSkillNameChatBubble( pChar );

	SetCameraBuff( pChar );

	return true;
}

void ioMoveAttackSkill2::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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

				EndPreDelayEffect( pChar );
				SetWaitState( pChar );
				SetOwnerBuffList( pChar, pStage );

				SetWeakAttackFire( pChar );
			}
		}
		break;
	case SS_WAIT:
		OnWaitState( pChar );
		break;
	case SS_MOVE:
		OnMoveState( pChar );
		break;
	case SS_WEAK_ATTACK:
		CheckExtraAniJump( pChar );
		OnWeakAttackState( pChar );
		break;
	case SS_STRONG_ATTACK:
		CheckExtraAniJump( pChar );
		OnStrongAttackState( pChar );
		break;
	}
}

void ioMoveAttackSkill2::OnSkillEnd( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioAttackSkill::OnSkillEnd( pChar );

	EndCameraBuff( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_dwWeakAttackEndTime = 0;
	m_dwStrongAttackEndTime = 0;

	m_bReduceGauge = false;
	pChar->SetSkillMoveEnable( false, false );
	pChar->SetMoveDirByRotate( pChar->GetTargetRot() );
	pChar->RestoreSkillProtection();

	m_SkillState = SS_NONE;
	m_bUseActiveCnt = false;
	
	int nBuffCnt = (int)m_OwnerBuffList.size();	
	for ( int i=0; i<nBuffCnt; ++i )
		pChar->RemoveBuff( m_OwnerBuffList[i], true );
}

void ioMoveAttackSkill2::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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
			pZone->SetZoneValue( false, true );
		}
	}
}

void ioMoveAttackSkill2::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SS_WAIT:
		SetWaitState( pChar );
		break;
	case SS_MOVE:
		SetMoveState( pChar );
		break;
	case SS_WEAK_ATTACK:
		{
			int iCurKey;
			rkPacket >> iCurKey;

			if ( (ioUserKeyInput::DirKeyInput)iCurKey != ioUserKeyInput::DKI_NONE )
				pChar->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)iCurKey );

			SetWeakAttackFire( pChar );
		}
		break;
	case SS_STRONG_ATTACK:
		SetStrongAttackFire( pChar );
		break;
	}
}

bool ioMoveAttackSkill2::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioMoveAttackSkill2::SetWaitState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_SkillState = SS_WAIT;

	pChar->SetSkillMoveEnable( true, m_bDirection );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_WaitAni );
	pGrp->SetLoopAni( iAniID, FLOAT100 );

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioMoveAttackSkill2::SetMoveState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_SkillState = SS_MOVE;
	pChar->SetSkillMoveEnable( true, m_bDirection );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_MoveAni );
	float fTimeRate = pChar->GetRunDashAniRate();
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, fTimeRate );
}

void ioMoveAttackSkill2::SetWeakAttackFire( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	int iAniID = pGrp->GetAnimationIdx( m_WeakAttack.m_AttackAnimation );
	float fTimeRate = m_WeakAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iAniID,
										m_WeakAttack.m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										0 );

	m_dwWeaponBaseIndex++;

	m_dwWeakAttackEndTime = dwCurTime;
	m_dwWeakAttackEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pChar->SetReservedSliding( m_WeakAttack.m_vForceInfoList, iAniID, fTimeRate, 0 );

	//
	SetExtraAniJump( pChar, iAniID, fTimeRate );

	pChar->SetSkillMoveEnable( false, m_bDirection );

	//
	DWORD dwTrackingTime = dwCurTime + m_WeakAttack.m_dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTarget( ATT_SKILL );
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	m_SkillState = SS_WEAK_ATTACK;

	m_nCurRepeatCnt++;
}

void ioMoveAttackSkill2::SetStrongAttackFire( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_StrongAttack.m_AttackAnimation );
	float fTimeRate = m_StrongAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iAniID,
										m_StrongAttack.m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										0 );

	m_dwWeaponBaseIndex++;

	m_dwStrongAttackEndTime = dwCurTime;
	m_dwStrongAttackEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pChar->SetReservedSliding( m_StrongAttack.m_vForceInfoList, iAniID, fTimeRate, 0 );

	//
	SetExtraAniJump( pChar, iAniID, fTimeRate );

	pChar->SetSkillMoveEnable( false, m_bDirection );

	m_SkillState = SS_STRONG_ATTACK;
}

void ioMoveAttackSkill2::SetEndState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	pChar->SetSkillMoveEnable( false, m_bDirection );

	m_SkillState = SS_END;
}

void ioMoveAttackSkill2::OnWaitState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	if( CheckEndTime( pChar ) )
	{
		SetEndState( pChar );
		return;
	}

	if( CheckAttackState( pChar ) )
		return;

	if( pChar->IsNeedProcess() && pChar->IsSettedDirection() )
	{
		SetMoveState( pChar );

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << m_SkillState;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		return;
	}
}

void ioMoveAttackSkill2::OnMoveState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	if( CheckEndTime( pChar ) )
	{
		SetEndState( pChar );
		return;
	}

	if( CheckAttackState( pChar ) )
		return;

	if( pChar->IsNeedProcess() && !pChar->IsSettedDirection() )
	{
		SetWaitState( pChar );

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << m_SkillState;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		return;
	}
}

void ioMoveAttackSkill2::OnWeakAttackState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	bool bCheckTimeOver = false;
	if( CheckEndTime( pChar ) )
	{
		bCheckTimeOver = true;
	}

	if( m_dwWeakAttackEndTime > 0 && m_dwWeakAttackEndTime < dwCurTime )
	{
		m_dwWeakAttackEndTime = 0;

		if( m_nMaxRepeatCnt > 0 && m_nCurRepeatCnt >= m_nMaxRepeatCnt ) 
			SetEndState( pChar );
		else
			SetWaitState( pChar );
		return;
	}
	else if( !bCheckTimeOver && m_dwEnableReserveTime > 0 && m_dwEnableReserveTime < dwCurTime )
	{
		if( CheckAttackState( pChar ) )
			return;
	}
}

void ioMoveAttackSkill2::OnStrongAttackState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwStrongAttackEndTime > 0 && m_dwStrongAttackEndTime < dwCurTime )
	{
		m_dwStrongAttackEndTime = 0;

		SetEndState( pChar );
	}
}

bool ioMoveAttackSkill2::CheckEndTime( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwSkillStartTime > 0 && m_dwSkillStartTime+m_dwSkillDuration < dwCurTime )
		return true;

	return false;
}

bool ioMoveAttackSkill2::CheckAttackState( ioBaseChar *pChar )
{
	if( !pChar || ( m_nMaxRepeatCnt > 0 && m_nCurRepeatCnt >= m_nMaxRepeatCnt ) ) 
		return false;

	if( pChar->IsNeedProcess() )
	{
		if( pChar->IsAttackKey() )
		{
			ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
			if ( m_bSetEnableRotate )
			{
				if( pChar->IsNeedProcess() && pChar->IsSettedDirection() )
				{
					eNewDirKey = pChar->CheckCurDirKey();
					pChar->SetTargetRotToDirKey( eNewDirKey );
				}
			}

			SetWeakAttackFire( pChar );

			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << m_SkillState;
				kPacket << eNewDirKey;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return true;
		}
		else if( pChar->IsDefenseKey() && !m_bSetNoDefenseKey )
		{
			SetStrongAttackFire( pChar );

			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << m_SkillState;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return true;
		}
	}

	return false;
}

bool ioMoveAttackSkill2::IsEnableReserve() const
{
	return false;
}

bool ioMoveAttackSkill2::IsProtected( int iDefenseBreakType ) const
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

	if( m_dwPreDelayStartTime == 0 )
	{
		if( m_dwSkillProtectTime > 0 && m_dwSkillStartTime+m_dwSkillProtectTime > FRAMEGETTIME() )
			return true;
	}

	return false;
}

bool ioMoveAttackSkill2::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	if( m_SkillState != SS_END )
		return false;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioMoveAttackSkill2::IsUseActiveCnt() const
{
	return m_bUseActiveCnt;
}

int ioMoveAttackSkill2::GetMaxActiveCnt( ioBaseChar *pChar ) const
{
	return m_nMaxRepeatCnt;
}

int ioMoveAttackSkill2::GetCurActiveCnt( ioBaseChar *pChar ) const
{
	return m_nCurRepeatCnt;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//버프 리스트 읽기
void ioMoveAttackSkill2::LoadOwnerBuffList( ioINILoader &rkLoader )
{
	int nCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();

	if( nCnt <= 0 )
		return;

	m_OwnerBuffList.reserve( nCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < nCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_OwnerBuffList.push_back( szBuf );
	}
}

void ioMoveAttackSkill2::SetOwnerBuffList( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if ( !pOwner || !pStage )
		return;
	
	if( m_OwnerBuffList.empty() ) 
		return;

	int nBuffCnt = m_OwnerBuffList.size();

	//해당 버프를 생성
	for( int i=0; i < nBuffCnt; ++i )
	{
		ioHashString szBuffName = m_OwnerBuffList[i];

		if( !szBuffName.IsEmpty() )
			pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), "", NULL );
	}
}