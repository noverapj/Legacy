

#include "stdafx.h"

#include "ioFallWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioFallWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioFallWeapon::m_DieActionStopType = AST_NONE;

ioFallWeapon::ioFallWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	m_pFallWeapon = NULL;
	m_FallWeaponState = FS_FALL;

	m_bResistance = false;
	m_dwFirstStopTime = 0;
}

ioFallWeapon::~ioFallWeapon()
{
	if( m_pFallWeapon )
	{
		LOG.PrintTimeAndLog( 0, "ioFallWeapon::~ioFallWeapon - UnReleased Resource" );
	}
}

bool ioFallWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return CreateEffect( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioFallWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	m_fStartHeight = rkLoader.LoadFloat_e( "start_height", 0.0f );
	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );
}

bool ioFallWeapon::CreateEffect( const ioHashString &szFileName )
{
	m_pFallWeapon = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pFallWeapon )
		return false;

	GetParentSceneNode()->AttachObject( m_pFallWeapon );
	GetParentSceneNode()->SetScale( FLOAT1, FLOAT1, FLOAT1 );

	AddEffectEvent( m_pFallWeapon );
	return true;
}

void ioFallWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	m_vStartPos = vPos;
	SetPosition( m_vStartPos );

	if( m_pFallWeapon )
	{
		D3DXVECTOR3 vNewPos( vPos.x, vPos.y+m_fStartHeight, vPos.z );
		SetPosition( vNewPos );
	}
}

void ioFallWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	switch( m_FallWeaponState )
	{
	case FS_FALL:
		OnFalling( fTimePerSec, pStage );
		break;
	case FS_STOP:
		OnStoping( fTimePerSec, pStage );
		break;
	}
}

void ioFallWeapon::OnFalling( float fTimePerSec, ioPlayStage *pStage )
{
	D3DXVECTOR3 vPrePos = GetPosition();
	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	D3DXVECTOR3 vMoveAmt = D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) * fMoveDist;

	D3DXVECTOR3 vNewPos = vPrePos + vMoveAmt;
	float fMapHeight = pStage->GetMapHeightForBlockGuid( vPrePos, GetLocalAttackBox() );

	if( vNewPos.y < fMapHeight )
		SetStoping( fMapHeight );
	else
		GetParentSceneNode()->Translate( vMoveAmt );

	UpdateWorldAttackBox( fTimePerSec, false );

	if( m_pFallWeapon )
	{
		m_pFallWeapon->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioFallWeapon::OnStoping( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwFirstStopTime+m_dwLiveTime < dwCurTime )
	{
		SetWeaponDead();
		return;
	}

	D3DXVECTOR3 vPrePos = GetPosition();
	float fMapHeight = pStage->GetMapHeightForBlockGuid( vPrePos, GetLocalAttackBox() );

	if( vPrePos.y > fMapHeight )
	{
		SetFalling();
		UpdateWorldAttackBox( fTimePerSec, false );
	}

	if( m_pFallWeapon )
	{
		m_pFallWeapon->Update( fTimePerSec * FLOAT1000 );
	}
}

bool ioFallWeapon::IsFalling() const
{
	if( m_FallWeaponState == FS_FALL )
		return true;

	return false;
}

void ioFallWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFallWeapon )
	{
		g_EffectFactory.DestroyEffect( m_pFallWeapon );
		m_pFallWeapon = NULL;
	}
}

const ioOrientBox& ioFallWeapon::GetLocalAttackBox() const
{
	if( m_pFallWeapon )
		return m_pFallWeapon->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioFallWeapon::GetType() const
{
	return WT_FALL_WEAPON;
}

ActionStopType ioFallWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioFallWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioFallWeapon::GetMoveSpeed()
{
	return GetAttribute()->m_fMoveSpeed;
}

void ioFallWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	if( !IsLive() && bSetNull )
	{
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
		return;
	}

	m_WorldAttackBox = GetLocalAttackBox();
	m_WorldAttackBoxForMap = m_WorldAttackBox;

	float fMapCollisionRate = GetAttribute()->m_fMapCollisionBoxRate;
	m_WorldAttackBoxForMap.SetExtents( 0, m_WorldAttackBox.GetExtents(0)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 1, m_WorldAttackBox.GetExtents(1)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 2, m_WorldAttackBox.GetExtents(2)*fMapCollisionRate );

	if( !m_WorldAttackBox.IsNull() )
	{
		m_WorldAttackBox.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
		m_WorldAttackBoxForMap.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
	}
}

bool ioFallWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( !IsFalling() )
		return false;

	bool bCollision = false;

	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
		bCollision = true;

	CheckPiercing( bCollision );

	return bCollision;
}

void ioFallWeapon::SetFalling()
{
	m_FallWeaponState = FS_FALL;
}

void ioFallWeapon::SetStoping( float fHeight )
{
	m_FallWeaponState = FS_STOP;

	if( m_dwFirstStopTime == 0 )
		m_dwFirstStopTime = FRAMEGETTIME();
}




