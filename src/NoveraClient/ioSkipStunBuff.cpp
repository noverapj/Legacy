#include "stdafx.h"

#include "ioSkipStunBuff.h"

ioSkipStunBuff::ioSkipStunBuff()
{
}

ioSkipStunBuff::ioSkipStunBuff( const ioSkipStunBuff &rhs )
: ioBuff( rhs ),
m_StunAnimation( rhs.m_StunAnimation ),
m_CatchAnimation( rhs.m_CatchAnimation ),
m_WeaponAttachType( rhs.m_WeaponAttachType ),
m_WeaponDeadBuffList( rhs.m_WeaponDeadBuffList ),
m_WeaponDeadByMapCollBuffList( rhs.m_WeaponDeadByMapCollBuffList ),
m_bLiveOnWeapon( rhs.m_bLiveOnWeapon ),
m_dwCheckDeadWeaponAttrIndex( rhs.m_dwCheckDeadWeaponAttrIndex ),
m_SkillName( rhs.m_SkillName )
{
}

ioSkipStunBuff::~ioSkipStunBuff()
{
}

void ioSkipStunBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	rkLoader.LoadString_e( "stun_animation", "", szBuf, MAX_PATH );
	m_StunAnimation = szBuf;

	rkLoader.LoadString_e( "stun_catch_animation", "", szBuf, MAX_PATH );
	m_CatchAnimation = szBuf;

	m_WeaponAttachType = (WeaponAttachType)rkLoader.LoadInt_e( "weapon_attach_type", WAT_NONE );

	m_WeaponDeadBuffList.clear();
	int iCnt = rkLoader.LoadInt_e( "weapon_dead_buff_cnt", 0 );
	for( int i = 0; i < iCnt; ++i )
	{
		wsprintf_e( szKey, "weapon_dead_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_WeaponDeadBuffList.push_back( szBuf );
	}

	m_WeaponDeadByMapCollBuffList.clear();
	iCnt = rkLoader.LoadInt_e( "weapon_dead_by_map_call_buff_cnt", 0 );
	for( int i = 0; i < iCnt; ++i )
	{
		wsprintf_e( szKey, "weapon_dead_by_map_call_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_WeaponDeadByMapCollBuffList.push_back( szBuf );
	}

	m_bLiveOnWeapon					= rkLoader.LoadBool_e( "live_on_weapon", false );
	m_dwCheckDeadWeaponAttrIndex	= rkLoader.LoadInt_e( "check_weapon_index", 0 );

	rkLoader.LoadString_e( "skill_name", "", szBuf, MAX_PATH );
	m_SkillName = szBuf;
}

ioBuff* ioSkipStunBuff::Clone()
{
	return new ioSkipStunBuff( *this );
}

void ioSkipStunBuff::StartBuff( ioBaseChar *pOwner )
{
#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		LOG.PrintTimeAndLog( 0, "BuffCheck - %s, StartTime: %d", m_Name.c_str(), FRAMEGETTIME() );
	}
#endif

	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_pOwner = pOwner;
	m_OwnerName = pOwner->GetCharName();

	m_dwBuffStartTime = dwCurTime;
	if( m_iOperationType == OT_DELAY )
		m_dwBuffStartTime += m_dwDelayDuration;

	m_dwBuffEndTime = m_dwBuffStartTime + GetBuffDuration();
	m_dwCheckTime = dwCurTime;
	
	m_bLive = true;

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}
}

bool ioSkipStunBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	m_pOwner = pOwner;
	DWORD dwGlobalTimeGap = g_GlobalTimer.CalcGlobalGapTime( dwGlobalTime );

	if( m_iOperationType != OT_SWITCH )
	{
		if( dwGlobalTimeGap > dwGapTime )
			return false;

		dwGapTime -= dwGlobalTimeGap;
		m_dwBuffEndTime = FRAMEGETTIME() + dwGapTime;
		m_dwBuffStartTime = m_dwBuffEndTime - GetBuffDuration();
	}

	m_dwCheckTime = FRAMEGETTIME();
	m_bLive = true;

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}

	return true;
}

void ioSkipStunBuff::ProcessBuff( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	CheckWeaponDieCheck();

	if( m_iOperationType == OT_DELAY )
	{
		if( m_bBuffStart )
		{
			if( pOwner->GetState() != CS_SKIP_STUN ||
				pOwner->GetCurSkipStunBuffName() != GetName() )
			{
				SetReserveEndBuff();
				return;
			}
		}
	}
	else
	{
		if( pOwner->GetState() != CS_SKIP_STUN ||
			pOwner->GetCurSkipStunBuffName() != GetName() )
		{
			SetReserveEndBuff();
			return;
		}
	}

	if( !CheckWeaponAttach() )
	{
		SetReserveEndBuff();
		return;
	}

	if( m_bLiveOnWeapon )
	{
		ioBaseChar *pCreator = pOwner->GetBaseChar( m_CreateChar );
		if( !pCreator )
		{
			SetReserveEndBuff();
			return;
		}

		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pCreator, m_dwCreateWeaponIndex );
		if( !pWeapon )
		{
			SetReserveEndBuff();
			return;
		}
	}

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
		ProcessDelay( fTimePerSec );
		break;
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

void ioSkipStunBuff::EndBuff()
{
	ioBuff::EndBuff();

	if( m_pOwner->GetState() == CS_SKIP_STUN && m_pOwner->GetCurSkipStunBuffName() == GetName() )
	{
		m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
		m_pOwner->SetDashFailStart();
	}
}

void ioSkipStunBuff::SetAction()
{
	CheckInvisibleWeapon( false );
	CheckInvisibleCloak( false );
	CheckInvisibleHelmet( false );

	CheckChangeSkill( true );
	CheckChangeColor( true );
	CheckBuffDesc();

	if( m_CreatorLookType != CLT_NONE && !m_CreateChar.IsEmpty() )
	{
		if( m_CreateChar != m_pOwner->GetCharName() )
			m_pOwner->RotateByBuffCollision( m_CreateChar, m_CreatorLookType );
	}

	m_pOwner->AttachEffect( m_BuffEffect, GetEffectEventOwner() );

	if( !m_LoopSound.IsEmpty() )
	{
		m_LoopSoundID = m_pOwner->PlaySound( m_LoopSound, true );
		m_bLoopSoundPlayed = true;
	}

	if( m_pOwner->GetState() != CS_DIE &&
		m_pOwner->GetState() != CS_LOADING &&
		m_pOwner->GetState() != CS_OBSERVER &&
		m_pOwner->GetState() != CS_VIEW )
	{
		m_pOwner->SetSkipStunState( GetName() );

		ioEntityGroup *pGrp = m_pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		if( !m_pOwner->IsPrisonerMode() && !m_pOwner->IsCatchMode() )
			pGrp->SetLoopAni( m_StunAnimation, FLOAT100 );
		else
			pGrp->SetLoopAni( m_CatchAnimation, FLOAT100 );
	}

	m_bBuffStart = true;
}

void ioSkipStunBuff::ProcessDelay( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( CheckEnableDelayState() )
		return;

	if( GetBuffDuration() == 0 )
	{
		if( !m_bBuffStart && m_dwBuffStartTime < dwCurTime )
			SetAction();
		return;
	}

	if( !m_bBuffStart && m_dwBuffStartTime < dwCurTime )
		SetAction();

	if( m_dwBuffEndTime <= dwCurTime )
	{
		SetReserveEndBuff();
		return;
	}

	CheckCallingProcess();
}

bool ioSkipStunBuff::CheckWeaponAttach()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	if( m_WeaponAttachType != WAT_XZ && m_WeaponAttachType != WAT_ALL && 
		m_WeaponAttachType != WAT_ALL_BY_WEAPON && m_WeaponAttachType != WAT_ALL_BY_WEAPON2 )
		return true;

	if( m_dwCreateWeaponIndex == 0 )
		return true;

	ioBaseChar *pCreator = pOwner->GetBaseChar( m_CreateChar );
	if( !pCreator )
		return false;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pCreator, m_dwCreateWeaponIndex );
	if( !pWeapon || !pWeapon->IsLive() )
	{
		WeaponEventType eType = g_WeaponMgr.GetWeaponEvent( m_dwCreateWeaponIndex );
		switch( eType )
		{
		case WET_MAP_COLL:
			SetWeaponDeadCancelByMap();
			break;
		default:
			SetWeaponDeadCancel();
			break;
		}
		return false;
	}
	else
	{
		WeaponEventType eType = g_WeaponMgr.GetWeaponEvent( m_dwCreateWeaponIndex );
		if( eType == WET_MAP_COLL_NO_DEAD )
		{
			SetWeaponDeadCancelByMap();
			return false;
		}
	}

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( !pStage )
		return false;	

	switch( m_WeaponAttachType )
	{
	case WAT_XZ:
		{
			D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
			D3DXVECTOR3 vNewPos = vPos;
			vNewPos.x = pWeapon->GetPosition().x;
			vNewPos.z = pWeapon->GetPosition().z;

			D3DXVECTOR3 vDir = pWeapon->GetMoveDir();
			vDir.y = 0.0f;
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXQUATERNION qtRot;
			ioMath::CalcDirectionQuaternion( qtRot, -vDir );
			pOwner->SetTargetRotToRotate( qtRot, true );

			D3DXVECTOR3 vMove = vNewPos - vPos;

			bool bCol = false;
			if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMove, bCol ) )
			{
				pOwner->CheckAxisLimit( &vNewPos );
				pOwner->SetWorldPosition( vNewPos );
			}
		}
		return true;
	case WAT_ALL:
		{
			float fGap = pOwner->GetMidPositionByRate().y - pOwner->GetWorldPosition().y;
			
			D3DXVECTOR3 vNewPos = pWeapon->GetPosition();
			vNewPos.y = vNewPos.y-fGap;

			pOwner->SetWorldPosition( vNewPos );

			D3DXVECTOR3 vDir = pWeapon->GetMoveDir();
			vDir.y = 0.0f;
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXQUATERNION qtRot;
			ioMath::CalcDirectionQuaternion( qtRot, -vDir );
			pOwner->SetTargetRotToRotate( qtRot, true );
		}
		return true;
	case WAT_ALL_BY_WEAPON:
		{	
			D3DXVECTOR3 vNewPos = pWeapon->GetPosition();			
			pOwner->SetWorldPosition( vNewPos );
		}
		return true;
	case WAT_ALL_BY_WEAPON2:
		{	
			float fGap = pOwner->GetMidPositionByRate().y - pOwner->GetWorldPosition().y;			
			D3DXVECTOR3 vNewPos = pWeapon->GetPosition();
			vNewPos.y = vNewPos.y-fGap;

			pOwner->SetWorldPosition( vNewPos );
		}
		return true;
	}

	return false;
}

void ioSkipStunBuff::CheckWeaponDieCheck()
{
	if( m_pOwner )
	{
		ioBaseChar* pCrator = m_pOwner->GetBaseChar( m_CreateChar );
		if( pCrator )
		{			
			if( 0 < m_dwCheckDeadWeaponAttrIndex && pCrator->GetCurActiveSkill() == NULL )
			{
				DWORDVec vList;
				g_WeaponMgr.FindWeaponByAttributeIndex( pCrator, m_dwCheckDeadWeaponAttrIndex, vList );
				if( vList.empty() )
				{
					m_CallAttributeList.clear();
					SetReserveEndBuff();
				}
			}
		}
		else
		{
			m_CallAttributeList.clear();
			SetReserveEndBuff();
		}
	}
}

void ioSkipStunBuff::SetWeaponDeadCancel()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	if( m_WeaponDeadBuffList.empty() )
		return;

	for( int i = 0; i < (int)m_WeaponDeadBuffList.size(); ++i )
	{
		pOwner->ReserveAddNewBuff( m_WeaponDeadBuffList[i], m_CreateChar, m_ItemName, NULL );
	}
}

void ioSkipStunBuff::SetWeaponDeadCancelByMap()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	if( m_WeaponDeadByMapCollBuffList.empty() )
		return;
		
	for( int i = 0; i < (int)m_WeaponDeadByMapCollBuffList.size(); ++i )
	{
		pOwner->ReserveAddNewBuff( m_WeaponDeadByMapCollBuffList[i], m_CreateChar, m_ItemName, NULL );
	}
}