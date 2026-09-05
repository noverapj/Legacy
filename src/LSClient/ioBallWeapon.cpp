

#include "stdafx.h"
#include "ioBallWeapon.h"

ActionStopType ioBallWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioBallWeapon::m_DieActionStopType = AST_NONE;

ioBallWeapon::ioBallWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_DEF_NO_SOUND );

	m_pFireMesh = NULL;
	m_BombState = BS_FLOATING;
	m_iCollisionCnt = 0;

	m_bCheckCollision = false;
	m_bArleadyCallWeapon = false;
	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;
}

ioBallWeapon::~ioBallWeapon()
{
	if( m_pFireMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioBallWeapon::~ioBallWeapon - UnReleased Resource" );
	}
}

bool ioBallWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();

		return SetFireMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioBallWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );

	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	m_bEndExplosion = rkLoader.LoadBool_e( "end_explosion", false );
	m_bCallExplosion = rkLoader.LoadBool_e( "call_explosion", false );
	m_fExplosionRange = rkLoader.LoadFloat_e( "explosion_range", 0.0f );

	m_ExplosionType = (ExplosionType)rkLoader.LoadInt_e( "explosion_type", ET_RANGE );
	m_vExplosionAxis.x = rkLoader.LoadFloat_e( "explosion_axis_x", 0.0f );
	m_vExplosionAxis.y = rkLoader.LoadFloat_e( "explosion_axis_y", 0.0f );
	m_vExplosionAxis.z = rkLoader.LoadFloat_e( "explosion_axis_z", 0.0f );

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

	LoadCallAttributeList( rkLoader );
	LoadDummyCharProperty( rkLoader );
}

void ioBallWeapon::LoadCallAttributeList( ioINILoader &rkLoader )
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

void ioBallWeapon::LoadDummyCharProperty( ioINILoader &rkLoader )
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

void ioBallWeapon::SetLiveTime( DWORD dwTime )
{
}

void ioBallWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireMesh && m_pFireMesh->GetSkeletonName().IsEmpty() && m_pFireMesh->HasLinkedSystem() )
		return;

	SetPosition( vPos );
	m_vPrePos = GetPosition();
}

void ioBallWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
	m_vAttackDir = m_vMoveDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetOrientation( qtRot );

	D3DXVec3Cross( &m_vRollAxis, &D3DXVECTOR3( 0.0f, FLOAT1, 0.0f ), &m_vMoveDir );
}

bool ioBallWeapon::SetFireMesh( const ioHashString &szFileName )
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

	m_vPrePos = GetPosition();

	AddEffectEvent( m_pFireMesh );
	return true;
}

bool ioBallWeapon::SetWallCollision( const D3DXVECTOR3 &vFaceNrm, bool bMap, bool bOnlyAttack )
{
	float fCheck = D3DXVec3Dot( &m_vMoveDir, &vFaceNrm );
	if( bMap && fCheck >= 0.0f )
	{
		return false;	// 나가는 방향
	}

	if( !bOnlyAttack )
	{
 		D3DXVECTOR3 vReflect = m_vMoveDir - 2.0f * D3DXVec3Dot( &m_vMoveDir, &vFaceNrm ) * vFaceNrm;
		D3DXVec3Normalize( &m_vMoveDir, &vReflect );
	}

	if( bMap )
	{
		m_vAttackDir = m_vMoveDir;
		PlayWoundedSound( NULL );
	}
	else
	{
		m_vAttackDir = -vFaceNrm;
		D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );
	}

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetOrientation( qtRot );

	D3DXVec3Cross( &m_vRollAxis, &D3DXVECTOR3( 0.0f, FLOAT1, 0.0f ), &m_vMoveDir );

	m_bCheckCollision = true;

	return true;
}

void ioBallWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
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

void ioBallWeapon::OnFloating( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwLooseTime = 0;
	if( GetOwner() )
		dwLooseTime = GetOwner()->GetActionStopTotalDelay();

	if( m_dwCreatedTime + m_dwLiveTime + dwLooseTime < FRAMEGETTIME() )
	{
		pStage->CreateMapEffect( GetWeaponEndEffect(), GetPosition(), GetScale() );
		DestroyResource( pStage );

		if( m_bEndExplosion )
			Explosion( pStage );
		else
			SetWeaponDead();

		return;
	}

	if( !m_bCheckCollision )
		CheckChangeDir();

	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	m_fCurMoveRange += fMoveDist;

	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
	
	if( m_pFireMesh )
	{
		m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

bool ioBallWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨
	int iFaceIndex = 0;

	if( m_BombState != BS_FLOATING )
		return true;

	// 1차 충돌 영역으로 체크
	if( !pShape->FindIntersection( pShapeWorldMat, GetWorldAttackBox(true), &iFaceIndex ) )
		return false;

	D3DXVECTOR3 vNormal;
	pShape->GetTriNormalInModelSpace( iFaceIndex, vNormal );

	if( SetWallCollision( vNormal, true, false ) )
	{
		pStage->CreateMapEffect( GetExplosionEffectName(), GetWorldAttackBox( true ).GetCenter(), GetScale() );
		return true;
	}

	// 2차 라인으로 체크
	D3DXVECTOR3 vEnd = GetPosition();
	D3DXVECTOR3 vOrgDiff = vEnd - m_vPrePos;
	float fOrgDiffSq = D3DXVec3LengthSq( &vOrgDiff );

	D3DXVECTOR3 vDir;
	D3DXVec3Normalize( &vDir, &vOrgDiff );

	D3DXVECTOR3 vColPoint;
	ioRay kLineRay( m_vPrePos, vDir );

	if( !pShape->TestIntersection( pShapeWorldMat, kLineRay, &iFaceIndex, &vColPoint ) )
		return false;

	pShape->GetTriNormalInModelSpace( iFaceIndex, vNormal );

	if( SetWallCollision( vNormal, true, false ) )
	{
		pStage->CreateMapEffect( GetExplosionEffectName(), GetWorldAttackBox( true ).GetCenter(), GetScale() );
	}

	return true;
}

void ioBallWeapon::RollWeapon( float fTimePerSec )
{
	if( m_pFireMesh )
	{
		GetParentSceneNode()->Rotate( m_vRollAxis, 1600.0f * fTimePerSec );
	}
}

void ioBallWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireMesh )
	{
		g_EffectFactory.DestroyEffect( m_pFireMesh );
		m_pFireMesh = NULL;
	}
}

ioWeapon::WeaponType ioBallWeapon::GetType() const
{
	return WT_BALL_WEAPON;
}

ActionStopType ioBallWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioBallWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

D3DXVECTOR3 ioBallWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return m_vAttackDir;
}

const ioOrientBox& ioBallWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

bool ioBallWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( !pEntity )
		return false;

	bool bCollision = pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision) );
	if( bCollision )
	{
		const ioOrientBox &kBallBox = GetWorldAttackBox();
		const ioOrientBox &kWoundedBox = pEntity->GetWorldCollisionBox();
		D3DXVECTOR3 vDiff = kBallBox.GetCenter() - kWoundedBox.GetCenter();
		vDiff.y = 0.0f;
		D3DXVec3Normalize( &vDiff, &vDiff );

		if( ToSymbolStruct(pEntity) || ToPushStruct(pEntity) || ToDummyChar(pEntity) || ToWallZone(pEntity) )
		{
			bool bShapeCol = false;
			ioEntityGroup *pGrp = pEntity->GetGroup();
			if( pGrp )
			{
				ioOpcodeShape *pShape = pGrp->GetOpcodeShape();
				if( pShape )
				{
					int iFaceIndex = 0;
					D3DXVECTOR3 vNormal;

					// 1차
					ioOrientBox kTest = GetWorldAttackBox(true);
					if( pShape->FindIntersection( pEntity->GetFullTransform(), kTest, &iFaceIndex ) )
					{
						pShape->GetTriNormalInModelSpace( iFaceIndex, vNormal );

						vNormal = pEntity->GetWorldOrientation() * vNormal;
						if( SetWallCollision( vNormal, true, false ) )
							bShapeCol = true;
					}

					// 2차 라인으로 체크
					if( !bShapeCol )
					{
						D3DXVECTOR3 vStart = m_vPrePos - pEntity->GetWorldPosition();
						D3DXVECTOR3 vEnd = GetPosition() - pEntity->GetWorldPosition();
						D3DXVECTOR3 vOrgDiff = vEnd - vStart;

						D3DXVECTOR3 vDir;
						D3DXVec3Normalize( &vDir, &vOrgDiff );

						D3DXVECTOR3 vColPoint;
						ioRay kLineRay( vStart, vDir );

						if( pShape->TestIntersection( pEntity->GetFullTransform(), kLineRay, &iFaceIndex, &vColPoint ) )
						{
							pShape->GetTriNormalInModelSpace( iFaceIndex, vNormal );
							
							if( SetWallCollision( vNormal, true, false ) )
								bShapeCol = true;
						}
					}
				}
			}

			if( !bShapeCol )
			{
				SetWallCollision( vDiff, true, false );
			}
		}
		else if( ToBallStruct(pEntity) )
		{
			SetWallCollision( vDiff, false, false );
		}
		else if( ToBaseChar(pEntity) )
		{
			if( GetPiercingType() == CPT_NONE )
				SetWallCollision( vDiff, false, false );
			else
				SetWallCollision( vDiff, false, true );

			PlayWoundedSound( NULL );
		}
	}

	CheckPiercing( bCollision );

	if( bCollision )
		m_bCheckCollision = true;

	return bCollision;
}

DWORD ioBallWeapon::GetCollisionEnableTime() const
{
	if( m_BombState == BS_EXPLOSION || m_BombState == BS_EXPLOSION_END )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

void ioBallWeapon::ChangeBallWeapon( int iCurCnt,
									 const D3DXVECTOR3 &vPos,
									 const D3DXVECTOR3 &vMoveDir,
									 const D3DXVECTOR3 &vAttackDir )
{
	m_iCollisionCnt = iCurCnt;

	m_vPrePos = GetPosition();
	SetPosition( vPos );

	m_vMoveDir = vMoveDir;
	m_vAttackDir = vAttackDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetOrientation( qtRot );

	D3DXVec3Cross( &m_vRollAxis, &D3DXVECTOR3( 0.0f, FLOAT1, 0.0f ), &m_vMoveDir );
}

D3DXVECTOR3 ioBallWeapon::GetMoveDir()
{
	return m_vMoveDir;
}

void ioBallWeapon::CheckChangeDir()
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

void ioBallWeapon::RotateMoveDir( float fAngleH, float fAngleV )
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

float ioBallWeapon::GetMoveSpeed()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMoveDelayTime > 0 && m_dwCreatedTime+m_dwMoveDelayTime >= dwCurTime )
		return 0.0f;

	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed * m_fOwnerChargeSpeedRate;
}

float ioBallWeapon::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

void ioBallWeapon::Explosion( ioPlayStage *pStage )
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

bool ioBallWeapon::IsExplosion() const
{
	if( m_BombState == BS_EXPLOSION || m_BombState == BS_EXPLOSION_END )
		return true;

	return false;
}

bool ioBallWeapon::IsCanMineExplosion()
{
	if( !ioWeapon::IsCanMineExplosion() )
		return false;

	if( !IsExplosion() )
		return false;

	return true;
}

void ioBallWeapon::CreateDummyCharbyExplosion( ioBaseChar *pOwner )
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

void ioBallWeapon::CallAttribute()
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