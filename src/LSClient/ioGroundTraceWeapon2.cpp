

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioGroundTraceWeapon2.h"

ActionStopType ioGroundTraceWeapon2::m_ActionStopType = AST_NONE;
ActionStopType ioGroundTraceWeapon2::m_DieActionStopType = AST_NONE;

ioGroundTraceWeapon2::ioGroundTraceWeapon2( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ATTACK_OWNER | WP_MINE_EXPLOSION | WP_WOUND_NO_SOUND | WP_DEF_NO_SOUND );

	m_pFireBall = NULL;
	m_State = TS_PRE;
	m_PrevState = TS_PRE;

	m_dwExplosionStart = 0;
	m_dwExplosionTime = 0;
	m_fStartHeight = 0.0f;
	
	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;

	m_dwSensingStartTime = 0;	
	m_dwTraceEndTime = 0;
	m_dwExplosionLoopStartTime = 0;
	m_dwDealyTime = 0;

	m_bResistance = false;
	m_bWeaponDead = false;

	m_bArleadyCallWeapon = false;

	m_TargetName.Clear();
}

ioGroundTraceWeapon2::~ioGroundTraceWeapon2()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioGroundTraceWeapon2::~ioGroundTraceWeapon2 - UnReleased Resource" );
	}
}

void ioGroundTraceWeapon2::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );
	m_fEnableMoveHeight = rkLoader.LoadFloat_e( "enable_move_height", 0.0f );

	// Sensing
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_dwSensingEnableTime = rkLoader.LoadInt_e( "sensing_enable_time", 0 );
	m_dwSensingDuration = rkLoader.LoadInt_e( "sensing_duration", 0 );
	m_fSensingRange = rkLoader.LoadFloat_e( "sensing_range", 0.0f );
	m_fSensingMaxAngle = rkLoader.LoadFloat_e( "sensing_max_angle", 0.0f );
	m_fSensingMinAngle = rkLoader.LoadFloat_e( "sensing_min_angle", 0.0f );

	// Tracing
	m_fTraceSpeed = rkLoader.LoadFloat_e( "trace_speed", 0.0f );
	m_fTraceRange = rkLoader.LoadFloat_e( "trace_range", 0.0f );
	m_fTraceRotSpeed = rkLoader.LoadFloat_e( "trace_rotate_speed", 0.0f );

	m_dwTraceDuration = (DWORD)rkLoader.LoadInt_e( "trace_duration", 0 );

	// Explosion Wait
	m_dwExplosionWaitTime = rkLoader.LoadInt_e( "explosion_wait_time", 0 );

	// Explosion
	m_fExplosionRange = rkLoader.LoadFloat_e( "explosion_range", 0.0f );
	m_dwExplosionDuration = rkLoader.LoadInt_e( "explosion_duration", 0 );
	m_dwExplosionLoopTime = rkLoader.LoadInt_e( "explosion_loop_time", 300 );

	LoadCallAttributeList( rkLoader );

	m_bDropZoneCheck = rkLoader.LoadBool("DropZoneCheck", false );

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

void ioGroundTraceWeapon2::LoadCallAttributeList( ioINILoader &rkLoader )
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

bool ioGroundTraceWeapon2::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

bool ioGroundTraceWeapon2::CreateFireBall( const ioHashString &szFileName )
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

void ioGroundTraceWeapon2::SetLiveTime( DWORD dwTime )
{
}

void ioGroundTraceWeapon2::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
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

void ioGroundTraceWeapon2::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

bool ioGroundTraceWeapon2::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨
	if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
	{
		CallAttribute();
		SetWeaponDead();
		return true;
	}

	return false;
}

void ioGroundTraceWeapon2::Process( float fTimePerSec, ioPlayStage *pStage )
{
	ioBaseChar* pOwner = GetOwner();
	if( !pOwner )
		SetWeaponDead();

	DWORD dwCurTime = FRAMEGETTIME();

	// 생존시간 넘긴 경우 폭발
	if( m_dwCreatedTime + m_dwLiveTime + m_dwDealyTime < dwCurTime )
	{
		if( m_State != TS_WAIT_EXPLOSION && m_State != TS_EXPLOSION )
		{
			UpdateWorldAttackBox( fTimePerSec, false );

			if( m_pFireBall )
				m_pFireBall->Update( fTimePerSec * FLOAT1000 );

			CallAttribute();
			SetWeaponDead();
			return;
		}
	}

	//
	switch( m_State )
	{
	case TS_PRE:
		m_dwExplosionLoopStartTime = dwCurTime;
		m_State = TS_MOVE;		
		break;
	case TS_MOVE:
		OnMove( pStage, fTimePerSec );
		break;
	case TS_SENSING:
		OnSensing( pStage, fTimePerSec );
		break;
	case TS_TRACING:
		OnTracing( pStage, fTimePerSec );
		break;
	case TS_WAIT_EXPLOSION:
		OnWaitExplosion( pStage );
		break;
	case TS_EXPLOSION:
		OnExplosion( fTimePerSec );
		break;
	}

	if( m_pFireBall )
	{
		m_pFireBall->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioGroundTraceWeapon2::OnMove( ioPlayStage *pStage, float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCreatedTime + m_dwSensingEnableTime < dwCurTime )
	{
		m_State = TS_SENSING;
		m_dwSensingStartTime = dwCurTime;
		return;
	}

	if( m_dwExplosionLoopStartTime + m_dwExplosionLoopTime < dwCurTime )
	{
		m_dwDealyTime += m_dwExplosionWaitTime + m_dwExplosionDuration;
		SetExplosionWaitState( pStage, false, false );
	}
	else
	{
		CheckChangeDir();

		// 이동처리	
		float fMoveDist = GetMoveSpeed() * fTimePerSec;
		ProcessMove( pStage, fMoveDist, fTimePerSec );
	}
}

void ioGroundTraceWeapon2::OnSensing( ioPlayStage *pStage, float fTimePerSec )
{
	if( !m_TargetName.IsEmpty() )
		m_TargetName.Clear();

	DWORD dwCurTime = FRAMEGETTIME();

	// Sensing 시간동안 타겟 잡지 못하면 이동만 한다
	if( m_dwSensingStartTime + m_dwSensingDuration < dwCurTime )
	{
		if( m_dwExplosionLoopStartTime + m_dwExplosionLoopTime < dwCurTime )
		{
			m_dwDealyTime += m_dwExplosionWaitTime + m_dwExplosionDuration;
			SetExplosionWaitState( pStage, false, false );
		}
		else
		{
			float fMoveDist = m_fTraceSpeed * fTimePerSec;
			ProcessMove( pStage, fMoveDist, fTimePerSec );
		}
		return;
	}

	if( m_dwExplosionLoopStartTime + m_dwExplosionLoopTime < dwCurTime )
	{
		m_dwDealyTime += m_dwExplosionWaitTime + m_dwExplosionDuration;
		SetExplosionWaitState( pStage, false, false );
	}
	else
	{
		float fMoveDist = GetMoveSpeed() * fTimePerSec;
		ProcessMove( pStage, fMoveDist, fTimePerSec );
	}

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vCurPos = GetPosition();
	float fSensingRangeSq = m_fSensingRange * m_fSensingRange;
	
	// Char Check
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	float fValue = 0.0f;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( pTarget->GetTeam() == GetTeam() ) continue;
		if( pTarget->GetCharName() == GetOwnerName() ) continue;
		if( pTarget->GetExperienceMode() == EMS_EXPERIENCE ) continue;
		if( IsExperienceOwner() ) continue;
		if( !IsEnableTargetState( pTarget ) )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vCurPos - vTargetPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );
		float fCurValue = 0.0f;

		// 각도 체크
		if( m_fSensingMaxAngle != 0.0f && m_fSensingMinAngle != 0.0f )
		{
			if( !CheckMaxAngle( pTarget, GetPosition(), m_vMoveDir, m_fSensingMaxAngle ) )
				continue;

			D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - GetPosition();

			float fCurAngleRate = D3DXVec3LengthSq( &vDiff ) / fSensingRangeSq;
			float fCurAngle = m_fSensingMinAngle * fCurAngleRate + m_fSensingMaxAngle * (1.0f - fCurAngleRate );
			float fHalfCosine = cosf( DEGtoRAD( fCurAngle ) * FLOAT05 );

			vDiff.y = 0.0f;
			D3DXVec3Normalize( &vDiff, &vDiff );

			if( D3DXVec3Dot( &m_vMoveDir, &vDiff ) < fHalfCosine )
				continue;
		}

		// 거리 체크
		if( fDiffRangeSq < fSensingRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
				fValue = fCurValue;
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
				fValue = fCurValue;
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

bool ioGroundTraceWeapon2::CheckMaxAngle( ioBaseChar*pTarget, D3DXVECTOR3 vPos, D3DXVECTOR3 vDir, float fMaxAngle )
{

	float fMaxHalfCosine = cosf( DEGtoRAD( fMaxAngle ) * FLOAT05  );
	D3DXVECTOR3 vDiff;
	vDiff = pTarget->GetWorldPosition() - vPos;
	vDiff.y = 0.0f;

	D3DXVec3Normalize( &vDiff, &vDiff );
	float fHalfCosine = D3DXVec3Dot( &vDir, &vDiff );

	if( fHalfCosine >= fMaxHalfCosine )
		return true;

	return false;					
}

void ioGroundTraceWeapon2::OnTracing( ioPlayStage *pStage, float fTimePerSec )
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
			D3DXVECTOR3 vPos = GetPosition();
			D3DXVECTOR3 vTargetPos =pTarget->GetWorldPosition();
			vPos.y = 0;
			vTargetPos.y = 0;

			if( !ProcessTrace( vPos, vTargetPos, fTimePerSec ) )			
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

	if( m_dwExplosionLoopStartTime + m_dwExplosionLoopTime < dwCurTime )
	{
		m_dwDealyTime += m_dwExplosionWaitTime + m_dwExplosionDuration;
		SetExplosionWaitState( pStage, false, false );
	}
	else
	{
		float fMoveDist = m_fTraceSpeed * fTimePerSec;
		ProcessMove( pStage, fMoveDist, fTimePerSec );
	}
}

bool ioGroundTraceWeapon2::ProcessTrace( D3DXVECTOR3 vPos, D3DXVECTOR3 vTargetPos, float fTimePerSec, bool bIgnoreTraceTime )
{
	D3DXVECTOR3 vDiff = vTargetPos - vPos;

	float fRangeSq = D3DXVec3LengthSq( & vDiff );
	if( fRangeSq < m_fTraceRange * m_fTraceRange || bIgnoreTraceTime )
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

			vDiff = vTargetPos - vPos;
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
		}

		return true;
	}
	else
	{
		return false;
	}
}

void ioGroundTraceWeapon2::OnWaitExplosion( ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwExplosionTime < dwCurTime )
		SetExplosionState( pStage );
}

void ioGroundTraceWeapon2::OnExplosion( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwExplosionDuration == 0 )
	{
		if( m_bWeaponDead )
		{
			SetWeaponDead();
		}
		else
		{
			m_State = m_PrevState;
			m_dwExplosionLoopStartTime = dwCurTime;
		}

		return;
	}

	
	if( m_dwExplosionStart + m_dwExplosionDuration < dwCurTime )
	{
		if( m_bWeaponDead )
		{
			SetWeaponDead();
		}
		else
		{
			m_State = m_PrevState;
			m_dwExplosionLoopStartTime = dwCurTime;
		}
		return;
	}
}

void ioGroundTraceWeapon2::SetTraceTarget( ioHashString &szName )
{
	m_TargetName = szName;
}

void ioGroundTraceWeapon2::SetTraceState( ioPlayStage *pStage, bool bSendNetwork )
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

void ioGroundTraceWeapon2::SetExplosionWaitState( ioPlayStage *pStage, bool bWeaponDead, bool bSendNetwork )
{
	if( m_State == TS_WAIT_EXPLOSION || m_State == TS_EXPLOSION )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwExplosionTime = dwCurTime + m_dwExplosionWaitTime;

	m_PrevState = m_State;
	m_State = TS_WAIT_EXPLOSION;

	m_bWeaponDead = bWeaponDead;

	if( bSendNetwork )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_TRACE_WEAPON_WAIT );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << GetPosition();
			kPacket << bWeaponDead;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioGroundTraceWeapon2::SetExplosionState( ioPlayStage *pStage )
{
	if( m_State == TS_EXPLOSION ) return;

	m_dwExplosionStart = FRAMEGETTIME();

	UpdateWorldAttackBox( 0.0f, true );

	if( !m_BoomEffect.IsEmpty() )
	{
		pStage->CreateMapEffect( m_BoomEffect,
								 GetPosition(),
								 GetScale() );

		ioWeapon::PlayExplosionSound();
	}

	m_State = TS_EXPLOSION;
}

bool ioGroundTraceWeapon2::IsCanMineExplosion()
{
	if( !IsLive() )	return false;
	if( !HasPeculiarity( WP_MINE_EXPLOSION ) )
		return false;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if( m_State != TS_EXPLOSION )
		return false;

	return true;
}

void ioGroundTraceWeapon2::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

const ioOrientBox& ioGroundTraceWeapon2::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioGroundTraceWeapon2::GetType() const
{
	return WT_GROUND_TRACE2;
}

ActionStopType ioGroundTraceWeapon2::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioGroundTraceWeapon2::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

bool ioGroundTraceWeapon2::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_State != TS_EXPLOSION )
		return false;

	float fTargetDist = -FLOAT1;
	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();
	if( !pEntity->CheckSphereDistance( vCenter, m_fExplosionRange, fTargetDist ) )
		return false;

	if( COMPARE( fTargetDist, 0.0f, m_fExplosionRange ) )	
		return true;

	return false;
}

DWORD ioGroundTraceWeapon2::GetCollisionEnableTime() const
{
	if( m_State != TS_MOVE )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

bool ioGroundTraceWeapon2::IsEnableTargetState( ioBaseChar *pChar )
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

void ioGroundTraceWeapon2::CheckChangeDir()
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

void ioGroundTraceWeapon2::RotateMoveDir( float fAngleH, float fAngleV )
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

float ioGroundTraceWeapon2::GetMoveSpeed()
{
	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed;
}

void ioGroundTraceWeapon2::ProcessMove( ioPlayStage *pStage, float fMoveAmt, float fTimePerSec )
{
	m_fCurMoveRange += fMoveAmt;

	m_vMoveDir.y = 0.0f;
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
		
	D3DXVECTOR3 vCurPos = GetPosition();
	D3DXVECTOR3 vNextPos = vCurPos + (m_vMoveDir * fMoveAmt);
		
	float fHeight = pStage->GetMapHeightNoneEntity( vNextPos.x,
													vNextPos.z,
													vNextPos.y + m_pFireBall->GetBoundBox().GetExtents(1) + 3.0f );

	

	if( 0.0f < fHeight )
	{
		if( m_fEnableMoveHeight < fabs( vNextPos.y - vCurPos.y ) )
		{			
			CallAttribute();
			SetWeaponDead();
			return;
		}
		else
		{
			vNextPos.y = pStage->GetMapHeight( vNextPos.x, vNextPos.z );			
		}
	}
	else
	{
		if( m_bDropZoneCheck )
		{
			vNextPos.y = vCurPos.y;
		}
		else
		{
			CallAttribute();
			SetWeaponDead();
			return;
		}
		
		
	}

	SetPosition( vNextPos );
	UpdateWorldAttackBox( fTimePerSec, false );
}

void ioGroundTraceWeapon2::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

void ioGroundTraceWeapon2::CallAttribute()
{	
	if( m_bArleadyCallWeapon )
		return;

	if( m_CallWeaponInfoList.empty() )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

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