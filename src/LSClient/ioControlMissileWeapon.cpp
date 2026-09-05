

#include "stdafx.h"

#include "ioControlMissileWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioControlMissileWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioControlMissileWeapon::m_DieActionStopType = AST_NONE;

ioControlMissileWeapon::ioControlMissileWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ONE_TIME_USE | WP_MISSILE_WEAPON | WP_DIVIDE_ACTION );

	m_pMissileMesh = NULL;
	m_ControlState = CTS_IN_RANGE;

	m_bResistance = false;
	m_bSetGraceClose = false;
	m_fCurMoveSpeed = 0.0f;
	m_dwSyncTime = 0;

	m_dwReturnStartTime = 0;
}

ioControlMissileWeapon::~ioControlMissileWeapon()
{
	if( m_pMissileMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioControlMissileWeapon::~ioControlMissileWeapon - UnReleased Resouce" );
	}
}

bool ioControlMissileWeapon::InitResource()
{
	return true;
}

void ioControlMissileWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );

	rkLoader.LoadString_e( "red_team_effect", "", szBuf, MAX_PATH );
	m_RedTeamEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_effect", "", szBuf, MAX_PATH );
	m_BlueTeamEffect = szBuf;

	m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "rotate_speed", 0 );
	m_dwRotateStartTime = (DWORD)rkLoader.LoadInt_e( "rotate_start_time", 0 );
	m_dwRotateEndTime = (DWORD)rkLoader.LoadInt_e( "rotate_end_time", 0 );

	// Return
	m_bEnableCollisionReturn = rkLoader.LoadBool_e( "enable_collision_return", false );

	m_dwReturnDuration = (DWORD)rkLoader.LoadFloat_e( "return_duration", 0 );
	m_fReturnEndRange = rkLoader.LoadFloat_e( "return_end_range", 0.0f );

	m_fEnableMoveGap = rkLoader.LoadFloat_e( "enable_move_gap_height", 0.0f );
}

void ioControlMissileWeapon::SetWeaponIndex( DWORD dwIndex )
{
	m_dwWeaponIdx = dwIndex;
}

void ioControlMissileWeapon::SetTeam( ioPlayStage *pStage, TeamType eType )
{
	if( !pStage ) return;

	ioWeapon::SetTeam( pStage, eType );

	DestroyResource();

	ioHashString szCurWeaponEffect;

	if( eType == TEAM_BLUE )
	{
		szCurWeaponEffect = m_BlueTeamEffect;
	}
	else if( eType == TEAM_RED )
	{
		szCurWeaponEffect = m_RedTeamEffect;
	}
	else //개인전
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner && pOwner->GetSingleConvertTeam() == TEAM_BLUE )
		{
			szCurWeaponEffect = m_BlueTeamEffect;
		}
		else
		{
			szCurWeaponEffect = m_RedTeamEffect;
		}
	}

	if( !SetMesh( szCurWeaponEffect ) )
		DestroyResource();
}

bool ioControlMissileWeapon::SetMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pMissileMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pMissileMesh )	return false;

	GetParentSceneNode()->AttachObject( m_pMissileMesh );
	GetParentSceneNode()->SetScale( ioMath::UNIT_ALL );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pMissileMesh->GetSkeletonName().IsEmpty() && m_pMissileMesh->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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

	m_pMissileMesh->UpdateBounds( true );
	m_LocalAttackBox = m_pMissileMesh->GetCollisionBox();

	m_vPrePos = GetPosition();

	m_fDefaultGapHeight = fabs( m_vPrePos.y - pOwner->GetWorldPosition().y );

	AddEffectEvent( m_pMissileMesh );
	return true;
}

void ioControlMissileWeapon::SetLiveTime( DWORD dwTime )
{
}

void ioControlMissileWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

D3DXVECTOR3 ioControlMissileWeapon::GetMoveDir()
{
	return m_vMoveDir;
}

void ioControlMissileWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pMissileMesh && m_pMissileMesh->GetSkeletonName().IsEmpty() && m_pMissileMesh->HasLinkedSystem() )
		return;

	SetPosition( vPos );

	m_fDefaultGapHeight = 0.0f;
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		m_fDefaultGapHeight = fabs( vPos.y - pOwner->GetWorldPosition().y );
	}
}

bool ioControlMissileWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨
	// 
	if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner && pOwner->IsNeedProcess() )
		{
			// weapon dead
			if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
			{
				SP2Packet kPacket( CUPK_WEAPON_SYNC );
				kPacket << GetOwnerName();
				kPacket << m_dwWeaponIdx;
				kPacket << STC_DIE;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			SetWeaponDead();
		}
		return true;
	}

	return false;
}

void ioControlMissileWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetWeaponDead();
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	if( !CheckPiercing() )
		return;

	if( m_ControlState == CTS_IN_RANGE || m_ControlState == CTS_OUT_RANGE )
	{
		if( m_dwCreatedTime+GetLiveTime() < dwCurTime )
		{
			ChangeToReturnState();
		}
	}

	switch( m_ControlState )
	{
	case CTS_IN_RANGE:
		if( CheckInRange() )
			OnInRangeState( fTimePerSec, pStage );
		else
			ChangeToOutRange();	
		break;
	case CTS_OUT_RANGE:
		if( CheckInRange() )
			ChangeToInRange();
		else
			OnOutRangeState( fTimePerSec, pStage );
		break;
	case CTS_RETURN:
		if( CheckReturnEndTime() )
		{
			SetWeaponDead();
			return;
		}
		else if( CheckReturnEndRange() )
		{
			SetWeaponDead();
			return;
		}
		else
		{
			OnReturnState( fTimePerSec, pStage );
		}
		break;
	}

	UpdateWorldAttackBox( fTimePerSec, false );

	// Sync
	if( m_dwSyncTime == 0 || m_dwSyncTime < dwCurTime )
	{
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_WEAPON_SYNC );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << STC_SYNC;
			kPacket << GetPosition();
			kPacket << GetOrientation();
			kPacket << GetMoveDir();
			kPacket << (int)m_ControlState;
			P2PNetwork::SendToAllPlayingUser( kPacket );

			m_dwSyncTime = dwCurTime + 250;
		}
	}
}

void ioControlMissileWeapon::SetGraceClose()
{
	if( m_bSetGraceClose ) return;

	if( m_pMissileMesh )
	{
		if( m_bEndEffectGrace )
			m_pMissileMesh->EndEffectGrace();
		else
			m_pMissileMesh->EndEffectForce();
	}

	m_bSetGraceClose = true;
}

bool ioControlMissileWeapon::CheckInRange()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetWeaponDead();
		return true;
	}

	D3DXVECTOR3 vDiff = GetPosition() - pOwner->GetMidPositionByRate();
	float fLengthSq = D3DXVec3LengthSq( &vDiff );
	float fMaxRangeSq = GetMaxRange() * GetMaxRange();

	if( fLengthSq < fMaxRangeSq )
		return true;

	return false;
}

void ioControlMissileWeapon::ChangeToInRange()
{
	m_ControlState = CTS_IN_RANGE;
}

void ioControlMissileWeapon::ChangeToOutRange()
{
	m_ControlState = CTS_OUT_RANGE;
}

void ioControlMissileWeapon::ChangeToReturnState()
{
	m_dwReturnStartTime = FRAMEGETTIME();
	m_ControlState = CTS_RETURN;
}

void ioControlMissileWeapon::OnInRangeState( float fTimePerSec, ioPlayStage *pStage )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetWeaponDead();
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	CheckOwnerRotateDir( fTimePerSec );

	float fCurSpeed = GetMoveSpeed();
	float fMoveDist = fCurSpeed * fTimePerSec;

	m_vPrePos = GetPosition();
	D3DXVECTOR3 vNewPos = m_vPrePos + (m_vMoveDir * fMoveDist);

	float fHeight = pStage->GetMapHeightNoneEntity( vNewPos.x, vNewPos.z, vNewPos.y+m_pMissileMesh->GetBoundBox().GetExtents(1) );
	if( fabs( m_vPrePos.y - fHeight ) < m_fEnableMoveGap )
	{
		vNewPos.y = fHeight + m_fDefaultGapHeight;
	}

	SetPosition( vNewPos );

	if( m_pMissileMesh )
	{
		m_pMissileMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioControlMissileWeapon::OnOutRangeState( float fTimePerSec, ioPlayStage *pStage )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetWeaponDead();
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	CheckOwnerReturnDir( fTimePerSec );

	float fCurSpeed = GetMoveSpeed();
	float fMoveDist = fCurSpeed * fTimePerSec;

	m_vPrePos = GetPosition();
	D3DXVECTOR3 vNewPos = m_vPrePos + (m_vMoveDir * fMoveDist);

	float fHeight = pStage->GetMapHeightNoneEntity( vNewPos.x, vNewPos.z, vNewPos.y+m_pMissileMesh->GetBoundBox().GetExtents(1) );
	if( fabs( m_vPrePos.y - fHeight ) < m_fEnableMoveGap )
	{
		vNewPos.y = fHeight + m_fDefaultGapHeight;
	}

	SetPosition( vNewPos );

	if( m_pMissileMesh )
	{
		m_pMissileMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioControlMissileWeapon::OnReturnState( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ioBaseChar *pTarget = GetOwner();
	if( !pTarget )
	{
		SetWeaponDead();
		return;
	}

	CharState eState = pTarget->GetState();
	switch( eState )
	{
	case CS_DIE:
	case CS_DROP_ZONE_DOWN:
	case CS_READY:
	case CS_VIEW:
	case CS_OBSERVER:
	case CS_LOADING:
	case CS_STARTMOTION:
	case CS_ENDMOTION:
		SetWeaponDead();
		return;
	}

	D3DXVECTOR3 vTargetDir = pTarget->GetMidPositionByRate() - GetPosition();
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	//
	D3DXVECTOR3 vTargetXZ = vTargetDir;
	vTargetXZ.y = 0.0f;
	D3DXVec3Normalize( &vTargetXZ, &vTargetXZ );

	D3DXQUATERNION	qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vTargetXZ );

	D3DXVECTOR3 vMoveXZ = m_vMoveDir;
	vMoveXZ.y = 0.0f;
	D3DXVec3Normalize( &vMoveXZ, &vMoveXZ );

	//

	float fDotValue = D3DXVec3Dot( &vTargetXZ, &vMoveXZ );
	if( fDotValue > 0.98f )
	{
		vMoveXZ = vTargetXZ;
		vMoveXZ.y = vTargetDir.y;
		D3DXVec3Normalize( &vMoveXZ, &vMoveXZ );
		m_vMoveDir = vMoveXZ;

		D3DXQUATERNION	qtNewRot;
		ioMath::CalcDirectionQuaternion( qtNewRot, -m_vMoveDir );
		SetOrientation( qtNewRot );
	}
	else if( m_dwRotateTime > 0 )
	{
		float fAngleGap = RADtoDEG( acosf(fDotValue) );
		float fRate = (fTimePerSec*1000) / m_dwRotateTime;
		float fRotateAngle = 360.0f * fRate;

		if( fAngleGap < fRotateAngle )
		{
			vMoveXZ = vTargetXZ;
			vMoveXZ.y = vTargetDir.y;
			D3DXVec3Normalize( &vMoveXZ, &vMoveXZ );
			m_vMoveDir = vMoveXZ;

			D3DXQUATERNION	qtNewRot;
			ioMath::CalcDirectionQuaternion( qtNewRot, -m_vMoveDir );
			SetOrientation( qtNewRot );
		}
		else
		{
			D3DXQUATERNION qtRotate;
			ioMath::CalcDirectionQuaternion( qtRotate, -vMoveXZ );

			bool bLeftRot = false;
			if( D3DXVec3Dot( &vTargetXZ, &vMoveXZ ) > -FLOAT1 )
			{
				float fCurYaw, fTargetYaw;
				fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );
				fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRot ) );

				fCurYaw = ioMath::ArrangeHead( fCurYaw );
				fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

				float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
				if( fRotAngle < 0.0f )
					bLeftRot = true;
			}

			if( bLeftRot )
			{
				fRotateAngle *= -1;
			}

			fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

			D3DXVECTOR3 vAxis;
			D3DXVec3Cross( &vAxis, &vTargetXZ, &vMoveXZ );
			D3DXVec3Normalize( &vAxis, &-vAxis );

			D3DXQUATERNION	qtAxisRot;
			D3DXQuaternionRotationYawPitchRoll( &qtAxisRot, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );
			vMoveXZ = qtAxisRot * vMoveXZ;
			vMoveXZ.y = vTargetDir.y;
			m_vMoveDir = vMoveXZ;
			D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

			D3DXQUATERNION	qtNewRot;
			ioMath::CalcDirectionQuaternion( qtNewRot, -m_vMoveDir );
			SetOrientation( qtNewRot );
		}
	}

	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	D3DXVECTOR3 vPos = GetPosition();
	vPos = vPos + (fMoveDist * m_vMoveDir);

	SetPosition( vPos );

	if( m_pMissileMesh )
	{
		m_pMissileMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioControlMissileWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pMissileMesh )
	{
		g_EffectFactory.DestroyEffect( m_pMissileMesh );
		m_pMissileMesh = NULL;
	}
}

const ioOrientBox& ioControlMissileWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioControlMissileWeapon::GetType() const
{
	return WT_CONTROL_MISSILE;
}

ActionStopType ioControlMissileWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioControlMissileWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioControlMissileWeapon::GetMoveSpeed()
{
	return GetAttribute()->m_fMoveSpeed * m_fOwnerChargeSpeedRate;
}

float ioControlMissileWeapon::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

void ioControlMissileWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

bool ioControlMissileWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	bool bCollision = false;

	if( ToBaseChar(pEntity) && !ToBaseChar(pEntity)->IsNeedProcess() )
		return false;

	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
		bCollision = true;

	CheckPiercing( bCollision );

	if( bCollision && m_bEnableCollisionReturn )
	{
		if( ToBaseChar(pEntity) && ToBaseChar(pEntity)->IsNeedProcess() )
		{
			ChangeToReturnState();

			if( ToBaseChar(pEntity)->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_WEAPON_SYNC );
				kPacket << GetOwnerName();
				kPacket << m_dwWeaponIdx;
				kPacket << STC_RETURN;
				kPacket << GetPosition();
				kPacket << GetOrientation();
				kPacket << GetMoveDir();
				P2PNetwork::SendToAllPlayingUser( kPacket );

				m_dwSyncTime = FRAMEGETTIME() + 200;
			}
		}
	}

	return bCollision;
}

void ioControlMissileWeapon::CheckOwnerRotateDir( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwRotateTime == 0 ) return;

	DWORD dwGapTime = dwCurTime - m_dwCreatedTime;

	if( dwGapTime < m_dwRotateStartTime )
		return;

	D3DXQUATERNION	qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vTargetDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	float fAngle = D3DXVec3Dot( &vTargetDir, &m_vMoveDir );
	if( fAngle > 0.98f )
	{
		SetOrientation( qtRot );
		m_vMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	}
	else
	{
		float fAngleGap = RADtoDEG( acosf(fAngle) );
		float fRate = (fTimePerSec*1000) / m_dwRotateTime;
		float fRotateAngle = 360.0f * fRate;

		if( fAngleGap < fRotateAngle )
		{
			SetOrientation( qtRot );
			m_vMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		}
		else
		{
			D3DXQUATERNION qtRotate;
			ioMath::CalcDirectionQuaternion( qtRotate, -m_vMoveDir );

			bool bLeftRot = false;
			if( D3DXVec3Dot( &vTargetDir, &m_vMoveDir ) > -FLOAT1 )
			{
				float fCurYaw, fTargetYaw;
				fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );
				fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRot ) );

				fCurYaw = ioMath::ArrangeHead( fCurYaw );
				fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

				float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
				if( fRotAngle < 0.0f )
					bLeftRot = true;
			}

			if( bLeftRot )
				fRotateAngle *= -1;

			fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

			D3DXQUATERNION	qtNewRot;
			D3DXQuaternionRotationYawPitchRoll( &qtNewRot, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );
			m_vMoveDir = qtNewRot * m_vMoveDir;

			ioMath::CalcDirectionQuaternion( qtNewRot, -m_vMoveDir );
			SetOrientation( qtNewRot );
		}
	}
}

void ioControlMissileWeapon::CheckOwnerReturnDir( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwRotateTime == 0 ) return;

	DWORD dwGapTime = dwCurTime - m_dwCreatedTime;
	if( dwGapTime < m_dwRotateStartTime )
		return;

	D3DXVECTOR3 vTargetDir = pOwner->GetMidPositionByRate() - GetPosition();
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vTargetXZDir, vMoveXZDir;
	vTargetXZDir = vTargetDir;
	vTargetDir.y = 0.0f;
	D3DXVec3Normalize( &vTargetXZDir, &vTargetXZDir );

	vMoveXZDir = m_vMoveDir;
	vMoveXZDir.y = 0.0f;
	D3DXVec3Normalize( &vMoveXZDir, &vMoveXZDir );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vTargetXZDir );

	float fAngle = D3DXVec3Dot( &vTargetXZDir, &vMoveXZDir );
	if( fAngle > 0.98f )
	{
		SetOrientation( qtRot );
		m_vMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		m_vMoveDir.y = vTargetDir.y;
		D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
	}
	else
	{
		float fAngleGap = RADtoDEG( acosf(fAngle) );
		float fRate = (fTimePerSec*1000) / m_dwRotateTime;
		float fRotateAngle = 360.0f * fRate;

		if( fAngleGap < fRotateAngle )
		{
			SetOrientation( qtRot );
			m_vMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			m_vMoveDir.y = vTargetDir.y;
			D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
		}
		else
		{
			D3DXQUATERNION qtRotate;
			ioMath::CalcDirectionQuaternion( qtRotate, -vMoveXZDir );

			bool bLeftRot = false;
			if( D3DXVec3Dot( &vTargetXZDir, &vMoveXZDir ) > -FLOAT1 )
			{
				float fCurYaw, fTargetYaw;
				fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );
				fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRot ) );

				fCurYaw = ioMath::ArrangeHead( fCurYaw );
				fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

				float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
				if( fRotAngle < 0.0f )
					bLeftRot = true;
			}

			if( bLeftRot )
				fRotateAngle *= -1;

			fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

			D3DXQUATERNION	qtNewRot;
			D3DXQuaternionRotationYawPitchRoll( &qtNewRot, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );
			vMoveXZDir = qtNewRot * vMoveXZDir;

			m_vMoveDir = vMoveXZDir;
			m_vMoveDir.y = vTargetDir.y;
			D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

			ioMath::CalcDirectionQuaternion( qtNewRot, -m_vMoveDir );
			SetOrientation( qtNewRot );
		}
	}
}

DWORD ioControlMissileWeapon::GetCurRotateEndTime()
{
	return m_dwRotateEndTime * m_fOwnerChargeLiveTimeRate;
}

void ioControlMissileWeapon::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case STC_SYNC:
		{
			D3DXVECTOR3 vPos, vDir;
			D3DXQUATERNION qtRot;

			rkPacket >> vPos >> qtRot >> vDir;

			int iState;
			rkPacket >> iState;

			SetPosition( vPos );
			SetOrientation( qtRot );
			m_vMoveDir = vDir;

			m_ControlState = (ControlState)iState;
		}
		break;
	case STC_RETURN:
		{
			D3DXVECTOR3 vPos, vDir;
			D3DXQUATERNION qtRot;

			rkPacket >> vPos >> qtRot >> vDir;

			SetPosition( vPos );
			SetOrientation( qtRot );
			m_vMoveDir = vDir;

			ChangeToReturnState();
		}
		break;
	case STC_IN:
		{
			m_ControlState = CTS_IN_RANGE;

			D3DXVECTOR3 vPos, vDir;
			D3DXQUATERNION qtRot;

			rkPacket >> vPos >> qtRot >> vDir;

			SetPosition( vPos );
			SetOrientation( qtRot );
			m_vMoveDir = vDir;
		}
		break;
	case STC_OUT:
		{
			m_ControlState = CTS_OUT_RANGE;

			D3DXVECTOR3 vPos, vDir;
			D3DXQUATERNION qtRot;

			rkPacket >> vPos >> qtRot >> vDir;

			SetPosition( vPos );
			SetOrientation( qtRot );
			m_vMoveDir = vDir;
		}
		break;
	case STC_DIE:
		SetWeaponDead();
		break;
	}
}

void ioControlMissileWeapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
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
	}

	if( !bCollision )
		return;

	if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
		ToDummyChar(pTarget) || ToWallZone(pTarget) ||
		GetPiercingType() == CPT_NONE )
	{
		CheckCurAttribute( pTarget );

		ApplyWeaponExp eExplosionType = pTarget->ApplyWeapon( this );
		pStage->AddWeaponExplosionEffect( pTarget, this, eExplosionType );

		AddCollisionEntity( pTarget );

		ioBaseChar *pOwner = GetOwner();
		if( pOwner && pOwner->IsNeedProcess() )
			SetWeaponDead();

		return;
	}
}

bool ioControlMissileWeapon::CheckReturnEndTime()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return true;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwReturnStartTime+m_dwReturnDuration < dwCurTime )
		return true;

	return false;
}

bool ioControlMissileWeapon::CheckReturnEndRange()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return true;

	D3DXVECTOR3 vDiff = pOwner->GetMidPositionByRate() - GetPosition();
	
	float fLengthSq = D3DXVec3LengthSq( &vDiff );
	if( fLengthSq < m_fReturnEndRange*m_fReturnEndRange )
		return true;

	return false;
}






