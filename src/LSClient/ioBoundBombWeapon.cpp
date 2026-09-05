

#include "stdafx.h"

#include "ioBoundBombWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioBoundBombWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioBoundBombWeapon::m_DieActionStopType = AST_NONE;

ioBoundBombWeapon::ioBoundBombWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ATTACK_OWNER |
					WP_MINE_EXPLOSION |
					WP_WOUND_NO_SOUND |
					WP_DEF_NO_SOUND );

	m_pFireMesh = NULL;

	m_EffectID = -1;

	m_dwDuration = 0;
	m_dwExplosionStart = 0;
	m_fMoveSpeed = 0.0f;
	m_fPingPongPower = 400.0f;

	m_State = TS_MOVE;
	m_bObjectCollision = false;
	m_bResistance = false;
	m_bDropState = false;
}

ioBoundBombWeapon::~ioBoundBombWeapon()
{
	if( m_pFireMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioBoundBombWeapon::~ioBoundBombWeapon - UnReleased Resource" );
	}
}

bool ioBoundBombWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();

		return SetFireMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioBoundBombWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	DWORD dwCurTime = FRAMEGETTIME();

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	m_bDirectlyBoom = rkLoader.LoadBool_e( "directly_boom", false );

	m_fExplosionRange = rkLoader.LoadFloat_e( "explosion_range", 0.0f );

	m_dwThrowDuration = rkLoader.LoadInt_e( "throw_explosion_duration", 5000 );
	m_dwDropDuration = rkLoader.LoadInt_e( "drop_explosion_duration", 5000 );
	m_dwExplosionDuration = rkLoader.LoadInt_e( "explosion_duration", 0 );

	m_dwDuration = dwCurTime + m_dwThrowDuration;

	m_fJumpFloatingAngle = rkLoader.LoadFloat_e( "jump_floating_angle", 20.0f );
	
	m_fMoveSpeed = GetAttribute()->m_fMoveSpeed;
	m_fDropMoveSpeed = rkLoader.LoadFloat_e( "drop_move_speed", 0.0f );

	m_fFloatingPower = GetAttribute()->m_fFloatPower;
	m_fDropFloatingPower = rkLoader.LoadFloat_e( "drop_floating_power", 0.0f );

	m_bSetAngle = rkLoader.LoadBool_e( "set_angle", false );
	m_fXAngle = rkLoader.LoadFloat_e( "x_angle", 0.0f );
	m_fYAngle = rkLoader.LoadFloat_e( "y_angle", 0.0f );
}

void ioBoundBombWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	D3DXVECTOR3 vNewMoveDir = vDir;

	if ( m_bSetAngle )
	{
		D3DXQUATERNION qtRot;
		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &vNewMoveDir, &ioMath::UNIT_Y );

		D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(m_fXAngle) );
		vNewMoveDir = qtRot * vNewMoveDir;

		D3DXQuaternionRotationYawPitchRoll( &qtRot, DEGtoRAD(m_fYAngle), 0.0f, 0.0f );
		vNewMoveDir = qtRot * vNewMoveDir;
	}

	m_vMoveDir = vNewMoveDir;

	D3DXVec3Cross( &m_vRollAxis,
				   &D3DXVECTOR3( 0.0f, FLOAT1, 0.0f ),
				   &m_vMoveDir );
}

bool ioBoundBombWeapon::SetFireMesh( const ioHashString &szFileName )
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

void ioBoundBombWeapon::SetWallCollision( const D3DXVECTOR3 &vFaceNrm )
{
	if( m_State != TS_MOVE && m_State != TS_PINGPONG ) return;

	D3DXVECTOR3 vReflect = m_vMoveDir - 2.0f * D3DXVec3Dot( &m_vMoveDir, &vFaceNrm ) * vFaceNrm;
	D3DXVec3Normalize( &m_vMoveDir, &vReflect );

	m_State = TS_PINGPONG;
}

void ioBoundBombWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	switch( m_State )
	{
	case TS_NONE:
		OnNoneState( fTimePerSec, pStage );
		break;
	case TS_MOVE:
		OnMoveState( fTimePerSec, pStage );
		break;
	case TS_PINGPONG:
		if( m_bDirectlyBoom )
			Explosion( pStage );
		else
			OnPingPongState( fTimePerSec, pStage );
		break;
	case TS_EXPLOSION:
		OnExplosionState( fTimePerSec, pStage );
		break;
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

bool ioBoundBombWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( m_State != TS_MOVE && m_State != TS_PINGPONG )
		return true;

	int iFaceIndex = 0;
	if( !pShape->FindIntersection( pShapeWorldMat, GetWorldAttackBox(true), &iFaceIndex ) )
		return false;

	D3DXVECTOR3 vNormal;
	pShape->GetTriNormalInModelSpace( iFaceIndex, vNormal );

	SetWallCollision( vNormal );
	return true;
}

void ioBoundBombWeapon::OnNoneState( float fTimePerSec, ioPlayStage *pStage )
{
	if( m_dwDuration > 0 && m_dwDuration < FRAMEGETTIME() )
	{
		Explosion( pStage );
		return;
	}

	if( m_pFireMesh )
	{
		m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioBoundBombWeapon::OnMoveState( float fTimePerSec, ioPlayStage *pStage )
{
	if( m_dwDuration > 0 && m_dwDuration < FRAMEGETTIME() )
	{
		Explosion( pStage );
		return;
	}

	float fRotateAngle = 900.0f * fTimePerSec;
	GetParentSceneNode()->Rotate( m_vRollAxis, fRotateAngle );

	D3DXVECTOR3 vCurMove( 0.0, -FLOAT1, 0.0f );
	float fCurGravity = pStage->GetRoomGravity() * fTimePerSec;

	D3DXVECTOR3 vMove = m_vMoveDir * ( m_fMoveSpeed * fTimePerSec );

	vCurMove *= fCurGravity;
	vCurMove += vMove;
	D3DXVec3Normalize( &vCurMove, &vCurMove );


	vMove.y += m_fFloatingPower * fTimePerSec;
	m_fFloatingPower -= fCurGravity;
	GetParentSceneNode()->Translate( vMove );

	D3DXVECTOR3 vPos = GetPosition();

	float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false, vPos.y+m_pFireMesh->GetBoundBox().GetExtents(1) );
	if( vPos.y < fMapHeight+m_fExtentsOffSet )
	{
		vPos.y = fMapHeight;
		vPos.y += m_fExtentsOffSet;

		SetPosition( vPos );
		
		Explosion( pStage );
	}

	if( m_pFireMesh )
	{
		m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioBoundBombWeapon::OnPingPongState( float fTimePerSec, ioPlayStage *pStage )
{
	if( m_dwDuration > 0 && m_dwDuration < FRAMEGETTIME() )
	{
		Explosion( pStage );
		return;
	}
	
	float fRotateAngle = 900.0f * fTimePerSec;
	GetParentSceneNode()->PitchX( fRotateAngle, ioNode::TS_LOCAL );

	D3DXVECTOR3 vPos = GetPosition();
	vPos += m_vMoveDir * ( m_fMoveSpeed * fTimePerSec * 0.3f );
	vPos.y += m_fPingPongPower * fTimePerSec;
	m_fPingPongPower -= pStage->GetRoomGravity() * fTimePerSec;

	float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false, vPos.y+m_pFireMesh->GetBoundBox().GetExtents(1) );
	if( vPos.y < fMapHeight+m_fExtentsOffSet )
	{
		vPos.y = fMapHeight;
		vPos.y += m_fExtentsOffSet;

		SetPosition( vPos );
		SetOrientation( D3DXQUATERNION( 0.0f, 0.0f, 0.0f, FLOAT1 ) );
		m_fFloatingPower = 0.0f;

		Explosion( pStage );
	}
	else
	{
		SetPosition( vPos );
	}

	if( m_pFireMesh )
	{
		m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioBoundBombWeapon::OnExplosionState( float fTimePerSec, ioPlayStage *pStage )
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

	if( m_EffectID != -1 )
	{
		ioMapEffect *pEffect = pStage->FindMapEffect( m_EffectID );
		if( pEffect )
		{
			pEffect->SetWorldPosition( GetPosition() );
		}
	}

	D3DXVECTOR3 vPos = GetPosition();

	float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false, vPos.y+m_fExtentsOffSet );
	if( vPos.y > fMapHeight )
	{
		float fGap = vPos.y - fMapHeight;
		if( fGap > m_fExtentsOffSet+10.0f )
		{
			SetWeaponDead();
			return;
		}
	}
}

void ioBoundBombWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireMesh )
	{
		g_EffectFactory.DestroyEffect( m_pFireMesh );
		m_pFireMesh = NULL;
	}

	if( !IsLive() && m_EffectID != -1 )
	{
		ioMapEffect *pEffect = pStage->FindMapEffect( m_EffectID );
		if( pEffect )
		{
			pEffect->EndEffectForce();
			m_EffectID = -1;
		}
	}
}

ioWeapon::WeaponType ioBoundBombWeapon::GetType() const
{
	return WT_BOUNDBOMB;
}

ActionStopType ioBoundBombWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioBoundBombWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

const ioOrientBox& ioBoundBombWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

void ioBoundBombWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

void ioBoundBombWeapon::Explosion( ioPlayStage *pStage )
{
	if( m_State == TS_EXPLOSION )
		return;

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

		float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false, vPos.y+m_pFireMesh->GetBoundBox().GetExtents(1) );
		if( vPos.y < fMapHeight )
		{
			vPos.y = fMapHeight;
		}

		vPos.y += m_fExtentsOffSet;

		ioMapEffect *pEffect = pStage->CreateMapEffect( m_BoomEffect, vPos, GetScale() );
		if( pEffect )
		{
			m_EffectID = pEffect->GetUniqueID();
		}

		ioWeapon::PlayExplosionSound();
	}

	DestroyResource( pStage );

	CheckCreateWeaponList();
}

bool ioBoundBombWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_State != TS_EXPLOSION ) 
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	float fTargetDist = -FLOAT1;
	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();
	if( !pEntity->CheckSphereDistance( vCenter, m_fExplosionRange, fTargetDist ) )
		return false;

	if( COMPARE( fTargetDist, 0.0f, m_fExplosionRange ) )
		return true;

	return false;
}

void ioBoundBombWeapon::SetBombDrop()
{
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwDuration = dwCurTime + m_dwDropDuration;

	m_fMoveSpeed = m_fDropMoveSpeed;
	m_fFloatingPower = m_fDropFloatingPower;

	m_bDropState = true;
}

void ioBoundBombWeapon::SetJumpThrow()
{
	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(m_fJumpFloatingAngle) );
	m_vMoveDir = qtRot * m_vMoveDir;
}

void ioBoundBombWeapon::RotateMoveDir( float fAngle )
{
	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngle) );
	m_vMoveDir = qtRot * m_vMoveDir;
}

void ioBoundBombWeapon::SetBoomEffect( const ioHashString &rkName )
{
	m_BoomEffect = rkName;
}

bool ioBoundBombWeapon::IsExplosion() const
{
	if( m_State == TS_EXPLOSION )
		return true;

	return false;
}

bool ioBoundBombWeapon::IsCanMineExplosion()
{
	if( !ioWeapon::IsCanMineExplosion() )
		return false;

	if( !IsExplosion() )
		return false;

	return true;
}

void ioBoundBombWeapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( m_State != TS_MOVE && m_State != TS_PINGPONG )
		return;

	ioBaseChar *pChar = ToBaseChar( pTarget );
	if( pChar && pChar->GetCharName() == GetOwnerName() )
		return;

	if( pChar && pChar->IsApplyDownState( false ) )
		return;

	if( m_State == TS_PINGPONG && m_bObjectCollision )
		return;

	const ioOrientBox &kBombBox = GetWorldAttackBox();
	const ioOrientBox &kWoundedBox = pTarget->GetWorldCollisionBox();

	if( !pTarget->IsWoundedCollision( kBombBox ) )
		return;

	if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
		ToDummyChar(pTarget) || ToWallZone(pTarget) ||
		GetPiercingType() == CPT_NONE )
	{
		m_bObjectCollision = true;

		D3DXVECTOR3 vDiff = kBombBox.GetCenter() - kWoundedBox.GetCenter();
		vDiff.y = 0.0f;
		D3DXVec3Normalize( &vDiff, &vDiff );

		SetWallCollision( -vDiff );

		if( !pChar )
			return;
	}

	if( pTarget->GetTeam() == GetTeam() )
	{
		if( !pChar || !pChar->IsEnableTeamAttack() )
			return;
	}

	CheckCurAttribute( pTarget );

	ApplyWeaponExp eExplosionType = pTarget->ApplyWeapon( this );
	pStage->AddWeaponExplosionEffect( pTarget, this, eExplosionType );

	AddCollisionEntity( pTarget );
}

DWORD ioBoundBombWeapon::GetCollisionEnableTime() const
{
	if( m_State == TS_EXPLOSION )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

void ioBoundBombWeapon::TranslateByConveyer( const D3DXVECTOR3 &vMoveAmt )
{
	if( m_State == TS_EXPLOSION )
	{
		SetPosition( GetPosition() + vMoveAmt );
	}
}
