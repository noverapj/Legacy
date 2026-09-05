

#include "stdafx.h"

#include "ioSimpleBoxWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioSimpleBoxWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioSimpleBoxWeapon::m_DieActionStopType = AST_NONE;

ioSimpleBoxWeapon::ioSimpleBoxWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
	: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_DIVIDE_ACTION );

	m_pMesh = NULL;

	ClearData();	
}

ioSimpleBoxWeapon::~ioSimpleBoxWeapon()
{
	if( m_pMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioSimpleBoxWeapon::~ioSimpleBoxWeapon - UnReleased Resouce" );
	}
}


bool ioSimpleBoxWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return SetMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioSimpleBoxWeapon::ClearData()
{	
	m_vMoveDir = D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	m_State = WS_INIT;

	m_bOwnerAttach = false;
	m_bCheckDeadTime = false;
}

void ioSimpleBoxWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	DWORD dwCurTime = FRAMEGETTIME();

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_dwDurationTime = rkLoader.LoadInt_e( "duration", 0 );
	//////////////////////////////////////////////////////////////////////////
	m_bRotateMoveDir = rkLoader.LoadBool_e( "use_rotate_weapon", false );
	m_fRotateY = rkLoader.LoadFloat_e( "rotate_y_value", 0.f );
	m_fRotateX = rkLoader.LoadFloat_e( "rotate_x_value", 0.f );
}

bool ioSimpleBoxWeapon::SetMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	
		return false;

	m_pMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pMesh )	
		return false;

	GetParentSceneNode()->AttachObject( m_pMesh );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pMesh->GetSkeletonName().IsEmpty() && m_pMesh->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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
	
	AddEffectEvent( m_pMesh );
	return true;
}

void ioSimpleBoxWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pMesh )
	{
		g_EffectFactory.DestroyEffect( m_pMesh );
		m_pMesh = NULL;
	}
}

void ioSimpleBoxWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	SetPosition( vPos );
}

ioWeapon::WeaponType ioSimpleBoxWeapon::GetType() const
{
	return WT_SIMPLE_BOX;
}

ActionStopType ioSimpleBoxWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioSimpleBoxWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioSimpleBoxWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case WS_INIT:			
		m_State = WS_LIVE;
		m_dwWeaponDeadTime = dwCurTime + m_dwDurationTime;
		break;
	case WS_LIVE:
		{
			if( m_bOwnerAttach )
				ProcessAttach( fTimePerSec, pStage );
			else
				ProcessMove( fTimePerSec, pStage );

			if( m_bCheckDeadTime && m_dwWeaponDeadTime < FRAMEGETTIME() )
				SetWeaponDead();
		}
		break;
	}

	if( m_pMesh )
	{
		m_pMesh->Update( fTimePerSec * FLOAT1000 );
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

void ioSimpleBoxWeapon::ProcessMove( float fTimePerSec, ioPlayStage *pStage )
{
	if( fTimePerSec == 0.0f )
		fTimePerSec = 0.001f;
		
	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	D3DXVECTOR3 vMove = GetPosition();
	vMove += m_vMoveDir * fMoveDist;
	SetPosition( vMove );

	if( m_dwWeaponDeadTime < FRAMEGETTIME() )
	{
		SetWeaponDead();
	}
}

void ioSimpleBoxWeapon::ProcessAttach( float fTimePerSec, ioPlayStage *pStage )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pMesh->GetSkeletonName().IsEmpty() && m_pMesh->GetFirstLinkOffset( szLinkBiped,
																			 vLinkPosOffset,
																			 qtLinkRotOffset ) )
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
}

bool ioSimpleBoxWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{	
	bool bCollision = false;
	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
		bCollision = true;

	CheckPiercing( bCollision );
	
	return bCollision;
}

void ioSimpleBoxWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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
		if( m_fOwnerChargeScaleRate > FLOAT1 )
		{
			D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
			m_WorldAttackBox.Transform( GetPosition(), vScale, GetOrientation() );
			m_WorldAttackBoxForMap.Transform( GetPosition(), vScale, GetOrientation() );
		}
		else
		{
			m_WorldAttackBox.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
			m_WorldAttackBoxForMap.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
		}
	}
}

const ioOrientBox& ioSimpleBoxWeapon::GetLocalAttackBox() const
{
	if( m_pMesh )
		return m_pMesh->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

float ioSimpleBoxWeapon::GetMoveSpeed()
{
	return GetAttribute()->m_fMoveSpeed * m_fOwnerChargeSpeedRate;
}

void ioSimpleBoxWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	if( m_bRotateMoveDir )
	{
		float fAngleH = ioMath::ArrangeHead( m_fRotateY );
		float fAngleV = ioMath::ArrangeHead( m_fRotateX );

		D3DXQUATERNION qtRot;
		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );
		// vertical
		D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
		m_vMoveDir = qtRot * m_vMoveDir;
		// horizon
		D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
		m_vMoveDir = qtRot * m_vMoveDir;
	}

	D3DXQUATERNION qtRotDir;
	ioPlayEntityHelp::CheckOrientation( m_vMoveDir, qtRotDir );
	SetOrientation( qtRotDir );
}

void ioSimpleBoxWeapon::SetPointDir( const D3DXVECTOR3 &vPoint )
{
	m_vMoveDir = vPoint - GetPosition();
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

	D3DXQUATERNION qtRot;
	ioPlayEntityHelp::CheckOrientation( m_vMoveDir, qtRot );
	SetOrientation( qtRot );
}

void ioSimpleBoxWeapon::SetOwnerAttach( bool bAttach )
{
	m_bOwnerAttach = bAttach;
}


void ioSimpleBoxWeapon::SetCheckDeadTime( bool bCheckDeadTime )
{
	m_bCheckDeadTime = bCheckDeadTime;
}