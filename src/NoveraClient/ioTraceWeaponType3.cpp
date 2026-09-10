

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioTraceWeaponType3.h"

ActionStopType ioTraceWeaponType3::m_ActionStopType = AST_NONE;
ActionStopType ioTraceWeaponType3::m_DieActionStopType = AST_NONE;

ioTraceWeaponType3::ioTraceWeaponType3( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_MINE_EXPLOSION );

	m_pFireBall = NULL;
	m_State = TS_MOVE;

	m_dwExplosionTime = 0;
	m_fStartHeight = 0.0f;
	
	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;
	m_dwSensingStartTime = 0;

	m_dwTraceEndTime = 0;

	m_bResistance = false;

	m_dwLandMarkID = -1;

	m_bFirst = true;

	m_dwDeadWeaponID = -1;
	m_bArleadyCallWeapon = false;
}

ioTraceWeaponType3::~ioTraceWeaponType3()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioTraceWeaponType3::~ioTraceWeaponType3 - UnReleased Resource" );
	}
}

void ioTraceWeaponType3::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );
	m_bMapCheck = rkLoader.LoadBool_e( "trace_map_check", false );

	// Sensing
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_dwSensingEnableTime = rkLoader.LoadInt_e( "sensing_enable_time", 0 );

	// Tracing
	m_fTraceSpeed = rkLoader.LoadFloat_e( "trace_speed", 0.0f );
	m_fTraceRange = rkLoader.LoadFloat_e( "trace_range", 0.0f );
	m_fTraceRotSpeed = rkLoader.LoadFloat_e( "trace_rotate_speed", 0.0f );

	m_dwTraceDuration = (DWORD)rkLoader.LoadInt_e( "trace_duration", 0 );

	// Explosion Wait
	m_dwExplosionWaitTime = rkLoader.LoadInt_e( "explosion_wait_time", 0 );

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

	m_bUseLandMark		= rkLoader.LoadBool_e( "use_land_mark", false );
	rkLoader.LoadString_e( "land_mark_effect_red", "", szBuf, MAX_PATH );
	m_szLandMarkRed = szBuf;
	rkLoader.LoadString_e( "land_mark_effect_blue", "", szBuf, MAX_PATH );
	m_szLandMarkBlue = szBuf;

	rkLoader.LoadString_e( "dead_weapon", "", szBuf, MAX_PATH );
	m_szDeadWeapon = szBuf;

	LoadCallAttributeList( rkLoader );
}

void ioTraceWeaponType3::LoadCallAttributeList( ioINILoader &rkLoader )
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

bool ioTraceWeaponType3::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

bool ioTraceWeaponType3::CreateFireBall( const ioHashString &szFileName )
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

void ioTraceWeaponType3::SetLiveTime( DWORD dwTime )
{
}

void ioTraceWeaponType3::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
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

bool ioTraceWeaponType3::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( m_State == TS_WAIT_EXPLOSION || m_State == TS_EXPLOSION )
		return true;

	if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
	{
		if( m_dwDeadWeaponID == -1 && !m_szDeadWeapon.IsEmpty() )
		{
			D3DXVECTOR3		vPos = GetPosition();
			D3DXVECTOR3		vScale = GetScale();
			D3DXQUATERNION	qtRot = GetOrientation();
			ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_szDeadWeapon, vPos, vScale );
			if( pMapEffect )
			{
				m_dwDeadWeaponID = pMapEffect->GetUniqueID();
				ioEffect *pEffect = pMapEffect->GetEffect();
				if( pEffect )
				{
					pEffect->GetParentNode()->SetOrientation( qtRot );
				}
			}
		}

		SetExplosionWaitState( pStage, false );
		return true;
	}

	return false;
}

void ioTraceWeaponType3::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( m_bUseLandMark )
	{
		if( m_bFirst )
		{
			ioBaseChar *pOwner = GetOwner();
			if( pOwner )
			{
				D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
				D3DXVECTOR3 vPos = GetPosition();
				vPos.y = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false );

				ioMapEffect *pMapEffect = NULL;

				if( pOwner->GetTeam() == TEAM_RED )
				{
					pMapEffect = pStage->CreateMapEffect( m_szLandMarkRed,  vPos, vScale );
				}
				else if( pOwner->GetTeam() == TEAM_BLUE )
				{
					pMapEffect = pStage->CreateMapEffect( m_szLandMarkBlue,  vPos, vScale );
				}
				else
				{
					if( pOwner->GetSingleConvertTeam() == TEAM_RED )
					{
						pMapEffect = pStage->CreateMapEffect( m_szLandMarkRed,  vPos, vScale );
					}
					else
					{
						pMapEffect = pStage->CreateMapEffect( m_szLandMarkBlue,  vPos, vScale );
					}
				}
				
				if( pMapEffect )
				{
					m_dwLandMarkID = pMapEffect->GetUniqueID();
				}
			}
			m_bFirst = false;
		}
		else if( m_dwLandMarkID != -1 )
		{
			D3DXVECTOR3 vPos = GetPosition();
			vPos.y = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false );

			ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwLandMarkID );
			if( pMapEffect )
			{
				pMapEffect->SetWorldPosition( vPos );
			}
		}
	}

	// 생존시간 넘긴 경우 폭발
	if( m_dwCreatedTime + GetLiveTime() < FRAMEGETTIME() )
	{
		if( m_State != TS_WAIT_EXPLOSION && m_State != TS_EXPLOSION )
		{
			UpdateWorldAttackBox( fTimePerSec, false );

			if( m_pFireBall )
				m_pFireBall->Update( fTimePerSec * FLOAT1000 );

			SetExplosionWaitState( pStage, false );
			return;
		}
	}

	//
	switch( m_State )
	{
	case TS_MOVE:
		OnMove( pStage, fTimePerSec );
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

void ioTraceWeaponType3::OnMove( ioPlayStage *pStage, float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCreatedTime + m_dwSensingEnableTime < dwCurTime &&
		( m_State != TS_WAIT_EXPLOSION || m_State != TS_EXPLOSION ) )
	{
		m_State = TS_TRACING;
		m_dwSensingStartTime = dwCurTime;
		m_dwTraceEndTime = FRAMEGETTIME() + m_dwTraceDuration;
		return;
	}

	CheckChangeDir();

	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
	m_fCurMoveRange += fMoveDist;

	UpdateWorldAttackBox( fTimePerSec, false );
}

void ioTraceWeaponType3::OnTracing( ioPlayStage *pStage, float fTimePerSec )
{	
	ioBaseChar *pOwner = GetOwner();
	if( pStage && pOwner && pOwner->CheckMagicCircle() )
	{
		D3DXVECTOR3 vMagicCirclePosition = pOwner->GetMagicCirclePos();
		
		vMagicCirclePosition.y = pStage->GetMapHeight( vMagicCirclePosition.x, vMagicCirclePosition.z, NULL, false );

		m_vPrePos = GetPosition();
		D3DXVECTOR3 vDiff = vMagicCirclePosition - m_vPrePos;

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

				vDiff = vMagicCirclePosition - GetPosition();
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
		}
	}

	float fMoveDist = m_fTraceSpeed * fTimePerSec;
	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );

	UpdateWorldAttackBox( fTimePerSec, false );
}

void ioTraceWeaponType3::OnWaitExplosion( ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwLandMarkID != -1 )
	{
		if( pStage )
		{
			ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwLandMarkID );
			if( pMapEffect )
			{
				pMapEffect->EndEffectForce();
				m_dwLandMarkID = -1;	
			}
		}
	}

	if( m_dwExplosionTime < dwCurTime )
		SetExplosionState( pStage );
}

void ioTraceWeaponType3::OnExplosion( float fTimePerSec )
{
	if( m_dwLandMarkID != -1 )
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner )
		{
			ioPlayStage *pStage = pOwner->GetCreator();
			if( pStage )
			{
				ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwLandMarkID );
				if( pMapEffect )
				{
					pMapEffect->EndEffectForce();
					m_dwLandMarkID = -1;	
				}
			}
		}		
	}
	CallAttribute();
	SetWeaponDead();
}

void ioTraceWeaponType3::CallAttribute()
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

void ioTraceWeaponType3::SetExplosionWaitState( ioPlayStage *pStage, bool bSendNetwork )
{
	if( m_State == TS_WAIT_EXPLOSION || m_State == TS_EXPLOSION )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwExplosionTime = dwCurTime + m_dwExplosionWaitTime;
	m_State = TS_WAIT_EXPLOSION;

	if( bSendNetwork )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_TRACE_WEAPON_WAIT );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << GetPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioTraceWeaponType3::SetExplosionState( ioPlayStage *pStage )
{
	if( m_State == TS_EXPLOSION ) return;

	UpdateWorldAttackBox( 0.0f, true );

	if( !m_BoomEffect.IsEmpty() )
	{
		D3DXVECTOR3 vScale = GetScale();

		if( m_fOwnerChargeExplosionRate > FLOAT1 )
		{
			vScale = D3DXVECTOR3( m_fOwnerChargeExplosionRate, m_fOwnerChargeExplosionRate, m_fOwnerChargeExplosionRate );
		}

		pStage->CreateMapEffect( m_BoomEffect,
								 GetPosition(),
								 vScale );

		ioWeapon::PlayExplosionSound();
	}

	m_State = TS_EXPLOSION;

	if( m_bSkillDependency )
	{
		ioBaseChar *pOwner = GetOwner();
		if( !pOwner ) return;

		pOwner->SetSkillCreateWeaponDependency( GetWeaponBySkillName() );
	}
}

bool ioTraceWeaponType3::IsCanMineExplosion()
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

void ioTraceWeaponType3::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

const ioOrientBox& ioTraceWeaponType3::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioTraceWeaponType3::GetType() const
{
	return WT_TRACE3;
}

ActionStopType ioTraceWeaponType3::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioTraceWeaponType3::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioTraceWeaponType3::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( m_State == TS_WAIT_EXPLOSION || m_State == TS_EXPLOSION ) return;

	bool bCollision = false;
	ioBaseChar *pChar = ToBaseChar(pTarget);
	if( pTarget->IsWoundedCollision( GetWorldAttackBox() ) )
	{
		if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
			ToDummyChar(pTarget) || ToWallZone(pTarget) )
		{
			bCollision = true;
		}
		else if( pChar && pChar->IsNeedProcess() )
		{
			if( GetTeam() != pTarget->GetTeam() )
			{
				bCollision = true;
			}
			else if( GetOwner() != pTarget )
			{
				if( pChar )
				{
					if( pChar->IsEnableTeamCollision( this ) )
					{
						bCollision = true;
					}
				}
			}
		}
	}

	if( !bCollision )
		return;

	if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
		ToDummyChar(pTarget) || ToWallZone(pTarget) ||
		GetPiercingType() == CPT_NONE )
	{
		SetExplosionWaitState( pStage, true );
		return;
	}
}

bool ioTraceWeaponType3::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	bool bCollision = false;

	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	bCollision = pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint );
	
	CheckPiercing( bCollision );

	return bCollision;
}

DWORD ioTraceWeaponType3::GetCollisionEnableTime() const
{
	if( m_State != TS_MOVE )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

bool ioTraceWeaponType3::IsEnableTargetState( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	if( pChar->IsSystemState() )
		return false;
		
	if( !pChar->IsEnableTarget() )
		return false;

	return true;
}

void ioTraceWeaponType3::CheckChangeDir()
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

void ioTraceWeaponType3::RotateMoveDir( float fAngleH, float fAngleV )
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

float ioTraceWeaponType3::GetMoveSpeed()
{
	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed;
}

void ioTraceWeaponType3::SetOwnerChargeScaleRate( float fRate )
{
	m_fOwnerChargeScaleRate = fRate;

	if( m_fOwnerChargeScaleRate > FLOAT1 )
	{
		D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
		GetParentSceneNode()->SetScale( vScale );
	}
}

void ioTraceWeaponType3::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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