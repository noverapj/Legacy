

#include "stdafx.h"

#include "ioControlWeaponBuff2.h"

#include "WeaponDefine.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ioControlWeaponBuff2::ioControlWeaponBuff2()
{
}

ioControlWeaponBuff2::ioControlWeaponBuff2( const ioControlWeaponBuff2 &rhs )
	: ioBuff( rhs ),
	m_bDisableDownState( rhs.m_bDisableDownState ),
	m_bUseWeaponEquipMesh( rhs.m_bUseWeaponEquipMesh ),
	m_bEscapeCatchState( rhs.m_bEscapeCatchState ),
	m_fSearchAngle( rhs.m_fSearchAngle ),
	m_fSearchRange( rhs.m_fSearchRange ),
	m_TargetWoundType( rhs.m_TargetWoundType ),
	m_dwFireGapTime( rhs.m_dwFireGapTime ),
	m_nTargetMaxCount( rhs.m_nTargetMaxCount ),
	m_fTargetUpHeight( rhs.m_fTargetUpHeight ),
	m_fTargetDownHeight( rhs.m_fTargetDownHeight ),
	m_WeaponInfo( rhs.m_WeaponInfo ),
	m_nTargetFindMode( rhs.m_nTargetFindMode )
{
}

ioControlWeaponBuff2::~ioControlWeaponBuff2()
{
}

void ioControlWeaponBuff2::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bUseWeaponEquipMesh = rkLoader.LoadBool_e( "use_weapon_equip_mesh", false );
	m_bEscapeCatchState = rkLoader.LoadBool_e( "escape_catch_state", false );

	m_fSearchAngle = rkLoader.LoadFloat_e( "search_angle", 0.0f );
	m_fSearchRange = rkLoader.LoadFloat_e( "search_range", 0.0f );

	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_dwFireGapTime = (DWORD)rkLoader.LoadInt_e( "fire_gap_time", 0 );

	m_nTargetMaxCount = (DWORD)rkLoader.LoadInt_e( "max_target_cnt", 1 );

	m_fTargetUpHeight = rkLoader.LoadFloat_e( "search_cap_height", 0.f );
	m_fTargetDownHeight = rkLoader.LoadFloat_e( "search_cap_down", 0.f );

	char szBuf[MAX_PATH] = "";
	m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "attack_index", NO_WEAPON_ATTRIBUTE );
	m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "attack_index_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString_e( "attack_wounded", "", szBuf, MAX_PATH );
	m_WeaponInfo.m_WoundedAnimation = szBuf;
	m_WeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "attack_wounded_time", 0 );
	m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "attack_wounded_loop_ani", false );

	m_nTargetFindMode = rkLoader.LoadInt_e( "find_target_mode", 0 );
}

ioBuff* ioControlWeaponBuff2::Clone()
{
	return new ioControlWeaponBuff2( *this );
}

void ioControlWeaponBuff2::StartBuff( ioBaseChar *pOwner )
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

bool ioControlWeaponBuff2::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
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

void ioControlWeaponBuff2::ProcessBuff( float fTimePerSec )
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
	SetReserveEndBuff();
}

bool ioControlWeaponBuff2::CheckOwnerStateCheck()
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

bool ioControlWeaponBuff2::CheckRestoreStateCheck()
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

void ioControlWeaponBuff2::EndBuff()
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

void ioControlWeaponBuff2::FindTarget()
{
	if( !m_pOwner )	return;

	ioPlayStage *pStage = m_pOwner->GetCreator();
	if( !pStage )	return;

	ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
	if( !pCreator )
	{
		SetReserveEndBuff();
		return;
	}

	m_vTargetList.clear();

	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	D3DXQUATERNION qtRot = m_pOwner->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vCurPos = m_pOwner->GetWorldPosition();

	float fPreDistSq = m_fSearchRange * m_fSearchRange;
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	float fCurAngle = ioMath::ArrangeHead( m_fSearchAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fSearchRange * m_fSearchRange;

	int nTargetCnt = 0;
	ioHashString szTarget;

	BaseCharDistanceSq vTargetInfo;
	BaseCharDistanceSqList	m_vTargetListTemp;

	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		/*if( nTargetCnt >= m_nTargetMaxCount )
			break;*/

		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pCreator->GetTeam() ) 
			continue;

		if( pTarget->GetCharName() == m_pOwner->GetCharName() )
			continue;

		if( !IsEnableTargetState( pTarget ) )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = m_pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fTargetDownHeight )
			continue;

		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
		{
			/*if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
			else
				continue;*/

			vTargetInfo.m_Name = pTarget->GetCharName();
			vTargetInfo.m_fDistanceSq = fDiffRangeSq;
			m_vTargetListTemp.push_back( vTargetInfo );

			/*szTarget = pTarget->GetCharName();
			m_vTargetList.push_back(szTarget);
			nTargetCnt++;*/
		}
	}

	sort( m_vTargetListTemp.begin(), m_vTargetListTemp.end(), BaseCharDistanceSqSort() );
	if( m_vTargetListTemp.size() > 0 )
	{
		for( int i = 0; i < (int)m_vTargetListTemp.size() && i < m_nTargetMaxCount; i++ )
		{
			int nIndex = m_vTargetListTemp.size() - i - 1;
			ioHashString stTargetName = m_vTargetListTemp[nIndex].m_Name;
			m_vTargetList.push_back( stTargetName );
		}
	}

	//sort( m_vTargetList.begin(), m_vTargetList.end(), BaseCharDistanceSqSort() );
	//reverse( m_vTargetList.begin(), m_vTargetList.end() );
}

bool ioControlWeaponBuff2::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;
		
	if( !pTarget->IsEnableTarget() ) 
		return false;

	// 쓰러짐 관련 체크
	if( m_nTargetFindMode == 0 )
	{
		if( !pTarget->CheckTargetWoundType( TWT_DOWN_WOUND_EXCEPT ) )
			return false;
	}

	return true;
}

void ioControlWeaponBuff2::ProcessControlWeapon()
{
	if( !m_pOwner )	return;

	ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
	if( !pCreator )
		return;

	FindTarget();

	int nTargetCnt = m_vTargetList.size();
	DWORD dwWeaponIndex = pCreator->IncreaseWeaponIndexBase();

	for( int i = 0; i < nTargetCnt; i++ )
	{
		ioBaseChar *pTarget = m_pOwner->GetBaseChar( m_vTargetList[i] );
		if( !pTarget )
			continue;

		D3DXVECTOR3 vPos = m_pOwner->GetMidPositionByRate();
		ioWeapon *pWeapon = pCreator->DummyExplicit( m_WeaponInfo, vPos,
			dwWeaponIndex+i, false,
			m_pOwner->GetCharName(), m_Name,
			m_bUseCallAttWeaponCol );

		ioUroborusWeapon5 *pUroborusWeapon5 = ToUroborusWeapon5( pWeapon );
		if( pUroborusWeapon5)
		{
			pUroborusWeapon5->SetPassTargetName( m_pOwner->GetCharName() );

			D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - m_pOwner->GetMidPositionByRate();
			D3DXVec3Normalize( &vDiff, &vDiff );

			pUroborusWeapon5->SetMoveDir( vDiff );
		}
	}

	/*int iTargetCnt = m_vTargetList.size();
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
		if( m_pOwnerItem )
		{
			m_pOwnerItem->ReduceSkillGauge( m_pOwnerItem->GetMaxSkillGauge(m_pOwnerItem->GetName()) );
		}

		SetReserveEndBuff();
	}*/
}