

#include "stdafx.h"

#include "ioZoneEffect2Weapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioZoneEffect2Weapon::m_ActionStopType = AST_NONE;
ActionStopType ioZoneEffect2Weapon::m_DieActionStopType = AST_NONE;

ioZoneEffect2Weapon::ioZoneEffect2Weapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_DEF_NO_SOUND );

	m_fZoneRange		= 0.0f;
	m_fZoneAngle		= 0.0f;
	
	m_pWeaponEffect = NULL;
	m_OwnerAttachType = OAT_NONE;
	
	m_bSwinged	= false;
	m_vCollisionDir = D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
}

ioZoneEffect2Weapon::~ioZoneEffect2Weapon()
{
	if( m_pWeaponEffect )
	{
		LOG.PrintTimeAndLog( 0, "ioZoneEffect2Weapon::~ioZoneEffect2Weapon - UnReleased Resouce" );
	}
}

void ioZoneEffect2Weapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	DWORD dwCurTime = FRAMEGETTIME();

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	m_dwLiveTime = rkLoader.LoadInt_e( "zone_live_time", 0 );

	m_fZoneRange		= rkLoader.LoadFloat_e( "zone_range", 0.0f );
	m_fZoneAngle		= rkLoader.LoadFloat_e( "zone_angle", 0.0f );

	m_fUpHeight		= rkLoader.LoadFloat_e( "zone_up_height", 0.0f );
	m_fUnderHeight	= rkLoader.LoadFloat_e( "zone_under_height", 0.0f );
	m_fWidth		= rkLoader.LoadFloat_e( "zone_width", 0.0f );

	m_fOffSet		= rkLoader.LoadFloat_e( "zone_offset", 0.0f );
	m_fHeightOffSet	= rkLoader.LoadFloat_e( "zone_height_offset", 0.0f );

	m_TargetColType		= (TargetColType)rkLoader.LoadInt_e( "zone_collision_type", 0 );
	m_TargetWoundType	= (TargetWoundType)rkLoader.LoadInt_e( "zone_wound_type", 0 );

	m_bCheckHeight		= rkLoader.LoadBool_e( "zone_check_height", false );

	//Target Swing Animation
	rkLoader.LoadString_e( "skill_swinged_motion", "", szBuf, MAX_PATH );	m_szWoundedSwingAni = szBuf;
	m_fWoundedSwingRate = rkLoader.LoadFloat_e( "skill_swinged_motion_rate", FLOAT1 );

	//Swing Buff
	m_WoundedSwingInfo.m_bReverseTargetRot = rkLoader.LoadBool_e( "target_reverse_rotate", false );
	m_WoundedSwingInfo.m_GrapplingColType = (GrapplingColType)rkLoader.LoadInt_e( "wound_grappling_enable_col", GCT_NONE );

	rkLoader.LoadString_e( "wound_grappling_loop_motion", "", szBuf, MAX_PATH );
	m_WoundedSwingInfo.m_GrapplingLoopMotion = szBuf;
	m_WoundedSwingInfo.m_fGrapplingLoopMotionRate = rkLoader.LoadFloat_e( "wound_grappling_loop_motion_rate", FLOAT1 );

	//Swing Buff
	int i;
	int iBuffCnt = rkLoader.LoadInt_e( "swing_buff_cnt", 0 );
	for( i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "swing_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_WoundedSwingInfo.m_SwingBuff.push_back( szBuf );
	}

	iBuffCnt = rkLoader.LoadInt_e( "swing_wound_buff_cnt", 0 );
	for( i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "swing_wound_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_WoundedSwingInfo.m_SwingWoundBuff.push_back( szBuf );
	}

	iBuffCnt = rkLoader.LoadInt_e( "swing_cancel_buff_cnt", 0 );
	for( i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "swing_cancel_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_WoundedSwingInfo.m_SwingCancelBuff.push_back( szBuf );
	}
}

void ioZoneEffect2Weapon::SetZoneValue( bool bAttached, bool bSkill )
{
	if( bAttached )
	{
		if( bSkill )
			m_OwnerAttachType = OAT_SKILL;
		else
			m_OwnerAttachType = OAT_NORMAL;
	}
	else
	{
		m_OwnerAttachType = OAT_NONE;
	}

	m_LocalAttackBox.SetExtents( 0, m_fZoneRange );
	m_LocalAttackBox.SetExtents( 1, m_fZoneRange );
	m_LocalAttackBox.SetExtents( 2, m_fZoneRange );

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		if( m_iDummyIndex <= 0 )
		{
			D3DXVECTOR3 vPos = GetPosition();
			D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			vMoveDir = vMoveDir * m_fOffSet;
			vPos = vPos + vMoveDir;

			vPos.y += m_fHeightOffSet;

			SetPosition( vPos );
		}
	}

	UpdateWorldAttackBox( 0.0f, false );

	m_bSwinged = false;
}

void ioZoneEffect2Weapon::SetZoneValue( float fRange, float fAngle, float fUpHeight, float fUnderHeight, float fWidth, bool bAttached )
{
	m_fZoneRange = fRange;
	m_fZoneAngle = fAngle;

	m_fUpHeight    = fUpHeight;
	m_fUnderHeight = fUnderHeight;
	m_fWidth       = fWidth;

	if( bAttached )
	{
		m_OwnerAttachType = OAT_SKILL;
	}
	else
	{
		m_OwnerAttachType = OAT_NONE;
	}
	
	m_LocalAttackBox.SetExtents( 0, m_fZoneRange );
	m_LocalAttackBox.SetExtents( 1, m_fZoneRange );
	m_LocalAttackBox.SetExtents( 2, m_fZoneRange );
	UpdateWorldAttackBox( 0.0f, false );

	m_bSwinged = false;
}

void ioZoneEffect2Weapon::SetZoneValue( float fRange, bool bAttached )
{
	m_fZoneRange		= fRange;

	if( bAttached )
	{
		m_OwnerAttachType = OAT_SKILL;
	}
	else
	{
		m_OwnerAttachType = OAT_NONE;
	}

	m_LocalAttackBox.SetExtents( 0, m_fZoneRange );
	m_LocalAttackBox.SetExtents( 1, m_fZoneRange );
	m_LocalAttackBox.SetExtents( 2, m_fZoneRange );
	UpdateWorldAttackBox( 0.0f, false );

	m_bSwinged = false;
}

void ioZoneEffect2Weapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		m_vStartOffSet = vPos - pOwner->GetWorldPosition();
	}
	else
	{
		m_vStartOffSet = ioMath::VEC3_ZERO;
	}

	SetPosition( vPos );
}

void ioZoneEffect2Weapon::SetCollisionDir( D3DXVECTOR3 &vDir )
{
	m_vCollisionDir = vDir;
}

void ioZoneEffect2Weapon::SetCollisionWoundType( int iColType, int iWoundType )
{
	m_TargetColType = (TargetColType)iColType;
	m_TargetWoundType = (TargetWoundType)iWoundType;
}

ioWeapon::WeaponType ioZoneEffect2Weapon::GetType() const
{
	return WT_ZONE_EFFECT2;
}

void ioZoneEffect2Weapon::SetLiveTime( DWORD dwTime )
{
	if( m_dwLiveTime == 0 )
	{
		m_dwLiveTime = dwTime;
	}
}

ActionStopType ioZoneEffect2Weapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioZoneEffect2Weapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioZoneEffect2Weapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCreatedTime > dwCurTime )
	{
		LOG.PrintTimeAndLog( 0, "ioWeapon::Process - Create Time Error(%d/%d)", m_dwCreatedTime, dwCurTime );
		SetWeaponDead();
		return;
	}

	if( !CheckPiercing() )
		return;

	ioBaseChar *pOwner = GetOwner();

	DWORD dwLooseTime = 0;
	if( pOwner )
	{
		dwLooseTime = pOwner->GetActionStopTotalDelay();
	}

	if( m_dwCreatedTime + m_dwLiveTime + dwLooseTime < FRAMEGETTIME() )
	{
		SetWeaponDead();
	}
	else if( pOwner )
	{
		if( m_iDummyIndex > 0 )
		{
			ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iDummyIndex );
			if( pDummy && m_OwnerAttachType != OAT_NONE )
			{
				D3DXVECTOR3 vPos = pDummy->GetMidPositionByRate();
				SetPosition( vPos );
			}
		}
		else
		{
			if( m_OwnerAttachType == OAT_SKILL && pOwner->GetState() != CS_USING_SKILL )
			{
				SetWeaponDead();
			}
			else if( m_OwnerAttachType != OAT_NONE )
			{
				D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + m_vStartOffSet;
				SetPosition( vPos );
			}
		}
	}

	UpdateWorldAttackBox( fTimePerSec, false );

	if( m_pWeaponEffect )
	{
		m_pWeaponEffect->Update( fTimePerSec * FLOAT1000 );
	}
}

bool ioZoneEffect2Weapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_bSwinged )
		return false;

	CheckSwingBuff( pEntity );

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

	ioBaseChar *pChar = ToBaseChar(pEntity);

	if( bCollision && pEntity->GetSubType() == ioPlayEntity::PST_CHAR )
	{
		if( !pChar )
			return false;

		if( !pChar->CheckTargetWoundType(m_TargetWoundType) )
			return false;
	}

	if( bCollision )
	{
		bCollision = CheckHeight( pChar );
		if( bCollision && UseSwingBuff() )
		{
			SetSwingBuff( pChar );
			return false;
		}				
	}

	if( bCollision && pChar && pChar->IsProtectState() )
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

bool ioZoneEffect2Weapon::CheckCollisionSphere( ioPlayEntity *pEntity )
{
	D3DXVECTOR3 vPos = GetWorldAttackBox().GetCenter();

	D3DXVECTOR3 vPoint;
	if( !pEntity->CheckSpherePoint( vPos, m_fZoneRange, vPoint ) )
		return false;

	D3DXVECTOR3 vDir = vPoint - vPos;
	D3DXVec3Normalize( &vDir, &vDir );

	float fHalfCosine = cosf( DEGtoRAD( m_fZoneAngle ) * FLOAT05 );
	if( D3DXVec3Dot( &m_vCollisionDir, &vDir ) >= fHalfCosine )
		return true;

	return false;
}

bool ioZoneEffect2Weapon::CheckCollisionCylinder( ioPlayEntity *pEntity )
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
		return false;

	// 수평 각 체크
	float fHalfCosine = cosf( DEGtoRAD( m_fZoneAngle ) * FLOAT05 );
	D3DXVec3Normalize( &vDiff1, &vDiff1 );

	if( D3DXVec3Dot( &m_vCollisionDir, &vDiff1 ) < fHalfCosine )
		return false;

	// 높이 체크
	if( vCollisionPoint.y >= vBottom.y && vCollisionPoint.y <= vTop.y )
		return true;

	return false;
}

bool ioZoneEffect2Weapon::CheckCollisionBox( ioPlayEntity *pEntity )
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
	vCenter += m_vCollisionDir * m_fZoneRange * FLOAT05; 
	D3DXVECTOR3 vOffset( 0.0f, ( ( m_fUpHeight + m_fUnderHeight ) * FLOAT05 ) - m_fUpHeight, 0.0f );
	vCenter -= vOffset;

	// 사각형을 이루는 2점의 포인트를 구한다. 가장 먼거리의 대각선의 2점
	D3DXQUATERNION qtScale = pOwnerChar->GetWorldScale();
	D3DXVECTOR3 vMin, vMax;
	vMin = vMax = vCenter * qtScale.x;
	vMin.x -= ( m_fWidth * FLOAT05 ) * qtScale.x;
	vMin.y -= m_fUnderHeight * qtScale.x;
	vMax.x += ( m_fWidth * FLOAT05 ) * qtScale.x;
	vMax.y += m_fUpHeight * qtScale.x;
	vMax.z += m_fZoneRange * qtScale.x;



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

void ioZoneEffect2Weapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

const ioOrientBox& ioZoneEffect2Weapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

bool ioZoneEffect2Weapon::IsAlreadyBlocked( ioPlayEntity *pEntity ) const
{
	BlockEntityList::const_iterator iter;
	for( iter=m_vBlockEntityList.begin() ; iter!=m_vBlockEntityList.end() ; ++iter )
	{
		if( *iter == pEntity )
			return true;
	}
	return false;
}

void ioZoneEffect2Weapon::AddBlockEntity( ioPlayEntity *pEntity )
{
	m_vBlockEntityList.push_back( pEntity );
}

bool ioZoneEffect2Weapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();

		if( !GetAttribute()->m_WeaponEffect.IsEmpty() )
		{
			CreateWeaponEffect( GetAttribute()->m_WeaponEffect );
		}

		return true;
	}

	return false;
}

bool ioZoneEffect2Weapon::CreateWeaponEffect( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pWeaponEffect = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pWeaponEffect )	return false;

	D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );

	GetParentSceneNode()->AttachObject( m_pWeaponEffect );
	GetParentSceneNode()->SetScale( vScale );
	D3DXVECTOR3	vPos = GetPosition();

	AddEffectEvent( m_pWeaponEffect );
	return true;
}

void ioZoneEffect2Weapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pWeaponEffect )
	{
		g_EffectFactory.DestroyEffect( m_pWeaponEffect );

		m_pWeaponEffect = NULL;
	}
}

bool ioZoneEffect2Weapon::CheckHeight( ioBaseChar *pTarget )
{
	if( !pTarget )			return false;
	if( !m_bCheckHeight )	return true;

	float fTargetMidHeight = pTarget->GetMidHeightByRate();
	float fCenter = GetWorldAttackBox().GetCenter().y;

	float fUp = fCenter + m_fUpHeight;
	float fUnder = fCenter - m_fUnderHeight;

	if( !COMPARE(fTargetMidHeight, fUnder, fUp) )
		return false;

	return true;
}

bool ioZoneEffect2Weapon::SetSwingBuff( ioBaseChar *pTarget )
{
	if( !m_WoundedSwingInfo.m_SwingBuff.empty() &&
		!m_WoundedSwingInfo.m_SwingCancelBuff.empty() &&
		!m_WoundedSwingInfo.m_SwingWoundBuff.empty() &&
		!m_bSwinged )
	{
		ioBaseChar *pOwner = GetOwner();

		if( pTarget->IsNeedProcess() )
		{
			// Swing Buff를 주기 위해 피격 판정은 하지 않은 채 이펙트만 보여준다.
			D3DXQUATERNION qtCharRot = pOwner->GetTargetRot();
			pTarget->SetTargetRotToRotate( qtCharRot, true, false );
			pTarget->SetGrapplingWoundSwing( GetOwnerName(), m_WoundedSwingInfo, m_szWoundedSwingAni, m_fWoundedSwingRate );

			return false;
		}
	}

	return true;
}

bool ioZoneEffect2Weapon::UseSwingBuff()
{
	if( m_WoundedSwingInfo.m_SwingBuff.empty() )
		return false;
	else
		return true;
}

void ioZoneEffect2Weapon::CheckSwingBuff( ioPlayEntity *pEntity )
{
	ioBaseChar *pTarget = ToBaseChar(pEntity);
	if( !pTarget )	return;

	int iCnt = m_WoundedSwingInfo.m_SwingBuff.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( pTarget->HasBuff( m_WoundedSwingInfo.m_SwingBuff[i] ) && !m_bSwinged )
		{
			pTarget->AttachEffect( GetExplosionEffectName() );
			m_bSwinged = true;
			return;
		}
	}
}