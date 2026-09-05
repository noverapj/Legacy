#include "StdAfx.h"
#include "ioTracingRushDummy.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioTracingRushDummy::ioTracingRushDummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode ), m_TraceState( TS_SENSING )
{
}

ioTracingRushDummy::~ioTracingRushDummy(void)
{
}

void ioTracingRushDummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	LoadInitProperty( rkLoader );
}

void ioTracingRushDummy::LoadInitProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
}

void ioTracingRushDummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	switch( m_State )
	{
	case DCS_START:
		ProcessStartState( fTimePerSec );
		break;	
	case DCS_MOVE:
		ProcessMoveState( fTimePerSec );	
		break;
	case DCS_DIE:
		ProcessDieState( fTimePerSec );
		break;
	}

	if( 0 < m_dwLifeTime && m_dwStartTime + m_dwLifeTime < FRAMEGETTIME() )
		SendDieState( true );

	ProcessEffectUpdate( fTimePerSec );
}

void ioTracingRushDummy::ProcessStartState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioBaseChar* pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	D3DXVECTOR3 vPos = GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z );
	if ( vPos.y < fMapHeight )
	{
		vPos.y = fMapHeight;
		SetWorldPosition( vPos );
	}

	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < dwCurTime )
	{
		m_vTracingMoveDir = m_vAttackDir;
		SetMoveState();
		return;
	}
}

void ioTracingRushDummy::ProcessMoveState( float fTimePerSec )
{
	ioBaseChar* pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	D3DXVECTOR3 vPos = GetWorldPosition();
	float fMoveValue = fTimePerSec * m_fMoveSpeed;
	vPos += m_vTracingMoveDir * fMoveValue;
	SetWorldPosition( vPos );

	switch( m_TraceState )
	{
	case TS_SENSING:
		ProcessSensing( fTimePerSec );
		break;
	case TS_TRACE:
		ProcessTracing( fTimePerSec, pStage );
		break;
	}

	int iCnt = m_vAttachWeaponIndexList.size();
	for( int i = 0; i < iCnt; ++i )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_vAttachWeaponIndexList[i] );
		if( !pWeapon )
			SetDieState( true );
	}
}

void ioTracingRushDummy::ProcessSensing( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->GetExperienceMode() == EMS_EXPERIENCE )
		return;

	ioHashString szTarget = GetTraceTarget( pStage, false );
	if( !szTarget.IsEmpty() )
	{		
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget && pTarget->IsNeedProcess() )
		{
			m_TargetTraceName = szTarget;			
			SetTracingState( pStage, true );
		}
	}
}

void ioTracingRushDummy::SetSensingState( ioPlayStage *pStage, bool bSendNetwork )
{
	m_TraceState = TS_SENSING;
	m_TargetTraceName.Clear();

	if( bSendNetwork )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << DUMMY_CHAR_TRACE;
			kPacket	<< (byte)m_TraceState;			
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioTracingRushDummy::SetTracingState( ioPlayStage *pStage, bool bSendNetwork )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;
		
	m_dwTracingEndTime = FRAMEGETTIME() + m_dwTraceDuration;
	m_TraceState = TS_TRACE;

	if( bSendNetwork )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << DUMMY_CHAR_TRACE;
			kPacket	<< (byte)m_TraceState;	
			kPacket << m_TargetTraceName;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioTracingRushDummy::ProcessTracing( float fTimePerSec, ioPlayStage *pStage )
{	
	if( m_dwTracingEndTime > FRAMEGETTIME() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetTraceName );
		if( pTarget && !pTarget->IsSystemState() && pTarget->IsEnableTarget() )
		{
			m_vPrePos = GetWorldPosition();
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

					vDiff = pTarget->GetMidPositionByRate() - GetWorldPosition();
					D3DXVec3Normalize( &vDiff, &vDiff );
					vDiff.y = 0.0f;

					D3DXVECTOR3 vPreDir = m_vTracingMoveDir;
					D3DXVECTOR3 vCurForce = m_vTracingMoveDir * m_fMoveSpeed * fCurLoopTimePerSec;
					m_vTracingMoveDir = vCurForce + (vDiff * m_dwRotateSpeed * fCurLoopTimePerSec);

					D3DXVec3Normalize( &m_vTracingMoveDir, &m_vTracingMoveDir );

					float fCos = D3DXVec3Dot( &vPreDir, &m_vTracingMoveDir );
					// 0도 일때
					if( fCos >= FLOAT1 )
					{
						// 처리 필요없음
					}
					else if( fCos <= -FLOAT1 ) // 180도 일때
					{
						D3DXVECTOR3 vAxis1, vAxis2;
						if( m_vTracingMoveDir != ioMath::UNIT_Y )
						{
							D3DXVec3Cross( &vAxis1, &m_vTracingMoveDir, &ioMath::UNIT_Y );
							D3DXVec3Normalize( &vAxis1, &-vAxis1 );
						}
						else
						{
							D3DXVec3Cross( &vAxis1, &m_vTracingMoveDir, &ioMath::UNIT_X );
							D3DXVec3Normalize( &vAxis1, &-vAxis1 );
						}

						D3DXVec3Cross( &vAxis2, &m_vTracingMoveDir, &vAxis1 );
						D3DXVec3Normalize( &vAxis2, &-vAxis2 );

						GetGroup()->GetParentSceneNode()->Rotate( vAxis2, FLOAT1 );
					}
					else // 그외
					{
						float fGapAngle = RADtoDEG( acos( fCos ) );

						D3DXVECTOR3 vAxis;
						D3DXVec3Cross( &vAxis, &vDiff, &m_vTracingMoveDir );
						D3DXVec3Normalize( &vAxis, &-vAxis );

						GetGroup()->GetParentSceneNode()->Rotate( vAxis, fGapAngle );
					}
				}
			}
			else
			{
				SetSensingState( pStage, false );
			}
		}
		else
		{
			SetSensingState( pStage, false );
		}
	}
	else
	{
		SetSensingState( pStage, false );
	}
}

void ioTracingRushDummy::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage )
		return;	

	int eType;
	rkPacket >> eType;

	switch( eType )
	{
	case DUMMY_CHAR_TRACE:
		{
			byte eSubState;
			rkPacket >> eSubState;

			switch( eSubState )
			{
			case TS_SENSING:
				{
					SetSensingState( pStage, false );
				}
				break;
			case TS_TRACE:
				{
					rkPacket >> m_TargetTraceName;
					SetTracingState( pStage, false );
				}
				break;
			}
		}
		break;
	default:
		{	
			ApplyDummyCharSyncByType( eType, rkPacket, pStage );	
		}
		break;
	}
}