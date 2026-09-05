

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioPullBuff.h"

ioPullBuff::ioPullBuff()
{
	m_dwTargetAreaWeapon = 0;
	m_vTargetAreaWeaponPos = ioMath::VEC3_ZERO;
}

ioPullBuff::ioPullBuff( const ioPullBuff &rhs )
: ioBuff( rhs ),
m_bDisableDownState( rhs.m_bDisableDownState ),
m_bUseWeaponEquipMesh( rhs.m_bUseWeaponEquipMesh ),
m_PullType( rhs.m_PullType ),
m_fPullSpeed( rhs.m_fPullSpeed ),
m_fPullSpeedRateForMaxRange( rhs.m_fPullSpeedRateForMaxRange ),
m_fPullSpeedRateForMinRange( rhs.m_fPullSpeedRateForMinRange ),
m_fPullMaxRange( rhs.m_fPullMaxRange ),
m_fPullMinRange( rhs.m_fPullMinRange )
{
	m_dwTargetAreaWeapon = 0;
	m_vTargetAreaWeaponPos = ioMath::VEC3_ZERO;
}

ioPullBuff::~ioPullBuff()
{
}

void ioPullBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bUseWeaponEquipMesh = rkLoader.LoadBool_e( "use_weapon_equip_mesh", false );

	m_PullType = (PullType)rkLoader.LoadInt_e( "pull_type", PT_NORMAL );
	m_fPullSpeed = rkLoader.LoadFloat_e( "pull_speed", 0.0f );
	m_fPullSpeedRateForMaxRange = rkLoader.LoadFloat_e( "pull_speed_rate_for_max_range", FLOAT1 );
	m_fPullSpeedRateForMinRange = rkLoader.LoadFloat_e( "pull_speed_rate_for_min_range", FLOAT1 );
	m_fPullMaxRange = rkLoader.LoadFloat_e( "pull_max_range", 0.0f );
	m_fPullMinRange = rkLoader.LoadFloat_e( "pull_min_range", 0.0f );
}

ioBuff* ioPullBuff::Clone()
{
	return new ioPullBuff( *this );
}

void ioPullBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}
}

bool ioPullBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}

	return true;
}

void ioPullBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckOwnerStateCheck() )
		return;

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			if( GetOwner() && CheckRestoreStateCheck() )
				GetOwner()->SetState( CS_DELAY, m_bSendDelayNetWork );

			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		{
			if( CheckEnableDelayState() )
				return;

			CheckCallingProcess();

			if( GetBuffDuration() == 0 )
				return;

			if( m_dwBuffEndTime <= FRAMEGETTIME() )
			{
				if( CheckRestoreStateCheck() )
				{
					m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
				}

				m_pOwner->CheckUseBuffSkill( m_Name, false );
				SetReserveEndBuff();
				return;
			}

			ProcessPullTarget( fTimePerSec );
		}
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

bool ioPullBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	bool bDown = false;
	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			bDown = true;
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			bDown = true;
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			bDown = true;
		else if( eState == CS_DROP_ZONE_DOWN )
			bDown = true;
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			bDown = true;

		if( bDown )
		{
			if( CheckRestoreStateCheck() )
			{
				m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
			}
 
			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return true;
		}
	}

	return false;
}

bool ioPullBuff::CheckRestoreStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	switch( eState )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		return true;
	}

	return false;
}

void ioPullBuff::EndBuff()
{
	if( m_bUseWeaponEquipMesh && m_pOwner )
	{
		m_pOwner->SetChangeEquipMesh( m_ItemName, false );
	}

	ioBuff::EndBuff();
}

void ioPullBuff::ProcessPullTarget( float fTimePerSec )
{
	if( !m_pOwner )
		return;

	D3DXVECTOR3 vCurAreaPos;

	switch( m_PullType )
	{
	case PT_NORMAL:
		{
			ioBaseChar *pCreator = m_pOwner->GetBaseChar(GetCreatorName());
			if( !pCreator )
			{
				SetReserveEndBuff();
				return;
			}
			
			vCurAreaPos = pCreator->GetWorldPosition();
		}
		break;
	case PT_AREA_RANGE:
		{
			ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwTargetAreaWeapon );
			if( pAreaWeapon )
			{
				vCurAreaPos = pAreaWeapon->GetStartPos();
				m_vTargetAreaWeaponPos = vCurAreaPos;
			}
			else
			{
				SetReserveEndBuff();
				return;
			}
		}
		break;
	}	

	D3DXVECTOR3 vTargetDir = vCurAreaPos - m_pOwner->GetWorldPosition();

	float fLength = D3DXVec3Length( &vTargetDir );
	if( fLength >= m_fPullMaxRange )
	{
		SetReserveEndBuff();
		return;
	}

	if( m_fPullMinRange > 0.0f && fLength <= m_fPullMinRange )
	{
		return;
	}

	float fLengthRate = fLength / m_fPullMaxRange;
	float fSpeedRate = ( 1-fLengthRate ) * m_fPullSpeedRateForMinRange + ( fLengthRate*m_fPullSpeedRateForMaxRange );

	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3	vXZDir( vTargetDir.x, 0.0f, vTargetDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );
	
	D3DXVECTOR3 vPos = m_pOwner->GetWorldPosition();
	float fPullValue = m_fPullSpeed * fTimePerSec * fSpeedRate;
	vPos += ( vXZDir * fPullValue );
	m_pOwner->SetWorldPosition( vPos );
}

void ioPullBuff::SetTargetAreaWeapon( DWORD dwIndex )
{
	m_dwTargetAreaWeapon = dwIndex;
}