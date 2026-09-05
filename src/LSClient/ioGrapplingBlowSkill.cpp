

#include "stdafx.h"

#include "ioNpcChar.h"
#include "ioPlayStage.h"

#include "ioGrapplingBlowSkill.h"

#include "WeaponDefine.h"

ioGrapplingBlowSkill::ioGrapplingBlowSkill()
{
	m_SkillState = SS_NONE;

	m_dwSkillLoopStartTime = 0;
	m_dwSkillLoopEndTime = 0;

	m_dwPreLoopStartTime = 0;
}

ioGrapplingBlowSkill::ioGrapplingBlowSkill( const ioGrapplingBlowSkill &rhs )
: ioAttackSkill( rhs ),
 m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
 m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
 m_fGrapplingRange( rhs.m_fGrapplingRange ),
 m_fGrapplingMinAngle( rhs.m_fGrapplingMinAngle ),
 m_fGrapplingMaxAngle( rhs.m_fGrapplingMaxAngle ),
 m_fTargetLoopGapRange( rhs.m_fTargetLoopGapRange ),
 m_dwLoopRotateTime( rhs.m_dwLoopRotateTime ),
 m_dwSkillLoopProtectTime( rhs.m_dwSkillLoopProtectTime ),
 m_LandAttackAttribute( rhs.m_LandAttackAttribute ),
 m_fCharJumpPower( rhs.m_fCharJumpPower ),
 m_fCharHeightRate( rhs.m_fCharHeightRate ),
 m_bUsePreLoopState( rhs.m_bUsePreLoopState ),
 m_PreLoopMotion( rhs.m_PreLoopMotion ),
 m_fPreLoopMotionRate( rhs.m_fPreLoopMotionRate ),
 m_dwPreLoopDuration( rhs.m_dwPreLoopDuration ),
 m_vPreLoopBuffList( rhs.m_vPreLoopBuffList )
{
	m_SkillState = SS_NONE;

	m_dwSkillLoopStartTime = 0;
	m_dwSkillLoopEndTime = 0;
}

ioGrapplingBlowSkill::~ioGrapplingBlowSkill()
{
}

void ioGrapplingBlowSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;

	m_fSkillLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );

	m_fTargetLoopGapRange = rkLoader.LoadFloat_e( "skill_target_loop_gap_range", 0.0f );

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
	m_LandAttackAttribute.Init();
	LoadAttackAttribute_e( "land_attack", m_LandAttackAttribute, rkLoader );

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
}

ioSkill* ioGrapplingBlowSkill::Clone()
{
	return new ioGrapplingBlowSkill( *this );
}

bool ioGrapplingBlowSkill::OnSkillStart( ioBaseChar *pChar )
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

	return true;
}

void ioGrapplingBlowSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
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
			}

			if( m_fCharJumpPower > 0.0f )
			{
				if( CheckLanding( pChar ) )
				{
					SetSkillEndState( pChar, true );
					return;
				}
			}
			else if( m_dwSkillLoopEndTime < dwCurTime )
			{
				SetSkillEndState( pChar, true );
				return;
			}

			ProcessLoopDir( pChar );
		}
		break;
	case SS_END:
		break;
	}
	
}

void ioGrapplingBlowSkill::OnSkillEnd( ioBaseChar *pChar )
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

void ioGrapplingBlowSkill::AttackSkillFire( ioBaseChar *pChar )
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

void ioGrapplingBlowSkill::SetSkillPreLoopState( ioBaseChar *pChar )
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

void ioGrapplingBlowSkill::SetSkillLoopState( ioBaseChar *pChar )
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
}

void ioGrapplingBlowSkill::SetSkillEndState( ioBaseChar *pChar, bool bSendNet )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_LandAttackAttribute.m_fAttackAniRate > 0.0f )
		fAniRate = m_LandAttackAttribute.m_fAttackAniRate;

	int iAniID = pGrp->GetAnimationIdx( m_LandAttackAttribute.m_AttackAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	m_SkillState = SS_END;

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iAniID,
										m_LandAttackAttribute.m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fAniRate,
										m_LandAttackAttribute.m_dwPreDelay );

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fAniRate;

	// Key Reserve
	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" );
}

void ioGrapplingBlowSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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

bool ioGrapplingBlowSkill::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) return false;
	if( m_SkillState != SS_ACTION )		return false;

	// 충돌처리 후 체크된 타겟을 m_GrapplingTargetName로 지정한다.
	m_GrapplingTargetName = szTargetName;

	SetSkillLoopState( pChar );

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

D3DXVECTOR3 ioGrapplingBlowSkill::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_fTargetLoopGapRange > 0.0f )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fCharHeightRate );
		D3DXVECTOR3 vDir = pChar->GetMoveDir();

		if( m_fTargetLoopGapRange > 0.0f )
			vPos += m_fTargetLoopGapRange * vDir;

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

bool ioGrapplingBlowSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_LOOP:
		return false;
	case SS_ACTION:
	case SS_END:
		if( m_dwMotionEndTime + dwActionStop < dwCurTime )
		{
			return true;
		}
		return false;
	}

	return false;
}

void ioGrapplingBlowSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
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

			SetSkillLoopState( pChar );

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
	}
}

void ioGrapplingBlowSkill::ProcessLoopDir( ioBaseChar *pChar )
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
	pChar->SetTargetRotToRotate( qtNewRot, true, false );
}

bool ioGrapplingBlowSkill::IsProtected( int iDefenseBreakType ) const
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

bool ioGrapplingBlowSkill::CheckLanding( ioBaseChar *pChar )
{
	if( !pChar ) return true;

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

	return true;
}

bool ioGrapplingBlowSkill::IsEnableReserve() const
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

int ioGrapplingBlowSkill::GetSkillState()
{
	return (int)m_SkillState;
}

bool ioGrapplingBlowSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioGrapplingBlowSkill::ClearPreLoopBuff( ioBaseChar *pChar )
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

void ioGrapplingBlowSkill::CheckPreLoopDirKey( ioBaseChar *pChar )
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





