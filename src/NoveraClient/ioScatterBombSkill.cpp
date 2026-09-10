

#include "stdafx.h"

#include "ioScatterBombSkill.h"

#include "ioBaseChar.h"
#include "WeaponDefine.h"

ioScatterBombSkill::ioScatterBombSkill()
{
	m_fScatterAngle = 0.0f;
	m_iMaxWeaponWay = 1;

	m_iWeaponWay = 0;

	m_dwFireStartTime = 0;
	m_bSetHandMesh = false;
}

ioScatterBombSkill::ioScatterBombSkill( const ioScatterBombSkill &rhs )
: ioAttackSkill( rhs )
{
	m_fScatterAngle = rhs.m_fScatterAngle;
	m_iMaxWeaponWay = rhs.m_iMaxWeaponWay;

	m_iWeaponWay = 0;
}

ioScatterBombSkill::~ioScatterBombSkill()
{
}

void ioScatterBombSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	m_fScatterAngle = rkLoader.LoadFloat_e( "scatter_angle",FLOAT90 );
	m_iMaxWeaponWay = rkLoader.LoadInt_e( "max_bomb_cnt", 1 );
	m_iMaxWeaponWay = max( 1, m_iMaxWeaponWay );
}

ioSkill* ioScatterBombSkill::Clone()
{
	return new ioScatterBombSkill( *this );
}


bool ioScatterBombSkill::OnSkillStart( ioBaseChar *pChar )
{
	m_iWeaponWay = 0;
	m_dwFireStartTime = 0;
	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	return ioAttackSkill::OnSkillStart( pChar );
}

void ioScatterBombSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioAttackSkill::AttackSkillFire( pChar );

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iAniID == -1 )	return;

	float fTimeRate  = GetAttackTimeRate();
	m_dwFireStartTime = FRAMEGETTIME();
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
}

void ioScatterBombSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioBombWeapon *pBomb = ToBombWeapon( pWeapon );

	float fCurAngle = 0.0f;
	if( m_iMaxWeaponWay > 1 )
	{
		float fWayAngle = m_fScatterAngle / (float)m_iMaxWeaponWay;

		fCurAngle = fWayAngle * (float)m_iWeaponWay;
		fCurAngle -= m_fScatterAngle * FLOAT05 - fWayAngle * FLOAT05;
		fCurAngle = ioMath::ArrangeHead( fCurAngle );
	}

	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fCurAngle), 0.0f, 0.0f );

	D3DXVECTOR3 vMoveDir = qtAngle * pWeapon->GetOwner()->GetMoveDir();
	pWeapon->SetMoveDir( vMoveDir );

	m_iWeaponWay++;
	m_iWeaponWay %= m_iMaxWeaponWay;
}

void ioScatterBombSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	ioAttackSkill::OnProcessState( pChar, pStage );

	if( m_dwFireStartTime == 0 ) return;
	if( m_dwFireStartTime < FRAMEGETTIME() )
	{
		m_dwFireStartTime = 0;

		if( m_bSetHandMesh )
		{
			if( !m_HandMesh.IsEmpty() )
			{
				pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
			}
			m_bSetHandMesh = false;
		}
	}
}

void ioScatterBombSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

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

bool ioScatterBombSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}


