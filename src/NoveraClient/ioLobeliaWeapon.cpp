

#include "stdafx.h"

#include "ioLobeliaWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioLobeliaWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioLobeliaWeapon::m_DieActionStopType = AST_NONE;

ioLobeliaWeapon::ioLobeliaWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_MINE_EXPLOSION |
					WP_WOUND_NO_SOUND |
					WP_DEF_NO_SOUND );

	m_pFireMesh = NULL;

	m_EffectID = -1;
	m_GroundID = -1;

	m_dwDuration = 0;
	m_dwExplosionStart = 0;
	m_fMoveSpeed = 0.0f;
	m_fPingPongPower = 400.0f;

	m_dwGroundStart = 0;

	m_State = TS_MOVE;
	m_bObjectCollision = false;
	m_bResistance = false;
	m_bDropState = false;
}

ioLobeliaWeapon::~ioLobeliaWeapon()
{
	if( m_pFireMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioLobeliaWeapon::~ioLobeliaWeapon - UnReleased Resource" );
	}
}

bool ioLobeliaWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();

		return SetFireMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioLobeliaWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	DWORD dwCurTime = FRAMEGETTIME();

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	rkLoader.LoadString_e( "ground_effect", "", szBuf, MAX_PATH );
	m_GroundEffect = szBuf;

	m_bDirectlyBoom = rkLoader.LoadBool_e( "directly_boom", false );

	m_fExplosionRange = rkLoader.LoadFloat_e( "explosion_range", 0.0f );

	m_dwThrowDuration = (DWORD)rkLoader.LoadInt_e( "throw_explosion_duration", 5000 );
	m_dwDropDuration = (DWORD)rkLoader.LoadInt_e( "drop_explosion_duration", 5000 );
	m_dwExplosionDuration = (DWORD)rkLoader.LoadInt_e( "explosion_duration", 0 );
	m_dwGroundDuration = (DWORD)rkLoader.LoadInt_e( "ground_duration", 0 );

	m_dwDuration = dwCurTime + m_dwThrowDuration;

	m_fJumpFloatingAngle = rkLoader.LoadFloat_e( "jump_floating_angle", 20.0f );
	
	m_fMoveSpeed = GetAttribute()->m_fMoveSpeed;
	m_fDropMoveSpeed = rkLoader.LoadFloat_e( "drop_move_speed", 0.0f );

	m_fFloatingPower = GetAttribute()->m_fFloatPower;
	m_fDropFloatingPower = rkLoader.LoadFloat_e( "drop_floating_power", 0.0f );


	m_TargetColType		= (TargetColType)rkLoader.LoadInt_e( "collision_type", 0 );
	m_TargetWoundType	= (TargetWoundType)rkLoader.LoadInt_e( "wound_type", 0 );

	m_fZoneRange		= rkLoader.LoadFloat_e( "collision_range", 0.0f );
	m_fZoneMinRange		= rkLoader.LoadFloat_e( "collision_min_range", 0.0f );
	m_fZoneMaxRange		= rkLoader.LoadFloat_e( "collision_max_range", 0.0f );
	m_fZoneInnerRange	= rkLoader.LoadFloat_e( "collision_inner_range", 0.0f );
	m_fZoneAngle		= rkLoader.LoadFloat_e( "collision_angle", 0.0f );

	m_fUpHeight		= rkLoader.LoadFloat_e( "collision_up_height", 0.0f );
	m_fUnderHeight	= rkLoader.LoadFloat_e( "collision_under_height", 0.0f );
	m_fWidth		= rkLoader.LoadFloat_e( "collision_width", 0.0f );
	m_fOffSet		= rkLoader.LoadFloat_e( "collision_offset", 0.0f );

	//
	int iCount = rkLoader.LoadInt_e( "lobelia_buff_cnt", 0 );
	m_LobeliaBuffList.reserve( iCount );

	for( int i=0; i<iCount; i++ )
	{
		wsprintf_e( szKey, "lobelia_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_LobeliaBuffList.push_back( tBuffInfo );
			}
		}
	}
}

void ioLobeliaWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXVec3Cross( &m_vRollAxis,
				   &D3DXVECTOR3( 0.0f, FLOAT1, 0.0f ),
				   &m_vMoveDir );
}

bool ioLobeliaWeapon::SetFireMesh( const ioHashString &szFileName )
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

void ioLobeliaWeapon::SetWallCollision( const D3DXVECTOR3 &vFaceNrm )
{
	if( m_State != TS_MOVE && m_State != TS_PINGPONG ) return;

	D3DXVECTOR3 vReflect = m_vMoveDir - 2.0f * D3DXVec3Dot( &m_vMoveDir, &vFaceNrm ) * vFaceNrm;
	D3DXVec3Normalize( &m_vMoveDir, &vReflect );

	m_State = TS_PINGPONG;
}

void ioLobeliaWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
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
		OnPingPongState( fTimePerSec, pStage );
		break;
	case TS_GROUND:
		OnGroundState( fTimePerSec, pStage );
		break;
	case TS_EXPLOSION:
		OnExplosionState( fTimePerSec, pStage );
		break;
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

bool ioLobeliaWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
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

void ioLobeliaWeapon::OnNoneState( float fTimePerSec, ioPlayStage *pStage )
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

void ioLobeliaWeapon::OnMoveState( float fTimePerSec, ioPlayStage *pStage )
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
		
		SetGroundState( pStage );
	}

	if( m_pFireMesh )
	{
		m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioLobeliaWeapon::OnPingPongState( float fTimePerSec, ioPlayStage *pStage )
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

		SetGroundState( pStage );
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

void ioLobeliaWeapon::OnExplosionState( float fTimePerSec, ioPlayStage *pStage )
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

void ioLobeliaWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireMesh )
	{
		g_EffectFactory.DestroyEffect( m_pFireMesh );
		m_pFireMesh = NULL;
	}

	if( !IsLive() )
	{
		if( m_EffectID != -1 )
		{
			ioMapEffect *pEffect = pStage->FindMapEffect( m_EffectID );
			if( pEffect )
			{
				pEffect->EndEffectForce();
				m_EffectID = -1;
			}
		}

		if( m_GroundID != -1 )
		{
			ioMapEffect *pEffect = pStage->FindMapEffect( m_GroundID );
			if( pEffect )
			{
				pEffect->EndEffectForce();
				m_GroundID = -1;
			}
		}
	}
}

ioWeapon::WeaponType ioLobeliaWeapon::GetType() const
{
	return WT_LOBELIA;
}

ActionStopType ioLobeliaWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioLobeliaWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

const ioOrientBox& ioLobeliaWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

void ioLobeliaWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

void ioLobeliaWeapon::Explosion( ioPlayStage *pStage )
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

	if( !m_BoomEffect.IsEmpty() && pStage )
	{
		D3DXVECTOR3 vPos = GetPosition();

		float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false, vPos.y+m_fExtentsOffSet );
		if( vPos.y < fMapHeight )
		{
			vPos.y = fMapHeight;
		}

		ioMapEffect *pEffect = pStage->CreateMapEffect( m_BoomEffect, vPos, GetScale() );
		if( pEffect )
		{
			m_EffectID = pEffect->GetUniqueID();
		}

		ioWeapon::PlayExplosionSound();
	}

	if( m_GroundID != -1 )
	{
		ioMapEffect *pEffect = pStage->FindMapEffect( m_GroundID );
		if( pEffect )
		{
			pEffect->EndEffectForce();
			m_GroundID = -1;
		}
	}

	DestroyResource( pStage );
}

bool ioLobeliaWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_State != TS_EXPLOSION ) 
		return false;

	bool bCollision = false;

	if( m_TargetColType == TCT_SPHERE )
	{
		bCollision = CheckCollisionSphere( pEntity );
	}
	else if( m_TargetColType == TCT_CYLINDER )
	{
		bCollision = CheckCollisionCylinder( pEntity );
	}
	else if( m_TargetColType == TCT_BOX )
	{
		bCollision = CheckCollisionBox( pEntity );
	}
	else if( m_TargetColType == TCT_DONUT )
	{
		bCollision = CheckCollisionDonut( pEntity );
	}

	ioBaseChar *pChar = ToBaseChar(pEntity);
	if( bCollision && pEntity->GetSubType() == ioPlayEntity::PST_CHAR )
	{
		if( !pChar ) return false;

		// 쓰러짐 관련 체크
		if( m_TargetWoundType == TWT_AIR_WOUND_EXCEPT ) // 쓰러짐중 공중상태는 패스
		{
			if( pChar->IsApplyDownState() && !pChar->IsApplyDownState(false) )
				return false;
		}
		else if( m_TargetWoundType == TWT_DOWN_WOUND_EXCEPT ) // 쓰러짐중 바닥상태는 패스
		{
			if( pChar->IsApplyDownState(false) )
				return false;
		}
		else if( m_TargetWoundType == TWT_ALL_WOUND_EXCEPT )
		{
			if( pChar->IsApplyDownState() ||
				( pChar->GetState() == CS_GRAPPLING_WOUNDED ||
				pChar->GetState() == CS_GRAPPLING_PUSHED ||
				pChar->GetState() == CS_GRAPPLING_WOUNDED_SKILL ||
				pChar->GetState() == CS_GRAPPLING_WOUNDED_SWING ||
				pChar->GetState() == CS_GRAPPLING_WOUNDED_ITEM ||
				pChar->GetState() == CS_GRAPPLING_WOUNDED_BUFF ) )
			{
				return false;
			}
			else if( pChar->IsApplyDownState() )
			{
				return false;
			}
		}
		else if( m_TargetWoundType == TWT_FLOATING_STATE )	// 공중에 뜬 상태가 아니면 패스
		{
			if( !pChar->IsFloating() )
				return false;
		}
		else if( m_TargetWoundType == TWT_DOWN_WOUNDED ) //바닥에 쓰러져 있지 않거나 공중에 뜬 상태면 패스
		{
			if( pChar->IsApplyDownState() || !pChar->IsFloating() )
				return false;
		}
	}

	return bCollision;
}

void ioLobeliaWeapon::SetBombDrop()
{
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwDuration = dwCurTime + m_dwDropDuration;

	m_fMoveSpeed = m_fDropMoveSpeed;
	m_fFloatingPower = m_fDropFloatingPower;

	m_bDropState = true;
}

void ioLobeliaWeapon::SetJumpThrow()
{
	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(m_fJumpFloatingAngle) );
	m_vMoveDir = qtRot * m_vMoveDir;
}

void ioLobeliaWeapon::RotateMoveDir( float fAngle )
{
	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngle) );
	m_vMoveDir = qtRot * m_vMoveDir;
}

void ioLobeliaWeapon::SetBoomEffect( const ioHashString &rkName )
{
	m_BoomEffect = rkName;
}

bool ioLobeliaWeapon::IsExplosion() const
{
	if( m_State == TS_EXPLOSION )
		return true;

	return false;
}

bool ioLobeliaWeapon::IsCanMineExplosion()
{
	if( !ioWeapon::IsCanMineExplosion() )
		return false;

	if( !IsExplosion() )
		return false;

	return true;
}

void ioLobeliaWeapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
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

DWORD ioLobeliaWeapon::GetCollisionEnableTime() const
{
	if( m_State == TS_EXPLOSION )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

void ioLobeliaWeapon::TranslateByConveyer( const D3DXVECTOR3 &vMoveAmt )
{
	if( m_State == TS_EXPLOSION )
	{
		SetPosition( GetPosition() + vMoveAmt );
	}
}

void ioLobeliaWeapon::SetGroundState( ioPlayStage *pStage )
{
	switch( m_State )
	{
	case TS_MOVE:
	case TS_PINGPONG:
		break;
	case TS_GROUND:
	case TS_EXPLOSION:
		return;
	}

	m_State = TS_GROUND;
	m_dwGroundStart = FRAMEGETTIME();
	ClearCollisionList();

	if( !m_GroundEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos = GetPosition();

		float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false, vPos.y+m_fExtentsOffSet );
		vPos.y = fMapHeight;

		ioMapEffect *pEffect = pStage->CreateMapEffect( m_GroundEffect, vPos, GetScale() );
		if( pEffect )
		{
			m_GroundID = pEffect->GetUniqueID();
		}

		ioWeapon::PlayExplosionSound();
	}

	DestroyResource( pStage );
}

void ioLobeliaWeapon::OnGroundState( float fTimePerSec, ioPlayStage *pStage )
{
	if( m_dwGroundDuration == 0 )
	{
		SetWeaponDead();
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( dwCurTime - m_dwGroundStart > m_dwGroundDuration )
	{
		//Explosion( pStage );
		SetWeaponDead();
		return;
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

bool ioLobeliaWeapon::IsGroundState() const
{
	if( m_State == TS_GROUND )
		return true;

	return false;
}

bool ioLobeliaWeapon::CheckCollisionSphere( ioPlayEntity *pEntity )
{
	D3DXVECTOR3 vPos = GetWorldAttackBox().GetCenter();
	float fZoneRange = m_fZoneRange * m_fOwnerChargeRangeRate;

	D3DXVECTOR3 vDiff = vPos - pEntity->GetMidPositionByRate();
	float fLength = D3DXVec3Length( &vDiff );
	if( fLength <= m_fZoneMinRange )
		return false;

	D3DXVECTOR3 vPoint = ioMath::VEC3_ZERO;
	if( !pEntity->CheckSpherePoint( vPos, fZoneRange, vPoint ) )
		return false;

	D3DXVECTOR3 vDir = vPoint - vPos;
	D3DXVec3Normalize( &vDir, &vDir );

	float fCurCheckAngle = m_fZoneAngle;
	float fHalfCosine = cosf( DEGtoRAD( fCurCheckAngle ) * FLOAT05 );
	if( D3DXVec3Dot( &m_vMoveDir, &vDir ) >= fHalfCosine )
		return true;

	return false;
}

bool ioLobeliaWeapon::CheckCollisionCylinder( ioPlayEntity *pEntity )
{
	if( m_fUpHeight == 0.0f && m_fUnderHeight == 0.0f )
		return false;

	float fZoneRange = m_fZoneRange * m_fOwnerChargeRangeRate;

	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();

	D3DXVECTOR3 vTop, vBottom;
	vTop = vBottom = vCenter;
	vTop.y += m_fUpHeight;
	vBottom.y -= m_fUnderHeight;
	ioCylinder kCylinder( vTop, vBottom, fZoneRange );
	vCenter = kCylinder.GetCenter();


	// 사각형을 이루는 2점의 포인트를 구한다. 가장 먼거리의 대각선의 2점
	D3DXVECTOR3 vMin, vMax;
	vMax = vTop;
	vMin = vBottom;

	vMax.x += fZoneRange;
	vMax.z -= fZoneRange;

	vMin.x -= fZoneRange;
	vMin.z += fZoneRange;

	// 사각형 생성
	ioOrientBox kBuffBox;
	kBuffBox.SetBoxByMinMax( vMin, vMax );

#ifndef SHIPPING
	kBuffBox.GetVertices( ioBaseChar::m_vRangeBoxArrayForTest );
#endif


	D3DXVECTOR3 vCollisionPoint;
	if( !pEntity->CheckCylinderPoint( kCylinder, vCenter, vCollisionPoint ) )
		return false;

	// 수평 거리 체크
	D3DXVECTOR3 vDiff1 = vCollisionPoint - vCenter;
	vDiff1.y = 0.0f;

	float fTargetDist = D3DXVec3LengthSq( &vDiff1 );
	if( fTargetDist > fZoneRange * fZoneRange )
		return false;

	if( fTargetDist < m_fZoneMinRange * m_fZoneMinRange )
		return false;

	// 수평 각 체크
	float fCurCheckAngle = m_fZoneAngle;
	float fHalfCosine = cosf( DEGtoRAD( fCurCheckAngle ) * FLOAT05 );
	D3DXVec3Normalize( &vDiff1, &vDiff1 );

	if( D3DXVec3Dot( &m_vMoveDir, &vDiff1 ) < fHalfCosine )
		return false;

	// 높이 체크
	if( vCollisionPoint.y >= vBottom.y && vCollisionPoint.y <= vTop.y )
		return true;

	return false;
}

bool ioLobeliaWeapon::CheckCollisionBox( ioPlayEntity *pEntity )
{
	if( m_fUpHeight == 0.0f && m_fUnderHeight == 0.0f )
		return false;

	if( m_fWidth == 0.0f )
		return false;

	ioBaseChar *pOwnerChar = GetOwner();
	if( !pOwnerChar )
		return false;

	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();

	// 사각형의 center을 vPos로 설정하게 되므로 여기서 캐릭터앞쪽에 생성하고 높이를 보정한다.
	vCenter += m_vMoveDir * m_fZoneRange * FLOAT05; 
	D3DXVECTOR3 vOffset( 0.0f, ( ( m_fUpHeight + m_fUnderHeight ) * FLOAT05 ) - m_fUpHeight, 0.0f );
	vCenter -= vOffset;

	// 사각형을 이루는 2점의 포인트를 구한다. 가장 먼거리의 대각선의 2점
	D3DXVECTOR3 vMin, vMax;
	vMin = vMax = vCenter;
	vMin.x -= ( m_fWidth * FLOAT05 );
	vMin.y -= m_fUnderHeight;
	vMax.x += ( m_fWidth * FLOAT05 );
	vMax.y += m_fUpHeight;
	vMax.z += m_fZoneRange;

	// 사각형 생성
	ioOrientBox kBuffBox;
	kBuffBox.SetBoxByMinMax( vMin, vMax );
	// 유저 방향으로 사각형 회전
	kBuffBox.Transform( vCenter, ioMath::UNIT_ALL, pOwnerChar->GetWorldOrientation() );
	// 회전후 위치가 변경되므로 중심을 다시 pos로 이동
	kBuffBox.SetCenter( vCenter );
#ifndef SHIPPING
	kBuffBox.GetVertices( ioBaseChar::m_vRangeBoxArrayForTest );
#endif

	if( !pEntity )
		return false;

	if( !pEntity->IsWoundedCollision( kBuffBox ) )
		return false;

	return true;
}

bool ioLobeliaWeapon::CheckCollisionDonut( ioPlayEntity *pEntity )
{
	if( m_fUpHeight == 0.0f && m_fUnderHeight == 0.0f )
		return false;

	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();

	D3DXVECTOR3 vTop, vBottom;
	vTop = vBottom = vCenter;
	vTop.y += m_fUpHeight;
	vBottom.y -= m_fUnderHeight;
	ioCylinder kCylinder( vTop, vBottom, m_fZoneRange );
	vCenter = kCylinder.GetCenter();


	// 사각형을 이루는 2점의 포인트를 구한다. 가장 먼거리의 대각선의 2점
	D3DXVECTOR3 vMin, vMax;
	vMax = vTop;
	vMin = vBottom;

	vMax.x += m_fZoneRange;
	vMax.z -= m_fZoneRange;

	vMin.x -= m_fZoneRange;
	vMin.z += m_fZoneRange;

	// 사각형 생성
	ioOrientBox kBuffBox;
	kBuffBox.SetBoxByMinMax( vMin, vMax );

#ifndef SHIPPING
	kBuffBox.GetVertices( ioBaseChar::m_vRangeBoxArrayForTest );
#endif


	D3DXVECTOR3 vCollisionPoint;
	if( !pEntity->CheckCylinderPoint( kCylinder, vCenter, vCollisionPoint ) )
		return false;

	// 수평 거리 체크
	D3DXVECTOR3 vDiff1 = vCollisionPoint - vCenter;
	vDiff1.y = 0.0f;

	float fTargetDist = D3DXVec3LengthSq( &vDiff1 );
	if( fTargetDist > m_fZoneRange * m_fZoneRange )
	{
		return false;
	}

	float fInnerRange = m_fZoneRange - m_fZoneInnerRange;
	fInnerRange = max( 0, fInnerRange );

	if( fTargetDist < fInnerRange * fInnerRange )
	{
		return false;
	}

	// 수평 각 체크
	float fCurCheckAngle = m_fZoneAngle;
	float fHalfCosine = cosf( DEGtoRAD( fCurCheckAngle ) * FLOAT05 );
	D3DXVec3Normalize( &vDiff1, &vDiff1 );

	if( D3DXVec3Dot( &m_vMoveDir, &vDiff1 ) < fHalfCosine )
		return false;

	// 높이 체크
	if( vCollisionPoint.y >= vBottom.y && vCollisionPoint.y <= vTop.y )
	{
		return true;
	}

	return false;
}

const BuffInfoList& ioLobeliaWeapon::GetTargetAirBuffList() const
{
	if( m_State != TS_EXPLOSION )
		return GetAttribute()->m_TargetAirBuffList;

	return m_LobeliaBuffList;
}

const BuffInfoList& ioLobeliaWeapon::GetTargetBuffList() const
{
	if( m_State != TS_EXPLOSION )
		return GetAttribute()->m_TargetBuffList;

	return m_LobeliaBuffList;
}

const BuffInfoList& ioLobeliaWeapon::GetTargetDownBuffList() const
{
	if( m_State != TS_EXPLOSION )
		return GetAttribute()->m_TargetDownBuffList;

	return m_LobeliaBuffList;
}

