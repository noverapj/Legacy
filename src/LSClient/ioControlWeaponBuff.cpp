
#include "stdafx.h"

#include "ioControlWeaponBuff.h"

#include "WeaponDefine.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ioControlWeaponBuff::ioControlWeaponBuff()
{
}

ioControlWeaponBuff::ioControlWeaponBuff( const ioControlWeaponBuff &rhs )
: ioBuff( rhs ),
m_bDisableDownState( rhs.m_bDisableDownState ),
m_bUseWeaponEquipMesh( rhs.m_bUseWeaponEquipMesh ),
m_bEscapeCatchState( rhs.m_bEscapeCatchState ),
m_fSearchAngle( rhs.m_fSearchAngle ),
m_fSearchRange( rhs.m_fSearchRange ),
m_TargetWoundType( rhs.m_TargetWoundType ),
m_dwFireGapTime( rhs.m_dwFireGapTime )
{
}

ioControlWeaponBuff::~ioControlWeaponBuff()
{
}

void ioControlWeaponBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bUseWeaponEquipMesh = rkLoader.LoadBool_e( "use_weapon_equip_mesh", false );
	m_bEscapeCatchState = rkLoader.LoadBool_e( "escape_catch_state", false );

	m_fSearchAngle = rkLoader.LoadFloat_e( "search_angle", 0.0f );
	m_fSearchRange = rkLoader.LoadFloat_e( "search_range", 0.0f );

	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_dwFireGapTime = (DWORD)rkLoader.LoadInt_e( "fire_gap_time", 0 );
}

ioBuff* ioControlWeaponBuff::Clone()
{
	return new ioControlWeaponBuff( *this );
}

void ioControlWeaponBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}

	if( m_bEscapeCatchState && pOwner && pOwner->IsCatchMode() )
	{
		pOwner->SetEscapeCatchState();
	}
	m_iMaxFireCnt = m_CallAttributeList.size();
	m_iCurFireCnt = 0;
}

bool ioControlWeaponBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}

	if( m_bEscapeCatchState && pOwner && pOwner->IsCatchMode() )
	{
		pOwner->SetEscapeCatchState();
	}

	return true;
}

void ioControlWeaponBuff::ProcessBuff( float fTimePerSec )
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
		}
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	ProcessControlWeapon();
}

bool ioControlWeaponBuff::CheckOwnerStateCheck()
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

			int iSize = m_vWeaponIndexList.size();
			for( int i=0; i < iSize; ++i )
			{
				int iWeaponIndex = m_vWeaponIndexList[i];

				ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( m_pOwner, iWeaponIndex );
				if( pWeapon && pWeapon->IsLive() )
				{
					pWeapon->SetWeaponDead();
				}
			}

			return true;
		}
	}

	return false;
}

bool ioControlWeaponBuff::CheckRestoreStateCheck()
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

void ioControlWeaponBuff::EndBuff()
{
	if( m_bUseWeaponEquipMesh && m_pOwner )
	{
		m_pOwner->SetChangeEquipMesh( m_ItemName, false );
	}

	int iSize = m_vWeaponIndexList.size();
	for( int i=0; i < iSize; ++i )
	{
		int iWeaponIndex = m_vWeaponIndexList[i];

		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( m_pOwner, iWeaponIndex );
		if( pWeapon && pWeapon->IsLive() )
		{
			pWeapon->SetWeaponDead();
		}
	}

	ioBuff::EndBuff();
}

void ioControlWeaponBuff::FindTarget()
{
	if( !m_pOwner )	return;

	ioPlayStage *pStage = m_pOwner->GetCreator();
	if( !pStage )	return;

	m_vTargetList.clear();

	D3DXQUATERNION qtRot = m_pOwner->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	float fPreDistSq = m_fSearchRange * m_fSearchRange;
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pCurChar = *iter;
		if( !pCurChar )	continue;

		if( pCurChar == m_pOwner )
			continue;

		if( pCurChar->GetTeam() == m_pOwner->GetTeam() )
			continue;

		if( !IsEnableTargetState( pCurChar ) )
			continue;

		D3DXVECTOR3 vDiff = pCurChar->GetMidPositionByRate() - m_pOwner->GetMidPositionByRate();
		float fLengthSq = D3DXVec3LengthSq( &vDiff );

		if( fLengthSq <= fPreDistSq )
		{
			D3DXVec3Normalize( &vDiff, &vDiff );
			float fValue = D3DXVec3Dot( &vDir, &vDiff );
			float fAngleGap = RADtoDEG( acosf(fValue) );
			if( fAngleGap < m_fSearchAngle * FLOAT05 )
			{
				BaseCharDistanceSq vTargetInfo;
				vTargetInfo.m_Name = pCurChar->GetCharName();
				vTargetInfo.m_fDistanceSq = fLengthSq;
				m_vTargetList.push_back( vTargetInfo );
			}
		}
	}

	sort( m_vTargetList.begin(), m_vTargetList.end(), BaseCharDistanceSqSort() );
	reverse( m_vTargetList.begin(), m_vTargetList.end() );
}

bool ioControlWeaponBuff::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;
		
	if( !pTarget->IsEnableTarget() ) 
		return false;

	if( m_TargetWoundType == TWT_AIR_WOUND_EXCEPT ) // 쓰러짐중 공중상태는 패스
	{
		if( pTarget->IsApplyDownState() && !pTarget->IsApplyDownState(false) )
			return false;
	}
	else if( m_TargetWoundType == TWT_DOWN_WOUND_EXCEPT ) // 쓰러짐중 바닥상태는 패스
	{
		if( pTarget->IsApplyDownState(false) )
			return false;
	}
	else if( m_TargetWoundType == TWT_ALL_WOUND_EXCEPT )
	{
		if( pTarget->IsApplyDownState() ||
			( pTarget->GetState() == CS_GRAPPLING_WOUNDED ||
			pTarget->GetState() == CS_GRAPPLING_PUSHED ||
			pTarget->GetState() == CS_GRAPPLING_WOUNDED_SKILL ||
			pTarget->GetState() == CS_GRAPPLING_WOUNDED_SWING ) )
		{
			return false;
		}
		else if( pTarget->IsApplyDownState() )
		{
			return false;
		}
	}
	else if( m_TargetWoundType == TWT_FLOATING_STATE )	// 공중에 뜬 상태가 아니면 패스
	{
		if( !pTarget->IsFloating() )
			return false;
	}
	else if( m_TargetWoundType == TWT_DOWN_WOUNDED ) //바닥에 쓰러져 있지 않거나 공중에 뜬 상태면 패스
	{
		if( !pTarget->IsApplyDownState() || pTarget->IsFloating() )
			return false;
	}
    
	return true;
}

void ioControlWeaponBuff::ProcessControlWeapon()
{
	if( !m_pOwner )	return;

	FindTarget();

	int iTargetCnt = m_vTargetList.size();
	for( int i=0; i<iTargetCnt; ++i )
	{
		if( m_vWeaponIndexList.empty() )
			continue;

		int iWeaponIndex = m_vWeaponIndexList[0];

		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( m_pOwner, iWeaponIndex );
		ioTraceWeapon5 *pTrace = ToTraceWeapon5(pWeapon);

		if( pTrace && pTrace->IsLive() )
		{
			PreTargetInfo kInfo;
			kInfo.m_Name = m_vTargetList[i].m_Name;
			kInfo.m_dwNextFireTime = FRAMEGETTIME() + m_dwFireGapTime;

			bool bFire = true;
			PreTargetInfoList::iterator iter = m_vPreTargetList.begin();
			while( iter != m_vPreTargetList.end() )
			{
				if( iter->m_Name == kInfo.m_Name )
				{
					if( iter->m_dwNextFireTime > FRAMEGETTIME() )
					{
						bFire = false;
						++iter;
					}
					else if( iter->m_dwNextFireTime <= FRAMEGETTIME() )
					{
						m_vPreTargetList.erase( iter );
					}
				}
				else
					++iter;
			}

			if( bFire )
			{
				pTrace->SetTraceTarget( m_vTargetList[i].m_Name );
				m_vWeaponIndexList.erase( m_vWeaponIndexList.begin() );
				m_vPreTargetList.push_back(kInfo);
				m_iCurFireCnt++;
			}
		}
	}

	if( m_vWeaponIndexList.empty() && m_iCurFireCnt >= m_iMaxFireCnt-1 )
	{
		ioItem *pOwnerItem = m_pOwner->GetEquipedItem( m_ItemName );

		/*if( m_pOwnerItem )
		{
			m_pOwnerItem->ReduceSkillGauge( m_pOwnerItem->GetMaxSkillGauge(m_pOwnerItem->GetName()) );
		}*/

		if( pOwnerItem )
		{
			pOwnerItem->ReduceSkillGauge( pOwnerItem->GetMaxSkillGauge(pOwnerItem->GetName()) );
		}
			
		SetReserveEndBuff();
	}
}