

#include "stdafx.h"

#include "ioMapAttachWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioMapAttachWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioMapAttachWeapon::m_DieActionStopType = AST_NONE;

ioMapAttachWeapon::ioMapAttachWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ATTACK_OWNER |
					WP_MINE_EXPLOSION |
					WP_WOUND_NO_SOUND |
					WP_DEF_NO_SOUND );

	m_pFireMesh = NULL;
	m_State = TS_DELAY;

	m_dwExplosionStart = 0;

	m_bResistance = false;

	m_iConveyerMoveFrameIdx = -1;
}

ioMapAttachWeapon::~ioMapAttachWeapon()
{
	if( m_pFireMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioMapAttachWeapon::~ioMapAttachWeapon - UnReleased Resource" );
	}
}

bool ioMapAttachWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();

		return SetFireMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioMapAttachWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	DWORD dwCurTime = FRAMEGETTIME();

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	m_dwDelayDuration = rkLoader.LoadInt_e( "delay_duration", 0 );

	m_fExplosionRange = rkLoader.LoadFloat_e( "explosion_range", 0.0f );
	m_dwExplosionDuration = rkLoader.LoadInt_e( "explosion_duration", 0 );

	m_fDirOffset = rkLoader.LoadFloat_e( "x_offset", 0.0f );
}

bool ioMapAttachWeapon::SetFireMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pFireMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pFireMesh )	return false;

	GetParentSceneNode()->AttachObject( m_pFireMesh );
	GetParentSceneNode()->SetScale( FLOAT1, FLOAT1, FLOAT1 );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pFireMesh->GetSkeletonName().IsEmpty() && m_pFireMesh->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
	{
		ioBiped *pLinked = pOwner->GetGroup()->GetBiped( szLinkBiped );
		if( pLinked )
		{
			vLinkPosOffset = pLinked->GetDerivedOrientation() * vLinkPosOffset + pLinked->GetDerivedPosition();
			vLinkPosOffset = vLinkPosOffset * pOwner->GetWorldScale();
			vLinkPosOffset = pOwner->GetWorldOrientation() * vLinkPosOffset + pOwner->GetWorldPosition();
			SetPosition( vLinkPosOffset );
		}
	}

	m_pFireMesh->UpdateBounds( true );
	m_LocalAttackBox = m_pFireMesh->GetCollisionBox();

	m_fExtentsOffSet = m_LocalAttackBox.GetExtents( 1 );

	AddEffectEvent( m_pFireMesh );
	return true;
}

void ioMapAttachWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	D3DXVECTOR3 vNewPos = vPos;
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		D3DXQUATERNION qtRotate = pOwner->GetTargetRot();
		D3DXVECTOR3 vDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		if( pStage && m_pFireMesh )
		{
			float fRayHeight = vPos.y+30.0f;
			float fMapHeight = pStage->GetMapHeight( vNewPos.x, vNewPos.z, NULL, false, fRayHeight );

			vNewPos.y = fMapHeight;
		}

		vNewPos += m_fDirOffset * vDir;

		SetOrientation( qtRotate );
	}

	SetPosition( vNewPos );
}

void ioMapAttachWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !m_AreaWeaponName.IsEmpty() &&
		m_dwCreatedTime+m_dwAttachAreaWeaponTime < FRAMEGETTIME() )
	{
		CreateAttachAreaWeapon();
	}

	switch( m_State )
	{
	case TS_DELAY:
		OnDelayState( fTimePerSec, pStage );
		break;
	case TS_DROP:
		OnDropState( fTimePerSec, pStage );
		break;
	case TS_EXPLOSION:
		OnExplosionState( fTimePerSec, pStage );
		break;
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

void ioMapAttachWeapon::OnDelayState( float fTimePerSec, ioPlayStage *pStage )
{
	if( m_dwCreatedTime + m_dwDelayDuration < FRAMEGETTIME() )
	{
		Explosion( pStage );
		return;
	}

	if( m_iConveyerMoveFrameIdx > 0 )
	{
		if( m_iConveyerMoveFrameIdx + 1 < (int)g_FrameTimer.GetFrameCounter() )
		{
			Explosion( pStage );
			return;
		}
	}

	// drop Check
	D3DXVECTOR3 vPos = GetPosition();
	float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false, vPos.y+30.0f );

	if( fabs(vPos.y - fMapHeight) > 5.0f )
	{
		m_State = TS_DROP;
	}

	if( m_pFireMesh )
	{
		m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
	}

	UpdateAttachAreaWeapon( pStage );
}

void ioMapAttachWeapon::OnDropState( float fTimePerSec, ioPlayStage *pStage )
{
	if( m_dwCreatedTime + m_dwDelayDuration < FRAMEGETTIME() )
	{
		Explosion( pStage );
		return;
	}

	D3DXVECTOR3 vPos = GetPosition();

	float fPreMapHeight = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false, vPos.y+30.0f );
	float fCurGravity = pStage->GetRoomGravity() * fTimePerSec;
	vPos.y -= fCurGravity;

	if( vPos.y < fPreMapHeight )
	{
		vPos.y = fPreMapHeight;
		m_State = TS_DELAY;
	}

	SetPosition( vPos );

	if( m_pFireMesh )
	{
		m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
	}

	UpdateAttachAreaWeapon( pStage );
}

void ioMapAttachWeapon::OnExplosionState( float fTimePerSec, ioPlayStage *pStage )
{
	if( m_dwExplosionDuration == 0 )
	{
		SetWeaponDead();
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( dwCurTime - m_dwExplosionStart > m_dwExplosionDuration )
	{
		SetWeaponDead();
		return;
	}

	if( m_pFireMesh )
	{
		m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioMapAttachWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireMesh )
	{
		g_EffectFactory.DestroyEffect( m_pFireMesh );
		m_pFireMesh = NULL;
	}
}

ioWeapon::WeaponType ioMapAttachWeapon::GetType() const
{
	return WT_MAP_ATTACH;
}

ActionStopType ioMapAttachWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioMapAttachWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

const ioOrientBox& ioMapAttachWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

void ioMapAttachWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	if( !IsLive() || bSetNull )
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
		m_WorldAttackBox.Transform( GetPosition(),
									ioMath::UNIT_ALL,
									GetParentSceneNode()->GetDerivedOrientation() );

		m_WorldAttackBoxForMap.Transform( GetPosition(),
										  ioMath::UNIT_ALL,
										  GetParentSceneNode()->GetDerivedOrientation() );
	}
}

void ioMapAttachWeapon::Explosion( ioPlayStage *pStage )
{
	if( m_State == TS_EXPLOSION )
		return;

	DestroyAttachAreaWeapon();

	m_State = TS_EXPLOSION;
	m_dwExplosionStart = FRAMEGETTIME();

	float fHalfRange = m_fExplosionRange;
	m_LocalAttackBox.SetExtents( 0, fHalfRange );
	m_LocalAttackBox.SetExtents( 1, fHalfRange );
	m_LocalAttackBox.SetExtents( 2, fHalfRange );
	UpdateWorldAttackBox( 0.0f, false );

	if( !m_BoomEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos = GetPosition();

		float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false, vPos.y+30.0f );
		if( vPos.y < fMapHeight )
		{
			vPos.y = fMapHeight;
		}

		vPos.y += FLOAT1;

		pStage->CreateMapEffect( m_BoomEffect, vPos, GetScale() );
		ioWeapon::PlayExplosionSound();
	}
}

bool ioMapAttachWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_State != TS_EXPLOSION ) 
		return false;

	if( m_fExplosionRange <= 0.0f )
		return false;

	float fTargetDist = -FLOAT1;
	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();
	if( !pEntity->CheckSphereDistance( vCenter, m_fExplosionRange, fTargetDist ) )
		return false;

	if( COMPARE( fTargetDist, 0.0f, m_fExplosionRange ) )
		return true;

	return false;
}

bool ioMapAttachWeapon::IsExplosion() const
{
	if( m_State == TS_EXPLOSION )
		return true;

	return false;
}

bool ioMapAttachWeapon::IsCanMineExplosion()
{
	if( !ioWeapon::IsCanMineExplosion() )
		return false;

	if( !IsExplosion() )
		return false;

	return true;
}

DWORD ioMapAttachWeapon::GetCollisionEnableTime() const
{
	if( m_State == TS_EXPLOSION )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

void ioMapAttachWeapon::TranslateByConveyer( const D3DXVECTOR3 &vMoveAmt )
{
	if( m_State != TS_EXPLOSION )
	{
		SetPosition( GetPosition() + vMoveAmt );
		m_iConveyerMoveFrameIdx = g_FrameTimer.GetFrameCounter();
	}
}




