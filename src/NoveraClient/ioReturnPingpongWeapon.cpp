

#include "stdafx.h"

#include "ioReturnPingpongWeapon.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioReturnPingpongWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioReturnPingpongWeapon::m_DieActionStopType = AST_NONE;

ioReturnPingpongWeapon::ioReturnPingpongWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ATTACK_OWNER | WP_DEF_NO_SOUND );

	m_pFireMesh = NULL;
	m_iCollisionCnt = 0;

	m_bCheckCollision = false;
	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;

	m_iCurPingpongcount = 0;

	m_pSecondAttr = NULL;

	m_bArleadyCallWeapon = false;

	m_bTimeOver = false;
}

ioReturnPingpongWeapon::~ioReturnPingpongWeapon()
{
	if( m_pFireMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioReturnPingpongWeapon::~ioReturnPingpongWeapon - UnReleased Resource" );
	}
}

bool ioReturnPingpongWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();

		return SetFireMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioReturnPingpongWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );
	m_fReturnHeightRate = rkLoader.LoadFloat_e( "return_height_rate", FLOAT05 );

	m_iTotalPingpongCount = rkLoader.LoadInt_e( "pingpong_count", 0 );
	m_dwEnablePingpongTime = rkLoader.LoadInt_e( "enable_pingpong_time", 0 );

	int iAttackTypeCnt = rkLoader.LoadInt_e( "call_weapon_type_count", 0 );
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

	m_bDisableMaxCount = rkLoader.LoadBool_e( "disable_max_count", true );

	for( int i=0; i<m_iTotalPingpongCount-1; ++i )
	{
		SecondEffect eSecondEffect;
		wsprintf_e( szKey, "second_effect%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		eSecondEffect.m_szSecondEffect = szBuf;
		m_SecontEffectList.push_back( eSecondEffect );
	}

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

void ioReturnPingpongWeapon::SetLiveTime( DWORD dwTime )
{
}

void ioReturnPingpongWeapon::SetSecondAttribute( SecondAttributeList vSecondList )
{
	int iSize = vSecondList.size();
	for( int i=0; i < iSize; ++i )
	{
		SecondAttribute kSecondAttr = vSecondList[i];
		m_vSecondAttrList.push_back( kSecondAttr );
	}
}

void ioReturnPingpongWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireMesh && m_pFireMesh->GetSkeletonName().IsEmpty() && m_pFireMesh->HasLinkedSystem() )
		return;

	SetPosition( vPos );
	m_vPrePos = GetPosition();
}

void ioReturnPingpongWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
	m_vAttackDir = m_vMoveDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetOrientation( qtRot );

	D3DXVec3Cross( &m_vRollAxis, &D3DXVECTOR3( 0.0f, FLOAT1, 0.0f ), &m_vMoveDir );
}

bool ioReturnPingpongWeapon::SetFireMesh( const ioHashString &szFileName )
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

bool ioReturnPingpongWeapon::SetWallCollision( const D3DXVECTOR3 &vFaceNrm, bool bMap, bool bOnlyAttack )
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

	ChangeSecondEffect();

	m_iCurPingpongcount++;
	m_iCurPingpongcount = min( m_iCurPingpongcount, m_iTotalPingpongCount );

	return true;
}

void ioReturnPingpongWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwLooseTime = 0;
	if( GetOwner() )
		dwLooseTime = GetOwner()->GetActionStopTotalDelay();

	if( m_dwCreatedTime + m_dwLiveTime + dwLooseTime < FRAMEGETTIME() )
	{
		pStage->CreateMapEffect( GetWeaponEndEffect(), GetPosition(), GetScale() );
		DestroyResource( pStage );
		m_bTimeOver = true;
		SetWeaponDead();
		return;
	}

	if( m_iCurPingpongcount >= m_iTotalPingpongCount )
	{
		SetWeaponDead();
		return;
	}

	if( !m_bCheckCollision )
		CheckChangeDir();

	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	m_fCurMoveRange += fMoveDist;

	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );

	UpdateSecondEffect( pStage );
	UpdateWorldAttackBox( fTimePerSec, false );

	if( m_pFireMesh )
	{
		m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

bool ioReturnPingpongWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( !pShape )
		return false;

	int iFaceIndex = 0;

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

void ioReturnPingpongWeapon::RollWeapon( float fTimePerSec )
{
	if( m_pFireMesh )
	{
		GetParentSceneNode()->Rotate( m_vRollAxis, 1600.0f * fTimePerSec );
	}
}

void ioReturnPingpongWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireMesh )
	{
		g_EffectFactory.DestroyEffect( m_pFireMesh );
		m_pFireMesh = NULL;
	}
}

ioWeapon::WeaponType ioReturnPingpongWeapon::GetType() const
{
	return WT_RETURN_PINGPONG;
}

ActionStopType ioReturnPingpongWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioReturnPingpongWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioReturnPingpongWeapon::CheckCurAttribute( const ioPlayEntity *pTarget )
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

D3DXVECTOR3 ioReturnPingpongWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return m_vAttackDir;
}

const ioOrientBox& ioReturnPingpongWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

bool ioReturnPingpongWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
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

						//if( pShape->TestIntersection( NULL, kLineRay, &iFaceIndex, &vColPoint ) )
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
	{
		m_bCheckCollision = true;

		if( m_iCurPingpongcount >= m_iTotalPingpongCount )
			bCollision = false;
	}

	return bCollision;
}

void ioReturnPingpongWeapon::SetWeaponDead()
{
	ioWeapon::SetWeaponDead();
	if( !m_bTimeOver )
		CallAttribute();

	DestroySecondEffect();
	m_bTimeOver = false;
}

void ioReturnPingpongWeapon::ChangeBallWeapon( int iCurCnt,
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

D3DXVECTOR3 ioReturnPingpongWeapon::GetMoveDir()
{
	return m_vMoveDir;
}

void ioReturnPingpongWeapon::CheckChangeDir()
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

void ioReturnPingpongWeapon::RotateMoveDir( float fAngleH, float fAngleV )
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

float ioReturnPingpongWeapon::GetMoveSpeed()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMoveDelayTime > 0 && m_dwCreatedTime+m_dwMoveDelayTime >= dwCurTime )
	{
		return 0.0f;
	}

	if( m_fCurMoveSpeed == 0.0f )
	{
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;
	}

	return m_fCurMoveSpeed * m_fOwnerChargeSpeedRate;
}

float ioReturnPingpongWeapon::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}


void ioReturnPingpongWeapon::SetReturn( const D3DXVECTOR3 &vPos )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return;

	D3DXVECTOR3 vReturnPos = vPos;

	if( vPos == ioMath::VEC3_ZERO )
	{
		vReturnPos = pOwner->GetMidPositionByRate( m_fReturnHeightRate );
	}

	D3DXVECTOR3 vDir = vReturnPos - GetPosition();
	D3DXVec3Normalize( & vDir, &vDir );

	m_vMoveDir = vDir;
	m_vAttackDir = m_vMoveDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetOrientation( qtRot );

	D3DXVec3Cross( &m_vRollAxis, &D3DXVECTOR3( 0.0f, FLOAT1, 0.0f ), &m_vMoveDir );

	ChangeSecondEffect();

	m_iCurPingpongcount++;
	m_iCurPingpongcount = min( m_iCurPingpongcount, m_iTotalPingpongCount );
}

void ioReturnPingpongWeapon::CallAttribute()
{
	if( m_bArleadyCallWeapon )	return;
	if( m_CallWeaponInfoList.empty() )	return;

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
											  dwCurGenerateTime + pInfo.m_dwCallTime,
											  m_dwWeaponIdx );
	}

	m_bArleadyCallWeapon = true;
}

void ioReturnPingpongWeapon::ChangeSecondEffect()
{
	if( m_SecontEffectList.empty() )	return;
	if( (int)m_SecontEffectList.size() <= m_iCurPingpongcount )	return;
	
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )	return;

	int iCount = m_iCurPingpongcount;

	int iCnt = m_SecontEffectList.size();
	for( int i=0; i<iCnt; ++i )
	{
		DWORD dwEffID = m_SecontEffectList[i].m_dwSecondEffectID;
		if( dwEffID != -1 )
		{
			pStage->DestroyMapEffect( dwEffID );
			m_SecontEffectList[i].m_dwSecondEffectID = -1;
		}
	}

	ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_SecontEffectList[iCount].m_szSecondEffect, GetPosition(), GetScale() );
	if( pMapEffect )
	{
		m_SecontEffectList[iCount].m_dwSecondEffectID = pMapEffect->GetUniqueID();
	}

}

void ioReturnPingpongWeapon::UpdateSecondEffect( ioPlayStage *pStage )
{
	if( m_SecontEffectList.empty() )	return;

	int iCnt = m_SecontEffectList.size();
	for( int i=0; i<iCnt; ++i )
	{
		DWORD dwEffID = m_SecontEffectList[i].m_dwSecondEffectID;
		if( dwEffID != -1 )
		{
			ioMapEffect *pMapEffect = pStage->FindMapEffect( dwEffID );
			pMapEffect->SetWorldPosition( GetPosition() );
			pMapEffect->SetWorldOrientation( GetOrientation() );
		}
	}
}

void ioReturnPingpongWeapon::DestroySecondEffect()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )	return;

	int iCnt = m_SecontEffectList.size();
	for( int i=0; i<iCnt; ++i )
	{
		DWORD dwEffID = m_SecontEffectList[i].m_dwSecondEffectID;
		if( dwEffID != -1 )
		{
			pStage->DestroyMapEffect( dwEffID );
			m_SecontEffectList[i].m_dwSecondEffectID = -1;
		}
	}
}

bool ioReturnPingpongWeapon::EnablePingpong() const
{
	if( m_dwCreatedTime + m_dwEnablePingpongTime > FRAMEGETTIME() )
		return false;

	return true;
}