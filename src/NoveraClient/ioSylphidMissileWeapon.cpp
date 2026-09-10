

#include "stdafx.h"

#include "ioSylphidMissileWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioSylphidMissileWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioSylphidMissileWeapon::m_DieActionStopType = AST_NONE;

ioSylphidMissileWeapon::ioSylphidMissileWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ONE_TIME_USE |
					WP_DEF_NO_SOUND );

	m_pWeaponMesh = NULL;
	m_pSylphidEffect = NULL;
	m_WeaponState = WS_MOVE;

	m_bResistance = false;
	m_fCurMoveSpeed = 0.0f;

	m_bSetSylphid = false;
	m_dwSylphidStartTime = 0;
	m_fCurGravityAmt = 0.0f;
}

ioSylphidMissileWeapon::~ioSylphidMissileWeapon()
{
	if( m_pWeaponMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioSylphidMissileWeapon::~ioSylphidMissileWeapon - UnReleased Resouce" );
	}
}

bool ioSylphidMissileWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return SetWeaponMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioSylphidMissileWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	rkLoader.LoadString_e( "sylphid_effect", "", szBuf, MAX_PATH );
	m_SylphidEffect = szBuf;

	m_fEnableMoveGap = rkLoader.LoadFloat_e( "enable_move_gap_height", 0.0f );
	m_fVerticalSpeed = rkLoader.LoadFloat_e( "air_vertical_speed", 0.0f );

	//
	m_WeaponMoveType = (WeaponMoveType)rkLoader.LoadInt_e( "weapon_move_type", WMT_AIR );

	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );

	m_bDisableSylphidOwnerRot = rkLoader.LoadBool_e( "disable_sylphid_owner_rot", false );
	m_fSylphidSpeedRate = rkLoader.LoadFloat_e( "sylphid_speed_rate", FLOAT1 );
	m_dwSylphidDuration = (DWORD)rkLoader.LoadInt_e( "sylphid_duration", 0 );
}

bool ioSylphidMissileWeapon::SetWeaponMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pWeaponMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pWeaponMesh )	return false;

	GetParentSceneNode()->AttachObject( m_pWeaponMesh );
	GetParentSceneNode()->SetScale( ioMath::UNIT_ALL );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pWeaponMesh->GetSkeletonName().IsEmpty() && m_pWeaponMesh->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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

	m_pWeaponMesh->UpdateBounds( true );
	m_LocalAttackBox = m_pWeaponMesh->GetCollisionBox();

	m_vPrePos = GetPosition();

	m_fDefaultGapHeight = fabs( m_vPrePos.y - pOwner->GetWorldPosition().y );

	AddEffectEvent( m_pWeaponMesh );
	return true;
}

void ioSylphidMissileWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

void ioSylphidMissileWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pWeaponMesh && m_pWeaponMesh->GetSkeletonName().IsEmpty() && m_pWeaponMesh->HasLinkedSystem() )
		return;

	SetPosition( vPos );

	m_fDefaultGapHeight = 0.0f;
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		m_fDefaultGapHeight = fabs( vPos.y - pOwner->GetWorldPosition().y );
	}
}

void ioSylphidMissileWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckPiercing() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCreatedTime + GetLiveTime() < dwCurTime )
	{
		SetWeaponDead();
		return;
	}

	if( m_bSetSylphid && m_dwSylphidStartTime > 0 && m_dwSylphidStartTime+m_dwSylphidDuration < dwCurTime )
	{
		m_bSetSylphid = false;

		if( m_pSylphidEffect )
		{
			g_EffectFactory.DestroyEffect( m_pSylphidEffect );
			m_pSylphidEffect = NULL;
		}
	}

	switch( m_WeaponState )
	{
	case WS_MOVE:
		OnMove( fTimePerSec, pStage );
		break;
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

bool ioSylphidMissileWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( m_WeaponState != WS_MOVE )
		return true;

	if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
	{
		SetWeaponDead();
		return true;
	}

	return false;
}

void ioSylphidMissileWeapon::OnMove( float fTimePerSec, ioPlayStage *pStage )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner || !pStage )
		return;

	if( !m_bSetSylphid || !m_bDisableSylphidOwnerRot )
		CheckOwnerRotate();

	float fCurSpeed = GetMoveSpeed();
	float fMoveDist = fCurSpeed * fTimePerSec;

	m_vPrePos = GetPosition();
	D3DXVECTOR3 vNewPos = m_vPrePos + (m_vMoveDir * fMoveDist);

	switch( m_WeaponMoveType )
	{
	case WMT_AIR:
		if( m_fVerticalSpeed != 0.0f )
		{
			D3DXVECTOR3 vDir = ioMath::UNIT_Y * m_fVerticalSpeed * fTimePerSec;

			vNewPos += vDir;
		}
		break;
	case WMT_GROUND:
		{
			float fMapHeight = pStage->GetMapHeightNoneEntity( vNewPos.x, vNewPos.z, vNewPos.y+70.0f );
			if( m_vPrePos.y < fMapHeight )
			{
				m_fCurGravityAmt = 0.0f;
				if( fabs( m_vPrePos.y - fMapHeight ) < m_fEnableMoveGap )
				{
					vNewPos.y = fMapHeight;
				}
				else
				{
					vNewPos = m_vPrePos;
				}
			}
			else if( m_vPrePos.y > fMapHeight )
			{
				m_fCurGravityAmt += pStage->GetRoomGravity() * fTimePerSec;

				float fNewHeight = m_vPrePos.y - m_fCurGravityAmt;
				if( fNewHeight < fMapHeight )
				{
					vNewPos.y = fMapHeight;
				}
				else
				{
					vNewPos.y = fNewHeight;
				}
			}
			else
			{
				m_fCurGravityAmt = 0.0f;
			}
		}
		break;
	}

	SetPosition( vNewPos );
	if( vNewPos.y <= FLOAT1 )
	{
		SetWeaponDead();
		return;
	}

	if( m_pWeaponMesh )
	{
		m_pWeaponMesh->Update( fTimePerSec * FLOAT1000 );
	}

	if( m_pSylphidEffect )
	{
		m_pSylphidEffect->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioSylphidMissileWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pWeaponMesh )
	{
		g_EffectFactory.DestroyEffect( m_pWeaponMesh );
		m_pWeaponMesh = NULL;
	}

	if( m_pSylphidEffect )
	{
		g_EffectFactory.DestroyEffect( m_pSylphidEffect );
		m_pSylphidEffect = NULL;
	}
}

const ioOrientBox& ioSylphidMissileWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioSylphidMissileWeapon::GetType() const
{
	return WT_SYLPHID_MISSILE;
}

ActionStopType ioSylphidMissileWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioSylphidMissileWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioSylphidMissileWeapon::GetMoveSpeed()
{
	m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	if( m_bSetSylphid )
		m_fCurMoveSpeed *= m_fSylphidSpeedRate;

	return m_fCurMoveSpeed;
}

void ioSylphidMissileWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

bool ioSylphidMissileWeapon::IsFloating() const
{
	if( m_WeaponState == WS_MOVE )
		return true;

	return false;
}

void ioSylphidMissileWeapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( m_WeaponState != WS_MOVE )
		return;

	bool bCollision = false;
	if( pTarget->IsWoundedCollision( GetWorldAttackBox() ) )
	{
		if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
			ToDummyChar(pTarget) || ToWallZone(pTarget) )
		{
			bCollision = true;
		}
	}
		
	if( !bCollision )
		return;

	if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
		ToDummyChar(pTarget) || ToWallZone(pTarget) ||
		GetPiercingType() == CPT_NONE )
	{
		SetWeaponDead();
		return;
	}
}

bool ioSylphidMissileWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_WeaponState != WS_MOVE )
		return false;

	bool bCollision = false;

	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
		bCollision = true;

	CheckPiercing( bCollision );

	return bCollision;
}

bool ioSylphidMissileWeapon::CheckCollisionLine( ioPlayStage *pStage )
{
	if( !pStage ) return false;

	D3DXVECTOR3 vPos = GetPosition();

	if( m_vPrePos == vPos )
		return false;

	bool bMapCollision = pStage->CheckCollisionLine( m_vPrePos, vPos, true );

	if( !bMapCollision )
	{
		SetWeaponDead();
		return true;
	}

	return false;
}

void ioSylphidMissileWeapon::SetWeaponDead()
{
	ioPlayStage *pStage = g_WeaponMgr.GetCreator();
	if( pStage && !m_BoomEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos =  GetPosition();
		ioMapEffect *pBoomEffect = pStage->CreateMapEffect( m_BoomEffect, vPos, ioMath::UNIT_ALL );
		if( pBoomEffect && pBoomEffect->GetEffect() )
		{
			D3DXQUATERNION qtRot;
			ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
			pBoomEffect->SetWorldOrientation( qtRot );
		}

		ioWeapon::PlayExplosionSound();
	}

	ioWeapon::SetWeaponDead();
}

void ioSylphidMissileWeapon::CheckOwnerRotate()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	SetOrientation( qtRot );

	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	m_vMoveDir = vDir;
}

void ioSylphidMissileWeapon::SetLiveTime( DWORD dwTime )
{
}

int ioSylphidMissileWeapon::SetSylphid()
{
	if( GetAttribute()->m_bEnableSylphid )
	{
		m_dwSylphidStartTime = FRAMEGETTIME();
		m_bSetSylphid = true;

		if( m_pSylphidEffect )
		{
			g_EffectFactory.DestroyEffect( m_pSylphidEffect );
			m_pSylphidEffect = NULL;
		}

		m_pSylphidEffect = g_EffectFactory.CreateEffect( m_SylphidEffect );
		if( m_pSylphidEffect )
		{
			GetParentSceneNode()->AttachObject( m_pSylphidEffect );
			GetParentSceneNode()->SetScale( ioMath::UNIT_ALL );
		}

		return GetAttribute()->m_iSylphidNeed;
	}

	m_dwSylphidStartTime = 0;
	m_bSetSylphid = false;
	return 0;
}


