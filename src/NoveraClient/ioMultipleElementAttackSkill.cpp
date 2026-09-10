

#include "stdafx.h"

#include "ioMultipleElementAttackSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioMultipleElementAttackSkill::ioMultipleElementAttackSkill()
{
	Init();
}

ioMultipleElementAttackSkill::ioMultipleElementAttackSkill( const ioMultipleElementAttackSkill &rhs )
	: ioAttackSkill( rhs ),	
	m_LoopAnimation( rhs.m_LoopAnimation ),
	m_fLoopAnimationRate( rhs.m_fLoopAnimationRate ),
	m_dwLoopDuration( rhs.m_dwLoopDuration ),
	m_dwChargingTime( rhs.m_dwChargingTime ),
	m_ElectricAttack( rhs.m_ElectricAttack ),
	m_IceAttack( rhs.m_IceAttack ),
	m_PhysicsAttack( rhs.m_PhysicsAttack ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime )
{
	Init();
}

ioMultipleElementAttackSkill::~ioMultipleElementAttackSkill()
{
}

void ioMultipleElementAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	//Loop
	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_LoopAnimation			= szBuf;
	m_fLoopAnimationRate	= rkLoader.LoadFloat_e( "loop_animation_rate", FLOAT1 );
	m_dwLoopDuration		= rkLoader.LoadInt_e( "loop_duration", 0 );

	//Elemnt
	m_dwChargingTime		= rkLoader.LoadInt_e( "charging_time", 100 );
	LoadAttackAttribute( "electric_attack", m_ElectricAttack, rkLoader );
	LoadAttackAttribute( "ice_attack", m_IceAttack, rkLoader );
	LoadAttackAttribute( "physics_attack", m_PhysicsAttack, rkLoader );

	m_dwSkillProtectTime = rkLoader.LoadInt_e( "skill_protect_time", 0 );
}

ioSkill* ioMultipleElementAttackSkill::Clone()
{
	return new ioMultipleElementAttackSkill( *this );
}

void ioMultipleElementAttackSkill::Init()
{	
	m_SkillState				= SS_PRE;
	m_ElementKey				= EK_NONE;

	m_dwLoopStartTime			= 0;
	m_dwChargingStartTime		= 0;
	m_dwMotionEndTime			= 0;

	m_bSetHandMesh				= false;
}

bool ioMultipleElementAttackSkill::OnSkillStart( ioBaseChar *pOwner )
{
	ioAttackSkill::OnSkillStart( pOwner );

	Init();	

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pOwner->ShowWeaponItemMesh( false );
	}

	if( !m_bReduceGauge )
	{
		pOwner->ReduceNeedGaugeBySkill( this, GetNeedGauge( pOwner->GetCharName() ) );
		m_bReduceGauge = true;
	}

	return true;
}

void ioMultipleElementAttackSkill::OnProcessState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_SkillState )
	{
	case SS_PRE: 
		{
			CheckPreState( pOwner, pStage );
		}
		break;
	case SS_LOOP:
		{
			CheckLoopState( pOwner, pStage );
		}
		break;
	case SS_CHARGE:
		{
			CheckChargeState( pOwner, pStage );
		}
		break;
	case SS_ATTACK:
		{
			CheckActionState( pOwner, pStage );
		}
		break;
	}
}

void ioMultipleElementAttackSkill::CheckPreState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	DWORD dwCurTime  = FRAMEGETTIME();
	DWORD dwPreDelay = GetPreDelayTime();

	if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
	{
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pOwner );
		SetLoopState( pOwner, pStage );			
	}
}

void ioMultipleElementAttackSkill::SetLoopState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	int iAniID = pGrp->GetAnimationIdx( m_LoopAnimation );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, m_fLoopAnimationRate );
	m_SkillState		= SS_LOOP;
	m_dwLoopStartTime	= FRAMEGETTIME();
}

void ioMultipleElementAttackSkill::CheckLoopState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( m_dwLoopStartTime + m_dwLoopDuration < FRAMEGETTIME() )
	{
		m_SkillState = SS_END;
	}
	else
	{
		if( pOwner->IsAttackKey() && !pOwner->IsDefenseKey() && !pOwner->IsJumpKey() )
		{
			SetChargeState( pOwner, pStage );
		}
		else if( !pOwner->IsAttackKey() && pOwner->IsDefenseKey() && !pOwner->IsJumpKey() )
		{
			m_ElementKey = EK_ELECTRIC_KEY;
			SetSkillActionState( pOwner, pStage );
		}
		else if( !pOwner->IsAttackKey() && !pOwner->IsDefenseKey() && pOwner->IsJumpKey() )
		{
			m_ElementKey = EK_ICE_KEY;
			SetSkillActionState( pOwner, pStage );
		}
	}
}

void ioMultipleElementAttackSkill::SetChargeState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	m_SkillState			= SS_CHARGE;
	m_dwChargingStartTime	= FRAMEGETTIME();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<byte>( SSS_CHARGE );
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMultipleElementAttackSkill::CheckChargeState( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargingStartTime + m_dwChargingTime < FRAMEGETTIME() )
		{
			m_ElementKey = EK_PHYSICS_KEY;
			SetSkillActionState( pOwner, pStage );
		}
	}
	else
	{
		m_ElementKey = EK_FIRE_KEY;
		SetSkillActionState( pOwner, pStage );
	}
}

void ioMultipleElementAttackSkill::SetSkillActionState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pOwner->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_SkillState = SS_ATTACK;
	AttackSkillFire( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<byte>( SSS_ATTACK );
		kPacket << pOwner->GetTargetRot();
		kPacket << (byte)m_ElementKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMultipleElementAttackSkill::CheckActionState( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_SkillState = SS_END;
	}
}

AttackAttribute& ioMultipleElementAttackSkill::GetAttribute()
{
	switch( m_ElementKey )
	{	
	case EK_ELECTRIC_KEY:
		return m_ElectricAttack;
	case EK_ICE_KEY:
		return m_IceAttack;
	case EK_PHYSICS_KEY:
		return m_PhysicsAttack;
	}

	return ioAttackSkill::GetAttribute();
}

const AttackAttribute& ioMultipleElementAttackSkill::GetAttributeConst() const
{
	switch( m_ElementKey )
	{	
	case EK_ELECTRIC_KEY:
		return m_ElectricAttack;
	case EK_ICE_KEY:
		return m_IceAttack;
	case EK_PHYSICS_KEY:
		return m_PhysicsAttack;
	}

	return ioAttackSkill::GetAttributeConst();
}

void ioMultipleElementAttackSkill::OnSkillEnd( ioBaseChar *pOwner )
{
	ioAttackSkill::OnSkillEnd( pOwner );
	
	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pOwner->ShowWeaponItemMesh( true );
	}

	if( !m_HandMesh.IsEmpty() )
	{
		pOwner->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}

	Init();
}

bool ioMultipleElementAttackSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioMultipleElementAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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
			pZone->SetZoneValue( false, false );
		}
	}
}

void ioMultipleElementAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	byte eState;
	rkPacket >> eState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	if( pOwner )
		pOwner->SetTargetRot( qtRot );

	switch( eState )
	{
	case SSS_CHARGE:
		{
			SetChargeState( pOwner, pStage );
		}
		break;
	case SSS_ATTACK:
		{
			byte eElement;
			rkPacket >> eElement;
			m_ElementKey = (ElementKey)eElement;
			SetSkillActionState( pOwner, pStage );
		}
		break;
	}
}

bool ioMultipleElementAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioMultipleElementAttackSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	// 스킬 무적 파괴
	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )
			return false;
	}

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	if( 0 < m_dwSkillProtectTime && FRAMEGETTIME() < m_dwSkillStartTime + m_dwSkillProtectTime )
		return true;

	return false;
}