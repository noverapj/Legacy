

#include "stdafx.h"

#include "ioFireSkill3.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioFireSkill3::ioFireSkill3()
{
	m_bSetHandMesh = false;
}

ioFireSkill3::ioFireSkill3( const ioFireSkill3 &rhs )
: ioAttackSkill( rhs )
{
	m_bSetHandMesh = false;
}

ioFireSkill3::~ioFireSkill3()
{
}

void ioFireSkill3::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );
}

ioSkill* ioFireSkill3::Clone()
{
	return new ioFireSkill3( *this );
}

bool ioFireSkill3::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	m_SkillState = SS_DELAY;
	m_bSetHandMesh = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	return true;
}

void ioFireSkill3::SetSkillActionState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	m_SkillState = SS_ACTION;
	m_CurDirKey = pChar->CheckCurDirKey();
	CheckDirKey( pChar );
	AttackSkillFire( pChar );

	if( pChar && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<int>(m_SkillState);
		kPacket << static_cast<int>(m_CurDirKey);
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFireSkill3::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwPreDelay = GetPreDelayTime();

	switch( m_SkillState )
	{
	case SS_DELAY:
		if( m_dwPreDelayStartTime + dwPreDelay < FRAMEGETTIME() )
		{
			m_dwPreDelayStartTime = 0;
			EndPreDelayEffect( pChar );

			if( pChar->IsNeedProcess() )
				SetSkillActionState( pChar );
		}
		break;
	case SS_ACTION:
		{
			CheckExtraAniJump( pChar );
			CheckAniRotate( pChar );
		}
		break;
	}
}

void ioFireSkill3::OnSkillEnd( ioBaseChar *pChar )
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
}

bool ioFireSkill3::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioFireSkill3::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	SetChargingRateForWeapon( pWeapon );

	ioSylphidMissileWeapon *pSylphid = ToSylphidMissileWeapon( pWeapon );
	if( pSylphid )
	{
		g_WeaponMgr.CheckSylphidMissileWeapon( pWeapon );
	}
}

void ioFireSkill3::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iState, iCurDirKey;
	rkPacket >> iState;
	rkPacket >> iCurDirKey;
	m_SkillState = static_cast<SkillState>(iState);
	m_CurDirKey = static_cast<ioUserKeyInput::DirKeyInput>(iCurDirKey);
	
	CheckDirKey( pChar );
	AttackSkillFire( pChar );
}

bool ioFireSkill3::IsNoDropState() const
{
	if( m_dwPreDelayStartTime > 0 )
		return true;

	return false;
}

void ioFireSkill3::CheckDirKey( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	D3DXVECTOR3 vTargetDir;
	if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		vTargetDir = qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
		D3DXVec3Normalize( &vTargetDir, &vTargetDir );
	}
	else
	{
		vTargetDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	}

	pChar->SetTargetRotToDir( vTargetDir, true );
}