

#include "stdafx.h"

#include "ioFlame8WaySkill.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioFlame8WaySkill::ioFlame8WaySkill()
{
	m_iWeaponWay = 0;
}

ioFlame8WaySkill::ioFlame8WaySkill( const ioFlame8WaySkill &rhs )
: ioAttackSkill( rhs ),
 m_FlameAngleList( rhs.m_FlameAngleList )
{
	m_iMaxWeaponWay = rhs.m_iMaxWeaponWay;

	m_iWeaponWay = 0;
}

ioFlame8WaySkill::~ioFlame8WaySkill()
{
}

ioSkill* ioFlame8WaySkill::Clone()
{
	return new ioFlame8WaySkill( *this );
}

void ioFlame8WaySkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	m_iMaxWeaponWay = rkLoader.LoadInt_e( "max_flame_cnt", 1 );
	m_iMaxWeaponWay = max( 1, m_iMaxWeaponWay );

	char szKey[MAX_PATH];
	m_FlameAngleList.clear();
	for( int i=0; i < m_iMaxWeaponWay; ++i )
	{
		wsprintf_e( szKey, "flame_angle%d", i+1 );

		float fAngle = rkLoader.LoadFloat( szKey, 0.0f );
		m_FlameAngleList.push_back( fAngle );
	}
}

bool ioFlame8WaySkill::OnSkillStart( ioBaseChar *pChar )
{
	m_iWeaponWay = 0;
	return ioAttackSkill::OnSkillStart( pChar );
}

void ioFlame8WaySkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if( m_iMaxWeaponWay <= m_iWeaponWay )
		return;

	float fCurAngle = m_FlameAngleList[m_iWeaponWay];
	
	fCurAngle = ioMath::ArrangeHead( fCurAngle );

	D3DXVECTOR3 vMoveDir;
	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fCurAngle), 0.0f, 0.0f );

	ioBaseChar *pOwner = pWeapon->GetOwner();
	if( pOwner )
	{
		vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vMoveDir = qtAngle * vMoveDir;
	}
	else
	{
		vMoveDir = ioMath::UNIT_Z;
	}

	pWeapon->SetMoveDir( vMoveDir );
	m_iWeaponWay++;
}

