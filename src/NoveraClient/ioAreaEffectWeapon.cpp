

#include "stdafx.h"

#include "ioAreaEffectWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioAreaEffectWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioAreaEffectWeapon::m_DieActionStopType = AST_NONE;

ioAreaEffectWeapon::ioAreaEffectWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_DEF_NO_SOUND );

	m_TargetColType = TCT_CYLINDER;
	m_TargetWoundType = TWT_ALL;

	m_fCollisionRange = 0.0f;
	m_fCollisionInRange = 0.0f;

	m_fAreaAngle = 360.0f;
	m_fUpHeight = 0.0f;
	m_fUnderHeight = 0.0f;
	m_fWidth = 0.0f;

	m_vCollisionDir = D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
}

ioAreaEffectWeapon::~ioAreaEffectWeapon()
{
	StopLoopSound();
}

void ioAreaEffectWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	m_bCheckCollinsionOnlyOne = rkLoader.LoadBool_e("check_collision_only_one", false );

	m_bEndAreaAttack = rkLoader.LoadBool_e( "end_area_attack", false );
}

void ioAreaEffectWeapon::SetAreaValue( float fRange, float fInRange, float fAngle, float fUpHeight, float fUnderHeight, float fWidth )
{
	m_fCollisionRange = fRange;
	m_fCollisionInRange = fInRange;
	m_fAreaAngle = fAngle;

	m_fUpHeight    = fUpHeight;
	m_fUnderHeight = fUnderHeight;
	m_fWidth       = fWidth;

	m_LocalAttackBox.SetExtents( 0, fRange );
	m_LocalAttackBox.SetExtents( 1, fRange );
	m_LocalAttackBox.SetExtents( 2, fRange );

	UpdateWorldAttackBox( 0.0f, false );
}

void ioAreaEffectWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	SetPosition( vPos );
}

void ioAreaEffectWeapon::SetCollisionDir( D3DXVECTOR3 &vDir )
{
	m_vCollisionDir = vDir;
}

D3DXVECTOR3 ioAreaEffectWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return m_vCollisionDir;
}

void ioAreaEffectWeapon::SetCollisionWoundType( int iColType, int iWoundType )
{
	m_TargetColType = (TargetColType)iColType;
	m_TargetWoundType = (TargetWoundType)iWoundType;
}

void ioAreaEffectWeapon::UpdatePosition( const D3DXVECTOR3 &vPos )
{
	SetPosition( vPos );
}

void ioAreaEffectWeapon::UpdateRange( float fRange, float fInRange )
{
	m_fCollisionRange = fRange;
	m_fCollisionInRange = fInRange;
}

void ioAreaEffectWeapon::UpdateCollisionList( BaseCharList &rkCharList, bool bRefresh )
{
	if( bRefresh )
	{
		ClearCollisionList();
	}

	m_vNewTargetList.clear();

	if( rkCharList.empty() )
		return;

	BaseCharList::iterator iter = rkCharList.begin();
	while( iter != rkCharList.end() )
	{
		ioBaseChar *pChar = (*iter);
		if( pChar )
		{
			ioHashString szName = pChar->GetCharName();
			m_vNewTargetList.push_back( szName );

			stWantCheckList stInfo;
			stInfo.m_stName = szName;
			m_vWantCheckList.push_back( stInfo );
		}

		++iter;
	}
}

ioWeapon::WeaponType ioAreaEffectWeapon::GetType() const
{
	return WT_AREA_EFFECT;
}

ActionStopType ioAreaEffectWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioAreaEffectWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioAreaEffectWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( !CheckPiercing() )
		return;

	UpdateWorldAttackBox( fTimePerSec, false );

	if ( m_bEndAreaAttack )
	{
		if( m_dwLiveTime > 0 && m_dwCreatedTime + m_dwLiveTime < dwCurTime )
			SetWeaponDead();
	}
}

bool ioAreaEffectWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	DWORD dwCurTime = FRAMEGETTIME();

	// 피터팬 SD 위한 추가 보완
	if( m_dwLiveTime > 1 && m_dwCreatedTime + m_dwLiveTime < dwCurTime )
	{
		return false;
	}

	bool bCollision = false;

	ioBaseChar *pChar = ToBaseChar(pEntity);
	if( pChar && IsExistTarget(pChar->GetCharName()) )
		bCollision = true;

	//피터팬때 추가( by 윤태철 )
	if( m_bCheckCollinsionOnlyOne && bCollision && pChar)
	{
		for( size_t i = 0; i < m_vWantCheckList.size(); i++ )
		{
			if( m_vWantCheckList[i].m_stName == pChar->GetCharName() )
			{
				if( m_vWantCheckList[i].m_bTested )
					return false;

				m_vWantCheckList[i].m_bTested = true;
				break;
			}
		}
	}

	if( bCollision && pChar && (pChar->IsProtectState() || pChar->IsStateProtect(AS_NONE, GetDefenseBreakType() )) )
	{
		int iType = pChar->CheckBlockEmoticonType();
		if( iType >= 0 && !IsAlreadyBlocked( pChar ) )
		{
			AddBlockEntity( pChar );
			pChar->SetProtectWound( iType );
		}

		bCollision = false;
	}

	return bCollision;
}

bool ioAreaEffectWeapon::CheckCollisionSphere( ioPlayEntity *pEntity )
{
	D3DXVECTOR3 vPos = GetWorldAttackBox().GetCenter();

	D3DXVECTOR3 vPoint;
	if( !pEntity->CheckSpherePoint( vPos, m_fCollisionRange, vPoint ) )
		return false;

	D3DXVECTOR3 vDir = vPoint - vPos;
	D3DXVec3Normalize( &vDir, &vDir );

	float fHalfCosine = cosf( DEGtoRAD( m_fAreaAngle ) * FLOAT05 );
	if( D3DXVec3Dot( &m_vCollisionDir, &vDir ) >= fHalfCosine )
		return true;

	return false;
}

bool ioAreaEffectWeapon::CheckCollisionCylinder( ioPlayEntity *pEntity )
{
	if( m_fUpHeight == 0.0f && m_fUnderHeight == 0.0f )
		return false;

	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();

	D3DXVECTOR3 vTop, vBottom;
	vTop = vBottom = vCenter;
	vTop.y += m_fUpHeight;
	vBottom.y -= m_fUnderHeight;
	ioCylinder kCylinder( vTop, vBottom, m_fCollisionRange );
	vCenter = kCylinder.GetCenter();


	// 사각형을 이루는 2점의 포인트를 구한다. 가장 먼거리의 대각선의 2점
	D3DXVECTOR3 vMin, vMax;
	vMax = vTop;
	vMin = vBottom;

	vMax.x += m_fCollisionRange;
	vMax.z -= m_fCollisionRange;

	vMin.x -= m_fCollisionRange;
	vMin.z += m_fCollisionRange;

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
	float fRangeSq = m_fCollisionRange * m_fCollisionRange;
	float fInRangeSq = m_fCollisionInRange * m_fCollisionInRange;

	if( fTargetDist > fRangeSq )
		return false;

	if( m_fCollisionInRange > 0.0f && fTargetDist < fInRangeSq )
		return false;

	// 수평 각 체크
	float fHalfCosine = cosf( DEGtoRAD( m_fAreaAngle ) * FLOAT05 );
	D3DXVec3Normalize( &vDiff1, &vDiff1 );

	if( D3DXVec3Dot( &m_vCollisionDir, &vDiff1 ) < fHalfCosine )
		return false;

	// 높이 체크
	if( vCollisionPoint.y >= vBottom.y && vCollisionPoint.y <= vTop.y )
		return true;

	return false;
}

bool ioAreaEffectWeapon::CheckCollisionBox( ioPlayEntity *pEntity )
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
	vCenter += m_vCollisionDir * m_fCollisionRange * FLOAT05; 
	D3DXVECTOR3 vOffset( 0.0f, ( ( m_fUpHeight + m_fUnderHeight ) * FLOAT05 ) - m_fUpHeight, 0.0f );
	vCenter -= vOffset;

	// 사각형을 이루는 2점의 포인트를 구한다. 가장 먼거리의 대각선의 2점
	D3DXVECTOR3 vMin, vMax;
	vMin = vMax = vCenter;
	vMin.x -= ( m_fWidth * FLOAT05 );
	vMin.y -= m_fUnderHeight;
	vMax.x += ( m_fWidth * FLOAT05 );
	vMax.y += m_fUpHeight;
	vMax.z += m_fCollisionRange;

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

void ioAreaEffectWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	if( !IsLive() && bSetNull )
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
		m_WorldAttackBox.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
		m_WorldAttackBoxForMap.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
	}
}

const ioOrientBox& ioAreaEffectWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

bool ioAreaEffectWeapon::IsAlreadyBlocked( ioPlayEntity *pEntity ) const
{
	BlockEntityList::const_iterator iter;
	for( iter=m_vBlockEntityList.begin() ; iter!=m_vBlockEntityList.end() ; ++iter )
	{
		if( *iter == pEntity )
			return true;
	}
	return false;
}

void ioAreaEffectWeapon::AddBlockEntity( ioPlayEntity *pEntity )
{
	m_vBlockEntityList.push_back( pEntity );
}

bool ioAreaEffectWeapon::IsExistTarget( const ioHashString &szName )
{
	int iCnt = m_vNewTargetList.size();
	for( int i=0; i < iCnt; ++i )
	{
		if( m_vNewTargetList[i] == szName )
			return true;
	}

	return false;
}


