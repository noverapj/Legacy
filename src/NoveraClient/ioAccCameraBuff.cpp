#include "StdAfx.h"
#include "ioAccCameraBuff.h"

ioAccCameraBuff::ioAccCameraBuff(void)
{
	m_fBuffCamDistance = 0.0f;
	m_fBuffCamHeight = 0.0f;
}

ioAccCameraBuff::ioAccCameraBuff( const ioAccCameraBuff &rhs ):
ioBuff( rhs ),
m_fBuffCamHeightRate( rhs.m_fBuffCamHeightRate ),
m_fBuffCamFov( rhs.m_fBuffCamFov ),
m_dwBuffCamZoomTime( rhs.m_dwBuffCamZoomTime ),
m_bPriorityOverSkill( rhs.m_bPriorityOverSkill )
{
	m_fBuffCamDistance = 0.0f;
	m_fBuffCamHeight = 0.0f;
}

ioAccCameraBuff::~ioAccCameraBuff(void)
{
}

ioBuff* ioAccCameraBuff::Clone()
{
	return new ioAccCameraBuff( *this );
}

void ioAccCameraBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fBuffCamHeightRate = rkLoader.LoadFloat_e( "camera_height_rate", FLOAT1 );
	m_fBuffCamFov = rkLoader.LoadFloat_e( "camera_fov", 0.0f );
	m_dwBuffCamZoomTime = rkLoader.LoadInt_e( "camera_zoom_time", 0 );

	m_bPriorityOverSkill = rkLoader.LoadBool_e( "priority_over_skill", false );
}

void ioAccCameraBuff::StartBuff( ioBaseChar *pOwner )
{
	ioItem *pItem= GetOwnerItem();
	if ( !pItem )
	{
		SetReserveEndBuff();
		return;
	}

	float fNum = 0.f;
	ioSkill *pSkill = pOwner->GetEquipedSkill( m_SkillName );
	if( pSkill )
		fNum =  pSkill->GetAccessoryRandomNum() / FLOAT1000;

	if ( fNum <= 0.0f )
	{
		SetReserveEndBuff();
		return;
	}
	m_fBuffCamDistance = fNum;
	m_fBuffCamHeight = fNum * m_fBuffCamHeightRate;
	pOwner->SetBuffCamera( m_Name, m_iCameraType, m_fBuffCamDistance, m_fBuffCamHeight, m_fBuffCamFov, m_dwBuffCamZoomTime, m_bPriorityOverSkill );

	ioBuff::StartBuff( pOwner );
}

bool ioAccCameraBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	m_pOwner = pOwner;

	ioItem *pItem= GetOwnerItem();
	if ( !pItem )
	{
		SetReserveEndBuff();
		return false;
	}

	float fNum = 0.f;
	ioSkill *pSkill = pOwner->GetEquipedSkill( m_SkillName );
	if( pSkill )
		fNum =  pSkill->GetAccessoryRandomNum() / FLOAT1000;

	if ( fNum <= 0.0f )
	{
		SetReserveEndBuff();
		return false;
	}
	m_fBuffCamDistance = fNum;
	m_fBuffCamHeight = fNum * m_fBuffCamHeightRate;
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

void ioAccCameraBuff::ProcessBuff( float fTimePerSec )
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

void ioAccCameraBuff::EndBuff()
{
	if ( m_pOwner )
		m_pOwner->SetBuffCameraEnd( m_Name );
	ioBuff::EndBuff();
}
