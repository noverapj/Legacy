

#include "stdafx.h"

#include "ioIceMissileWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioIceMissileWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioIceMissileWeapon::m_DieActionStopType = AST_NONE;

ioIceMissileWeapon::ioIceMissileWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_WOUND_NO_SOUND |
					WP_DEF_NO_SOUND );

	m_pIceMesh = NULL;
	m_IceState = IS_MOVE;

	m_bFirst = true;
	m_bResistance = false;
	m_bSetGraceClose = false;

	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;

	m_bWeaponSync = false;
	m_PreSyncTime = 0;
	m_SyncTerm = 500;
}

ioIceMissileWeapon::~ioIceMissileWeapon()
{
	if( m_pIceMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioIceMissileWeapon::~ioIceMissileWeapon - UnReleased Resouce" );
	}
}

bool ioIceMissileWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return SetIceMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioIceMissileWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	m_bEndExplosion = rkLoader.LoadBool_e( "end_explosion", false );
	m_fEnableMoveGap = rkLoader.LoadFloat_e( "enable_move_gap_height", 0.0f );

	int iCnt = rkLoader.LoadInt_e( "change_dir_cnt", 0 );
	if( iCnt > 0 )
	{
		m_ChangeDirInfoList.clear();
		m_ChangeDirInfoList.reserve( iCnt );

		for( int i=0; i < iCnt; ++i )
		{
			ChangeDirInfo kInfo;

			wsprintf_e( szBuf, "change_range%d", i+1 );
			kInfo.m_fChangeRange = rkLoader.LoadFloat( szBuf, 0.0f );

			wsprintf_e( szBuf, "change_speed%d", i+1 );
			kInfo.m_fChangeSpeed = rkLoader.LoadFloat( szBuf, 0.0f );

			wsprintf_e( szBuf, "change_angle_h%d", i+1 );
			kInfo.m_fChangeAngleH = rkLoader.LoadFloat( szBuf, 0.0f );

			wsprintf_e( szBuf, "change_angle_v%d", i+1 );
			kInfo.m_fChangeAngleV = rkLoader.LoadFloat( szBuf, 0.0f );

			m_ChangeDirInfoList.push_back( kInfo );
		}

		std::sort( m_ChangeDirInfoList.begin(), m_ChangeDirInfoList.end(), ChangeDirSort() );
	}
}

bool ioIceMissileWeapon::SetIceMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pIceMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pIceMesh )	return false;

	GetParentSceneNode()->AttachObject( m_pIceMesh );
	GetParentSceneNode()->SetScale( ioMath::UNIT_ALL );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pIceMesh->GetSkeletonName().IsEmpty() && m_pIceMesh->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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

	m_pIceMesh->UpdateBounds( true );
	m_LocalAttackBox = m_pIceMesh->GetCollisionBox();

	m_vPrePos = GetPosition();

	m_fDefaultGapHeight = fabs( m_vPrePos.y - pOwner->GetWorldPosition().y );

	AddEffectEvent( m_pIceMesh );
	return true;
}

D3DXVECTOR3 ioIceMissileWeapon::GetMoveDir()
{
	return m_vMoveDir;
}

void ioIceMissileWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

void ioIceMissileWeapon::CheckChangeDir()
{
	if( m_ChangeDirInfoList.empty() ) return;

	float fChangeRange = m_ChangeDirInfoList[0].m_fChangeRange;

	if( m_fCurMoveRange >= fChangeRange )
	{
		if( m_ChangeDirInfoList[0].m_fChangeSpeed > 0.0f )
			m_fCurMoveSpeed = m_ChangeDirInfoList[0].m_fChangeSpeed;

		RotateMoveDir( m_ChangeDirInfoList[0].m_fChangeAngleH, m_ChangeDirInfoList[0].m_fChangeAngleV );
		m_ChangeDirInfoList.erase( m_ChangeDirInfoList.begin() );
	}
}

void ioIceMissileWeapon::RotateMoveDir( float fAngleH, float fAngleV )
{
	fAngleH = ioMath::ArrangeHead( fAngleH );
	fAngleV = ioMath::ArrangeHead( fAngleV );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	// vertical
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
	m_vMoveDir = qtRot * m_vMoveDir;

	// horizon
	D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
	m_vMoveDir = qtRot * m_vMoveDir;

	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetOrientation( qtRot );
}

void ioIceMissileWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pIceMesh && m_pIceMesh->GetSkeletonName().IsEmpty() && m_pIceMesh->HasLinkedSystem() )
		return;

	SetPosition( vPos );

	m_fDefaultGapHeight = 0.0f;
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		m_fDefaultGapHeight = fabs( vPos.y - pOwner->GetWorldPosition().y );
	}
}

void ioIceMissileWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckPiercing() )
		return;

	switch( m_IceState )
	{
	case IS_MOVE:
		OnFloating( fTimePerSec, pStage );
		break;
	case IS_EXPLOSION:
		{
			SetGraceClose();

			if( !m_pIceMesh || !m_pIceMesh->IsLive() )
			{
				SetWeaponDead();
			}
		}
		break;
	}

	if( m_pIceMesh )
	{
		m_pIceMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

bool ioIceMissileWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( m_IceState != IS_MOVE )
		return true;

	if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
	{
		Explosion( pStage );
		return true;
	}

	return false;
}

void ioIceMissileWeapon::OnFloating( float fTimePerSec, ioPlayStage *pStage )
{
	if( !m_bFirst )
	{
		CheckChangeDir();

		ioBaseChar *pOwner = GetOwner();

		float fCurSpeed = GetMoveSpeed();
		float fMoveDist = fCurSpeed * fTimePerSec;

		m_vPrePos = GetPosition();
		D3DXVECTOR3 vNewPos = m_vPrePos + (m_vMoveDir * fMoveDist);

		float fHeight = pStage->GetMapHeightNoneEntity( vNewPos.x, vNewPos.z, vNewPos.y+m_pIceMesh->GetBoundBox().GetExtents(1) );
		if( fabs( m_vPrePos.y - fHeight ) < m_fEnableMoveGap )
		{
			vNewPos.y = fHeight + m_fDefaultGapHeight;
		}

		SetPosition( vNewPos );
		m_fCurMoveRange += fMoveDist;

		float fMaxRange = GetMaxRange() + m_fGrowthMoveRange;
		if( m_fCurMoveRange >= fMaxRange )
		{
			if( m_bEndExplosion )
			{
				Explosion( pStage );
				return;
			}
			else
			{
				UpdateWorldAttackBox( fTimePerSec, true );

				SetGraceClose();

				if( !m_pIceMesh || !m_pIceMesh->IsLive() )
				{
					SetWeaponDead();
				}
			}
		}
		else
		{
			UpdateWorldAttackBox( fTimePerSec, false );
		}

		DWORD dwCurTime = FRAMEGETTIME();

		if( m_bWeaponSync && m_PreSyncTime + m_SyncTerm <= dwCurTime && GetOwner()->IsNeedProcess() )
		{
			if( GetOwner()->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_WEAPON_SYNC );			
				kPacket << GetOwnerName();
				kPacket << m_dwWeaponIdx;
				kPacket << GetPosition();
				kPacket << m_vMoveDir;
				P2PNetwork::SendToAllPlayingUser( kPacket );

				m_PreSyncTime = dwCurTime;
			}				
		}
	}

	m_bFirst = false;
}

void ioIceMissileWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pIceMesh )
	{
		g_EffectFactory.DestroyEffect( m_pIceMesh );
		m_pIceMesh = NULL;
	}
}

const ioOrientBox& ioIceMissileWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioIceMissileWeapon::GetType() const
{
	return WT_ICE_MISSILE;
}

ActionStopType ioIceMissileWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioIceMissileWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioIceMissileWeapon::GetMoveSpeed()
{
	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed;
}

float ioIceMissileWeapon::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

void ioIceMissileWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

void ioIceMissileWeapon::Explosion( ioPlayStage *pStage )
{
	if( m_IceState == IS_EXPLOSION )
		return;

	m_IceState = IS_EXPLOSION;

	if( !m_BoomEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos =  GetPosition();
		ioMapEffect *pIceBoom = pStage->CreateMapEffect( m_BoomEffect, vPos, ioMath::UNIT_ALL );
		if( pIceBoom && pIceBoom->GetEffect() )
		{
			D3DXQUATERNION qtRot;
			ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
			pIceBoom->SetWorldOrientation( qtRot );

			pIceBoom->GetEffect()->UpdateBounds( true );
			m_LocalAttackBox = pIceBoom->GetEffect()->GetCollisionBox();

			UpdateWorldAttackBox( 0.0f, false );
		}

		ioWeapon::PlayExplosionSound();
	}
}

bool ioIceMissileWeapon::IsFloating() const
{
	if( m_IceState == IS_MOVE )
		return true;

	return false;
}

bool ioIceMissileWeapon::IsExplosion() const
{
	if( m_IceState == IS_EXPLOSION )
		return true;

	return false;
}

void ioIceMissileWeapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( m_IceState != IS_MOVE )
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

	if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
		ToDummyChar(pTarget) || ToWallZone(pTarget) ||
		GetPiercingType() == CPT_NONE )
	{
		Explosion( pStage );
		return;
	}

	CheckCurAttribute( pTarget );

	ApplyWeaponExp eExplosionType = pTarget->ApplyWeapon( this );
	pStage->AddWeaponExplosionEffect( pTarget, this, eExplosionType );

	AddCollisionEntity( pTarget );
}

bool ioIceMissileWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_IceState != IS_EXPLOSION )
		return false;

	bool bCollision = false;

	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
		bCollision = true;

	CheckPiercing( bCollision );

	return bCollision;
}

DWORD ioIceMissileWeapon::GetCollisionEnableTime() const
{
	if( m_IceState == IS_EXPLOSION )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

void ioIceMissileWeapon::SetOwnerChargeScaleRate( float fRate )
{
	m_fOwnerChargeScaleRate = fRate;

	if( m_fOwnerChargeScaleRate > FLOAT1 )
	{
		D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
		GetParentSceneNode()->SetScale( vScale );
	}
}

bool ioIceMissileWeapon::CheckCollisionLine( ioPlayStage *pStage )
{
	if( !pStage ) return false;

	D3DXVECTOR3 vPos = GetPosition();

	if( m_vPrePos == vPos )
		return false;

	bool bMapCollision = pStage->CheckCollisionLine( m_vPrePos, vPos, true );

	if( !bMapCollision )
	{
		Explosion( pStage );
		return true;
	}

	return false;
}

void ioIceMissileWeapon::SetGraceClose()
{
	if( m_bSetGraceClose )	return;

	if( m_pIceMesh )
	{
		if( m_bEndEffectGrace )
			m_pIceMesh->EndEffectGrace();
		else
			m_pIceMesh->EndEffectForce();
	}

	StopLoopSound();
	m_bSetGraceClose = true;
}

void ioIceMissileWeapon::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	D3DXVECTOR3 vPos, vDir;

	rkPacket >> vPos;
	rkPacket >> vDir;

	SetPosition( vPos );
	m_vMoveDir = vDir;

	m_bFirst = false;
}