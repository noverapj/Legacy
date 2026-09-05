

#include "stdafx.h"

#include "ioElevator.h"
#include "WeaponDefine.h"
#include "ioWeaponFeature.h"
#include "FindListPredicateImpl.h"

ioElevator::ioElevator( ioEntityGroup *pGrp, ioPlayMode *pMode, int iCreateIndex )
: ioPlayEntity( pGrp, pMode )
{
	m_iCreateIndex = iCreateIndex;

	m_State = ES_STOP;
	m_fMoveSpeed = FLOAT100;
	m_dwWaitTime = 1000;
	m_dwFirstWaitTime = 1000;

	m_dwMoveNextStartTime = 0;
	m_bEnableFunction = true;
}

ioElevator::~ioElevator()
{
}

void ioElevator::LoadProperty( ioINILoader &rkLoader )
{
	m_fMoveSpeed = rkLoader.LoadFloat_e( "move_speed", FLOAT100 );
	m_dwWaitTime = rkLoader.LoadInt_e( "wait_time", 1000 );
	m_dwFirstWaitTime = rkLoader.LoadInt_e( "first_wait_add_time", 0 );

	D3DXVECTOR3 vPos;
	vPos.x = rkLoader.LoadFloat_e( "pos_x", 0.0f );
	vPos.y = rkLoader.LoadFloat_e( "pos_y", 0.0f );
	vPos.z = rkLoader.LoadFloat_e( "pos_z", 0.0f );
	m_vStartPos = vPos;

	vPos.x = rkLoader.LoadFloat_e( "end_pos_x", 0.0f );
	vPos.y = rkLoader.LoadFloat_e( "end_pos_y", 0.0f );
	vPos.z = rkLoader.LoadFloat_e( "end_pos_z", 0.0f );
	m_vEndPos = vPos;

	SetWorldPosition( m_vStartPos );
}

void ioElevator::Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	if( !m_bEnableFunction || m_State == ES_STOP )
		return;

	UpdateMountedEntityList();
	UpdateMountedWeaponList();

	ElevatorState ePreState = m_State;
	D3DXVECTOR3 vPrePos = GetWorldPosition();

	DWORD dwGapTime = dwCurTime - dwPreTime;

	do 
	{
		DWORD dwCurGap = min( dwGapTime, 10 );
		if( dwGapTime >= 10 )
		{
			dwGapTime -= 10;
		}
		else
		{
			dwCurGap = dwGapTime;
			dwGapTime = 0;
		}

		float fNewTimePerSec = (float)dwCurGap / FLOAT1000;
		dwPreTime += dwCurGap;

		UpdatePositionLoop( fNewTimePerSec, dwPreTime );
	} while( dwGapTime > 0 );

	D3DXVECTOR3 vMoveAmt = GetWorldPosition() - vPrePos;
	if( D3DXVec3LengthSq( &vMoveAmt ) > 0.0f )
	{
		TranslateMountEntity( vMoveAmt );
		TranslateMountWeapon( vMoveAmt );
	}

	NotifyStateChanged( ePreState, dwPreTime );
}

void ioElevator::UpdatePositionLoop( float fTimePerSec, DWORD dwCurTime )
{
	switch( m_State )
	{
	case ES_BOTTOM:
		if( dwCurTime >= m_dwMoveNextStartTime )
		{
			m_State = ES_MOVE_UP;
		}
		break;
	case ES_MOVE_UP:
		MovePosition( fTimePerSec, dwCurTime );
		break;
	case ES_TOP:
		if( dwCurTime >= m_dwMoveNextStartTime )
		{
			m_State = ES_MOVE_DOWN;
		}
		break;
	case ES_MOVE_DOWN:
		MovePosition( fTimePerSec, dwCurTime );
		break;
	}
}

void ioElevator::MovePosition( float fTimePerSec, DWORD dwCurTime )
{
	D3DXVECTOR3 vCurPos = GetWorldPosition();
	float fMoveAmt = m_fMoveSpeed * fTimePerSec;

	if( m_State == ES_BOTTOM || m_State == ES_MOVE_UP )
	{
		// Start -> End
		D3DXVECTOR3 vDir = m_vEndPos - m_vStartPos;
		D3DXVECTOR3 vNormalDir;
		D3DXVec3Normalize( &vNormalDir, &vDir );

		vCurPos += (vNormalDir*fMoveAmt);

		D3DXVECTOR3 vGap = vCurPos - m_vStartPos;
		if( D3DXVec3LengthSq( &vGap ) > D3DXVec3LengthSq( &vDir ) )
		{
			vCurPos = m_vEndPos;

			m_State = ES_TOP;
			m_dwMoveNextStartTime = dwCurTime + m_dwWaitTime;
		}
	}
	else
	{
		// End -> Start
		D3DXVECTOR3 vDir = m_vStartPos - m_vEndPos;
		D3DXVECTOR3 vNormalDir;
		D3DXVec3Normalize( &vNormalDir, &vDir );

		vCurPos += (vNormalDir*fMoveAmt);

		D3DXVECTOR3 vGap = vCurPos - m_vEndPos;
		if( D3DXVec3LengthSq( &vGap ) > D3DXVec3LengthSq( &vDir ) )
		{
			vCurPos = m_vStartPos;

			m_State = ES_BOTTOM;
			m_dwMoveNextStartTime = dwCurTime + m_dwWaitTime;
		}
	}

	SetWorldPosition( vCurPos );
}

void ioElevator::NotifyStateChanged( ElevatorState ePreState, DWORD dwCurTime )
{
	if( !g_MyInfo.IsHost() || !P2PNetwork::IsNetworkPlaying() )
		return;

	if( ePreState == m_State )
		return;

	if( m_State != ES_BOTTOM && m_State != ES_TOP )
		return;

	SP2Packet kPacket( CUPK_ELEVATOR_CHANGED );
	kPacket << GetCreateIndex();
	kPacket << (int)m_State;
	kPacket << m_dwMoveNextStartTime - dwCurTime;
	kPacket << g_GlobalTimer.GetGlobalTime();
	P2PNetwork::SendToAllPlayingUser( kPacket );
}

void ioElevator::UpdateMountedEntityList()
{
	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( pStage )
	{
		pStage->UpdateMountedEntityList( this, m_vMountEntityList, NULL );
	}
}

void ioElevator::TranslateMountEntity( const D3DXVECTOR3 &vMoveAmt )
{
	MountedEntityList::iterator iter = m_vMountEntityList.begin();
	for( ; iter!=m_vMountEntityList.end() ; ++iter )
	{
		(*iter)->TranslateByWorldEntity( vMoveAmt );
	}
}

void ioElevator::UpdateMountedWeaponList()
{
	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( pStage )
	{
		pStage->UpdateMountedWeaponListByBox( this, m_vMountWeaponList );
	}
}

void ioElevator::TranslateMountWeapon( const D3DXVECTOR3 &vMoveAmt )
{
	if( m_vMountWeaponList.empty() )
		return;

	const ioOrientBox &rkBox = GetWorldCollisionBox();
	float fNewHeight = rkBox.GetCenter().y + rkBox.GetExtents( 1 );

	D3DXVECTOR3 vNewPos;
	MountedWeaponList::iterator iter = m_vMountWeaponList.begin();
	for( ; iter!=m_vMountWeaponList.end() ; ++iter )
	{
		ioWeapon *pWeapon = *iter;

		vNewPos = pWeapon->GetPosition();
		vNewPos.x += vMoveAmt.x;
		vNewPos.y  = fNewHeight;
		vNewPos.x += vMoveAmt.z;
		pWeapon->SetPosition( vNewPos );
	}
}

void ioElevator::EnableWorldFunction( bool bEnable )
{
	m_bEnableFunction = bEnable;

	if( m_bEnableFunction && m_State == ES_STOP )
	{
		m_dwMoveNextStartTime = FRAMEGETTIME() + m_dwWaitTime + m_dwFirstWaitTime;
		m_State = ES_BOTTOM;
	}
}

void ioElevator::OnElevatorChanged( SP2Packet &rkPacket )
{
	int iState = 0;
	rkPacket >> iState;

	// 아직 준비중인 상태였다면 아래쪽 메시지 일때부터 받자 ( 위에 오브젝트 떨어질까봐 )
	if( m_State == ES_STOP && iState != ES_BOTTOM )
		return;

	m_State = (ElevatorState)iState;

	if( m_State == ES_BOTTOM )
		SetWorldPosition( m_vStartPos );
	else
		SetWorldPosition( m_vEndPos );

	DWORD dwNextStartGap = 0;
	rkPacket >> dwNextStartGap;

	DWORD dwGapTime = 0;
	rkPacket >> dwGapTime;

	DWORD dwDelayTime = g_GlobalTimer.CalcGlobalGapTime( dwGapTime );
	m_dwMoveNextStartTime = ( FRAMEGETTIME() + dwNextStartGap ) - dwDelayTime;
}

void ioElevator::UpdateAfter( float fFrameGap )
{
	if( m_pGroup )
	{
		m_pGroup->Update( fFrameGap );
	}

	ioPlayEntity::UpdateAfter( fFrameGap );
}

bool ioElevator::IsMountAvailable() const
{
	return true;
}

bool ioElevator::IsMountingAvailable() const
{
	return false;
}

bool ioElevator::IsNowMountEnable() const
{
	return true;
}

void ioElevator::Translate( const D3DXVECTOR3 &vMove )
{
	UpdateMountedEntityList();
	TranslateMountEntity( vMove );

	UpdateMountedWeaponList();
	TranslateMountWeapon( vMove );

	ioPlayEntity::Translate( vMove );
}

void ioElevator::UpdateBottomHeight() const
{
	m_fCurBottomHeight = GetWorldPosition().y;
}

ApplyWeaponExp ioElevator::ApplyWeapon( ioWeapon *pWeapon )
{
	bool bContactType = false;
	if( pWeapon->GetType() == ioWeapon::WT_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_RUN_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_CONTACT2 ||
		pWeapon->GetType() == ioWeapon::WT_CONTACT3 )
		bContactType = true;

	if( pWeapon )
	{
		ioBaseChar *pAttacker = pWeapon->GetOwner();

		int iDummyIndex = pWeapon->GetWeaponByDummyChar();
		g_DummyCharMgr.CheckEntityCollision( pAttacker, this, iDummyIndex, false );
	}

	if( !pWeapon || bContactType )
		return AWE_NO_EXPLOSION;

	if( pWeapon->HasPeculiarity( WP_MISSILE_WEAPON ) )
		return AWE_MISSILE_EXPLOSION;

	return AWE_EXPLOSION;
}

bool ioElevator::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
{
	if( pWeapon->IsAlreadyCollisioned( this ) )
		return false;

	if( ToBoundBombWeapon( pWeapon ) )
		return false;

	return true;
}

ioPlayEntity::PlaySubType ioElevator::GetSubType() const
{
	return PST_ELEVATOR;
}

ArmorType ioElevator::GetArmorType() const
{
	return AT_UNARMOR;
}

int ioElevator::DontMoveEntityLevel() const
{
	return 6;
}

void ioElevator::FillCurElevatorInfo( SP2Packet &rkPacket )
{
	DWORD dwCurTime = FRAMEGETTIME();

	rkPacket << GetCreateIndex();
	rkPacket << (int)m_State;
	rkPacket << GetWorldPosition();
	rkPacket << m_dwMoveNextStartTime - dwCurTime;
	rkPacket << g_GlobalTimer.GetGlobalTime();
}

void ioElevator::ApplyCurElevatorInfo( SP2Packet &rkPacket )
{
	int iState;
	D3DXVECTOR3 vCurPos;
	
	rkPacket >> iState;
	m_State = (ElevatorState)iState;

	rkPacket >> vCurPos;

	if( m_State == ES_BOTTOM )
		SetWorldPosition( m_vStartPos );
	else if( m_State == ES_TOP )
		SetWorldPosition( m_vEndPos );
	else
		SetWorldPosition( vCurPos );

	DWORD dwNextStartGap = 0;
	rkPacket >> dwNextStartGap;

	DWORD dwGapTime = 0;
	rkPacket >> dwGapTime;

	DWORD dwDelayTime = g_GlobalTimer.CalcGlobalGapTime( dwGapTime );
	m_dwMoveNextStartTime = ( FRAMEGETTIME() + dwNextStartGap ) - dwDelayTime;
}