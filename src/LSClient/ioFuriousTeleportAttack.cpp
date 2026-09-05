

#include "stdafx.h"

#include "ioFuriousTeleportAttack.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "WeaponDefine.h"

ioFuriousTeleportAttack::ioFuriousTeleportAttack()
{
	Init();
}

ioFuriousTeleportAttack::ioFuriousTeleportAttack( const ioFuriousTeleportAttack &rhs )
	: ioAttackSkill( rhs ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_dwSkillJumpPower( rhs.m_dwSkillJumpPower ),	
	m_vTeleportOffset( rhs.m_vTeleportOffset ),
	m_nTeleportMaxCount( rhs.m_nTeleportMaxCount ),
	m_TeleportAttack( rhs.m_TeleportAttack ),
	m_szTeleportStartEffect( rhs.m_szTeleportStartEffect ),
	m_szTeleportEndEffect( rhs.m_szTeleportEndEffect ),
	m_Attack( rhs.m_Attack )
{
	Init();
}

ioFuriousTeleportAttack::~ioFuriousTeleportAttack()
{
}

void ioFuriousTeleportAttack::Init()
{
	m_SkillState			= SS_PRE;

	m_nCurrTeleportCount	= 0;

	m_bFinishAttack			= false;

	m_SkillEndJumpType		= SEJT_DEFAULT;
	m_CurrDirKey			= ioUserKeyInput::DKI_NONE;
}

void ioFuriousTeleportAttack::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );
	
	char szBuf[MAX_PATH];

	m_dwSkillProtectTime	= rkLoader.LoadInt_e( "skill_protect_time", 0 );
	m_dwSkillJumpPower		= rkLoader.LoadInt_e( "skill_jump_power", 0 );
		
	m_vTeleportOffset.x		= rkLoader.LoadFloat_e( "teleport_offset_x", 0.0f );
	m_vTeleportOffset.y		= rkLoader.LoadFloat_e( "teleport_offset_y", 0.0f );
	m_vTeleportOffset.z		= rkLoader.LoadFloat_e( "teleport_offset_z", 0.0f );

	m_nTeleportMaxCount		= rkLoader.LoadInt_e( "teleport_max_count", 0 );
	LoadAttackAttribute( "teleport", m_TeleportAttack, rkLoader );

	rkLoader.LoadString_e( "teleport_start_effect", "", szBuf, MAX_PATH );
	m_szTeleportStartEffect = szBuf;
	rkLoader.LoadString_e( "teleport_end_effect", "", szBuf, MAX_PATH );
	m_szTeleportEndEffect = szBuf;

	LoadAttackAttribute( "attack", m_Attack, rkLoader );	
}

ioSkill* ioFuriousTeleportAttack::Clone()
{
	return new ioFuriousTeleportAttack( *this );
}

bool ioFuriousTeleportAttack::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	Init();

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	return true;
}

void ioFuriousTeleportAttack::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	switch( m_SkillState )
	{
	case SS_PRE:
		{
			CheckPreState( pChar, pStage );
		}
		break;
	case SS_ATTACK:
		{
			CheckActionState( pChar, pStage );
		}
		break;
	case SS_TELEPORT:
		{
			CheckTeleportState( pChar, pStage );
		}
		break;
	case SS_FINISH_ATTACK:
		{
			CheckFinishAttackState( pChar, pStage );
		}
		break;
	case SS_END:
		{
		}
		break;
	}
}

void ioFuriousTeleportAttack::CheckPreState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelay = GetPreDelayTime();

	if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
	{	
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pChar );
		SetSkillActionState( pChar, pStage );
	}
}

void ioFuriousTeleportAttack::SetSkillActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_SkillState = SS_ATTACK;
	ioAttackSkill::AttackSkillFire( pChar );
}

void ioFuriousTeleportAttack::CheckActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pChar->IsNeedProcess() )
		return;

	DWORD dwCurrTime = FRAMEGETTIME();
	if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < dwCurrTime )
	{
		if( m_CurrDirKey != ioUserKeyInput::DKI_NONE )
		{			
			SetSkillTeleportState( pChar, pStage );
		}
		else
		{
			SetEndState( pChar, pStage );
		}
	}
	else
	{
		if( pChar->CheckCurDirKey() != ioUserKeyInput::DKI_NONE )
			m_CurrDirKey = pChar->CheckCurDirKey();
	}
}

void ioFuriousTeleportAttack::SetSkillTeleportState( ioBaseChar* pChar, ioPlayStage* pStage )
{
	m_SkillState = SS_TELEPORT;	
	pChar->SetTargetRotToDirKey( m_CurrDirKey );
	
	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << pChar->GetWorldOrientation();
		kPacket << pChar->GetWorldPosition();
		kPacket << (byte)SSS_TELEPORT;
		kPacket	<< (byte)m_CurrDirKey;
		kPacket	<< m_nCurrTeleportCount;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_CurrDirKey = ioUserKeyInput::DKI_NONE;
	pStage->CreateMapEffect( m_szTeleportStartEffect, pChar->GetWorldPosition(), D3DXVECTOR3(1.0f,1.0f,1.0f) );
	ioAttackSkill::AttackSkillFire( pChar );

	D3DXVECTOR3 vNewPos	= pChar->GetWorldPosition() + pChar->GetTargetRot() * m_vTeleportOffset;
	float fMapHeight	= pStage->GetMapHeightNoneEntity( vNewPos.x, vNewPos.z );

	if ( fMapHeight > 0.0f )
		vNewPos.y = fMapHeight;

	pChar->SetWorldPosition( vNewPos );
	pStage->CreateMapEffect( m_szTeleportEndEffect, vNewPos, D3DXVECTOR3( FLOAT1,1.0f,1.0f ) );

	m_nCurrTeleportCount++;
}

void ioFuriousTeleportAttack::CheckTeleportState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pChar->IsNeedProcess() )
		return;

	DWORD dwCurrTime = FRAMEGETTIME();
	if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < dwCurrTime )
	{
		if( m_bFinishAttack )
		{
			SetFinishAttackState( pChar );
			return;
		}

		if( m_CurrDirKey != ioUserKeyInput::DKI_NONE && m_nCurrTeleportCount < m_nTeleportMaxCount )
		{
			SetSkillTeleportState( pChar, pStage );
		}
		else
		{
			SetEndState( pChar, pStage );
		}
	}
	else
	{
		if( pChar->CheckCurDirKey() != ioUserKeyInput::DKI_NONE )
			m_CurrDirKey = pChar->CheckCurDirKey();

		if( pChar->IsAttackKey() )
			m_bFinishAttack = true;
	}
}

void ioFuriousTeleportAttack::SetFinishAttackState( ioBaseChar* pChar )
{
	m_SkillState = SS_FINISH_ATTACK;
	ioAttackSkill::AttackSkillFire( pChar );

	if( pChar->IsNeedProcess() && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();		
		kPacket << pChar->GetWorldOrientation();
		kPacket << pChar->GetWorldPosition();
		kPacket << (byte)SSS_FINISH_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFuriousTeleportAttack::CheckFinishAttackState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	DWORD dwCurrTime = FRAMEGETTIME();
	if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < dwCurrTime )
	{	
		m_SkillState = SS_END;
	}	
}

void ioFuriousTeleportAttack::SetEndState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_SkillState = SS_END;
	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << pChar->GetWorldOrientation();
		kPacket << pChar->GetWorldPosition();
		kPacket << (byte)SSS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFuriousTeleportAttack::OnSkillEnd( ioBaseChar *pChar )
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

	Init();
}

AttackAttribute& ioFuriousTeleportAttack::GetAttribute()
{
	switch( m_SkillState )
	{
	case SS_TELEPORT:
		return m_TeleportAttack;
	case SS_FINISH_ATTACK:
		return m_Attack;
	}

	return ioAttackSkill::GetAttribute();
}

const AttackAttribute& ioFuriousTeleportAttack::GetAttributeConst() const
{
	switch( m_SkillState )
	{	
	case SS_TELEPORT:
		return m_TeleportAttack;
	case SS_FINISH_ATTACK:
		return m_Attack;
	}

	return ioAttackSkill::GetAttributeConst();
}


bool ioFuriousTeleportAttack::IsEndState() const
{
	if( 0.0f < m_fSkillEndNextJumpAmt )
		return false;

	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioFuriousTeleportAttack::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_PRE:
	case SS_ATTACK:
	case SS_TELEPORT:
	case SS_FINISH_ATTACK:
		return false;
	}

	if( m_fSkillEndNextJumpAmt == 0.0f )
		return true;

	return false;
}

bool ioFuriousTeleportAttack::IsAttackEndState() const
{
	if( m_fSkillEndNextJumpAmt == 0.0f )
		return false;

	switch( m_SkillState )
	{
	case SS_PRE:	
	case SS_ATTACK:
	case SS_TELEPORT:
	case SS_FINISH_ATTACK:
		return false;
	}

	return true;
}

bool ioFuriousTeleportAttack::IsNoDropState() const
{
	switch( m_SkillState )
	{	
	case SS_PRE:
		return false;
	case SS_ATTACK:
	case SS_TELEPORT:
	case SS_FINISH_ATTACK:
		return true;
	}

	if( 0 < m_fSkillEndNextJumpAmt )
		return true;
	else
		return false;
}

SkillEndJumpType ioFuriousTeleportAttack::GetSkillEndNextJumpMustEnable()
{
	return m_SkillEndJumpType;
}

bool ioFuriousTeleportAttack::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioFuriousTeleportAttack::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioAttackSkill::ApplyAttributeToWeapon( pWeapon, pStage );	

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

void ioFuriousTeleportAttack::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;

	byte eState;
	rkPacket >> eState;

	pChar->SetTargetRotToRotate( qtRot, true, true );
	pChar->SetWorldPosition( vPos );

	switch( eState )
	{
	case SSS_TELEPORT:
		{
			byte eDirKey;
			rkPacket >> eDirKey;
			rkPacket >> m_nCurrTeleportCount;

			m_CurrDirKey = static_cast<DirKey>( eDirKey );
			SetSkillTeleportState( pChar, pStage );
		}
		break;
	case SSS_FINISH_ATTACK:
		{
			SetFinishAttackState( pChar );
		}
		break;
	case SSS_END:
		{
			SetEndState( pChar, pStage );
		}
		break;
	}
}

bool ioFuriousTeleportAttack::IsCanSendControl() const
{
	switch( m_SkillState )
	{
	case SS_PRE:
	case SS_ATTACK:
	case SS_TELEPORT:
	case SS_FINISH_ATTACK:
		return false;
	}

	return true;
}

bool ioFuriousTeleportAttack::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	// 스킬 무적 파괴
	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{		
		if( iDefenseBreakType == m_ProtectCancelType )
			return false;
	}

	if( 0 < m_dwSkillProtectTime && FRAMEGETTIME() < m_dwSkillStartTime + m_dwSkillProtectTime )
		return true;

	return false;
}