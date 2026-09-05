

#include "stdafx.h"

#include "ioBombMissileWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ItemDefine.h"

ActionStopType ioBombMissileWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioBombMissileWeapon::m_DieActionStopType = AST_NONE;

ioBombMissileWeapon::ioBombMissileWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ATTACK_OWNER | WP_MINE_EXPLOSION );

	m_pBombMesh = NULL;
	m_BombState = BS_FLOATING;

	m_bFirst = true;
	m_bResistance = false;
	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;

	m_bArleadyCallWeapon = false;

	m_dwAreaWeaponIndex = 0;

	//추가 2014.05.28
	m_OrientationState = OS_NONE;
	m_PointEffectID	= -1;
}

ioBombMissileWeapon::~ioBombMissileWeapon()
{
	if( m_pBombMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioBombMissileWeapon::~ioBombMissileWeapon - UnReleased Resouce" );
	}
}

bool ioBombMissileWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return SetBombMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioBombMissileWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	m_bDisableMapCollision = rkLoader.LoadBool_e( "disable_map_collision", false );
	m_bEnableMapCollision = rkLoader.LoadBool_e( "enable_map_collision", false );
	m_bEndExplosion = rkLoader.LoadBool_e( "end_explosion", false );
	m_bCallExplosion = rkLoader.LoadBool_e( "call_explosion", false );
	m_fExplosionRange = rkLoader.LoadFloat_e( "explosion_range", 0.0f );

	m_ExplosionType = (ExplosionType)rkLoader.LoadInt_e( "explosion_type", ET_RANGE );
	m_vExplosionAxis.x = rkLoader.LoadFloat_e( "explosion_axis_x", 0.0f );
	m_vExplosionAxis.y = rkLoader.LoadFloat_e( "explosion_axis_y", 0.0f );
	m_vExplosionAxis.z = rkLoader.LoadFloat_e( "explosion_axis_z", 0.0f );

	m_vStartOffSet.x = rkLoader.LoadFloat_e( "start_offset_x", 0.0f );
	m_vStartOffSet.y = rkLoader.LoadFloat_e( "start_offset_y", 0.0f );
	m_vStartOffSet.z = rkLoader.LoadFloat_e( "start_offset_z", 0.0f );

	m_OwnerRotate = (OwnerRotType)rkLoader.LoadInt_e( "owner_rotate", ORT_NONE );
	m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "rotate_speed", 0 );
	m_fRotateEndRange = rkLoader.LoadFloat_e( "rotate_end_range", 0.0f );

	m_fStartSpeed = rkLoader.LoadFloat_e( "start_speed", 0.0f );
	m_fMaxSpeed = rkLoader.LoadFloat_e( "max_speed", 0.0f );
	m_fAccelSpeed = rkLoader.LoadFloat_e( "accel_speed", 0.0f );

	m_bPiercingStruct = rkLoader.LoadBool_e( "piercing_struct", false );

	//추가 2014.05.28
	m_OrientationState = (Orientation_State)rkLoader.LoadInt_e( "orientation_state", OS_NONE );

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

	LoadCallAttributeList( rkLoader );

	rkLoader.LoadString( "end_point_effect", "", szBuf, MAX_PATH );
	m_PointEffect = szBuf;

	LoadDummyCharProperty( rkLoader );

}

void ioBombMissileWeapon::LoadCallAttributeList( ioINILoader &rkLoader )
{
	m_CallWeaponInfoList.clear();

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "call_attribute_cnt", 0 );
	if( iCnt == 0 )
		return;

	m_CallWeaponInfoList.reserve( iCnt );

	for( int i=0; i<iCnt; ++i )
	{
		WeaponInfo kCallAttribute;

		wsprintf_e( szKey, "call_attribute%d_index", i+1 );
		kCallAttribute.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_resistance", i+1 );
		kCallAttribute.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_time", i+1 );
		kCallAttribute.m_dwCallTime = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_wound_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kCallAttribute.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "call_attribute%d_wound_duration", i+1 );
		kCallAttribute.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_wound_loop_ani", i+1 );
		kCallAttribute.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_CallWeaponInfoList.push_back( kCallAttribute );
	}
}

void ioBombMissileWeapon::LoadDummyCharProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	m_DummyCharInfoList.clear();

	int nMaxInfo = rkLoader.LoadInt_e( "max_dummy_cnt", 0 );
	for ( int i=0; i<nMaxInfo; i++ )
	{
		CreateInfo sDummyInfo;
		wsprintf_e( szKey, "dummy%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		sDummyInfo.m_szCreateName = szBuf;

		wsprintf_e( szKey, "dummy%d_x_offset", i+1 );
		sDummyInfo.m_vPos.x = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "dummy%d_y_offset", i+1 );
		sDummyInfo.m_vPos.y = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "dummy%d_z_offset", i+1 );
		sDummyInfo.m_vPos.z = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "dummy%d_angle", i+1 );
		sDummyInfo.m_fAngle = rkLoader.LoadFloat( szKey, 0.0f );

		m_DummyCharInfoList.push_back( sDummyInfo );
	}
}

bool ioBombMissileWeapon::SetBombMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pBombMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pBombMesh )	return false;

	GetParentSceneNode()->AttachObject( m_pBombMesh );
	GetParentSceneNode()->SetScale( ioMath::UNIT_ALL );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pBombMesh->GetSkeletonName().IsEmpty() && m_pBombMesh->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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

	m_pBombMesh->UpdateBounds( true );
	m_LocalAttackBox = m_pBombMesh->GetCollisionBox();

	AddEffectEvent( m_pBombMesh );
	return true;
}

void ioBombMissileWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

	//변경 2014.05.28
	switch(m_OrientationState)
	{
	case OS_NONE:
		{
			D3DXQUATERNION qtRot;
			ioMath::CalcDirectionQuaternion( qtRot, -vDir );
			SetOrientation( qtRot );
		}
		break;
	case OS_ORIENTATION:
		{
			D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( m_vMoveDir );
			SetOrientation( qtRot );
		}
		break;
	}	
}

void ioBombMissileWeapon::CheckChangeDir()
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

void ioBombMissileWeapon::RotateMoveDir( float fAngleH, float fAngleV )
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

	switch(m_OrientationState)
	{
	case OS_NONE:
		{
			ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
			SetOrientation( qtRot );
		}
		break;
	case OS_ORIENTATION:
		{
			qtRot = Help::ConvertDirToQuaternion( m_vMoveDir );
			SetOrientation( qtRot );
		}
		break;
	}
}

void ioBombMissileWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pBombMesh && m_pBombMesh->GetSkeletonName().IsEmpty() && m_pBombMesh->HasLinkedSystem() )
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
	CreateMapEffect( GetOwner(), pStage );
}

void ioBombMissileWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckPiercing() )
		return;

	switch( m_BombState )
	{
	case BS_FLOATING:
		OnFloating( fTimePerSec, pStage );
		break;
	case BS_EXPLOSION:
		CreateDummyCharbyExplosion( GetOwner() );
		m_BombState = BS_EXPLOSION_END;
		break;
	case BS_EXPLOSION_END:
		SetWeaponDead();
		break;
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

bool ioBombMissileWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( m_bDisableMapCollision ) 
		return true;

	if( !m_bEnableMapCollision && m_BombState != BS_FLOATING )
		return true;

	if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
	{
		Explosion( pStage );
		return true;
	}

	return false;
}

void ioBombMissileWeapon::OnFloating( float fTimePerSec, ioPlayStage *pStage )
{
	if( !m_bFirst )
	{
		CheckChangeDir();

		ioBaseChar *pOwner = GetOwner();
		if( pOwner )
		{
			switch( m_OwnerRotate )
			{
			case ORT_NORMAL:
				CheckOwnerRotateDir( fTimePerSec );
				break;
			case ORT_STAFF3:
				{
					ioMagicStaffItem3 *pStaff3 = ToMagicStaffItem3( pOwner->GetPriorityItem( SPT_ATTACK ) );
					if( pStaff3 && pStaff3->GetCurFireState() == ioMagicStaffItem3::FS_GATHERING )
					{
						CheckOwnerRotateDir( fTimePerSec );
					}
				}
				break;
			}
		}

		float fCurSpeed = CheckCurSpeed( fTimePerSec );
		float fMoveDist = fCurSpeed * fTimePerSec;

		GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
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
				SetWeaponDead();
			}
		}
	}

	m_bFirst = false;
	
	if( m_pBombMesh )
	{
		m_pBombMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioBombMissileWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pBombMesh )
	{
		g_EffectFactory.DestroyEffect( m_pBombMesh );
		m_pBombMesh = NULL;
	}
}

const ioOrientBox& ioBombMissileWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioBombMissileWeapon::GetType() const
{
	return WT_BOMB_MISSILE;
}

ActionStopType ioBombMissileWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioBombMissileWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioBombMissileWeapon::GetMoveSpeed()
{
	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = m_fStartSpeed;

	return m_fCurMoveSpeed;
}

float ioBombMissileWeapon::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

void ioBombMissileWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

bool ioBombMissileWeapon::IsCanMineExplosion()
{
	if( !ioWeapon::IsCanMineExplosion() )
		return false;

	if( !IsExplosion() )
		return false;

	return true;
}

void ioBombMissileWeapon::Explosion( ioPlayStage *pStage )
{
	if( m_BombState == BS_EXPLOSION || m_BombState == BS_EXPLOSION_END )
		return;

	m_BombState = BS_EXPLOSION;

	if( m_bCallExplosion )
	{
		CallAttribute();
	}
	else
	{
		if ( m_ExplosionType == ET_RANGE )
		{
			float fHalfRange = m_fExplosionRange;
			m_LocalAttackBox.SetExtents( 0, fHalfRange );
			m_LocalAttackBox.SetExtents( 1, fHalfRange );
			m_LocalAttackBox.SetExtents( 2, fHalfRange );
		}
		else if ( m_ExplosionType == ET_AXIS )
		{
			m_LocalAttackBox.SetExtents( 0, m_vExplosionAxis.x );
			m_LocalAttackBox.SetExtents( 1, m_vExplosionAxis.y);
			m_LocalAttackBox.SetExtents( 2, m_vExplosionAxis.z );
		}
		
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
}

bool ioBombMissileWeapon::IsFloating() const
{
	if( m_BombState == BS_FLOATING )
		return true;

	return false;
}

bool ioBombMissileWeapon::IsExplosion() const
{
	if( m_BombState == BS_EXPLOSION || m_BombState == BS_EXPLOSION_END )
		return true;

	return false;
}

void ioBombMissileWeapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
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

	if( !m_bPiercingStruct &&
		( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
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

bool ioBombMissileWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_BombState != BS_EXPLOSION && m_BombState != BS_EXPLOSION_END )
		return false;


	if ( m_ExplosionType == ET_RANGE )
	{
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
	}
	else if ( m_ExplosionType == ET_AXIS )
	{
		D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
		if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
			return true;
	}
	

	return false;
}

DWORD ioBombMissileWeapon::GetCollisionEnableTime() const
{
	if( m_BombState == BS_EXPLOSION || m_BombState == BS_EXPLOSION_END )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

void ioBombMissileWeapon::SetOwnerChargeScaleRate( float fRate )
{
	m_fOwnerChargeScaleRate = fRate;

	if( m_fOwnerChargeScaleRate > FLOAT1 )
	{
		D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
		GetParentSceneNode()->SetScale( vScale );
	}
}

void ioBombMissileWeapon::CheckOwnerRotateDir( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( m_dwRotateTime == 0 ) return;
	if( m_fCurMoveRange > m_fRotateEndRange )
		return;

	D3DXQUATERNION	qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vTargetDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	float fDotValue = D3DXVec3Dot( &vTargetDir, &m_vMoveDir );
	if( fDotValue >= 0.98f )
	{
		SetOrientation( qtRot );
		m_vMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -0.98f )
	{
		vAxis = ioMath::UNIT_Y;
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vTargetDir, &m_vMoveDir );
		D3DXVec3Normalize( &vAxis, &-vAxis );
	}

	float fAngleGap = RADtoDEG( acosf(fDotValue) );
	float fRate = (fTimePerSec*1000) / m_dwRotateTime;
	float fRotateAngle = 360.0f * fRate;

	if( fAngleGap < fRotateAngle )
	{
		SetOrientation( qtRot );
		m_vMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	}
	else
	{
		D3DXQUATERNION	qtNewRot;
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle) );

		m_vMoveDir = qtNewRot * m_vMoveDir;

		ioMath::CalcDirectionQuaternion( qtNewRot, -m_vMoveDir );
		SetOrientation( qtNewRot );
	}
}

float ioBombMissileWeapon::CheckCurSpeed( float fTimePerSec )
{
	float fCurSpeed = GetMoveSpeed();

	if( m_fStartSpeed != m_fMaxSpeed || m_fAccelSpeed > 0.0f )
	{
		float fGapTime = (float)(FRAMEGETTIME() - m_dwCreatedTime);
		float fDTime = fGapTime / FLOAT1000;
		float fCurAccel = m_fAccelSpeed * fDTime * fDTime;

		fCurSpeed += fCurAccel;
		fCurSpeed = min( fCurSpeed, m_fMaxSpeed );
	}

	m_fCurMoveSpeed = fCurSpeed;
	return fCurSpeed * m_fOwnerChargeSpeedRate;
}

void ioBombMissileWeapon::CallAttribute()
{
	if( m_bArleadyCallWeapon )
		return;

	if( m_CallWeaponInfoList.empty() )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	DWORD dwCurGenerateTime = FRAMEGETTIME();

	WeaponInfoList::iterator iter;
	for( iter = m_CallWeaponInfoList.begin(); iter != m_CallWeaponInfoList.end(); ++iter )
	{
		WeaponInfo pInfo = (*iter);

		g_WeaponMgr.SetReserveCallWeaponInfo( GetOwnerName(), 
			pInfo,
			GetPosition(),
			m_vMoveDir,
			dwCurGenerateTime + pInfo.m_dwCallTime,
			m_dwWeaponIdx );
	}

	m_bArleadyCallWeapon = true;
}


void ioBombMissileWeapon::SetWeaponDead()
{
	ioWeapon::SetWeaponDead();

	ioPlayStage *pStage = g_WeaponMgr.GetCreator();
	if( pStage && m_dwAreaWeaponIndex > 0 )
	{
		pStage->DestroyAreaWeapon( m_dwAreaWeaponIndex );
	}

	if( pStage && m_PointEffectID != -1 )
	{
		pStage->DestroyMapEffect( m_PointEffectID );
	}
}


void ioBombMissileWeapon::SetDestroyAreaWeaponIndex( DWORD dwIndex )
{
	m_dwAreaWeaponIndex = dwIndex;
}


void ioBombMissileWeapon::CreateMapEffect( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	D3DXVECTOR3 vPos = GetPosition();
	vPos.y = pStage->GetMapHeight( vPos.x, vPos.z );

	if( m_PointEffectID != -1 )
	{
		pStage->DestroyMapEffect( m_PointEffectID );
	}

	ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_PointEffect, vPos, vScale );
	if( pMapEffect )
	{
		m_PointEffectID = pMapEffect->GetUniqueID();
	}
}

void ioBombMissileWeapon::CreateDummyCharbyExplosion( ioBaseChar *pOwner )
{
	if ( !pOwner || m_DummyCharInfoList.empty() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	D3DXVECTOR3 vMoveDir = D3DXVECTOR3( m_vMoveDir.x, 0.0f, m_vMoveDir.z );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );
	D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( vMoveDir );
	D3DXVECTOR3 vPos = GetPosition();

	int nMaxCnt = (int)m_DummyCharInfoList.size();
	for ( int i=0; i<nMaxCnt; i++ )
	{	
		//g_DummyCharMgr.DestroyDummyCharByName( pOwner, m_DummyCharInfoList[i].m_szCreateName );

		if ( vPos.y <= 0.0f )
			continue;

		D3DXVECTOR3 vNewPos = vPos + qtRot * m_DummyCharInfoList[i].m_vPos;
		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(m_DummyCharInfoList[i].m_fAngle), 0.0f, 0.0f );

		D3DXVECTOR3 vRotDir = qtAngle * vMoveDir;
		qtAngle = Help::ConvertDirToQuaternion( vRotDir );

		int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
		ioDummyChar *pDummy = pStage->CreateDummyChar( m_DummyCharInfoList[i].m_szCreateName, iCurIndex, pOwner->GetCharName(), vNewPos, m_DummyCharInfoList[i].m_fAngle, 0, true );
		if ( pDummy )
		{
			pDummy->SetWorldPosition( vNewPos );
			pDummy->SetWorldOrientationAndDir( qtAngle );
		}
	}
}