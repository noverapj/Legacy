

#include "stdafx.h"

#include "ioTraceWeapon4.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioTraceWeapon4::m_ActionStopType = AST_NONE;
ActionStopType ioTraceWeapon4::m_DieActionStopType = AST_NONE;

ioTraceWeapon4::ioTraceWeapon4( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ONE_TIME_USE | WP_MISSILE_WEAPON | WP_DIVIDE_ACTION );

	m_pFireBall = NULL;
	m_State = TS_MOVE;

	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;
	m_dwSensingStartTime = 0;

	m_dwTraceEndTime = 0;

	m_bResistance = false;

	m_iTest = 0;
}

ioTraceWeapon4::~ioTraceWeapon4()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioTraceWeapon4::~ioTraceWeapon4 - UnReleased Resource" );
	}
}

void ioTraceWeapon4::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );
	m_fStartHeight = rkLoader.LoadFloat_e( "start_height", 0.0f );

	// Sensing
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_dwSensingEnableTime = rkLoader.LoadInt_e( "sensing_enable_time", 0 );
	m_dwSensingDuration = rkLoader.LoadInt_e( "sensing_duration", 0 );
	m_fSensingRange = rkLoader.LoadFloat_e( "sensing_range", 0.0f );
	m_fSensingAngle = rkLoader.LoadFloat_e( "sensing_angle", 0.0f );

	// Tracing
	m_fTraceSpeed = rkLoader.LoadFloat_e( "trace_speed", 0.0f );
	m_fTraceRange = rkLoader.LoadFloat_e( "trace_range", 0.0f );
	m_fTraceRotSpeed = rkLoader.LoadFloat_e( "trace_rotate_speed", 0.0f );

	m_dwTraceDuration = (DWORD)rkLoader.LoadInt_e( "trace_duration", 0 );

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

bool ioTraceWeapon4::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

bool ioTraceWeapon4::CreateFireBall( const ioHashString &szFileName )
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

void ioTraceWeapon4::SetLiveTime( DWORD dwTime )
{
}

void ioTraceWeapon4::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireBall && m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->HasLinkedSystem() )
	{
		if( GetOwner() )
		{
			m_vMoveDir = GetOwner()->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			SetOrientation( GetOwner()->GetTargetRot() );
		}

		D3DXVECTOR3 vNewPos = vPos;
		vNewPos.y += m_fStartHeight;
		SetPosition( vNewPos );
		return;
	}

	if( GetOwner() )
	{
		m_vMoveDir = GetOwner()->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		SetOrientation( GetOwner()->GetTargetRot() );
	}

	D3DXVECTOR3 vNewPos = vPos;
	vNewPos.y += m_fStartHeight;
	SetPosition( vNewPos );
}

void ioTraceWeapon4::Process( float fTimePerSec, ioPlayStage *pStage )
{
	// 생존시간 넘긴 경우 폭발
	if( m_dwCreatedTime + GetLiveTime() < FRAMEGETTIME() )
	{
		SetWeaponDead();
	}

	//
	switch( m_State )
	{
	case TS_MOVE:
		OnMove( pStage, fTimePerSec );
		break;
	case TS_SENSING:
		OnSensing( pStage, fTimePerSec );
		break;
	case TS_TRACING:
		OnTracing( pStage, fTimePerSec );
		break;
	}

	if( m_pFireBall )
	{
		m_pFireBall->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioTraceWeapon4::OnMove( ioPlayStage *pStage, float fTimePerSec )
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

	UpdateWorldAttackBox( fTimePerSec, false );
}

void ioTraceWeapon4::OnSensing( ioPlayStage *pStage, float fTimePerSec )
{
	if( !m_TargetName.IsEmpty() )
		m_TargetName.Clear();	
	
	DWORD dwCurTime = FRAMEGETTIME();
	// Sensing 시간동안 타겟 잡지 못하면 이동만 한다
	if( m_dwSensingStartTime + m_dwSensingDuration < dwCurTime )
	{
		float fMoveDist = m_fTraceSpeed * fTimePerSec;
		GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );

		UpdateWorldAttackBox( fTimePerSec, false );
		return;
	}

	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vCurPos = GetPosition();
	float fSensingRangeSq = m_fSensingRange * m_fSensingRange;

	float fSensingHalfAnigle = m_fSensingAngle * FLOAT05;
	fSensingHalfAnigle = ioMath::ArrangeHead(fSensingHalfAnigle);
	float fHalfValue = cosf(fSensingHalfAnigle);

	// Char Check
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pChar = *iter;
		if( !pChar ) continue;

		if( pChar->GetTeam() == GetTeam() ) continue;
		if( pChar->GetCharName() == GetOwnerName() ) continue;
		if( pChar->GetExperienceMode() == EMS_EXPERIENCE ) continue;
		if( IsExperienceOwner() ) continue;
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

	UpdateWorldAttackBox( fTimePerSec, false );
}

void ioTraceWeapon4::OnTracing( ioPlayStage *pStage, float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetName );
	if( pTarget )
	{
		if( !m_TargetName.IsEmpty() && pTarget->IsSystemState() )
		{
			m_TargetName.Clear();
		}

		if( !m_TargetName.IsEmpty() && pTarget->HasHideBuff() )
		{
			m_TargetName.Clear();
		}

		if( !m_TargetName.IsEmpty() && pTarget->IsChatModeState( false ) )
		{
			m_TargetName.Clear();
		}

		if( !m_TargetName.IsEmpty() )
		{
			m_vPrePos = GetPosition();
			D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - m_vPrePos;

			float fRangeSq = D3DXVec3LengthSq( & vDiff );
			if( fRangeSq < m_fTraceRange * m_fTraceRange )
			{
				float fTimeGap = fTimePerSec * FLOAT1000;
				float fCurLoopTimeGap;
				float fCurLoopTimePerSec;
				while( fTimeGap > 0.0f )
				{
					if( fTimeGap > FLOAT10 )
					{
						fCurLoopTimeGap = FLOAT10;
						fCurLoopTimePerSec = FLOAT10 / FLOAT1000;

						fTimeGap -= FLOAT10;
					}
					else
					{
						fCurLoopTimeGap = fTimeGap;
						fCurLoopTimePerSec = fTimeGap / FLOAT1000;

						fTimeGap = 0.0f;
					}

					vDiff = pTarget->GetMidPositionByRate() - GetPosition();
					D3DXVec3Normalize( &vDiff, &vDiff );

					D3DXVECTOR3 vPreDir = m_vMoveDir;
					D3DXVECTOR3 vCurForce = m_vMoveDir * m_fTraceSpeed * fCurLoopTimePerSec;
					m_vMoveDir = vCurForce + (vDiff * m_fTraceRotSpeed * fCurLoopTimePerSec);

					D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

					float fCos = D3DXVec3Dot( &vPreDir, &m_vMoveDir );
					// 0도 일때
					if( fCos >= FLOAT1 )
					{
						// 처리 필요없음
					}
					else if( fCos <= -FLOAT1 ) // 180도 일때
					{
						D3DXVECTOR3 vAxis1, vAxis2;
						if( m_vMoveDir != ioMath::UNIT_Y )
						{
							D3DXVec3Cross( &vAxis1, &m_vMoveDir, &ioMath::UNIT_Y );
							D3DXVec3Normalize( &vAxis1, &-vAxis1 );
						}
						else
						{
							D3DXVec3Cross( &vAxis1, &m_vMoveDir, &ioMath::UNIT_X );
							D3DXVec3Normalize( &vAxis1, &-vAxis1 );
						}

						D3DXVec3Cross( &vAxis2, &m_vMoveDir, &vAxis1 );
						D3DXVec3Normalize( &vAxis2, &-vAxis2 );

						GetParentSceneNode()->Rotate( vAxis2, FLOAT1 );
					}
					else // 그외
					{
						float fGapAngle = RADtoDEG( acos( fCos ) );

						D3DXVECTOR3 vAxis;
						D3DXVec3Cross( &vAxis, &vDiff, &m_vMoveDir );
						D3DXVec3Normalize( &vAxis, &-vAxis );

						GetParentSceneNode()->Rotate( vAxis, fGapAngle );
					}

					m_iTest++;
				}
			}
			else
			{
				m_TargetName.Clear();
			}
		}

		if( !m_TargetName.IsEmpty() )
		{
			if( m_dwTraceEndTime < dwCurTime )
			{
				m_TargetName.Clear();
			}
		}
	}
	else
	{
		m_TargetName.Clear();
	}

	float fMoveDist = m_fTraceSpeed * fTimePerSec;
	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );

	UpdateWorldAttackBox( fTimePerSec, false );
}

void ioTraceWeapon4::SetTraceTarget( ioHashString &szName )
{
	m_TargetName = szName;
}

void ioTraceWeapon4::SetTraceState( ioPlayStage *pStage, bool bSendNetwork )
{
	if( m_State == TS_WAIT_EXPLOSION || m_State == TS_EXPLOSION )
		return;

	m_State = TS_TRACING;
	m_dwTraceEndTime = FRAMEGETTIME() + m_dwTraceDuration;

	if( bSendNetwork )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_TRACE_WEAPON );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << GetPosition();
			kPacket << m_TargetName;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioTraceWeapon4::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

const ioOrientBox& ioTraceWeapon4::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioTraceWeapon4::GetType() const
{
	return WT_TRACE4;
}

ActionStopType ioTraceWeapon4::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioTraceWeapon4::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

bool ioTraceWeapon4::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	bool bCollision = false;

	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
		bCollision = CheckCollisionLine( pEntity, vColPoint );

	CheckPiercing( bCollision );

	return bCollision;
}

DWORD ioTraceWeapon4::GetCollisionEnableTime() const
{
	if( m_State != TS_MOVE )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

bool ioTraceWeapon4::IsEnableTargetState( ioBaseChar *pChar )
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

void ioTraceWeapon4::CheckChangeDir()
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

void ioTraceWeapon4::RotateMoveDir( float fAngleH, float fAngleV )
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

float ioTraceWeapon4::GetMoveSpeed()
{
	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed;
}

void ioTraceWeapon4::SetOwnerChargeScaleRate( float fRate )
{
	m_fOwnerChargeScaleRate = fRate;

	if( m_fOwnerChargeScaleRate > FLOAT1 )
	{
		D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
		GetParentSceneNode()->SetScale( vScale );
	}
}

void ioTraceWeapon4::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

bool ioTraceWeapon4::CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint )
{
	if( GetPiercingType() == CPT_ALL || GetPiercingType() == CPT_ALL_LIMITE )
		return true;

	// 타겟이 캐릭터가 아니라면 충돌로 처리한다
	ioBaseChar *pTargetChar = ToBaseChar( pTarget );
	if( !pTargetChar )	return true;

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
		return pOwner->CheckCollisionLine( GetPosition(), vColPoint );

	return false;	// 주인이 없는 무기라면 충돌안한걸로 하자
}


