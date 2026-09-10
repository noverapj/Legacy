

#include "stdafx.h"

#include "ioMissileWeapon3.h"

#include "ioBaseChar.h"

ActionStopType ioMissileWeapon3::m_ActionStopType = AST_NONE;
ActionStopType ioMissileWeapon3::m_DieActionStopType = AST_NONE;

ioMissileWeapon3::ioMissileWeapon3( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ONE_TIME_USE | WP_MISSILE_WEAPON | WP_DIVIDE_ACTION );

	m_pFireBall = NULL;

	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;
	m_fCurMoveRate = 0.0f;

	m_bSetGraceClose = false;
	m_bFirst = true;

	m_pSecondAttr = NULL;
}

ioMissileWeapon3::~ioMissileWeapon3()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioMissileWeapon3::~ioMissileWeapon3 - UnReleased Resouce" );
	}

	m_vSecondAttrList.clear();
}

bool ioMissileWeapon3::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioMissileWeapon3::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	
	m_bDisableOwnerScale = rkLoader.LoadBool_e( "disable_owner_scale", false );

	m_vStartOffSet.x = rkLoader.LoadFloat_e( "start_offset_x", 0.0f );
	m_vStartOffSet.y = rkLoader.LoadFloat_e( "start_offset_y", 0.0f );
	m_vStartOffSet.z = rkLoader.LoadFloat_e( "start_offset_z", 0.0f );

	int iCnt = rkLoader.LoadInt_e( "change_dir_cnt", 0 );
	if( iCnt <= 0 ) return;

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

bool ioMissileWeapon3::CreateFireBall( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pFireBall = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pFireBall )	return false;

	GetParentSceneNode()->AttachObject( m_pFireBall );

	if( m_bDisableOwnerScale )
		GetParentSceneNode()->SetScale( FLOAT1, FLOAT1, FLOAT1 );

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

	m_StartPos = GetPosition();

	AddEffectEvent( m_pFireBall );
	return true;
}

bool ioMissileWeapon3::ChangeWeaponEffect( const ioHashString& szEffectName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	if( szEffectName.IsEmpty() )
		return false;

	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}

	m_pFireBall = g_EffectFactory.CreateEffect( szEffectName );
	if( !m_pFireBall )	return false;

	GetParentSceneNode()->AttachObject( m_pFireBall );

	AddEffectEvent( m_pFireBall );
	return true;
}

void ioMissileWeapon3::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

void ioMissileWeapon3::SetMoveSpeed( float fSpeed )
{
	m_fCurMoveSpeed = fSpeed;
}

void ioMissileWeapon3::CheckChangeDir()
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

void ioMissileWeapon3::RotateMoveDir( float fAngleH, float fAngleV )
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

void ioMissileWeapon3::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireBall && m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->HasLinkedSystem() )
		return;

	ioBaseChar *pOwner = GetOwner();
	D3DXVECTOR3 vNewPos = vPos;
	if( pOwner && m_vStartOffSet != ioMath::VEC3_ZERO )
	{
		D3DXQUATERNION qtRot = pOwner->GetTargetRot();
		D3DXVECTOR3 vOffSet = qtRot * m_vStartOffSet;

		vNewPos += vOffSet;
	}

	SetPosition( vNewPos );
	m_StartPos = GetPosition();
}

void ioMissileWeapon3::SetGraceClose()
{
	if( m_bSetGraceClose )	return;

	if( m_pFireBall )
	{
		if( m_bEndEffectGrace )
			m_pFireBall->EndEffectGrace();
		else
			m_pFireBall->EndEffectForce();
	}

	StopLoopSound();
	m_bSetGraceClose = true;
}

void ioMissileWeapon3::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckPiercing() )
		return;

	if( !m_bFirst )
	{
		CheckChangeDir();

		float fMoveDist = GetMoveSpeed() * fTimePerSec;
		GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
		m_fCurMoveRange += fMoveDist;

		float fMaxRange = GetMaxRange() + m_fGrowthMoveRange;
		if( fMaxRange > 0.0f )
			m_fCurMoveRate = m_fCurMoveRange / fMaxRange;

		if( m_fCurMoveRange < fMaxRange )
		{
			UpdateWorldAttackBox( fTimePerSec, false );
		}
		else
		{
			UpdateWorldAttackBox( fTimePerSec, true );

			SetGraceClose();

			if( !m_pFireBall || !m_pFireBall->IsLive() )
			{
				SetWeaponDead();
			}
		}
	}

	m_bFirst = false;
	
	if( m_pFireBall )
	{
		m_pFireBall->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioMissileWeapon3::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

D3DXVECTOR3 ioMissileWeapon3::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return m_vMoveDir;
}

const ioOrientBox& ioMissileWeapon3::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioMissileWeapon3::GetType() const
{
	return WT_MISSILE3;
}

ActionStopType ioMissileWeapon3::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioMissileWeapon3::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioMissileWeapon3::GetMoveSpeed()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMoveDelayTime > 0 && m_dwCreatedTime+m_dwMoveDelayTime >= dwCurTime )
		return 0.0f;

	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed * m_fOwnerChargeSpeedRate;
}

float ioMissileWeapon3::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

void ioMissileWeapon3::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	if( !IsLive() || bSetNull )
	{
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
		return;
	}

	m_WorldAttackBox = GetLocalAttackBox();

	float fScaleRate = FLOAT1 + (GetAttribute()->m_fScaleRate * m_fCurMoveRate );
	float fExtentsX = m_WorldAttackBox.GetExtents(0) * fScaleRate;
	float fExtentsY = m_WorldAttackBox.GetExtents(1) * fScaleRate;

	m_WorldAttackBox.SetExtents( 0, fExtentsX );
	m_WorldAttackBox.SetExtents( 1, fExtentsY );

	float fMapCollisionRate = GetAttribute()->m_fMapCollisionBoxRate;
	m_WorldAttackBoxForMap = m_WorldAttackBox;
	m_WorldAttackBoxForMap.SetExtents( 0, m_WorldAttackBox.GetExtents(0)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 1, m_WorldAttackBox.GetExtents(1)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 2, m_WorldAttackBox.GetExtents(2)*fMapCollisionRate );

	if( !m_WorldAttackBox.IsNull() )
	{
		if( m_bDisableOwnerScale )
		{
			m_WorldAttackBox.Transform( GetPosition(), ioMath::UNIT_ALL, GetParentSceneNode()->GetDerivedOrientation() );

			m_WorldAttackBoxForMap.Transform( GetPosition(), ioMath::UNIT_ALL, GetParentSceneNode()->GetDerivedOrientation() );
		}
		else
		{
			m_WorldAttackBox.Transform( GetPosition(),
										GetScale(),
										GetParentSceneNode()->GetDerivedOrientation() );

			m_WorldAttackBoxForMap.Transform( GetPosition(),
											  GetScale(),
											  GetParentSceneNode()->GetDerivedOrientation() );
		}
	}
}

void ioMissileWeapon3::SetSecondAttribute( SecondAttributeList vSecondList )
{
	int iSize = vSecondList.size();
	for( int i=0; i < iSize; ++i )
	{
		SecondAttribute kSecondAttr = vSecondList[i];
		m_vSecondAttrList.push_back( kSecondAttr );
	}
}

void ioMissileWeapon3::CheckCurAttribute( const ioPlayEntity *pTarget )
{
	ioWeapon::CheckCurAttribute( pTarget );

	if( m_bResistance ) return;

	int iSize = m_vSecondAttrList.size();
	if( iSize > 0 )
	{
		for( int i=0; i < iSize-1; ++i )
		{
			if( m_fCurMoveRange < m_vSecondAttrList[i].m_fLimiteValue )
			{
				m_pSecondAttr = g_WeaponMgr.FindAttribute( m_vSecondAttrList[i].m_iAttributeIndex );
				return;
			}
		}

		m_pSecondAttr = g_WeaponMgr.FindAttribute( m_vSecondAttrList[iSize-1].m_iAttributeIndex );
		return;
	}

	m_pSecondAttr = NULL;
	return;
}

const WeaponAttribute* ioMissileWeapon3::GetAttribute() const
{
	if( m_bResistance )
	{
		if( m_pResistanceAttr )
			return m_pResistanceAttr;
	}

	if( m_pSecondAttr )
		return m_pSecondAttr;

	return m_pConstAttr;
}

bool ioMissileWeapon3::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	bool bCollision = false;

	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
		bCollision = CheckCollisionLine( pEntity, vColPoint );

	CheckPiercing( bCollision );

	return bCollision;
}

bool ioMissileWeapon3::CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint )
{
	if( GetPiercingType() == CPT_ALL || GetPiercingType() == CPT_ALL_LIMITE )
		return true;

	// 타겟이 캐릭터가 아니라면 충돌로 처리한다
	ioBaseChar *pTargetChar = ToBaseChar( pTarget );
	if( !pTargetChar )	return true;

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
		return pOwner->CheckCollisionLine( m_StartPos, vColPoint );

	return false;	// 주인이 없는 무기라면 충돌안한걸로 하자
}

void ioMissileWeapon3::SetWeaponDead()
{
	CreateCollisionAreaWeapon();

	ioWeapon::SetWeaponDead();
}



