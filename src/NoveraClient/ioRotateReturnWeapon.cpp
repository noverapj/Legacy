

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioRotateReturnWeapon.h"

ActionStopType ioRotateReturnWeapon::m_ActionStopType		= AST_NONE;
ActionStopType ioRotateReturnWeapon::m_DieActionStopType	= AST_NONE;

ioRotateReturnWeapon::ioRotateReturnWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
	: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_MISSILE_WEAPON | WP_DIVIDE_ACTION );

	m_pFireBall			= NULL;

	m_State				= WS_INIT;
		
	m_fCurMoveRange		= 0.0f;
	m_vMoveDir			= ioMath::VEC3_ZERO;
	m_vStartPos			= ioMath::VEC3_ZERO;

	m_dwCollisionSync	= 0;
}

ioRotateReturnWeapon::~ioRotateReturnWeapon()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioRotateReturnWeapon::~ioRotateReturnWeapon - UnReleased Resource" );
	}
}

ActionStopType ioRotateReturnWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioRotateReturnWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioRotateReturnWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	rkLoader.LoadString_e( "fire_sound", "", szBuf, MAX_PATH );
	m_FireSound = szBuf;
	
	m_dwLiveTime				= rkLoader.LoadInt_e( "live_time", 0 );	
		
	m_fMaxMoveRange				= rkLoader.LoadFloat_e( "max_range", 0.0f );
		
	m_fReturnMovSpeed			= rkLoader.LoadFloat_e( "return_move_speed", 0.0f );
	m_fReturnRotSpeed			= rkLoader.LoadFloat_e( "return_rotate_speed", 0.0f );
	m_fMaxEndRange				= rkLoader.LoadFloat_e( "return_end_range", 0.0f );
	m_fMaxEndRangeOffset		= rkLoader.LoadFloat_e( "return_end_range_offset", 0.0f );
	m_bDisableCollisionReturn	= rkLoader.LoadBool_e( "disable_return_collision", false );
}

bool ioRotateReturnWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

bool ioRotateReturnWeapon::CreateFireBall( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	m_pFireBall = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pFireBall )
		return false;

	GetParentSceneNode()->AttachObject( m_pFireBall );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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

	AddEffectEvent( m_pFireBall );
	return true;
}

void ioRotateReturnWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}


ioWeapon::WeaponType ioRotateReturnWeapon::GetType() const
{
	return WT_ROTATE_RETURN_WEAPON;
}

const ioOrientBox& ioRotateReturnWeapon::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

void ioRotateReturnWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

void ioRotateReturnWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

	D3DXQUATERNION qtRot;
	qtRot = Help::ConvertDirToQuaternion( vDir );	
	SetOrientation( qtRot );
}

void ioRotateReturnWeapon::SetOwnerChargeScaleRate( float fRate )
{
	m_fOwnerChargeScaleRate = fRate;
	if( m_fOwnerChargeScaleRate > FLOAT1 )
	{
		D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
		GetParentSceneNode()->SetScale( vScale );
	}
}

float ioRotateReturnWeapon::GetMoveSpeed()
{
	return GetAttribute()->m_fMoveSpeed * m_fOwnerChargeSpeedRate;
}


void ioRotateReturnWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	ioBaseChar* pOwner = GetOwner();
	if( !CheckWeaponDead( pOwner ) )
		return;

	switch( m_State )
	{
	case WS_INIT:
		{
			m_vStartPos = GetPosition();
			m_State		= WS_MOVE;
		}
		break;
	case WS_MOVE:
		{
			OnMoveState( pStage, pOwner, fTimePerSec );
		}
		break;
	case WS_RETURN:
		{
			OnReturnState( pStage, pOwner, fTimePerSec );
		}
		break;
	}

	UpdateWorldAttackBox( fTimePerSec, false );
	if( m_pFireBall )
		m_pFireBall->Update( fTimePerSec * FLOAT1000 );
}

void ioRotateReturnWeapon::ProcessMove( ioPlayStage *pStage, ioBaseChar* pOwner, float fTimePerSec )
{
	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	m_fCurMoveRange += fMoveDist;

	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
}

void ioRotateReturnWeapon::OnMoveState( ioPlayStage *pStage, ioBaseChar* pOwner, float fTimePerSec )
{
	if( m_fMaxMoveRange <= m_fCurMoveRange )
	{
		m_State			= WS_RETURN;
		m_fCurMoveRange	= 0.0f;
	}
	else
	{
		ProcessMove( pStage, pOwner, fTimePerSec );
	}
}

void ioRotateReturnWeapon::ProcessRotate( ioPlayStage *pStage, ioBaseChar* pOwner, float fTimePerSec )
{
	if( fTimePerSec <= 0.0f )
		fTimePerSec = 0.0001f;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	vPos.y += m_fMaxEndRangeOffset;

	D3DXVECTOR3 vPreDir = m_vMoveDir;
	D3DXVECTOR3	vCurDir = vPos - GetPosition();
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXVECTOR3 vCurForce = m_vMoveDir * m_fReturnMovSpeed * fTimePerSec;
	m_vMoveDir = vCurForce + (vCurDir * m_fReturnRotSpeed * fTimePerSec);
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

	float fCos = D3DXVec3Dot( &vPreDir, &m_vMoveDir );

	// 0도 일때
	if( fCos >= FLOAT1 )
	{
		// 처리 필요없음
	}
	else if( fCos <= -FLOAT1 ) // 180도 일때
	{
		D3DXVECTOR3 vAxis1, vAxis2;
		if( m_vMoveDir != ioMath::UNIT_Y )
		{
			D3DXVec3Cross( &vAxis1, &m_vMoveDir, &ioMath::UNIT_Y );
			D3DXVec3Normalize( &vAxis1, &-vAxis1 );
		}
		else
		{
			D3DXVec3Cross( &vAxis1, &m_vMoveDir, &ioMath::UNIT_X );
			D3DXVec3Normalize( &vAxis1, &-vAxis1 );
		}

		D3DXVec3Cross( &vAxis2, &m_vMoveDir, &vAxis1 );
		D3DXVec3Normalize( &vAxis2, &-vAxis2 );

		GetParentSceneNode()->Rotate( vAxis2, FLOAT1 );
	}
	else // 그외
	{
		float fGapAngle = RADtoDEG( acos( fCos ) );

		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &vCurDir, &m_vMoveDir );
		D3DXVec3Normalize( &vAxis, &-vAxis );

		GetParentSceneNode()->Rotate( vAxis, fGapAngle );
	}
}

void ioRotateReturnWeapon::OnReturnState( ioPlayStage *pStage, ioBaseChar* pOwner, float fTimePerSec )
{
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	vPos.y += m_fMaxEndRangeOffset;

	D3DXVECTOR3	vDiff = pOwner->GetMidPositionByRate() - GetPosition();
	if( D3DXVec3LengthSq( &vDiff ) < m_fMaxEndRange * m_fMaxEndRange )
	{
		m_State			= WS_END;
		m_fCurMoveRange	= 0.0f;
	}
	else
	{
		ProcessRotate( pStage, pOwner, fTimePerSec );
		ProcessMove( pStage, pOwner, fTimePerSec );
	}
}

bool ioRotateReturnWeapon::CheckWeaponDead( ioBaseChar* pOwner )
{
	if( !pOwner )
	{
		SetWeaponDead();
		return false;
	}

	if( m_dwCreatedTime + m_dwLiveTime < FRAMEGETTIME() )
	{
		SetWeaponDead();
		return false;
	}

	if( m_State == WS_END )
	{
		SetWeaponDead();
		return false;
	}

	return true;
}

bool ioRotateReturnWeapon::IsIgnoreMapCollision() const
{
	return true;
}

bool ioRotateReturnWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	bool bCollision = false;

	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
		bCollision = true;

	ioBaseChar* pTarget = ToBaseChar( pEntity );
	if( bCollision && pTarget && pTarget->IsNeedProcess() )
	{
		if( pTarget->IsNeedSendNetwork() && !m_bDisableCollisionReturn && m_dwCollisionSync == 0 )
		{
			m_State				= WS_RETURN;
			m_fCurMoveRange		= 0.0f;
			m_dwCollisionSync	= FRAMEGETTIME();

			SP2Packet kPacket( CUPK_WEAPON_SYNC );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << (byte)WSS_RETURN;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	return bCollision;
}

bool ioRotateReturnWeapon::CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint )
{
	if( GetPiercingType() == CPT_ALL || GetPiercingType() == CPT_ALL_LIMITE )
		return true;

	// 타겟이 캐릭터가 아니라면 충돌로 처리한다
	ioBaseChar *pTargetChar = ToBaseChar( pTarget );
	if( !pTargetChar )
		return true;

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
		return pOwner->CheckCollisionLine( m_vStartPos, vColPoint );

	return false;	// 주인이 없는 무기라면 충돌안한걸로 하자
}

void ioRotateReturnWeapon::SetWeaponDead()
{
	ioWeapon::SetWeaponDead();
}

void ioRotateReturnWeapon::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	byte eState = 0;
	rkPacket >> eState;

	switch( eState )
	{
	case WSS_RETURN:
		{
			m_State			= WS_RETURN;
			m_fCurMoveRange	= 0.0f;
		}
		break;
	}
}
