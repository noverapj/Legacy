

#include "stdafx.h"

#include "io3DEngine/ioOrientBox.h"
#include "io3DEngine/ioNode.h"
#include "io3DEngine/ioMovableObject.h"
#include "io3DEngine/ioSceneNode.h"
#include "io3DEngine/ioSceneManager.h"
#include "io3DEngine/ioStringInterface.h"
#include "io3DEngine/ioBiped.h"
#include "io3DEngine/ioEntityGroup.h"
#include "io3DEngine/ioMath.h"
#include "io3DEngine/HelpFunc.h"

#include "io3DEngine/ioEffect.h"
#include "io3DEngine/ioEffectFactory.h"

#include "ioWeapon.h"
#include "ioFlameDashWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioWeaponFeature.h"
#include "WeaponAttribute.h"

ioFlameDashWeapon::ioFlameDashWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_BOMB_WEAPON | WP_ATTACK_OWNER | WP_MINE_EXPLOSION );

	m_pFlame = NULL;
	m_dwFlameStartTime = 0;
	m_dwFlameLiveTime = 0;
	m_fCurMoveRange = 0.0f;
}

ioFlameDashWeapon::~ioFlameDashWeapon()
{
	if( m_pFlame )
	{
		LOG.PrintTimeAndLog( 0, "ioFlameDashWeapon::~ioFlameDashWeapon - UnReleased Resouce" );
	}
}

void ioFlameDashWeapon::InitResource()
{
	ioWeapon::InitResource();

	DestroyFlame();
	CreateFlame( GetAttribute()->m_WeaponEffect );
}

void ioFlameDashWeapon::CreateFlame( const ioHashString &szFileName )
{
	m_pFlame = g_EffectFactory.CreateEffect( szFileName );
	GetParentSceneNode()->AttachObject( m_pFlame );

	if( m_pOwner && m_pFlame->HasLinkBiped() )
	{
		ioBiped *pLinked = m_pOwner->GetGroup()->GetBiped( m_pFlame->GetLinkBipedName() );
		if( pLinked )
		{
			D3DXVECTOR3 vPos = m_pFlame->GetLinkPosOffset();
			vPos = pLinked->GetDerivedOrientation() * vPos + pLinked->GetDerivedPosition();
			vPos = vPos * m_pOwner->GetWorldScale();
			vPos = m_pOwner->GetWorldOrientation() * vPos + m_pOwner->GetWorldPosition();
			SetPosition( vPos );
		}
	}

	m_kAttackBox = m_pFlame->GetCollisionBox();
}

void ioFlameDashWeapon::DestroyFlame()
{
	if( m_pFlame )
	{
		GetParentSceneNode()->DetachObject( m_pFlame );
		g_EffectFactory.DestroyEffect( m_pFlame );
		m_pFlame = NULL;
	}
}

void ioFlameDashWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

void ioFlameDashWeapon::RotateMoveDir( float fAngle )
{
	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -1.0f, 0.0f ) );

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngle) );
	m_vMoveDir = qtRot * m_vMoveDir;
	
	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetOrientation( qtRot );
}

void ioFlameDashWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( !m_pFlame || !m_pFlame->HasLinkBiped() )
	{
		D3DXVECTOR3 vStartPos;
		vStartPos.x = vPos.x;
		vStartPos.y = pStage->GetMapHeight( vPos.x, vPos.z, m_pOwner );
		vStartPos.z = vPos.z;

		SetPosition( vStartPos );
	}
}

void ioFlameDashWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( FRAMEGETTIME() - m_dwFlameStartTime > m_dwFlameLiveTime )
	{
		if( m_pFlame )
		{
			m_pFlame->EndEffectGrace();
			StopLoopSound();
		}

		m_kAttackBox.SetNull();

		if( !m_pFlame || !m_pFlame->IsLive() )
		{
			SetWeaponDead();
		}
	}
	
	if( m_pFlame )
	{
		m_pFlame->Update( fTimePerSec * 1000.0f );
	}
}

void ioFlameDashWeapon::DestroyResource()
{
	ioSceneNode *pParentNode = GetParentSceneNode();
	if( !pParentNode )
	{
		LOG.PrintTimeAndLog( 0, "ioMissileWeapon::DestroyResource - Not Parent" );
		return;
	}

	if( m_pFlame )
	{
		pParentNode->DetachObject( m_pFlame );
		g_EffectFactory.DestroyEffect( m_pFlame );
		m_pFlame = NULL;
	}
}

D3DXVECTOR3 ioFlameDashWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return m_vMoveDir;
}

ioWeapon::WeaponType ioFlameDashWeapon::GetType() const
{
	return WT_FLAME_DASH;
}

const ioAxisAlignBox& ioFlameDashWeapon::GetBoundBox() const
{
	if( m_pFlame )
		return m_pFlame->GetBoundBox();

	return ioWeapon::GetBoundBox();
}

float ioFlameDashWeapon::GetBoundRadius() const
{
	if( m_pFlame)
		return m_pFlame->GetBoundRadius();

	return ioWeapon::GetBoundRadius();
}

D3DXVECTOR3 ioFlameDashWeapon::GetPosition()
{
	return GetParentNode()->GetPosition();
}

void ioFlameDashWeapon::SetFlameLiveTime( DWORD dwLiveTime )
{
	m_dwFlameLiveTime = dwLiveTime;
	m_dwFlameStartTime = FRAMEGETTIME();
}
