

#include "stdafx.h"

#include "ioOwnerAttackWeapon.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ActionStopType ioOwnerAttackWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioOwnerAttackWeapon::m_DieActionStopType = AST_NONE;

ioOwnerAttackWeapon::ioOwnerAttackWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ATTACK_OWNER | WP_ONE_TIME_USE | WP_DIVIDE_ACTION );

	m_LocalAttackBox.SetExtents( 0, 50.0f );
	m_LocalAttackBox.SetExtents( 1, 50.0f );
	m_LocalAttackBox.SetExtents( 2, 50.0f );

	m_vMoveDir = ioMath::VEC3_ZERO;
}

ioOwnerAttackWeapon::~ioOwnerAttackWeapon()
{
}

void ioOwnerAttackWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	m_AttackDirType = (OwnerAttDirType)rkLoader.LoadInt_e( "owner_attack_dir_type", OADT_NONE );

	m_bUseAttackDir = rkLoader.LoadBool_e( "use_attack_dir", false );
	m_bUseReverseAttackDir = rkLoader.LoadBool (  "use_reverse_attack_dir", false );
}

void ioOwnerAttackWeapon::SetTargetName( const ioHashString& szTarget, ioPlayStage *pStage )
{
	m_TargetName = szTarget;

	if( pStage )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget )
		{
			SetPosition( pTarget->GetMidPositionByRate() );
			return;
		}
	}

	// ¾øÀ¸¸é ¼Ò¸ê
	SetWeaponDead();
}

void ioOwnerAttackWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_TargetName.IsEmpty() )
		SetPosition( vPos );
}

void ioOwnerAttackWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	m_dwLiveTime = 50;

	if( pStage )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetName );
		if( pTarget )
			SetPosition( pTarget->GetMidPositionByRate() );
		else
			SetWeaponDead();
	}

	if( !CheckPiercing() )
		return;

	DWORD dwLooseTime = 0;
	if( GetOwner() )
	{
		dwLooseTime = GetOwner()->GetActionStopTotalDelay();
	}

	if( m_dwCreatedTime + m_dwLiveTime + dwLooseTime < FRAMEGETTIME() )
	{
		SetWeaponDead();
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

const ioOrientBox& ioOwnerAttackWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioOwnerAttackWeapon::GetType() const
{
	return WT_OWNER_ATTACK;
}

ActionStopType ioOwnerAttackWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioOwnerAttackWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

bool ioOwnerAttackWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	ioBaseChar *pTarget = ToBaseChar(pEntity);
	if( !pTarget ) return false;

	if( !m_TargetName.IsEmpty() && pTarget->GetCharName() != m_TargetName )
		return false;

	bool bCollision = true;

	CheckPiercing( bCollision );

	return bCollision;
}

D3DXVECTOR3 ioOwnerAttackWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	if( m_AttackDirType == OADT_TARGET )
	{
		const ioBaseChar *pTarget = ToBaseCharConst(pEntity);
		if( pTarget )
		{
			D3DXVECTOR3	vMoveDir = pTarget->GetMoveDir();
			return vMoveDir;
		}
	}
	else if( m_AttackDirType == OADT_REVERSE_TARGET )
	{
		const ioBaseChar *pTarget = ToBaseCharConst(pEntity);
		if( pTarget )
		{
			D3DXVECTOR3	vMoveDir = pTarget->GetMoveDir();
			vMoveDir *= -1;
			return vMoveDir;
		}
	}
	else if( m_AttackDirType == OADT_MOVE_DIR )
	{
		return m_vMoveDir;
	}

	return ioWeapon::GetAttackDir( pEntity );
}

void ioOwnerAttackWeapon::SetMoveDir( const D3DXVECTOR3 &vMoveDir )
{
	m_vMoveDir = vMoveDir;
}

D3DXVECTOR3 ioOwnerAttackWeapon::GetMoveDir()
{
	return m_vMoveDir;
}