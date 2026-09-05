

#include "stdafx.h"

#include "ioFloatThrowBomWeapon.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ActionStopType ioFloatThrowBomWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioFloatThrowBomWeapon::m_DieActionStopType = AST_NONE;

ioFloatThrowBomWeapon::ioFloatThrowBomWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	m_BombState = BS_FLOATING;

	//SetPeculiarity( WP_ONE_TIME_USE | WP_MISSILE_WEAPON );
	SetPeculiarity(WP_ATTACK_OWNER | WP_MINE_EXPLOSION );

	m_pThrowMesh = NULL;
	m_fFloatingPower = 0.0f;

	m_bArleadyCallWeapon = false;
	m_bCharCollision = false;	
}

ioFloatThrowBomWeapon::~ioFloatThrowBomWeapon()
{
	if( m_pThrowMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioFloatThrowBomWeapon::~ioFloatThrowBomWeapon - UnReleased Resouce" );
	}
}

bool ioFloatThrowBomWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();

		m_fFloatingPower = GetAttribute()->m_fFloatPower;
		m_fMoveSpeed = GetAttribute()->m_fMoveSpeed;
		return SetThrowMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioFloatThrowBomWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );
	char szBuf[MAX_PATH] = "";

	m_fSpeedWeightRate = rkLoader.LoadFloat_e( "speed_weight_rate", FLOAT1 );
	m_fGravityRate = rkLoader.LoadFloat_e( "move_gravity_rate", FLOAT1 );

	m_vStartOffSet.x = rkLoader.LoadFloat_e( "start_offset_x", 0.0f );
	m_vStartOffSet.y = rkLoader.LoadFloat_e( "start_offset_y", 0.0f );
	m_vStartOffSet.z = rkLoader.LoadFloat_e( "start_offset_z", 0.0f );

	m_bCheckCharCollision = rkLoader.LoadBool_e( "enable_char_collision_call_attribute", false );

	m_bExplosionUse = rkLoader.LoadBool_e( "end_explosion", false );
	m_fExplosionRange = rkLoader.LoadFloat_e( "explosion_range", 0.0f );
	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	m_bUseFireStartPos = rkLoader.LoadBool_e( "use_start_pos", false );

	m_bSetBaseAttackDir = rkLoader.LoadBool_e( "set_base_attack_dir", false );

	LoadCallProperty( rkLoader );
}

void ioFloatThrowBomWeapon::LoadCallProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_bCallWeaponOnEnd = rkLoader.LoadBool_e( "call_weapon_on_weapon_dead", false );
	int iAttackTypeCnt = rkLoader.LoadInt_e( "call_weapon_type_count", 0 );

	m_CallWeaponInfoList.clear();

	if( 0 < iAttackTypeCnt )
		m_CallWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "call_weapon_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "call_weapon_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "call_weapon_type%d_time", i+1 );
		kInfo.m_dwCallTime = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_weapon_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "call_weapon_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_weapon_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_CallWeaponInfoList.push_back( kInfo );
	}
}

bool ioFloatThrowBomWeapon::SetThrowMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pThrowMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pThrowMesh )	return false;

	GetParentSceneNode()->AttachObject( m_pThrowMesh );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pThrowMesh->GetSkeletonName().IsEmpty() && m_pThrowMesh->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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

	m_pThrowMesh->UpdateBounds( true );

	AddEffectEvent( m_pThrowMesh );
	return true;
}
 
void ioFloatThrowBomWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pThrowMesh )
	{
		g_EffectFactory.DestroyEffect( m_pThrowMesh );
		m_pThrowMesh = NULL;
	}
}

void ioFloatThrowBomWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
	m_vPrevDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );

	D3DXVec3Cross( &m_vRollAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );
	D3DXVec3Normalize( &m_vRollAxis, &m_vRollAxis );
}

void ioFloatThrowBomWeapon::SetMoveSpeed( float fSpeed )
{
	m_fMoveSpeed = fSpeed;
}

void ioFloatThrowBomWeapon::SetFloatPower( float fFloatPower )
{
	m_fFloatingPower = fFloatPower * m_fOwnerChargeFloatRate;
}

void ioFloatThrowBomWeapon::RotateMoveDir( float fAngle )
{
	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngle) );
	m_vMoveDir = qtRot * m_vMoveDir;

	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetOrientation( qtRot );
}

void ioFloatThrowBomWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckPiercing() )
		return;

	if( m_BombState == BS_EXPLOSION )
	{
		UpdateWorldAttackBox( fTimePerSec, false );

		if( m_pThrowMesh )
		{
			m_pThrowMesh->Update( fTimePerSec * FLOAT1000 );
		}
		m_BombState = BS_EXPLOSION_END;

		return;
	}
	else if( m_BombState == BS_EXPLOSION_END )
	{
	
		UpdateWorldAttackBox( fTimePerSec, false );

		if( m_pThrowMesh )
		{
			m_pThrowMesh->Update( fTimePerSec * FLOAT1000 );
		}

		SetWeaponDead();	
		return;
	}

	/*D3DXVECTOR3 vMoveH = m_vMoveDir * m_fSpeedWeightRate * ( GetMoveSpeed() * fTimePerSec );
	D3DXVECTOR3 vMoveV = D3DXVECTOR3( 0.0f, FLOAT1, 0.0f );
	vMoveV *= m_fFloatingPower * fTimePerSec;

	D3DXVECTOR3 vDir = vMoveH + vMoveV;
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	//SetOrientation( qtRot );

	D3DXVECTOR3 vDiff = vDir - m_vPrevDir;
	float fCos = D3DXVec3Dot( &m_vPrevDir, &vDir );
	
	if( vDir != m_vPrevDir )
	{
		if( fCos >= FLOAT1 )
		{
		}
		else if( fCos <= -FLOAT1 ) // 180도 일때
		{
			D3DXVECTOR3 vAxis1, vAxis2;
			if( m_vMoveDir != ioMath::UNIT_Y )
			{
				D3DXVec3Cross( &vAxis1, &vDir, &ioMath::UNIT_Y );
				D3DXVec3Normalize( &vAxis1, &-vAxis1 );
			}
			else
			{
				D3DXVec3Cross( &vAxis1, &vDir, &ioMath::UNIT_X );
				D3DXVec3Normalize( &vAxis1, &-vAxis1 );
			}

			D3DXVec3Cross( &vAxis2, &vDir, &vAxis1 );
			D3DXVec3Normalize( &vAxis2, &-vAxis2 );

			GetParentSceneNode()->Rotate( vAxis2, FLOAT1 );
		}
		else // 그외
		{
			float fGapAngle = RADtoDEG( acos( fCos ) );

			D3DXVECTOR3 vAxis;
			D3DXVec3Cross( &vAxis, &vDiff, &m_vMoveDir );
			D3DXVec3Normalize( &vAxis, &-vAxis );

			GetParentSceneNode()->Rotate( vAxis, fGapAngle );
		}
	}*/
	

	D3DXVECTOR3 vMove = m_vMoveDir * ( GetMoveSpeed() * fTimePerSec );
	vMove.y += m_fFloatingPower * fTimePerSec;
	
	m_fFloatingPower -= (pStage->GetRoomGravity() * m_fGravityRate) * fTimePerSec;
	GetParentSceneNode()->Translate( vMove );

	float fMapHeight = pStage->GetMapHeight( GetPosition().x,
											 GetPosition().z,
											 NULL,
											 false,
											 GetPosition().y+m_pThrowMesh->GetBoundBox().GetExtents(1) );


	D3DXVECTOR3 vTargetDir = vMove;
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXQUATERNION qtTargetRot = Help::ConvertDirToQuaternion( vTargetDir );

	if( vMove != ioMath::VEC3_ZERO )
		SetOrientation( qtTargetRot );


	if( GetPosition().y <= fMapHeight )
	{
		D3DXVECTOR3 vPos = GetPosition();
		CharPiercingType eType = GetAttribute()->m_PiercingType;

		switch( eType )
		{
		case CPT_NONE:
		case CPT_NORMAL:
			vPos.y = fMapHeight+1.0f;
			SetPosition( vPos );
			break;
		}

		if( fMapHeight == 0.0f )
		{
			pStage->CreateMapEffect( GetExplosionEffectName(), vPos, GetScale() );
			PlayWoundedSound( NULL );
			//SetWeaponDead();
			Explosion(pStage);
			return;
		}
	}

	UpdateWorldAttackBox( fTimePerSec, false );

	if( m_pThrowMesh )
	{
		m_pThrowMesh->Update( fTimePerSec * FLOAT1000 );
	}

	m_vPrevDir = vMove;
	m_vPrevPos = GetPosition();
}

D3DXVECTOR3 ioFloatThrowBomWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	if ( m_bSetBaseAttackDir )
		return ioWeapon::GetAttackDir( pEntity );

	return m_vMoveDir;
}

const ioOrientBox& ioFloatThrowBomWeapon::GetLocalAttackBox() const
{
	if( m_pThrowMesh )
		return m_pThrowMesh->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioFloatThrowBomWeapon::GetType() const
{
	return WT_FLOAT_THROW_BOM_WEAPON;
}

ActionStopType ioFloatThrowBomWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioFloatThrowBomWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioFloatThrowBomWeapon::GetMoveSpeed()
{
	return m_fMoveSpeed * m_fOwnerChargeSpeedRate;
}


void ioFloatThrowBomWeapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( m_BombState != BS_FLOATING )
		return;

	bool bCollision = false;
	if( pTarget->IsWoundedCollision( GetWorldAttackBox() ) )
	{
		if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
			ToDummyChar(pTarget) || ToWallZone(pTarget) )
		{
			bCollision = true;
		}
		else if( GetTeam() != pTarget->GetTeam() )
		{
			bCollision = true;
		}
		else if( GetOwner() != pTarget )
		{
			ioBaseChar *pChar = ToBaseChar(pTarget);
			if( pChar )
			{
				if( pChar->IsEnableTeamCollision( this ) )
				{
					bCollision = true;
				}
			}
		}
	}

	if( !bCollision )
		return;

	if( ( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
		ToDummyChar(pTarget ) || ToWallZone(pTarget) ||
		GetPiercingType() == CPT_NONE ) )
	{
		Explosion( pStage );
		return;
	}

	CheckCurAttribute( pTarget );

	ApplyWeaponExp eExplosionType = pTarget->ApplyWeapon( this );
	pStage->AddWeaponExplosionEffect( pTarget, this, eExplosionType );

	AddCollisionEntity( pTarget );
}

bool ioFloatThrowBomWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	/*bool bCollision = false;

	if( m_bNoWoundedWeapon )
	{
		if( ToBaseChar(pEntity) )
			return false;

		D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
		bCollision = pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint );

		CheckPiercing( bCollision );
		return bCollision;
	}

	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	bCollision = pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint );

	CheckPiercing( bCollision );
	
	if( bCollision )
		m_bCharCollision = true;

	return bCollision;*/

	if( m_BombState != BS_EXPLOSION && m_BombState != BS_EXPLOSION_END )
		return false;

	float fTargetDist = -FLOAT1;
	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();
	D3DXVECTOR3 vDiff = pEntity->GetWorldPosition() - vCenter;
	float fLength = D3DXVec3Length( &vDiff );

	float fCurExplosionRange = m_fExplosionRange;
	if( m_fOwnerChargeExplosionRate > FLOAT1 )
	{
		fCurExplosionRange = m_fExplosionRange * m_fOwnerChargeExplosionRate;
	}

	if( !pEntity->CheckSphereDistance( vCenter, fCurExplosionRange, fTargetDist ) )
		return false;

	if( COMPARE( fTargetDist, 0.0f, fCurExplosionRange ) )
	{
		return true;
	}

	return false;
}

bool ioFloatThrowBomWeapon::CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint )
{
	// 타겟이 캐릭터가 아니라면 충돌로 처리한다
	ioBaseChar *pTargetChar = ToBaseChar( pTarget );
	if( !pTargetChar )	return true;

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
		return pOwner->CheckCollisionLine( vColPoint );

	return false;	// 주인이 없는 무기라면 충돌안한걸로 하자
}

void ioFloatThrowBomWeapon::SetOwnerChargeFloatRate( float fRate )
{
	m_fOwnerChargeFloatRate = fRate;

	m_fFloatingPower = GetAttribute()->m_fFloatPower * m_fOwnerChargeFloatRate;
}

void ioFloatThrowBomWeapon::SetGravityRate( float fGravityRate )
{
	m_fGravityRate = fGravityRate;
}

void ioFloatThrowBomWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pThrowMesh && m_pThrowMesh->GetSkeletonName().IsEmpty() && m_pThrowMesh->HasLinkedSystem() && !m_bUseFireStartPos )
	{
		//SetPosition( vPos );
		return;
	}

	ioBaseChar *pOwner = GetOwner();
	D3DXVECTOR3 vNewPos = vPos;
	if( pOwner && m_vStartOffSet != ioMath::VEC3_ZERO )
	{
		D3DXQUATERNION qtRot = pOwner->GetTargetRot();
		D3DXVECTOR3 vOffSet = qtRot * m_vStartOffSet;

		vNewPos += vOffSet;
	}

	SetPosition( vNewPos );
	m_vPrevPos = vNewPos;
}

bool ioFloatThrowBomWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
	{
		Explosion(pStage);
		return true;
	}

	return false;
}

void ioFloatThrowBomWeapon::CallAttribute()
{	
	if( m_bArleadyCallWeapon ) return;
	if( !m_bCallWeaponOnEnd ) return;
	if( !m_bCheckCharCollision && m_bCharCollision ) return;
	if( m_CallWeaponInfoList.empty() ) return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return;
	
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwCurGenerateTime = dwCurTime;
	WeaponInfoList::iterator iter;
	
	for( iter = m_CallWeaponInfoList.begin(); iter != m_CallWeaponInfoList.end(); ++iter )
	{
		WeaponInfo pInfo = (*iter);
		g_WeaponMgr.SetReserveCallWeaponInfo( GetOwnerName(),
											  pInfo,
											  GetPosition(),
											  m_vMoveDir,
											  dwCurTime + pInfo.m_dwCallTime,
											  m_dwWeaponIdx );

	}
	m_bArleadyCallWeapon = true;
}

void ioFloatThrowBomWeapon::SetWeaponDead()
{
	StopLoopSound();
	m_bLive = false;

	CallAttribute();
}


void ioFloatThrowBomWeapon::Explosion( ioPlayStage *pStage )
{	
	if( m_BombState == BS_EXPLOSION || m_BombState == BS_EXPLOSION_END )
		return;

	/*float fHeight = pStage->GetMapHeight( GetPosition().x, GetPosition().z );
	if( GetPosition().y > (fHeight + 100) )
		m_BombState = BS_EXPLOSION;*/

	m_BombState = BS_EXPLOSION;

	float fHalfRange = m_fExplosionRange;
	m_LocalAttackBox.SetExtents( 0, fHalfRange );
	m_LocalAttackBox.SetExtents( 1, fHalfRange );
	m_LocalAttackBox.SetExtents( 2, fHalfRange );
	UpdateWorldAttackBox( 0.0f, false );

	if( !m_BoomEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos =  GetPosition();
		D3DXVECTOR3 vScale = ioMath::UNIT_ALL;

		if( m_fOwnerChargeExplosionRate > FLOAT1 )
		{
			vScale = D3DXVECTOR3( m_fOwnerChargeExplosionRate, m_fOwnerChargeExplosionRate, m_fOwnerChargeExplosionRate );
		}

		pStage->CreateMapEffect( m_BoomEffect, vPos, vScale );

		ioWeapon::PlayExplosionSound();
	}
}