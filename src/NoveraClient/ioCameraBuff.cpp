#include "stdafx.h"

#include "ioCameraBuff.h"

#include "ioBaseChar.h"

ioCameraBuff::ioCameraBuff()
{
}

ioCameraBuff::ioCameraBuff( const ioCameraBuff &rhs )
: ioBuff( rhs ),
 m_fBuffCamDistance( rhs.m_fBuffCamDistance ),
 m_fBuffCamHeight( rhs.m_fBuffCamHeight ),
 m_fBuffCamFov( rhs.m_fBuffCamFov ),
 m_dwBuffCamZoomTime( rhs.m_dwBuffCamZoomTime ),
 m_bPriorityOverSkill( rhs.m_bPriorityOverSkill )
{
}

ioCameraBuff::~ioCameraBuff()
{
}

ioBuff* ioCameraBuff::Clone()
{
	return new ioCameraBuff( *this );
}

void ioCameraBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fBuffCamDistance = rkLoader.LoadFloat_e( "camera_distance", 0.0f );
	m_fBuffCamHeight = rkLoader.LoadFloat_e( "camera_height", 0.0f );
	m_fBuffCamFov = rkLoader.LoadFloat_e( "camera_fov", 0.0f );
	m_dwBuffCamZoomTime = rkLoader.LoadInt_e( "camera_zoom_time", 0 );

	m_bPriorityOverSkill = rkLoader.LoadBool_e( "priority_over_skill", false );
}

void ioCameraBuff::StartBuff( ioBaseChar *pOwner )
{
	pOwner->SetBuffCamera( m_Name, m_iCameraType, m_fBuffCamDistance, m_fBuffCamHeight, m_fBuffCamFov, m_dwBuffCamZoomTime, m_bPriorityOverSkill );
	//pOwner->SetCameraTypeByBuff( m_iCameraType );

	ioBuff::StartBuff( pOwner );
}

bool ioCameraBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	m_pOwner = pOwner;
	
	DWORD dwGlobalTimeGap = g_GlobalTimer.CalcGlobalGapTime( dwGlobalTime );
	if( m_iOperationType != OT_SWITCH )
	{
		if( dwGlobalTimeGap > dwGapTime )
			return false;

		dwGapTime -= dwGlobalTimeGap;
		m_dwBuffEndTime = FRAMEGETTIME() + dwGapTime;
	}

	m_dwCheckTime = FRAMEGETTIME();
	m_bLive = true;

	if( m_CreatorLookType != CLT_NONE && !m_CreateChar.IsEmpty() )
	{
		if( m_CreateChar != m_pOwner->GetCharName() )
			m_pOwner->RotateByBuffCollision( m_CreateChar, m_CreatorLookType );
	}
	
	m_pOwner->SetBuffCamera( m_Name, m_iCameraType, m_fBuffCamDistance, m_fBuffCamHeight, m_fBuffCamFov, m_dwBuffCamZoomTime, m_bPriorityOverSkill );
	//m_pOwner->SetCameraTypeByBuff( m_iCameraType );
	
	m_pOwner->AttachEffect( m_BuffEffect, GetEffectEventOwner() );

	if( !m_LoopSound.IsEmpty() )
	{
		m_LoopSoundID = m_pOwner->PlaySound( m_LoopSound, true );
		m_bLoopSoundPlayed = true;
	}

	CheckInvisibleWeapon( false );
	CheckInvisibleCloak( false );
	CheckInvisibleHelmet( false );
	CheckChangeSkill( true );
	CheckChangeColor( true );
	CheckBuffDesc();

	return true;
}

void ioCameraBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		if( CheckEnableDelayState() )
			return;
		break;
	}
}

void ioCameraBuff::EndBuff()
{
	m_pOwner->SetBuffCameraEnd( m_Name );
	//m_pOwner->SetCameraTypeByBuff( CT_DEFAULT );
	ioBuff::EndBuff();
}