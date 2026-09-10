

#include "stdafx.h"



#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioTargetWarpSkill.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioTargetWarpSkill::ioTargetWarpSkill()
{
	m_bSetHandMesh = false;
	m_SkillState = SS_NONE;
}

ioTargetWarpSkill::ioTargetWarpSkill( const ioTargetWarpSkill &rhs )
: ioAttackSkill( rhs ),
 m_WarpMotion( rhs.m_WarpMotion ),
 m_fWarpMotionRate( rhs.m_fWarpMotionRate ),
 m_fWarpOffsetHeight( rhs.m_fWarpOffsetHeight ),
 m_TargetWarpSkillType( rhs.m_TargetWarpSkillType ),
 m_fTeleportOffset( rhs.m_fTeleportOffset ),
 m_dwWarpStandbyTime( rhs.m_dwWarpStandbyTime ),
 m_dwWarpEnableTime( rhs.m_dwWarpEnableTime )
{
	m_bSetHandMesh = false;
	m_SkillState = SS_NONE;
	m_pTargetChar = NULL;
	m_dwWarpBackTime = 0;
	m_dwWarpEnableCheckTime = 0;
}

ioTargetWarpSkill::~ioTargetWarpSkill()
{
}

void ioTargetWarpSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "warp_motion", "", szBuf, MAX_PATH );
	m_WarpMotion = szBuf;

	m_fWarpMotionRate = rkLoader.LoadFloat_e( "warp_motion_rate", FLOAT1 );
	m_fWarpOffsetHeight = rkLoader.LoadFloat_e( "warp_offset_height", 0.0f );

	m_TargetWarpSkillType = (TargetWarpSkillType)rkLoader.LoadInt_e( "targetwarp_skill_type", 0 );
	m_fTeleportOffset = rkLoader.LoadFloat_e( "teleport_offset", 0.0f );
	m_dwWarpStandbyTime = rkLoader.LoadInt_e( "warp_standby_time", 1000 );
	m_dwWarpEnableTime = rkLoader.LoadInt_e( "warp_enable_time", 0 );
}

ioSkill* ioTargetWarpSkill::Clone()
{
	return new ioTargetWarpSkill( *this );
}

bool ioTargetWarpSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	ioAttackSkill::OnSkillStart( pChar );

	DWORD dwCurTime = FRAMEGETTIME();
	pChar->SetSkillProtection();

	m_dwAttackEndTime = 0;

	InitExtraAniJump();

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;
	
	m_dwEnableReserveTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = dwCurTime + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = dwCurTime + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();

	m_bSetHandMesh = false;
	m_SkillState = SS_NONE;
	m_fCurChargeRate = 0.0f;
	m_pTargetChar = NULL;
	m_dwWarpBackTime = 0;
	m_dwWarpEnableCheckTime = 0;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		AttackSkillFire( pChar );
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

	return true;
}

void ioTargetWarpSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );
	
	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}

	m_bSetHandMesh = false;
	m_SkillState = SS_NONE;
	m_fCurChargeRate = 0.0f;
	m_pTargetChar = NULL;
}

bool ioTargetWarpSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioTargetWarpSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioBaseChar *pChar = NULL;
	if( pWeapon )
		pChar = pWeapon->GetOwner();

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, true );
		}
	}
}

void ioTargetWarpSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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

				AttackSkillFire( pChar );
			}
		}
		break;
	case SS_FIRE:
		if( m_TargetWarpSkillType == ST_WARP_BACK && pChar->IsJumpKeyDown() && m_dwWarpEnableCheckTime < dwCurTime )
			SetWarpBackCancelState( pChar );
		else if( m_TargetWarpSkillType == ST_WARP_JUMP )
			CheckExtraAniJump( pChar );
		else
		{
			pChar->SetCurJumpPowerAmt( 0.0f );
			pChar->SetForcePowerAmt( 0.0f );
			pChar->SetGravityAmt( 0.0f );
		}
		break;
	case SS_WARP_BACK:
		if( m_dwWarpBackTime < FRAMEGETTIME() )
			SetWarpBackEndState( pChar );
		else
		{
			pChar->SetCurJumpPowerAmt( 0.0f );
			pChar->SetForcePowerAmt( 0.0f );
			pChar->SetGravityAmt( 0.0f );
		}
		break;
	}
}

bool ioTargetWarpSkill::IsProtected( int iDefenseBreakType ) const
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
	case SS_FIRE:
		return true;
	}

	return false;
}

void ioTargetWarpSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{	
	case SS_WARP_CANCEL_END:
		SetWarpBackCancelState( pChar );
		break;
	case SS_WARP_BACK_END:
		SetWarpBackEndState( pChar );
		break;
	}
}

bool ioTargetWarpSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
		return false;
	}

	if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
		return false;
	if( IsAttackEndState() )
		return false;

	return true;
}

bool ioTargetWarpSkill::IsAttackEndState() const
{
	if( m_TargetWarpSkillType != ST_WARP_BACK )
		return false;
	if( m_SkillState != SS_FIRE )
		return false;
	if( m_dwMotionEndTime > FRAMEGETTIME() )
		return false;
	if( m_fExtraAniJumpPower == 0.f )
		return false;

	return true;
}

void ioTargetWarpSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioAttackSkill::AttackSkillFire( pChar );

	m_SkillState = SS_FIRE;
	m_dwWarpEnableCheckTime = FRAMEGETTIME() + m_dwWarpEnableTime;

	if( m_TargetWarpSkillType == ST_WARP_BACK )
		pChar->SetSkillNextJumpAmt( m_fExtraAniJumpPower );
}

void ioTargetWarpSkill::SetWarpJumpMotionState( ioBaseChar *pChar, ioWeapon *pWeapon )
{
	if( !pChar ) return;
	if( !pWeapon ) return;
	if( m_TargetWarpSkillType != ST_WARP_JUMP || m_SkillState != SS_FIRE )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_WarpMotion );
	float fTimeRate = m_fWarpMotionRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwFireMotionEndTime = m_dwMotionEndTime;

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( pStage )
	{
		float fBoundHeight = pStage->GetBoundHeight();
		D3DXVECTOR3 vTargetPos = pWeapon->GetPosition();
		vTargetPos.y += m_fWarpOffsetHeight;

		D3DXVECTOR3 vBoundPos = vTargetPos;
		vBoundPos.y = fBoundHeight;
		
		float fMapHeight = pStage->GetMapHeight( vBoundPos.x, vBoundPos.z );
		if( vTargetPos.y <= fMapHeight )
			vTargetPos.y = fMapHeight;

		pChar->SetWorldPosition( vTargetPos );

		pStage->SetCameraToOwnerExplicit( pChar );
	}
	m_SkillState = SS_WARP_JUMP;
}

void ioTargetWarpSkill::SetWarpBackMotionState( ioBaseChar *pChar, ioBaseChar *pTarget )
{
	if( !pChar ) return;
	if( !pTarget ) return;
	if( m_TargetWarpSkillType != ST_WARP_BACK || m_SkillState != SS_FIRE )
		return;

	CharState eState = pTarget->GetState();
	if( eState != CS_WOUNDED && eState != CS_BLOW_WOUNDED && 
		eState != CS_BOUND_BLOW_WOUNDED && eState != CS_BLOW_EXTEND_WOUND )
		return;

	m_pTargetChar = pTarget;
	m_SkillState = SS_WARP_BACK;
	m_dwWarpBackTime = FRAMEGETTIME() + m_dwWarpStandbyTime;
	m_dwMotionEndTime = m_dwWarpBackTime + 100;
}

void ioTargetWarpSkill::SetWarpBackCancelState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_TargetWarpSkillType != ST_WARP_BACK || m_SkillState != SS_FIRE )
		return;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponBaseIndex );
	if( !pWeapon )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_WarpMotion );
	float fTimeRate = m_fWarpMotionRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwFireMotionEndTime = m_dwMotionEndTime;

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( pStage )
	{
		float fBoundHeight = pStage->GetBoundHeight();
		D3DXVECTOR3 vTargetPos = pWeapon->GetPosition();
		D3DXVECTOR3 vBoundPos = vTargetPos;
		vBoundPos.y = fBoundHeight;
		
		float fMapHeight = pStage->GetMapHeight( vBoundPos.x, vBoundPos.z );
		if( vTargetPos.y <= fMapHeight )
			vTargetPos.y = fMapHeight;

		pChar->SetWorldPosition( vTargetPos );
		pStage->SetCameraToOwnerExplicit( pChar );
	}
	m_SkillState = SS_WARP_CANCEL_END;

	pWeapon->SetWeaponDead();

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << (int)m_SkillState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetWarpSkill::SetWarpBackEndState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( !m_pTargetChar ) return;
	if( m_TargetWarpSkillType != ST_WARP_BACK || m_SkillState != SS_WARP_BACK )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_WarpMotion );
	float fTimeRate = m_fWarpMotionRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwFireMotionEndTime = m_dwMotionEndTime;

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( pStage )
	{
		D3DXVECTOR3 vOffset = m_pTargetChar->GetMoveDir() * m_fTeleportOffset;
		D3DXVECTOR3 vTargetPos = m_pTargetChar->GetWorldPosition();
		D3DXVECTOR3 vTeleportPos = vTargetPos - vOffset;

		pChar->SetWorldPosition( vTeleportPos );
		pChar->SetTargetRotToTargetPos( vTargetPos, true );
		pStage->SetCameraToOwnerExplicit( pChar );
	}
	m_SkillState = SS_WARP_BACK_END;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << (int)m_SkillState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioTargetWarpSkill::IsEndState() const
{
	if( m_SkillState == SS_WARP_JUMP || m_SkillState == SS_WARP_BACK_END || m_SkillState == SS_WARP_CANCEL_END )
	{
		return true;
	}

	return false;
}





