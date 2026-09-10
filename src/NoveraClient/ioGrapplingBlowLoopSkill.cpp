

#include "stdafx.h"

#include "ioNpcChar.h"
#include "ioPlayStage.h"

#include "ioGrapplingBlowLoopSkill.h"

#include "WeaponDefine.h"

ioGrapplingBlowLoopSkill::ioGrapplingBlowLoopSkill()
{
	m_SkillState = SS_NONE;

	m_dwSkillLoopStartTime = 0;
	m_dwSkillLoopEndTime = 0;

	m_dwPreLoopStartTime = 0;
}

ioGrapplingBlowLoopSkill::ioGrapplingBlowLoopSkill( const ioGrapplingBlowLoopSkill &rhs )
	: ioAttackSkill( rhs ),
	m_fGrapplingRange( rhs.m_fGrapplingRange ),
	m_fGrapplingMinAngle( rhs.m_fGrapplingMinAngle ),
	m_fGrapplingMaxAngle( rhs.m_fGrapplingMaxAngle ),
	m_fTargetLoopGapRange( rhs.m_fTargetLoopGapRange ),
	m_fTargetLoopGapHeight( rhs.m_fTargetLoopGapHeight ),
	m_dwLoopRotateTime( rhs.m_dwLoopRotateTime ),
	m_dwSkillLoopProtectTime( rhs.m_dwSkillLoopProtectTime ),
	m_fCharJumpPower( rhs.m_fCharJumpPower ),
	m_fCharHeightRate( rhs.m_fCharHeightRate ),
	m_bUsePreLoopState( rhs.m_bUsePreLoopState ),
	m_PreLoopMotion( rhs.m_PreLoopMotion ),
	m_fPreLoopMotionRate( rhs.m_fPreLoopMotionRate ),
	m_dwPreLoopDuration( rhs.m_dwPreLoopDuration ),
	m_vPreLoopBuffList( rhs.m_vPreLoopBuffList ),
	m_nMaxJmnpCnt( rhs.m_nMaxJmnpCnt ),
	m_vGrapJumpInfo( rhs.m_vGrapJumpInfo ),
	m_dwRotateSpeed( rhs.m_dwRotateSpeed )
{
	m_SkillState = SS_NONE;

	m_dwSkillLoopStartTime = 0;
	m_dwSkillLoopEndTime = 0;

	m_nCurJumpCnt = 0;
}

ioGrapplingBlowLoopSkill::~ioGrapplingBlowLoopSkill()
{
}

void ioGrapplingBlowLoopSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	/*rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;

	m_fSkillLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );*/

	m_fTargetLoopGapRange = rkLoader.LoadFloat_e( "skill_target_loop_gap_range", 0.0f );
	m_fTargetLoopGapHeight = rkLoader.LoadFloat_e( "skill_target_loop_gap_height", 0.f );

	m_fGrapplingRange = rkLoader.LoadFloat_e( "skill_grappling_range", 0.0f );
	m_fGrapplingMinAngle = rkLoader.LoadFloat_e( "skill_grappling_min_angle", 0.0f );
	m_fGrapplingMaxAngle = rkLoader.LoadFloat_e( "skill_grappling_max_angle", 0.0f );

	m_dwLoopRotateTime = rkLoader.LoadInt_e( "skill_loop_rotate_speed", 0 );
	m_dwSkillLoopProtectTime = rkLoader.LoadInt_e( "skill_loop_protect_time", 0 );

	m_fCharHeightRate = rkLoader.LoadFloat_e( "char_height_rate", FLOAT05 );
	m_fCharJumpPower = rkLoader.LoadFloat_e( "skill_jump_power", 0.0f );

	//
	int iBuffCnt = rkLoader.LoadInt_e( "blow_cancel_buff_cnt", 0 );
	for( int i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "blow_cancel_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SkillGrapplingInfo.m_SwingCancelBuff.push_back( szBuf );
	}

	//
	m_bUsePreLoopState = rkLoader.LoadBool_e( "use_pre_loop_state", false );

	rkLoader.LoadString_e( "pre_loop_motion", "", szBuf, MAX_PATH );
	m_PreLoopMotion = szBuf;
	m_fPreLoopMotionRate = rkLoader.LoadFloat_e( "pre_loop_motion_rate", FLOAT1 );

	m_dwPreLoopDuration = (DWORD)rkLoader.LoadInt_e( "pre_loop_duration", 0 );

	m_vPreLoopBuffList.clear();

	int iLoopBuffCnt = rkLoader.LoadInt_e( "pre_loop_buff_cnt", 0 );
	if( iLoopBuffCnt > 0 )
	{
		m_vPreLoopBuffList.reserve( iLoopBuffCnt );

		for( int i=0; i < iLoopBuffCnt; ++i )
		{
			wsprintf_e( szKey, "pre_loop_buff%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			ioHashString szBuffName = szBuf;

			m_vPreLoopBuffList.push_back( szBuffName );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	m_nMaxJmnpCnt = rkLoader.LoadInt_e( "max_grap_jump_cnt", 0 );
	for( int i = 0; i < m_nMaxJmnpCnt; i++ )
	{
		StGrapInfo Info;
		wsprintf_e( szKey, "skill_loop_grap_motion%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info.m_SkillLoopMotion = szBuf;
		wsprintf_e( szKey, "skill_loop_grap_motion_rate%d", i+1 );
		Info.m_fSkillLoopMotionRate = rkLoader.LoadInt( szKey, FLOAT1 );

		wsprintf_e( szKey, "skill_loop_grap_motion_stun%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info.m_SkillLoopMotionStun = szBuf;
		wsprintf_e( szKey, "skill_loop_grap_motion_stun_rate%d", i+1 );
		Info.m_fSkillLoopMotionRateStun = rkLoader.LoadInt( szKey, FLOAT1 );

		wsprintf_e( szKey, "skill_loop_grap_jump_power%d", i+1 );
		Info.m_fJumpPower = rkLoader.LoadFloat( szKey, 0.f );

		wsprintf_e( szKey, "land_attack%d", i+1 );
		Info.m_LandAttack.Init();
		LoadAttackAttribute( szKey, Info.m_LandAttack, rkLoader );

		wsprintf_e( szKey, "jump_move_speed%d", i+1 );
		Info.m_dwJumpMoveSpeed = (DWORD)rkLoader.LoadInt( szKey, 0 );

		m_vGrapJumpInfo.push_back( Info );
	}

	m_dwRotateSpeed = (DWORD)rkLoader.LoadInt_e( "attack_rotate_speed", 0 );
	//m_dwJumpMoveSpeed = (DWORD)rkLoader.LoadInt_e( "jump_move_speed", 0 );
}

ioSkill* ioGrapplingBlowLoopSkill::Clone()
{
	return new ioGrapplingBlowLoopSkill( *this );
}

bool ioGrapplingBlowLoopSkill::OnSkillStart( ioBaseChar *pChar )
{
	m_SkillState = SS_NONE;
	m_GrapplingTargetName.Clear();

	m_dwSkillLoopStartTime = 0;
	m_dwSkillLoopEndTime = 0;

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_nCurJumpCnt = 0;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	return true;
}

void ioGrapplingBlowLoopSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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

				if( m_bUsePreLoopState )
					SetSkillPreLoopState( pChar );
				else
					AttackSkillFire( pChar );
			}
		}
		break;
	case SS_PRE_LOOP:
		if( m_dwPreLoopStartTime+m_dwPreLoopDuration < dwCurTime )
		{
			m_SkillState = SS_OVER;
			return;
		}
		else if( pChar->IsNeedProcess() && pChar->IsAttackKey() )
		{
			AttackSkillFire( pChar );

			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SS_ACTION;
				kPacket << pChar->GetTargetRot();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}

		CheckPreLoopDirKey( pChar );
		break;
	case SS_ACTION:
		break;
	case SS_LOOP:
		{
			// Target 유효 체크
			/*ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget )
			{
				if( m_dwSkillLoopStartTime+g_App.GetGrapplingCheckDelay() < dwCurTime &&
					pTarget->GetState() != CS_GRAPPLING_WOUNDED_SKILL )
				{
					m_SkillState = SS_JUMP;
					return;
				}
			}
			else
			{
				m_SkillState = SS_JUMP;
				return;
			}*/

			if( m_fCharJumpPower > 0.0f )
			{
				if( CheckLanding( pChar ) )//|| CheckLanding( pTarget) )
				{
					//SetSkillEndState( pChar, true );
					//SetSkillLoopState_Test( pChar );
					SetSkillLandAttackState( pChar, max( m_nCurJumpCnt - 1, 0 ) );
					return;
				}
			}
			else if( m_dwSkillLoopEndTime < dwCurTime && pChar->IsNeedProcess() )
			{
				SetSkillEndState( pChar, true );
				return;
			}

			ProcessLoopDir( pChar );

			CheckKeyInput( pChar );

			ProcessRotate( pChar );
			ProcessJumpMove( pChar, pStage );
		}
		break;
	case SS_LAND_ATTACK:
		{
			if( pChar->IsNeedProcess() )
			{
				if( m_nCurJumpCnt < m_nMaxJmnpCnt )
				{
					if( m_dwEnableReserveTime > dwCurTime )
					{
						if( pChar->IsAttackKey() && !m_bReserveJumping )
						{
							m_bReserveJumping = true;
							//SendPacket
							if( pChar->IsNeedSendNetwork() )
							{
								SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
								kPacket << pChar->GetCharName();
								kPacket << GetName();
								kPacket << SS_JUMP_RESERVED;
								kPacket << pChar->GetTargetRot();
								kPacket << pChar->GetWorldPosition();
								P2PNetwork::SendToAllPlayingUser( kPacket );
							}
							//SS_JUMP_RESERVED
						}
					}
				}
			}

			if( dwCurTime > m_dwMotionEndTime )
			{
				ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
				if( !pTarget || pTarget->GetState() != CS_GRAPPLING_WOUNDED_SKILL )
				{
					SetSkillEndState( pChar, false );
					return;
				}

				if( m_nCurJumpCnt >= m_nMaxJmnpCnt )
				{
					SetSkillEndState( pChar, false );
					return;
				}

				if( m_bReserveJumping )
				{
					SetSkillLoopState_MultiJump( pChar, m_nCurJumpCnt );
					m_nCurJumpCnt++;
				}
				else if( pChar->IsNeedProcess() )
				{
					SetSkillEndState( pChar, true );
					return;
				}
			}
		}
		break;
	case SS_END:
		break;
	}

}

void ioGrapplingBlowLoopSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ClearPreLoopBuff( pChar );

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

	m_SkillState = SS_NONE;
	m_GrapplingTargetName.Clear();

	m_dwSkillLoopStartTime = 0;
	m_dwSkillLoopEndTime = 0;
}

void ioGrapplingBlowLoopSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ClearPreLoopBuff( pChar );

	ioAttackSkill::AttackSkillFire( pChar );

	m_SkillState = SS_ACTION;

	if( !pChar ) return;
	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage ) return;

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioGrapplingBlowLoopSkill::SetSkillPreLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	m_dwPreLoopStartTime = FRAMEGETTIME();
	m_SkillState = SS_PRE_LOOP;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_fPreLoopMotionRate > 0.0f )
		fAniRate = m_fPreLoopMotionRate;

	int iAniID = pGrp->GetAnimationIdx( m_PreLoopMotion );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	//
	bool bEnableBuff = true;
	if( pChar->IsHasCrown() && m_bDisableKingSkill )
		bEnableBuff = false;
	else if( pChar->HasBuff( BT_ABSOLUTE_PROTECTION ) )
		bEnableBuff = false;

	if( bEnableBuff )
	{
		int iCnt = m_vPreLoopBuffList.size();
		for( int i=0; i < iCnt; ++i )
		{
			ioHashString szBuffName = m_vPreLoopBuffList[i];
			if( szBuffName.IsEmpty() )
				continue;

			ioHashString szItemName;
			ioItem *pOwnerItem = GetOwnerItem();
			if( pOwnerItem )
				szItemName = pOwnerItem->GetName();

			pChar->AddNewBuff( szBuffName, pChar->GetCharName(), szItemName, this );
		}
	}
}

/*void ioGrapplingBlowLoopSkill::SetSkillLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_SkillState != SS_ACTION )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	m_dwSkillLoopStartTime = FRAMEGETTIME();
	m_SkillState = SS_LOOP;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_fSkillLoopMotionRate > 0.0f )
		fAniRate = m_fSkillLoopMotionRate;

	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );

	if( m_fCharJumpPower > 0.0f )
	{
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );
		pChar->SetJumpPower( m_fCharJumpPower );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );
		m_dwSkillLoopEndTime = m_dwSkillLoopStartTime;
		m_dwSkillLoopEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;
	}
}*/


void ioGrapplingBlowLoopSkill::SetSkillLoopState_MultiJump( ioBaseChar *pChar, int nJumpCnt )
{
	m_bReserveJumping = false;
	if( !pChar ) return;

	if( (int)m_vGrapJumpInfo.size() <= nJumpCnt )
		return;

	const StGrapInfo& JumpInfo = m_vGrapJumpInfo[ nJumpCnt ];
	/*if( m_SkillState != SS_ACTION )
		return;*/

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	m_dwSkillLoopStartTime = FRAMEGETTIME();
	m_SkillState = SS_LOOP;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	/*if( m_fSkillLoopMotionRate > 0.0f )
		fAniRate = m_fSkillLoopMotionRate;*/
	fAniRate = JumpInfo.m_fSkillLoopMotionRate;

	int iAniID = pGrp->GetAnimationIdx( JumpInfo.m_SkillLoopMotion );

	if( m_fCharJumpPower > 0.0f )
	{
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );
		pChar->SetJumpPower( JumpInfo.m_fJumpPower );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );
		m_dwSkillLoopEndTime = m_dwSkillLoopStartTime;
		m_dwSkillLoopEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;
	}

	if( nJumpCnt >= 1 && !m_GrapplingTargetName.IsEmpty() )
	{
		ioBaseChar* pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
		if( pTarget )
		{
			ioEntityGroup *pGrpTarget = pTarget->GetGroup();
			if( pGrpTarget )
			{
				pGrpTarget->ClearAllActionAni( FLOAT100, true );
				pGrpTarget->ClearAllLoopAni( FLOAT100, true );

				int iAniIDTarget = pGrpTarget->GetAnimationIdx( JumpInfo.m_SkillLoopMotionStun );
				pGrpTarget->SetLoopAni( iAniIDTarget, FLOAT100, FLOAT1, FLOAT1/JumpInfo.m_fSkillLoopMotionRateStun );
			}
		}
	}

	
}


void ioGrapplingBlowLoopSkill::SetSkillLandAttackState( ioBaseChar *pChar, int nJumpCnt )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	if( (int)m_vGrapJumpInfo.size() <= nJumpCnt )
		return;

	const StGrapInfo& JumpInfo = m_vGrapJumpInfo[ nJumpCnt ];

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	//if( m_LandAttackAttribute.m_fAttackAniRate > 0.0f )
	fAniRate = JumpInfo.m_LandAttack.m_fAttackAniRate;

	int iAniID = pGrp->GetAnimationIdx( JumpInfo.m_LandAttack.m_AttackAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	m_SkillState = SS_LAND_ATTACK;

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
		iAniID,
		JumpInfo.m_LandAttack.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fAniRate,
		JumpInfo.m_LandAttack.m_dwPreDelay );

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fAniRate;

	// Key Reserve
	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" );
}

void ioGrapplingBlowLoopSkill::SetSkillEndState( ioBaseChar *pChar, bool bSendNet )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	/*pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_LandAttackAttribute.m_fAttackAniRate > 0.0f )
		fAniRate = m_LandAttackAttribute.m_fAttackAniRate;

	int iAniID = pGrp->GetAnimationIdx( m_LandAttackAttribute.m_AttackAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;*/

	m_SkillState = SS_END;

	/*pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
		iAniID,
		m_LandAttackAttribute.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fAniRate,
		m_LandAttackAttribute.m_dwPreDelay );*/

	/*m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fAniRate;

	// Key Reserve
	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" );*/

	if( bSendNet && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_END;
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGrapplingBlowLoopSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
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
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_NO_CHANGE3 && !m_GrapplingTargetName.IsEmpty() )
	{
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	}
	else if( pWeapon->IsUseGrapplingTarget() && !m_GrapplingTargetName.IsEmpty() )
	{
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	}

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

bool ioGrapplingBlowLoopSkill::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) return false;
	if( m_SkillState != SS_ACTION )		return false;

	// 충돌처리 후 체크된 타겟을 m_GrapplingTargetName로 지정한다.
	m_GrapplingTargetName = szTargetName;

	//SetSkillLoopState( pChar );
	SetSkillLoopState_MultiJump( pChar, m_nCurJumpCnt );
	m_nCurJumpCnt++;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_LOOP;
		kPacket << pChar->GetTargetRot();
		kPacket << szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return false;

	// for NPC
	bool bNPC = false;
	bool bAttackerNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	if( ToNpcChar( pChar ) )
	{
		bAttackerNPC = true;
	}

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || (bNPC || bAttackerNPC) )
	{
		D3DXQUATERNION qtCharRot = pChar->GetTargetRot();
		pTarget->SetTargetRotToRotate( qtCharRot, true, false );
		pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
	}

	return true;
}

D3DXVECTOR3 ioGrapplingBlowLoopSkill::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_fTargetLoopGapRange > 0.0f )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fCharHeightRate );
		D3DXVECTOR3 vDir = pChar->GetMoveDir();

		if( m_fTargetLoopGapRange > 0.0f )
			vPos += m_fTargetLoopGapRange * vDir;

		//높이
		vPos.y = vPos.y + m_fTargetLoopGapHeight;

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

bool ioGrapplingBlowLoopSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_LOOP:
		return false;
	case SS_ACTION:
		if( m_dwMotionEndTime + dwActionStop < dwCurTime )
			return true;
		return false;
	case SS_END:
		//if( m_dwMotionEndTime + dwActionStop < dwCurTime )
		{
			return true;
		}
		//return false;
	}

	return false;
}

void ioGrapplingBlowLoopSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;
	if( !pStage ) return;

	int iState;
	rkPacket >> iState;

	D3DXQUATERNION qtCharRot;
	rkPacket >> qtCharRot;

	pChar->SetTargetRotToRotate( qtCharRot, true, false );

	switch( iState )
	{
	case SS_ACTION:
		{
			AttackSkillFire( pChar );
		}
		break;
	case SS_LOOP:
		{
			ioHashString szTargetName;
			rkPacket >> m_GrapplingTargetName;

			SetSkillLoopState_MultiJump( pChar, m_nCurJumpCnt );
			m_nCurJumpCnt++;

			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget && pTarget->IsNeedProcess() )
			{
				pTarget->SetTargetRotToRotate( qtCharRot, true, false );
				pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
			}
		}
		break;
	case SS_PRE_LOOP:
		break;
	case SS_JUMP_RESERVED:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			m_bReserveJumping = true;
			if( m_SkillState == SS_LAND_ATTACK )
				pChar->SetWorldPosition( vPos );
		}

		break;
	case SS_KEY_CHANGE:
		{
			int nKey;
			rkPacket >> nKey;
			m_PreDirKey = (ioUserKeyInput::DirKeyInput)nKey;
			rkPacket >> nKey;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)nKey;
		}
		break;
	case SS_END:
		SetSkillEndState(  pChar, false );
		break;
	}
}

void ioGrapplingBlowLoopSkill::ProcessLoopDir( ioBaseChar *pChar )
{
	if( m_dwLoopRotateTime == 0 )
		return;

	// 캐릭터 회전
	/*float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	float fRate = (fTimePerSec*1000) / m_dwLoopRotateTime;
	float fRotateAngle = 360.0f * fRate;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

	D3DXQUATERNION	qtNewRot;
	D3DXQuaternionRotationYawPitchRoll( &qtNewRot, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

	D3DXQUATERNION qtCharRot = pChar->GetTargetRot();
	D3DXVECTOR3 vDir = qtCharRot * D3DXVECTOR3( 0.0, 0.0, -FLOAT1 );
	vDir = qtNewRot * vDir;

	ioMath::CalcDirectionQuaternion( qtNewRot, -vDir );
	pChar->SetTargetRotToRotate( qtNewRot, true, false );*/
}

bool ioGrapplingBlowLoopSkill::IsProtected( int iDefenseBreakType ) const
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

	switch( m_SkillState )
	{
	case SS_ACTION:
		return true;
	case SS_LOOP:
	case SS_END:
		if( m_dwSkillLoopStartTime+m_dwSkillLoopProtectTime < dwCurTime )
			return false;

		return true;
	}

	return false;
}

bool ioGrapplingBlowLoopSkill::CheckLanding( ioBaseChar *pChar )
{
	/*if( !pChar ) return true;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pChar->CheckGhostState() )
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetPreHeightForGhostState();
	else
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();

	if( fHeightGap > 0.0f || !pChar->IsDownState() )
	{
		return false;
	}

	return true;*/


	//루시퍼꺼 그냥 붙여 넣기...잘된다..
	if( !pChar ) return true;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pChar->CheckGhostState() )
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetPreHeightForGhostState();
	else
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();

	float fHeightGapTarget = fHeightGap;
	ioBaseChar *pTarget = NULL;
	if( !m_GrapplingTargetName.IsEmpty() )
	{
		pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
		if( pTarget ) 
		{
			if( pTarget->CheckGhostState() )
				fHeightGapTarget = pTarget->GetWorldPosition().y - pTarget->GetPreHeightForGhostState();
			else
				fHeightGapTarget = pTarget->GetWorldPosition().y - pTarget->GetBottomHeight();
		}
	}

	if( fHeightGap > 0.0f || !pChar->IsDownState() )
	{
		if( pTarget && pTarget->GetState() == CS_GRAPPLING_WOUNDED_SKILL )
		{
			ioHashString stAttacker = pTarget->GetGrapplingAttacker();
			ioBaseChar *pAttacker = pTarget->GetBaseChar( stAttacker );
			if( pAttacker == pChar )
			{
				if(  fHeightGapTarget <= 0.0f && pChar->IsDownState() )
					return true;
			}	
		}

		return false;
	}

	return true;
}

bool ioGrapplingBlowLoopSkill::IsEnableReserve() const
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

int ioGrapplingBlowLoopSkill::GetSkillState()
{
	return (int)m_SkillState;
}

bool ioGrapplingBlowLoopSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioGrapplingBlowLoopSkill::ClearPreLoopBuff( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_vPreLoopBuffList.empty() )
		return;

	int iCnt = m_vPreLoopBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		ioHashString szBuffName = m_vPreLoopBuffList[i];

		pChar->RemoveBuff( szBuffName, false );
	}
}

void ioGrapplingBlowLoopSkill::CheckPreLoopDirKey( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() )
		return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
	if( pChar->IsChangeDirection() )
	{
		pChar->SetTargetRotToDirKey( eNewDirKey );

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SS_PRE_LOOP;
			kPacket << pChar->GetTargetRot();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioGrapplingBlowLoopSkill::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	bool bChangeDir = false;
	bool bChangeUpDown = false;

	// Check Dir
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		bChangeDir = true;

		m_PreDirKey = m_CurDirKey;
		m_CurDirKey = eNewDirKey;
	}

	//SS_KEY_CHANGE
	if( bChangeDir )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SS_KEY_CHANGE;
		kPacket << pOwner->GetTargetRot();
		kPacket << (int)m_PreDirKey;
		kPacket << (int)m_CurDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioGrapplingBlowLoopSkill::ProcessRotate( ioBaseChar *pOwner )
{
	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateSpeed;
	float fRotateAngle = 360.0f * fRate;

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vCurMoveDir = qtCurRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vCurMoveDir, &vCurMoveDir );

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	float fDotValue = D3DXVec3Dot( &vCurMoveDir, &vTargetDir );
	float fGapAngle = RADtoDEG( acosf(fDotValue) );

	if( fDotValue >= 0.98f )
	{
		pOwner->SetTargetRotToRotate( qtRotate, true );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -0.98f )
	{
		vAxis = ioMath::UNIT_Y;
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vTargetDir, &vCurMoveDir );
		D3DXVec3Normalize( &vAxis, &-vAxis );
	}

	D3DXQUATERNION qtNewRot;
	if( fGapAngle < fRotateAngle )
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fGapAngle) );
	else
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle) );

	D3DXVECTOR3 vNewDir = qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );
	pOwner->SetTargetRotToRotate( qtNewRot, true );
}


void ioGrapplingBlowLoopSkill::ProcessJumpMove( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( !pOwner ) return;
	if( !pStage ) return;

	if( (int)m_vGrapJumpInfo.size() <= max( m_nCurJumpCnt-1, 0 ) )
		return;

	const StGrapInfo& JumpInfo = m_vGrapJumpInfo[ max(m_nCurJumpCnt-1, 0) ];

	if( JumpInfo.m_dwJumpMoveSpeed == 0.0f ) return;
	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt = vMoveDir * JumpInfo.m_dwJumpMoveSpeed * fDivideTimePerSec;
		if( vMoveAmt != ioMath::VEC3_ZERO )
		{
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vTotalMove, bCol ) )
		{
			pOwner->Translate( vTotalMove );
		}
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pOwner->CheckAxisLimit( &vPos ) )
		pOwner->SetWorldPosition( vPos );
}


bool ioGrapplingBlowLoopSkill::IsNoDropState() const
{
	if( m_SkillState == SS_LAND_ATTACK )
		return true;

	return false;
}