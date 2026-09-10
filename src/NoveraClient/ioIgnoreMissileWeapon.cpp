

#include "stdafx.h"

#include "ioIgnoreMissileWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioIgnoreMissileWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioIgnoreMissileWeapon::m_DieActionStopType = AST_NONE;

ioIgnoreMissileWeapon::ioIgnoreMissileWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ONE_TIME_USE | WP_DIVIDE_ACTION );

	m_pFireBall				= NULL;
	m_pSecondAttr			= NULL;

	m_fCurMoveSpeed			= 0.0f;
	m_fCurMoveRange			= 0.0f;
	m_fCurMoveRate			= 0.0f;

	m_bSetGraceClose		= false;
	m_bArleadyCallWeapon	= false;
	m_bFirst				= true;

	m_nColCnt = 0;
}

ioIgnoreMissileWeapon::~ioIgnoreMissileWeapon()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioIgnoreMissileWeapon::~ioIgnoreMissileWeapon - UnReleased Resouce" );
	}

	m_vSecondAttrList.clear();
}

bool ioIgnoreMissileWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioIgnoreMissileWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_bEnableMapCollision	= rkLoader.LoadBool_e( "enable_map_collision", false );

	m_fExtraOffsetX			= rkLoader.LoadFloat_e( "extra_offset_x", 0.0f );
	m_fExtraOffsetY			= rkLoader.LoadFloat_e( "extra_offset_y", 0.0f );
	m_fExtraOffsetZ			= rkLoader.LoadFloat_e( "extra_offset_z", 0.0f );

	m_bSetColCnt = rkLoader.LoadBool_e( "set_col_cnt", false );
	m_nColMaxCnt = rkLoader.LoadInt_e( "col_max_cnt", 0 );

	LoadCallAttribute( rkLoader );
	LoadCallAttributeFireDirectionList( rkLoader );

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

	m_bDurationEndCallWeapon = rkLoader.LoadBool_e( "duration_end_call_weapon", false );
}

void ioIgnoreMissileWeapon::LoadCallAttribute( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	m_CallWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "call_attribute_index", 0 );		
	m_CallWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "call_attribute_resistance", 0 );		

	rkLoader.LoadString_e( "call_attribute_wound_ani", "", szBuf, MAX_PATH );
	m_CallWeaponInfo.m_WoundedAnimation = szBuf;		

	m_CallWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "call_attribute_wound_duration", 0 );
	m_CallWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "call_attribute_wound_loop_ani", false );
}

void ioIgnoreMissileWeapon::LoadCallAttributeFireDirectionList( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	int iCnt = rkLoader.LoadInt_e( "call_attribute_fire_dir_cnt", 0 );
	if( iCnt <= 0 ) return;

	m_CallAttFireList.clear();
	m_CallAttFireList.reserve( iCnt );

	for( int i= 0; i < iCnt; ++i )
	{
		ChangeDirInfo kInfo;

		wsprintf_e( szBuf, "call_attribute_fire%d_range", i+1 );
		kInfo.m_fChangeRange = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "call_attribute_fire%d_speed", i+1 );
		kInfo.m_fChangeSpeed = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "call_attribute_fire%d_angle_h", i+1 );
		kInfo.m_fChangeAngleH = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "call_attribute_fire%d_angle_v", i+1 );
		kInfo.m_fChangeAngleV = rkLoader.LoadFloat( szBuf, 0.0f );

		m_CallAttFireList.push_back( kInfo );
	}

	std::sort( m_CallAttFireList.begin(), m_CallAttFireList.end(), ChangeDirSort() );
}

bool ioIgnoreMissileWeapon::CreateFireBall( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pFireBall = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pFireBall )	return false;

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

	m_StartPos = GetPosition();

	AddEffectEvent( m_pFireBall );
	return true;
}

void ioIgnoreMissileWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioPlayEntityHelp::CheckOrientation( vDir, qtRot );
	SetOrientation( qtRot );
}

void ioIgnoreMissileWeapon::SetMoveSpeed( float fSpeed )
{
	m_fCurMoveSpeed = fSpeed;
}

void ioIgnoreMissileWeapon::CheckChangeDir()
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

void ioIgnoreMissileWeapon::RotateMoveDir( float fAngleH, float fAngleV )
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

void ioIgnoreMissileWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireBall && m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->HasLinkedSystem() )
		return;

	D3DXVECTOR3 vNewPos = vPos;
	if( m_fExtraOffsetX != 0.0f || m_fExtraOffsetY != 0.0f || m_fExtraOffsetZ != 0.0f )
	{
		ioBaseChar *pOwner = GetOwner();
		if( m_pFireBall && pOwner )
		{
			vNewPos = pOwner->GetWorldPosition();
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			
			D3DXVECTOR3 vZDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vZDir, &vZDir );

			D3DXVECTOR3 vXDir;
			D3DXVec3Cross( &vXDir, &vZDir, &ioMath::UNIT_Y );
			D3DXVec3Normalize( &vXDir, &vXDir );

			D3DXVECTOR3 vYDir;
			D3DXVec3Cross( &vYDir, &vXDir, &vZDir );
			D3DXVec3Normalize( &vYDir, &vYDir );

			vNewPos += m_fExtraOffsetX * vXDir;
			vNewPos += m_fExtraOffsetY * vYDir;
			vNewPos += m_fExtraOffsetZ * vZDir;
		}
	}

	SetPosition( vNewPos );
	m_StartPos = GetPosition();
}

void ioIgnoreMissileWeapon::SetGraceClose()
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

void ioIgnoreMissileWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckPiercing() )
		return;

	if( !m_bFirst )
	{
		CheckChangeDir();

		float fMoveDist = GetMoveSpeed() * fTimePerSec;

		GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
		m_fCurMoveRange += fMoveDist;

		float fMaxRange = GetAttribute()->m_fMoveRange + m_fGrowthMoveRange;
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
				if ( IsLive() && m_bDurationEndCallWeapon )
					CallAttribute();
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

void ioIgnoreMissileWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

D3DXVECTOR3 ioIgnoreMissileWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return m_vMoveDir;
}

const ioOrientBox& ioIgnoreMissileWeapon::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioIgnoreMissileWeapon::GetType() const
{
	return WT_IGNORE_MISSILE;
}

ActionStopType ioIgnoreMissileWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioIgnoreMissileWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioIgnoreMissileWeapon::GetMoveSpeed()
{
	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed;
}

void ioIgnoreMissileWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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
		m_WorldAttackBox.Transform( GetPosition(),
									GetScale(),
									GetParentSceneNode()->GetDerivedOrientation() );

		m_WorldAttackBoxForMap.Transform( GetPosition(),
										  GetScale(),
										  GetParentSceneNode()->GetDerivedOrientation() );
	}
}

void ioIgnoreMissileWeapon::SetSecondAttribute( SecondAttributeList vSecondList )
{
	int iSize = vSecondList.size();
	for( int i=0; i < iSize; ++i )
	{
		SecondAttribute kSecondAttr = vSecondList[i];
		m_vSecondAttrList.push_back( kSecondAttr );
	}
}

void ioIgnoreMissileWeapon::CheckCurAttribute( const ioPlayEntity *pTarget )
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

const WeaponAttribute* ioIgnoreMissileWeapon::GetAttribute() const
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

bool ioIgnoreMissileWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	bool bCollision = false;

	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
		bCollision = CheckCollisionLine( pEntity, vColPoint );

	CheckPiercing( bCollision );

	if ( m_bSetColCnt && bCollision )
	{
		m_nColCnt++;

		if ( m_nColCnt > m_nColMaxCnt )
		{
			CallAttribute();
			SetWeaponDead();
		}
	}
	return bCollision;
}

bool ioIgnoreMissileWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨
	// 
	if( !m_bEnableMapCollision )
		return true;

	if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
	{
		
		CallAttribute();
		SetWeaponDead();
		return true;
	}

	return false;
}

bool ioIgnoreMissileWeapon::CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint )
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

void ioIgnoreMissileWeapon::CallAttribute()
{	
	if( m_bArleadyCallWeapon )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwCurGenerateTime = dwCurTime;

	for( ChangeDirInfoList::size_type i = 0; i < m_CallAttFireList.size(); ++i )
	{
		D3DXVECTOR3 vDir = m_vMoveDir;
		D3DXVec3Normalize(&vDir, &vDir);
		float fAngleH = ioMath::ArrangeHead( m_CallAttFireList[i].m_fChangeAngleH );
		float fAngleV = ioMath::ArrangeHead( m_CallAttFireList[i].m_fChangeAngleV );

		D3DXQUATERNION qtRot;
		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &vDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

		// vertical
		D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
		vDir = qtRot * vDir;

		// horizon
		D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
		vDir = qtRot * vDir;

		D3DXVECTOR3	vPos = GetPosition();

		g_WeaponMgr.SetReserveCallWeaponInfo( GetOwnerName(), 
			m_CallWeaponInfo,
			vPos,
			-vDir,											
			dwCurGenerateTime,
			m_dwWeaponIdx );

		dwCurGenerateTime += 10;
	}

	m_bArleadyCallWeapon = true;

}
