

#include "stdafx.h"

#include "ioTargetWeapon.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ActionStopType ioTargetWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioTargetWeapon::m_DieActionStopType = AST_NONE;

ioTargetWeapon::ioTargetWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ONE_TIME_USE | WP_DIVIDE_ACTION );

	m_LocalAttackBox.SetExtents( 0, FLOAT10 );
	m_LocalAttackBox.SetExtents( 1, FLOAT10 );
	m_LocalAttackBox.SetExtents( 2, FLOAT10 );
}

ioTargetWeapon::~ioTargetWeapon()
{
}

void ioTargetWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_TargetName.IsEmpty() )
		SetPosition( vPos );
}

void ioTargetWeapon::SetTargetName( const ioHashString& szTarget, ioPlayStage *pStage )
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

void ioTargetWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
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

const ioOrientBox& ioTargetWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioTargetWeapon::GetType() const
{
	return WT_TARGET;
}

ActionStopType ioTargetWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioTargetWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

bool ioTargetWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	ioBaseChar *pTarget = ToBaseChar(pEntity);
	if( !pTarget ) return false;

	if( !m_TargetName.IsEmpty() && pTarget->GetCharName() != m_TargetName )
		return false;

	bool bCollision = true;

	CheckPiercing( bCollision );

	return bCollision;
}


