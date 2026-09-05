

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioTraceReturnWeapon.h"

ActionStopType ioTraceReturnWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioTraceReturnWeapon::m_DieActionStopType = AST_NONE;

ioTraceReturnWeapon::ioTraceReturnWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ONE_TIME_USE | WP_MISSILE_WEAPON | WP_DIVIDE_ACTION );

	m_pFireBall = NULL;
	m_State = TS_MOVE;

	m_dwSensingStartTime = 0;
	m_dwReturnStartTime = 0;

	m_bResistance = false;
	m_fCurMoveRange = 0.0f;
	m_fCurMoveSpeed = 0.0f;
}

ioTraceReturnWeapon::~ioTraceReturnWeapon()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioTraceReturnWeapon::~ioTraceReturnWeapon - UnReleased Resource" );
	}
}

void ioTraceReturnWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );

	// Sensing
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_dwSensingEnableTime = rkLoader.LoadInt_e( "sensing_enable_time", 0 );
	m_dwSensingDuration = rkLoader.LoadInt_e( "sensing_duration", 0 );
	m_fSensingRange = rkLoader.LoadFloat_e( "sensing_range", 0.0f );
	m_fSensingAngle = rkLoader.LoadFloat_e( "sensing_angle", 0.0f );

	// Tracing
	m_dwTraceRotSpeed = (DWORD)rkLoader.LoadInt_e( "trace_rotate_speed", 0 );

	// Return
	m_dwReturnDuration = (DWORD)rkLoader.LoadFloat_e( "return_duration", 0 );
	m_fReturnEndRange = rkLoader.LoadFloat_e( "return_end_range", 0.0f );


	// Move
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

bool ioTraceReturnWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

bool ioTraceReturnWeapon::CreateFireBall( const ioHashString &szFileName )
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

	AddEffectEvent( m_pFireBall );
	return true;
}

void ioTraceReturnWeapon::SetLiveTime( DWORD dwTime )
{
}

void ioTraceReturnWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireBall && m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->HasLinkedSystem() )
	{
		m_vMoveDir = GetOwner()->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		SetOrientation( GetOwner()->GetTargetRot() );
		return;
	}

	SetPosition( vPos );
	m_vMoveDir = GetOwner()->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	SetOrientation( GetOwner()->GetTargetRot() );
}

bool ioTraceReturnWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨
	
	bool bSend = false;
	if( !m_TargetName.IsEmpty() )
	{
		ioBaseChar *pOwner = pStage->GetOwnerChar();
		if( pOwner && pOwner->GetCharName() == m_TargetName )
			bSend = true;
	}

	if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
	{
		SetWeaponDead();
		return true;
	}

	return false;
}

void ioTraceReturnWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLiveTime = GetLiveTime();
	bool bReturnCheck = false;

	switch( m_State )
	{
	case TS_MOVE:
		bReturnCheck = true;
		OnMove( pStage, fTimePerSec );
		break;
	case TS_SENSING:
		bReturnCheck = true;
		OnSensing( pStage, fTimePerSec );
		break;
	case TS_TRACING:
		bReturnCheck = true;
		OnTracing( pStage, fTimePerSec );
		break;
	case TS_RETURN:
		if( m_dwReturnStartTime > 0 && m_dwReturnStartTime+m_dwReturnDuration < dwCurTime )
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
			OnTracingReturn( pStage, fTimePerSec );
		}
		break;
	}

	if( bReturnCheck && m_dwCreatedTime+dwLiveTime < dwCurTime )
	{
		ChangeToReturnState( pStage, true );
	}

	UpdateWorldAttackBox( fTimePerSec, false );

	if( m_pFireBall )
	{
		m_pFireBall->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioTraceReturnWeapon::OnMove( ioPlayStage *pStage, float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCreatedTime + m_dwSensingEnableTime < dwCurTime )
	{
		m_State = TS_SENSING;
		m_dwSensingStartTime = dwCurTime;
		return;
	}

	CheckChangeDir();

	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
	m_fCurMoveRange += fMoveDist;
}

void ioTraceReturnWeapon::OnSensing( ioPlayStage *pStage, float fTimePerSec )
{
	if( !m_TargetName.IsEmpty() )
		m_TargetName.Clear();

	DWORD dwCurTime = FRAMEGETTIME();
	// Sensing 시간동안 타겟 잡지 못하면 이동만 한다
	if( m_dwSensingStartTime + m_dwSensingDuration < dwCurTime )
	{
		float fMoveDist = GetMoveSpeed() * fTimePerSec;
		GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
		return;
	}

	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vCurPos = GetPosition();
	float fSensingRangeSq = m_fSensingRange * m_fSensingRange;

	float fSensingHalfAnigle = m_fSensingAngle * FLOAT05;
	fSensingHalfAnigle = ioMath::ArrangeHead(fSensingHalfAnigle);
	float fHalfValue = cosf( DEGtoRAD(fSensingHalfAnigle) );

	// Char Check
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pChar = *iter;
		if( pChar->GetTeam() == GetTeam() ) continue;
		if( pChar->GetCharName() == GetOwnerName() ) continue;
		if( !IsEnableTargetState( pChar ) )
			continue;

		D3DXVECTOR3 vTargetPos = pChar->GetMidPositionByRate();
		vDiff = vCurPos - vTargetPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( m_fSensingAngle != 0.0f && GetOwner() )
		{
			D3DXQUATERNION qtRot = GetOwner()->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			D3DXVECTOR3 vDiff2 = vTargetPos - GetOwner()->GetMidPositionByRate();
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 거리 체크
		if( fDiffRangeSq < fSensingRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pChar->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pChar->GetCharName();
			}
		}
	}

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget && pTarget->IsNeedProcess() )
		{
			m_TargetName = szTarget;
			SetTraceState( pStage, true );
		}
	}
}

void ioTraceReturnWeapon::OnTracing( ioPlayStage *pStage, float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetName );
	if( pTarget )
	{
		if( !m_TargetName.IsEmpty() && pTarget->IsSystemState() )
		{
			m_TargetName.Clear();
		}
		
		if( !m_TargetName.IsEmpty() && !pTarget->IsEnableTarget() )
		{
			m_TargetName.Clear();
		}

		if( !m_TargetName.IsEmpty() )
		{
			// 방향 계산
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
			else if( m_dwTraceRotSpeed > 0 )
			{
				float fAngleGap = RADtoDEG( acosf(fDotValue) );
				float fRate = (fTimePerSec*1000) / m_dwTraceRotSpeed;
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
		}
	}
	else
	{
		m_TargetName.Clear();
	}

	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	D3DXVECTOR3 vPos = GetPosition();
	vPos += fMoveDist * m_vMoveDir;

	SetPosition( vPos );
}

void ioTraceReturnWeapon::OnTracingReturn( ioPlayStage *pStage, float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ioBaseChar *pTarget = GetOwner();
	if( !pTarget )
	{
		SetWeaponDead();
		return;
	}

	if( !CheckReturnTargetValidate( pStage ) )
	{
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
	else if( m_dwTraceRotSpeed > 0 )
	{
		float fAngleGap = RADtoDEG( acosf(fDotValue) );
		float fRate = (fTimePerSec*1000) / m_dwTraceRotSpeed;
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
	vPos += fMoveDist * m_vMoveDir;

	SetPosition( vPos );
}

void ioTraceReturnWeapon::SetTraceTarget( ioHashString &szName )
{
	m_TargetName = szName;
}

void ioTraceReturnWeapon::SetTraceState( ioPlayStage *pStage, bool bSendNetwork )
{
	if( m_State == TS_RETURN )
		return;

	m_State = TS_TRACING;

	if( bSendNetwork )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_WEAPON_SYNC );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << m_State;
			kPacket << m_TargetName;
			kPacket << GetPosition();
			kPacket << m_vMoveDir;
			kPacket << GetOrientation();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioTraceReturnWeapon::ChangeToReturnState( ioPlayStage *pStage, bool bSendNetwork )
{
	if( !GetOwner() || !GetOwner()->IsNeedProcess() )
		return;

	if( m_State == TS_RETURN )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_State = TS_RETURN;
	m_TargetName = m_OwnerName;
	m_dwReturnStartTime = dwCurTime;

	if( bSendNetwork )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_WEAPON_SYNC );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << m_State;
			kPacket << m_TargetName;
			kPacket << GetPosition();
			kPacket << m_vMoveDir;
			kPacket << GetOrientation();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioTraceReturnWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

const ioOrientBox& ioTraceReturnWeapon::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioTraceReturnWeapon::GetType() const
{
	return WT_TRACE_RETURN;
}

ActionStopType ioTraceReturnWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioTraceReturnWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioTraceReturnWeapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
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
		{
			SetWeaponDead();
		}

		return;
	}
}

bool ioTraceReturnWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	bool bCollision = false;

	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
		bCollision = true;

	CheckPiercing( bCollision );

	if( bCollision && m_State != TS_RETURN )
	{
		m_TargetName.Clear();
	}

	return bCollision;
}

DWORD ioTraceReturnWeapon::GetCollisionEnableTime() const
{
	if( m_State != TS_MOVE )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

bool ioTraceReturnWeapon::IsEnableTargetState( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	if( pChar->IsSystemState() )
		return false;
		
	if( !pChar->IsEnableTarget() )
		return false;

	if( !pChar->CheckTargetWoundType(m_TargetWoundType) )
		return false;

	return true;
}

void ioTraceReturnWeapon::CheckChangeDir()
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

void ioTraceReturnWeapon::RotateMoveDir( float fAngleH, float fAngleV )
{
	fAngleH = ioMath::ArrangeHead( fAngleH );
	fAngleV = ioMath::ArrangeHead( fAngleV );

	D3DXQUATERNION qtRot;

	// horizon
	if( fAngleH != 0.0f )
	{
		D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
		m_vMoveDir = qtRot * m_vMoveDir;

		GetParentSceneNode()->Rotate( ioMath::UNIT_Y, fAngleH );
	}

	// vertical
	if( fAngleV != 0.0f )
	{
		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );
		D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
		m_vMoveDir = qtRot * m_vMoveDir;

		GetParentSceneNode()->Rotate( vAxis, fAngleV );
	}
}

float ioTraceReturnWeapon::GetMoveSpeed()
{
	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed * m_fOwnerChargeSpeedRate;

	return m_fCurMoveSpeed;
}

void ioTraceReturnWeapon::SetOwnerChargeScaleRate( float fRate )
{
	m_fOwnerChargeScaleRate = fRate;

	if( m_fOwnerChargeScaleRate > FLOAT1 )
	{
		D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
		GetParentSceneNode()->SetScale( vScale );
	}
}

void ioTraceReturnWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

void ioTraceReturnWeapon::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	int iState;
	rkPacket >> iState;

	ioHashString szTargetName;
	rkPacket >> szTargetName;

	D3DXVECTOR3 vPos, vDir;
	D3DXQUATERNION qtRot;
	rkPacket >> vPos >> vDir;
	rkPacket >> qtRot;

	switch( iState )
	{
	case TS_TRACING:
		{
			SetPosition( vPos );
			SetOrientation( qtRot );
			m_vMoveDir = vDir;
			m_TargetName = szTargetName;
			m_State = TS_TRACING;
		}
		break;
	case TS_RETURN:
		{
			SetPosition( vPos );
			SetOrientation( qtRot );
			m_vMoveDir = vDir;
			m_TargetName = szTargetName;
			m_State = TS_RETURN;

			m_dwReturnStartTime = FRAMEGETTIME();
		}
		break;
	}
}

bool ioTraceReturnWeapon::CheckReturnTargetValidate( ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioBaseChar *pTarget = GetOwner();
	if( pTarget )
	{
		if( !m_TargetName.IsEmpty() )
		{
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
				return false;
			}

			return true;
		}
	}

	return false;
}

bool ioTraceReturnWeapon::CheckReturnEndRange()
{
	ioBaseChar *pTarget = GetOwner();
	if( !pTarget ) return true;

	D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - GetPosition();
	float fDistanceSq = D3DXVec3LengthSq( &vDiff );

	if( fDistanceSq < m_fReturnEndRange * m_fReturnEndRange )
		return true;

	return false;
}




