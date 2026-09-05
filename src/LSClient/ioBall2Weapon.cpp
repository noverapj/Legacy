

#include "stdafx.h"

#include "ioBall2Weapon.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioBall2Weapon::m_ActionStopType = AST_NONE;
ActionStopType ioBall2Weapon::m_DieActionStopType = AST_NONE;

ioBall2Weapon::ioBall2Weapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ATTACK_OWNER | WP_DEF_NO_SOUND );

	m_pFireMesh = NULL;
	m_iCollisionCnt = 0;

	m_bCheckCollision = false;
	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;

	m_TargetInfoList.clear();
	m_BallState = BS_MOVE;

	m_dwDropZoneCheckStartTime = 0;

	m_bCheckPacket = true;
}

ioBall2Weapon::~ioBall2Weapon()
{
	if( m_pFireMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioBall2Weapon::~ioBall2Weapon - UnReleased Resource" );
	}
}

bool ioBall2Weapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();

		return SetFireMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioBall2Weapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );

	m_dwDropZoneLiveTime = (DWORD)rkLoader.LoadInt_e( "dropzone_live_time", 0 );
	m_dwRotateSpeed = (float)rkLoader.LoadFloat_e( "target_rotate_speed", 0.0f );

	m_TargetEndBuff.clear();
	int iBuffCnt = rkLoader.LoadInt_e( "target_end_buff_count", 0 );
	for( int i = 0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "target_end_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_TargetEndBuff.push_back( szBuf );
	}

	m_TargetCullingBuff.clear();
	iBuffCnt = rkLoader.LoadInt_e( "target_culling_buff_count", 0 );
	for( int i = 0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "target_culling_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_TargetCullingBuff.push_back( szBuf );
	}

	m_bRotateXAxis			= rkLoader.LoadBool_e( "rotate_x_axis", false );
	m_fOffsetByRotateX		= rkLoader.LoadFloat_e( "offset_by_rotate_x", 80.0f );
	m_fRotateAngle			= rkLoader.LoadFloat_e( "rotate_x_angle", 0.0f );
	m_bOnlyAttack			= rkLoader.LoadBool_e( "only_attack", false );
	m_bOffsetXDisable		= rkLoader.LoadBool_e( "offset_x_disable", false );

	m_iTargetMaxCount		= rkLoader.LoadInt_e( "target_count", 0 );
	m_dwTargetCullingTime	= rkLoader.LoadInt_e( "target_culling_time", 0 );

		
	int iCnt = rkLoader.LoadInt_e( "change_dir_cnt", 0 );
	if( iCnt <= 0 ) return;

	m_bMustChangeDir = rkLoader.LoadBool_e( "must_change_dir", false );

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

void ioBall2Weapon::SetLiveTime( DWORD dwTime )
{
}

void ioBall2Weapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireMesh && m_pFireMesh->GetSkeletonName().IsEmpty() && m_pFireMesh->HasLinkedSystem() )
		return;

	SetPosition( vPos );
	m_vPrePos = GetPosition();
}

void ioBall2Weapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
	m_vAttackDir = m_vMoveDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetOrientation( qtRot );

	m_qtOriginal = qtRot;

	D3DXVec3Cross( &m_vRollAxis, &D3DXVECTOR3( 0.0f, FLOAT1, 0.0f ), &m_vMoveDir );
}

bool ioBall2Weapon::SetFireMesh( const ioHashString &szFileName )
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

bool ioBall2Weapon::SetWallCollision( const D3DXVECTOR3 &vFaceNrm, bool bMap, bool bOnlyAttack )
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
	//SetOrientation( qtRot );

	D3DXVec3Cross( &m_vRollAxis, &D3DXVECTOR3( 0.0f, FLOAT1, 0.0f ), &m_vMoveDir );

	if( !m_bMustChangeDir )
	{
		m_bCheckCollision = true;
	}

	return true;
}

void ioBall2Weapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	switch( m_BallState )
	{
	case BS_MOVE:
		{
			DWORD dwLooseTime = 0;
			if( GetOwner() )
				dwLooseTime = GetOwner()->GetActionStopTotalDelay();

			if( m_dwCreatedTime + m_dwLiveTime + dwLooseTime < FRAMEGETTIME() )
			{
				pStage->CreateMapEffect( GetWeaponEndEffect(), GetPosition(), GetScale() );
				DestroyResource( pStage );
				SetEndWaitState();
				return;
			}

			if( m_dwDropZoneLiveTime > 0 && !m_TargetInfoList.empty() )
			{
				bool bCheckDropZone = false;
				if( m_dwDropZoneCheckStartTime <= 0 )
					m_dwDropZoneCheckStartTime = FRAMEGETTIME();

				D3DXVECTOR3 vCurPos = GetPosition();
				if( pStage->GetMapHeightNoneEntity( vCurPos.x, vCurPos.z, vCurPos.y+3.0f ) <= 0.0f )
					bCheckDropZone = true;

				if( !bCheckDropZone )
					m_dwDropZoneCheckStartTime = 0;

				if( bCheckDropZone && m_dwDropZoneCheckStartTime > 0 && m_dwDropZoneCheckStartTime + m_dwDropZoneLiveTime <= FRAMEGETTIME() )
					SetEndWaitState();
			}

			if( !m_bCheckCollision )
				CheckChangeDir();

			float fMoveDist = GetMoveSpeed() * fTimePerSec;
			m_fCurMoveRange += fMoveDist;

			if( m_bCheckPacket && m_dwCreatedTime + 100 <= FRAMEGETTIME() )
			{
				m_bCheckPacket = false;
				SendTargetPacket( pStage );
			}

			GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );

			UpdateWorldAttackBox( fTimePerSec, false );
			CheckTarget( pStage );
			CullingTarget( pStage );

			if( m_pFireMesh )
			{
				m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
			}
		}
		break;
	case BS_END_WAIT:
		SetEndState( pStage );
		break;
	case BS_END:
		SetWeaponDead();
		break;
	}
}

bool ioBall2Weapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( !pShape )
		return true;

	int iFaceIndex = 0;

	// 1차 충돌 영역으로 체크
	if( !pShape->FindIntersection( pShapeWorldMat, GetWorldAttackBox(true), &iFaceIndex ) )
		return false;

	D3DXVECTOR3 vNormal;
	pShape->GetTriNormalInModelSpace( iFaceIndex, vNormal );

	if( SetWallCollision( vNormal, true, false ) )
	{
		pStage->CreateMapEffect( GetExplosionEffectName(), GetWorldAttackBox( true ).GetCenter(), GetScale() );

		if( m_bOnlyAttack )
			SetEndState( pStage );
		
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

	if( m_bOnlyAttack )
		SetEndState( pStage );

	return true;
}

void ioBall2Weapon::RollWeapon( float fTimePerSec )
{
	if( m_pFireMesh )
	{
		GetParentSceneNode()->Rotate( m_vRollAxis, 1600.0f * fTimePerSec );
	}
}

void ioBall2Weapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireMesh )
	{
		g_EffectFactory.DestroyEffect( m_pFireMesh );
		m_pFireMesh = NULL;
	}
}

ioWeapon::WeaponType ioBall2Weapon::GetType() const
{
	return WT_BALL2_WEAPON;
}

ActionStopType ioBall2Weapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioBall2Weapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

D3DXVECTOR3 ioBall2Weapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return m_vAttackDir;
}

const ioOrientBox& ioBall2Weapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

bool ioBall2Weapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
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

	if( bCollision && !IsAlreadyCollisioned(pEntity) )
	{
		ioBaseChar *pTarget = ToBaseChar( pEntity );
		if( pTarget && pTarget->GetTeam() != GetTeam() && pTarget->IsNeedProcess() )
		{
			bool bProtected = false;

			if( pTarget->IsProtectState() )
				bProtected = true;

			if( pTarget->IsStateProtect( AS_NONE, GetDefenseBreakType() ) )
				bProtected = true;

			if( pTarget->GetState() == CS_DEFENSE && GetDefenseBreakType() == DBT_NONE && IsIgnoreProtect() == IPT_NONE )
				bProtected = true;
			
			if( !bProtected )
			{
				TargetInfo kInfo;
				kInfo.m_TargetName = pTarget->GetCharName();
				kInfo.m_AttachTime = FRAMEGETTIME();

				D3DXVECTOR3	vTargetPos = pTarget->GetWorldPosition();
				D3DXVECTOR3 vWeaponPos = GetPosition();
				D3DXVECTOR3 vDiff = vTargetPos - vWeaponPos;

				if( m_bOffsetXDisable )
				{
					kInfo.m_fLength = 0.0f;
				}
				else
				{
					kInfo.m_fLength = D3DXVec3Length( &vDiff );
				}

				if( m_bRotateXAxis )
				{
					kInfo.m_fLength = min( m_fOffsetByRotateX, kInfo.m_fLength );
					kInfo.m_vDir = m_vMoveDir;
				}
				else
				{
					D3DXVec3Normalize( &vDiff, &vDiff );
					kInfo.m_vDir = vDiff;
				}

				m_TargetInfoList.push_back( kInfo );

				if( pTarget->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_WEAPON_SYNC );			
					kPacket << GetOwnerName();
					kPacket << m_dwWeaponIdx;
					kPacket << SS_ADD_TARGET;
					kPacket << kInfo.m_TargetName;
					kPacket << kInfo.m_fLength;
					kPacket << kInfo.m_vDir;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
	}

	if( bCollision && !m_bMustChangeDir )
	{
		m_bCheckCollision = true;
	}

	return bCollision;
}

void ioBall2Weapon::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	if( !pStage )	return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SS_ADD_TARGET:
		{
			TargetInfo kInfo;
			rkPacket >> kInfo.m_TargetName;
			rkPacket >> kInfo.m_fLength;
			rkPacket >> kInfo.m_vDir;
			kInfo.m_AttachTime = FRAMEGETTIME();

			bool bAlreadyCollision = false;
			int iCnt = m_TargetInfoList.size();
			for( int i=0; i<iCnt; ++i )
			{
				if( m_TargetInfoList[i].m_TargetName == kInfo.m_TargetName )
					bAlreadyCollision = true;				
			}

			if( !bAlreadyCollision )
			{
				m_TargetInfoList.push_back( kInfo );
			}
		}
		break;
	case SS_END:
		{
			int iCnt = 0;
			rkPacket >> iCnt;

			for( int i=0; i<iCnt; ++i )
			{
				ioHashString szTargetName;
				rkPacket >> szTargetName;
				ioBaseChar *pTarget = pStage->GetBaseChar( szTargetName );
				if( pTarget )
				{
					int iBuffCnt = m_TargetEndBuff.size();
					for( int j=0; j<iBuffCnt; ++j )
					{
						pTarget->AddNewBuff( m_TargetEndBuff[j], "", "", NULL );
					}
				}
			}
			m_BallState = BS_END;
		}
		break;
	case SS_CULLING:
		{
			ioHashString szTargetName;
			rkPacket >> szTargetName;
			ioBaseChar *pTarget = pStage->GetBaseChar( szTargetName );
			RemoveTarget( pStage, szTargetName );
			if( pTarget )
			{
				int iBuffCnt = m_TargetCullingBuff.size();
				for( int j = 0; j < iBuffCnt; ++j )
				{
					pTarget->AddNewBuff( m_TargetCullingBuff[j], "", "", NULL );
				}
			}
		}
		break;
	}	
}

void ioBall2Weapon::ChangeBallWeapon( int iCurCnt,
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

D3DXVECTOR3 ioBall2Weapon::GetMoveDir()
{
	return m_vMoveDir;
}

void ioBall2Weapon::CheckChangeDir()
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

void ioBall2Weapon::RotateMoveDir( float fAngleH, float fAngleV )
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

float ioBall2Weapon::GetMoveSpeed()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMoveDelayTime > 0 && m_dwCreatedTime+m_dwMoveDelayTime >= dwCurTime )
		return 0.0f;

	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed * m_fOwnerChargeSpeedRate;
}

float ioBall2Weapon::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

void ioBall2Weapon::SetEndWaitState()
{
	m_BallState = BS_END_WAIT;
}

void ioBall2Weapon::SetEndState( ioPlayStage *pStage )
{
	if( !pStage )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	m_BallState = BS_END;

	int iCnt = m_TargetInfoList.size();
	for( int i = 0; i< iCnt; ++i )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetInfoList[i].m_TargetName );
		if( pTarget )
		{
			int iBuffCnt = m_TargetEndBuff.size();
			for( int j=0; j<iBuffCnt; ++j )
			{
				pTarget->AddNewBuff( m_TargetEndBuff[j], "", "", NULL );
			}
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_WEAPON_SYNC );			
		kPacket << GetOwnerName();
		kPacket << m_dwWeaponIdx;
		kPacket << SS_END;

		int iCnt = m_TargetInfoList.size();
		kPacket << iCnt;
		for( int i=0; i<iCnt; ++i )
			kPacket << m_TargetInfoList[i].m_TargetName;

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBall2Weapon::CullingTarget( ioPlayStage *pStage )
{
	if( !pStage )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;
	
	if( m_iTargetMaxCount == 0 )
		return;

	TargetInfoList::iterator iter = m_TargetInfoList.begin();
		
	for( int i = 0; iter < m_TargetInfoList.end(); )
	{
		const TargetInfo rkInfo = *iter;
		if( i < m_iTargetMaxCount )
		{
			++i;
			++iter;
			continue;
		}

		ioBaseChar *pTarget = pStage->GetBaseChar( rkInfo.m_TargetName );
		if( pTarget && pTarget->IsNeedProcess() && rkInfo.m_AttachTime + m_dwTargetCullingTime < FRAMEGETTIME() )
		{	
			int iBuffCnt = m_TargetCullingBuff.size();
			for( int j = 0; j < iBuffCnt; ++j )
			{
				pTarget->AddNewBuff( m_TargetCullingBuff[j], "", "", NULL );
			}
			
			if( pTarget->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_WEAPON_SYNC );
				kPacket << GetOwnerName();
				kPacket << m_dwWeaponIdx;
				kPacket << SS_CULLING;				
				kPacket << pTarget->GetCharName();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			iter = m_TargetInfoList.erase( iter );
		}
		else
		{
			++iter;
			++i;
		}
	}
}

void ioBall2Weapon::RemoveTarget( ioPlayStage *pStage, const ioHashString& szTarget )
{
	TargetInfoList::iterator iter = m_TargetInfoList.begin();
	for( ; iter < m_TargetInfoList.end(); ++iter )
	{
		const TargetInfo rkInfo = *iter;
		if( rkInfo.m_TargetName == szTarget )
		{
			m_TargetInfoList.erase( iter );
			return;
		}
	}
}

void ioBall2Weapon::CheckTarget( ioPlayStage *pStage )
{
	if( !pStage )
		return;

	if( m_TargetInfoList.empty() )
		return;

	int iCnt = m_TargetInfoList.size();
	for( int  i= 0; i < iCnt; ++i )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetInfoList[i].m_TargetName );
		if( pTarget )
		{
			if( !pTarget->CheckGrapplingExceptionState() )
				continue;

			if( !( pTarget->GetState() == CS_SKIP_STUN ) )
				continue;

			float fTimePerSec = g_FrameTimer.GetSecPerFrame();
			float fRate = (fTimePerSec*1000) / m_dwRotateSpeed;
			float fRotateAngle = 360.0f * fRate;
			fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

			D3DXQUATERNION	qtRot;
			D3DXVECTOR3 vAxis;
			if( m_bRotateXAxis )
			{
				D3DXVec3Cross( &vAxis, &ioMath::UNIT_Y, &m_vMoveDir );
			}
			else
			{
				vAxis = ioMath::UNIT_Y;
			}
			
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fRotateAngle) );

			D3DXVECTOR3 vDir = m_TargetInfoList[i].m_vDir;
			vDir = qtRot * vDir;
			D3DXVec3Normalize( &vDir, &vDir );
			m_TargetInfoList[i].m_vDir = vDir;

			// 위치
			D3DXVECTOR3 vNewPos;
			if( m_bRotateXAxis )
			{
				vNewPos = GetPosition();
				vNewPos += ( vDir * m_TargetInfoList[i].m_fLength );
				pTarget->SetWorldPosition( vNewPos );
			}
			else
			{
				vNewPos = GetPosition() + ( vDir * m_TargetInfoList[i].m_fLength );
				pTarget->SetWorldPosition( vNewPos );
			}

			// 위치에 따른 캐릭터 회전
			if( m_bRotateXAxis )
			{
				D3DXVECTOR3 vRotateAxis;
				D3DXVec3Cross( &vRotateAxis, &m_vMoveDir, &ioMath::UNIT_Y );
				D3DXVec3Normalize( &vRotateAxis, &vRotateAxis );

				D3DXQUATERNION qtNewRot, qtResult;
				float fDotValue = D3DXVec3Dot( &vDir, &ioMath::UNIT_Y );
				float fAngle = acosf( fDotValue );

				vDir *= -1;
				float fDot = D3DXVec3Dot( &m_vMoveDir, &vDir );
				float fRotateRadian = DEGtoRAD( m_fRotateAngle );
				if( fDot > 0.0f )
					D3DXQuaternionRotationAxis( &qtNewRot, &vRotateAxis, fAngle + fRotateRadian );
				else
					D3DXQuaternionRotationAxis( &qtNewRot, &vRotateAxis, ( 2 * D3DX_PI ) - fAngle + fRotateRadian );

				qtResult = m_qtOriginal * qtNewRot;
				pTarget->SetWorldOrientation( qtResult );
			}
			else
			{
				D3DXQUATERNION	qtNewRot;
				vDir.y = 0.0f;
				ioMath::CalcDirectionQuaternion( qtNewRot, vDir );
				pTarget->SetWorldOrientation( qtNewRot );
			}
		}
	}
}

void ioBall2Weapon::SendTargetPacket( ioPlayStage *pStage )
{
	if( m_TargetInfoList.empty() )
		return;

	int iCnt = m_TargetInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetInfoList[i].m_TargetName );
		if( !pTarget )
			continue;

		if( pTarget->IsNeedProcess() && pTarget->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_WEAPON_SYNC );			
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << SS_ADD_TARGET;
			kPacket << m_TargetInfoList[i].m_TargetName;
			kPacket << m_TargetInfoList[i].m_fLength;
			kPacket << m_TargetInfoList[i].m_vDir;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}