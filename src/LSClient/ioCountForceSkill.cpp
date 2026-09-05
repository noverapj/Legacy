

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioCountForceSkill.h"

ioCountForceSkill::ioCountForceSkill()
{
	ClearData();
}

ioCountForceSkill::ioCountForceSkill( const ioCountForceSkill &rhs )
	: ioCountSkill( rhs ),
	m_NormalGlideInfo( rhs.m_NormalGlideInfo ),
	m_EndAnimation( rhs.m_EndAnimation ),
	m_fEndAniRate( rhs.m_fEndAniRate ),
	m_JumpGlideInfo( rhs.m_JumpGlideInfo ),
	m_MoveEffect( rhs.m_MoveEffect ),
	m_fLand_Forward_Force( rhs.m_fLand_Forward_Force ),
	m_fLand_Backward_Force( rhs.m_fLand_Backward_Force ),
	m_fJumpForward_Force( rhs.m_fJumpForward_Force ),
	m_fJumpBackward_Force( rhs.m_fJumpBackward_Force ),
	m_fLand_Forward_Friction( rhs.m_fLand_Forward_Friction ),
	m_fLand_Backward_Friction( rhs.m_fLand_Backward_Friction ),
	m_fJumpForward_Friction( rhs.m_fJumpForward_Friction ),
	m_fJumpBackward_Friction( rhs.m_fJumpBackward_Friction ),
	m_fMoveEffectPosRange( rhs.m_fMoveEffectPosRange ),
	m_stMoveAni( rhs.m_stMoveAni ),
	m_fMoveAniRate( rhs.m_fMoveAniRate )
{
	ClearData();
}

ioCountForceSkill::~ioCountForceSkill()
{
	InitExtraAniJump();
}

void ioCountForceSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioCountSkill::LoadProperty( rkLoader );

	LoadGlideInfo( "normal", m_NormalGlideInfo, rkLoader );
	LoadGlideInfo( "jump", m_JumpGlideInfo, rkLoader );


	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "end_animation", "", szBuf, MAX_PATH );
	m_EndAnimation = szBuf;

	m_fEndAniRate = rkLoader.LoadFloat_e( "end_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "move_effect", "", szBuf, MAX_PATH );
	m_MoveEffect = szBuf;

	//////////////////////////////////////////////////////////////////////////
	m_fLand_Forward_Force = rkLoader.LoadFloat_e( "land_forward_force", FLOAT1 );
	m_fLand_Backward_Force = rkLoader.LoadFloat_e( "land_backward_force", FLOAT1 );
	m_fJumpForward_Force = rkLoader.LoadFloat_e( "jump_forward_force", FLOAT1 );
	m_fJumpBackward_Force = rkLoader.LoadFloat_e( "jump_backward_force", FLOAT1 );

	m_fLand_Forward_Friction = rkLoader.LoadFloat_e( "land_forward_friction", FLOAT1 );
	m_fLand_Backward_Friction = rkLoader.LoadFloat_e( "land_backward_friction", FLOAT1 );
	m_fJumpForward_Friction = rkLoader.LoadFloat_e( "jump_forward_friction", FLOAT1 );
	m_fJumpBackward_Friction = rkLoader.LoadFloat_e( "jump_backward_friction", FLOAT1 );

	m_fMoveEffectPosRange = rkLoader.LoadFloat_e( "move_effect_pos_range", 0.f );
	rkLoader.LoadString_e( "move_ani", "", szBuf, MAX_PATH );
	m_stMoveAni = szBuf;
	m_fMoveAniRate = rkLoader.LoadFloat_e( "move_ani_rate", FLOAT1 );

}

void ioCountForceSkill::ClearData()
{
	SetSkillState( SS_NONE );

	m_fMoveRange = 0;
	m_dwMotionEndTime = 0;
	m_dwEnableReserveTime = 0;

	m_bTargetRot = false;
	m_bLeftRot = false;

	m_dwMoveEffectID = -1;

	m_bSetHandMesh = false;

	m_vMoveDir = ioMath::VEC3_ZERO;

	m_dwProtectedLoopTime = 0;
}

void ioCountForceSkill::LoadGlideInfo( const char *szType, GlideInfo &rkInfo, ioINILoader &rkLoader )
{
	char szKey[MAX_PATH] = "", szBuf[MAX_PATH] = "";

	wsprintf_e( szKey, "%s_glide_animation", szType );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	rkInfo.m_Animation = szBuf;

	wsprintf_e( szKey, "%s_glide_ani_rate", szType );
	rkInfo.m_fAniRate = rkLoader.LoadFloat( szKey, FLOAT1 );

	wsprintf_e( szKey, "%s_glide_delay_animation", szType );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	rkInfo.m_DelayAnimation = szBuf;

	wsprintf_e( szKey, "%s_glide_delay_duration", szType );
	rkInfo.m_dwDelayDuration = (DWORD)rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "%s_glide_delay_ani_rate", szType );
	rkInfo.m_fDelayAniRate = rkLoader.LoadFloat( szKey, FLOAT1 );

	wsprintf_e( szKey, "%s_glide_charging_duration", szType );
	rkInfo.m_dwChargingDuration = (DWORD)rkLoader.LoadInt( szKey, 0 );
}

ioSkill* ioCountForceSkill::Clone()
{
	return new ioCountForceSkill( *this );
}

bool ioCountForceSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	ioAttackableItem *pAttackable = ToAttackableItem( GetOwnerItem() );
	if( pAttackable )
	{
		ioHashString szSkillMotion = GetSkillMotionName();

		if( !pAttackable->IsCanFire( pChar ) )
			return false;
	}

	if( m_iCurUseCount < m_iNeedCount )
		return false;

	return true;
}

bool ioCountForceSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioCountSkill::OnSkillStart(pChar) )
		return false;

	ClearData();

	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	return true;
}

void ioCountForceSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_dwPreDelayStartTime == 0 )
	{
		CheckExtraAniJump( pChar );
		CheckAniRotate( pChar );
	}

	switch( m_SkillState )
	{
	case SS_NONE:
		if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + GetPreDelayTime() < FRAMEGETTIME() )
		{
			m_dwPreDelayStartTime = 0;

			EndPreDelayEffect( pChar );
			SetLoopState( pChar );
		}
		break;
	case SS_GLIDE_LOOP:
		ProcessLoopState( pChar );
		break;
	case SS_GLIDE_MOVING:
		ProcessGlideState( pChar );
		break;
	}
}

void ioCountForceSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioCountSkill::OnSkillEnd( pChar );

	pChar->SetSkillMoveEnable( false, false );

	if( m_dwMoveEffectID != -1 )
	{
		pChar->DestroyMapEffect( m_dwMoveEffectID );
		m_dwMoveEffectID = -1;
	}

	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	ClearData();
}

bool ioCountForceSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	ioHashString szPreDelay = GetPreDelayAniName();

	if( szPreDelay.IsEmpty() )
	{
		m_dwPreDelayStartTime = 0;
		return false;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( szPreDelay, FLOAT100 );

	m_dwPreDelayStartTime = FRAMEGETTIME();
	StartPreDelayEffect( pChar );

	return true;
}


void ioCountForceSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )
		return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SC_MOVING:
		{
			D3DXVECTOR3 vPos;
			bool bLand;
			bool bJumpKey;
			D3DXVECTOR3 vMoveDir;
			rkPacket >> vPos;
			rkPacket >> bLand;
			rkPacket >> bJumpKey;
			rkPacket >> vMoveDir;
			pChar->SetWorldPosition( vPos );
			m_vMoveDir = vMoveDir;
			SetGlideState( pChar, bLand, bJumpKey );
		}
		break;
	case SC_END:
		{
			SetEndState( pChar );
		}
		break;
	}
}

bool ioCountForceSkill::IsAttackEndState() const
{
	if( m_dwAttackEndTime > 0 && m_dwAttackEndTime < FRAMEGETTIME() )
		return true;

	if( m_SkillState == SS_GLIDE_END )
		return true;

	return false;
}

bool ioCountForceSkill::IsNoDropState() const
{
	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_GLIDE_LOOP:
	case SS_GLIDE_MOVING:
		return true;
	}

	return false;
}

bool ioCountForceSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_GLIDE_MOVING:
		return false;
	case SS_GLIDE_LOOP:
	case SS_GLIDE_END:
		if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
			return false;
	}

	return true;
}

void ioCountForceSkill::SetLoopState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage )
		return;

	if( m_SkillState == SS_GLIDE_LOOP )
		return;

	SetSkillState( SS_GLIDE_LOOP );

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwProtectedLoopTime = dwCurTime;

	GlideInfo kInfo = GetGlideInfo();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx(  kInfo.m_DelayAnimation );
	if( iAniID == -1 ) 
		return;

	pGrp->SetLoopAni( kInfo.m_DelayAnimation, FLOAT100, FLOAT1, FLOAT1 / kInfo.m_fDelayAniRate );

	m_dwProtectedLoopTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * kInfo.m_fDelayAniRate;

	m_dwMotionEndTime = FRAMEGETTIME() + kInfo.m_dwDelayDuration;

	if( pChar->IsNeedProcess() )
	{
		m_bLandSkill = true;
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		//앞쪽높이
		vPos = vPos + (vDir*m_fMoveEffectPosRange);
		float fHeight = pStage->GetMapHeight( vPos.x, vPos.z );

		//내위치높이
		float fCharMapHeight = pStage->GetMapHeight( pChar->GetWorldPosition().x, pChar->GetWorldPosition().z );

		if( fHeight > FLOAT10 )
			vPos.y = fHeight;
		else if( fCharMapHeight > FLOAT10 )
			vPos.y = fCharMapHeight;

		if( pChar->GetWorldPosition().y > (fCharMapHeight+FLOAT10) )
			m_bLandSkill = false;

		ioMapEffect *pMapEffect = pChar->CreateMapEffectBySkill( m_MoveEffect, vPos );
		if( pMapEffect )
		{
			m_dwMoveEffectID = pMapEffect->GetUniqueID();
		}

		if( m_bLandSkill )
			m_vMoveDir = vDir;
		else
			m_vMoveDir = vPos - pChar->GetWorldPosition();

		D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
	}

	pChar->SetSkillMoveEnable( false, false );
}


void ioCountForceSkill::SetGlideState( ioBaseChar *pChar, bool bLandSkill, bool bJumpKey )
{
	if( !pChar )
		return;

	if( m_SkillState == SS_GLIDE_MOVING )
		return;

	SetSkillState( SS_GLIDE_MOVING );

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_stMoveAni );
	if( iAniID == -1 )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fMoveAniRate );
	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime  += pGrp->GetAnimationFullTime( iAniID ) * m_fMoveAniRate;

	pChar->SetSkillMoveEnable( false, false );
	pChar->CheckCharColSkipSkillTime( iAniID, m_fMoveAniRate, 0 );

	if( m_dwMoveEffectID != -1 )
	{
		pChar->DestroyMapEffect( m_dwMoveEffectID );
		m_dwMoveEffectID = -1;
	}

	if( bLandSkill )
	{
		if( bJumpKey )
			pChar->AddReservedSliding2( 0, m_fLand_Forward_Force, m_fLand_Forward_Friction, m_vMoveDir );
		else
			pChar->AddReservedSliding2( 0, m_fLand_Backward_Force, m_fLand_Backward_Friction, -m_vMoveDir );
	}
	else
	{
		if( bJumpKey )
			pChar->AddReservedSliding2( 0, m_fJumpForward_Force, m_fJumpForward_Friction, m_vMoveDir );
		else
			pChar->AddReservedSliding2( 0, m_fJumpBackward_Force, m_fJumpBackward_Friction, -m_vMoveDir );
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SC_MOVING;
		kPacket << pChar->GetWorldPosition();
		kPacket << bLandSkill;
		kPacket << bJumpKey;
		kPacket << m_vMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioCountForceSkill::SetEndState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_SkillState == SS_GLIDE_END )
		return;

	SetSkillState( SS_GLIDE_END );
	m_dwMotionEndTime = 0;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_EndAnimation );
	if( iAniID == -1 )	
		return;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fEndAniRate );
	m_dwMotionEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime( iAniID ) * m_fEndAniRate;

	pChar->SetSkillMoveEnable( false, false );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SC_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCountForceSkill::ProcessLoopState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	pChar->SetCurMoveSpeed( 0 );

	if( pChar->IsNeedProcess() )
	{
		if( m_dwMotionEndTime < FRAMEGETTIME() )
			SetEndState( pChar );

		else if( pChar->IsJumpKey() )
		{
			SetGlideState( pChar, m_bLandSkill, true );
		}
		else if( pChar->IsDefenseKey() )
		{
			SetGlideState( pChar, m_bLandSkill, false );
		}
	}
}

void ioCountForceSkill::ProcessGlideState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( !pChar->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( dwCurTime > m_dwMotionEndTime )
		SetEndState( pChar );
}


bool ioCountForceSkill::CheckLanding( ioBaseChar *pChar )
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

	if( m_SkillUseType == SUT_NORMAL && m_SkillState == SS_GLIDE_MOVING )
		return false;

	return true;
}


const ioCountForceSkill::GlideInfo &ioCountForceSkill::GetGlideInfo() const
{
	if( m_SkillUseType == SUT_JUMP )
		return m_JumpGlideInfo;

	return m_NormalGlideInfo;
}


bool ioCountForceSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )
			return false;
	}


	DWORD dwCurTime = FRAMEGETTIME();

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	if( m_dwProtectedLoopTime > dwCurTime )
		return true;

	return false;
}