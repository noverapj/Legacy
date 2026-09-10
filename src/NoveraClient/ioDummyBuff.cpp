

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioDummyBuff.h"

ioDummyBuff::ioDummyBuff()
{
	m_dwWeaponDeadIndex = 0;
	m_iCurHitCount = 0;
}

ioDummyBuff::ioDummyBuff( const ioDummyBuff &rhs )
	: ioBuff( rhs ),
	m_bDisableDownState( rhs.m_bDisableDownState ),
	m_bDisableDropDownState( rhs.m_bDisableDropDownState ),
	m_bUseWeaponEquipMesh( rhs.m_bUseWeaponEquipMesh ),
	m_bEscapeCatchState( rhs.m_bEscapeCatchState ),
	m_bEnableOutLine( rhs.m_bEnableOutLine ),
	m_bDeleteSameBuff( rhs.m_bDeleteSameBuff ),
	m_fOutLineR( rhs.m_fOutLineR ),
	m_fOutLineG( rhs.m_fOutLineG ),
	m_fOutLineB( rhs.m_fOutLineB ),
	m_fOutLineAlpha( rhs.m_fOutLineAlpha ),
	m_fOutLineThickness( rhs.m_fOutLineThickness ),
	m_iMaxHitCount( rhs.m_iMaxHitCount ),
	m_RemoveAreaWeaponList( rhs.m_RemoveAreaWeaponList )
{
	m_dwWeaponDeadIndex = 0;
	m_iCurHitCount = 0;
}

ioDummyBuff::~ioDummyBuff()
{
}

void ioDummyBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bDisableDownState				= rkLoader.LoadBool_e( "disable_down_state", false );
	m_bDisableDropDownState			= rkLoader.LoadBool_e( "disable_drop_down_state", false );
	m_bUseWeaponEquipMesh			= rkLoader.LoadBool_e( "use_weapon_equip_mesh", false );
	m_bEscapeCatchState				= rkLoader.LoadBool_e( "escape_catch_state", false );
	m_bDeleteSameBuff				= rkLoader.LoadBool_e( "delete_same_buff", false );

	m_bEnableOutLine				= rkLoader.LoadBool_e( "outline_enable", false );
	m_fOutLineR						= rkLoader.LoadFloat_e( "outline_color_red", FLOAT1 );
	m_fOutLineG						= rkLoader.LoadFloat_e( "outline_color_green", FLOAT1 );
	m_fOutLineB						= rkLoader.LoadFloat_e( "outline_color_blue", 0.0f );
	m_fOutLineAlpha					= rkLoader.LoadFloat_e( "outline_color_alpha", FLOAT05 );
	m_fOutLineThickness				= rkLoader.LoadFloat_e( "outline_thickness", 2.0f );

	m_iMaxHitCount = rkLoader.LoadInt_e( "hit_count", 0 );

	LoadRemoveAreaWeaponList( rkLoader );
}

ioBuff* ioDummyBuff::Clone()
{
	return new ioDummyBuff( *this );
}

void ioDummyBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_iCurHitCount = 0;

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}

	if( m_bEscapeCatchState && pOwner && pOwner->IsCatchMode() )
	{
		pOwner->SetEscapeCatchState();
	}

	if( m_bDeleteSameBuff )
	{
		ioBuff* pBuff = pOwner->GetBuff( GetName() );
		if( pBuff && pBuff->IsLive() )
			pBuff->SetReserveEndBuff();
	}
	if( m_bEnableOutLine )
	{
		D3DCOLORVALUE kColor;
		Help::InitColorValue( kColor, m_fOutLineR, m_fOutLineG, m_fOutLineB, m_fOutLineAlpha );
		m_pOwner->ApplyOutLineChange( COT_BUFF_PROTECTION, kColor, m_fOutLineThickness );
	}
}

bool ioDummyBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_iCurHitCount = 0;

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}

	if( m_bEscapeCatchState && pOwner && pOwner->IsCatchMode() )
	{
		pOwner->SetEscapeCatchState();
	}

	if( m_bEnableOutLine )
	{
		D3DCOLORVALUE kColor;
		Help::InitColorValue( kColor, m_fOutLineR, m_fOutLineG, m_fOutLineB, m_fOutLineAlpha );
		m_pOwner->ApplyOutLineChange( COT_BUFF_PROTECTION, kColor, m_fOutLineThickness );
	}

	return true;
}

void ioDummyBuff::ProcessBuff( float fTimePerSec )
{
	if( m_iMaxHitCount > 0 && m_iMaxHitCount - m_iCurHitCount <= 0 )
	{
		SetReserveEndBuff();
		return;
	}

	if( CheckOwnerStateCheck() )
		return;

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) && m_pOwner->GetState() != CS_OBJECT_RUNNING_ATTACK )
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
}

bool ioDummyBuff::CheckOwnerStateCheck()
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

	bool bWound = false;
	if( m_bEnableWoundCancel )
	{
		if( eState == CS_BLOW_WOUNDED )
			bWound = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED )
			bWound = true;
		else if( eState == CS_WOUNDED )
			bWound = true;
		else if( eState == CS_ETC_STATE )
			bWound = true;

		if( bWound )
		{
			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return true;
		}
	}

	if( m_bDisableDropDownState )
	{
		if( eState == CS_DROP_ZONE_DOWN )
		{
			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return true;
		}
	}

	return false;
}

bool ioDummyBuff::CheckRestoreStateCheck()
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

void ioDummyBuff::EndBuff()
{
	if( m_bUseWeaponEquipMesh && m_pOwner )
	{
		m_pOwner->SetChangeEquipMesh( m_ItemName, false );
	}

	if( m_bEnableOutLine )
	{
		m_pOwner->RestoreOutLineChange( COT_BUFF_PROTECTION );
	}

	ioBuff::EndBuff();

	if( 0 < m_dwWeaponDeadIndex )
	{
		ioBaseChar *pOwner = GetOwner();
		if( !pOwner )
			return;

		ioBaseChar *pCreator = pOwner->GetBaseChar( m_CreateChar );
		if( !pCreator )
			return;

		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pCreator, m_dwWeaponDeadIndex );
		if( pWeapon )	
			pWeapon->SetWeaponDead();
	}

	RemoveAreaWeapon( m_pOwner );
}

void ioDummyBuff::IncreaseHitCount()
{
	m_iCurHitCount++;
	m_iCurHitCount = min( m_iCurHitCount, m_iMaxHitCount );
}

void ioDummyBuff::LoadRemoveAreaWeaponList( ioINILoader &rkLoader )
{
	int nAreaWeaponMaxCnt = rkLoader.LoadInt_e( "remove_area_weapon_max_cnt", 0 );
	m_RemoveAreaWeaponList.clear();

	if( nAreaWeaponMaxCnt <= 0 ) 
		return;

	m_RemoveAreaWeaponList.reserve( nAreaWeaponMaxCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < nAreaWeaponMaxCnt; ++i )
	{
		wsprintf_e( szTitle, "remove_area_weapon_name%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_RemoveAreaWeaponList.push_back( szBuf );
	}
}

void ioDummyBuff::RemoveAreaWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if( m_RemoveAreaWeaponList.empty() ) 
		return;

	int nBuffCnt = m_RemoveAreaWeaponList.size();
	for( int i=0; i < nBuffCnt; ++i )
	{
		ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByName( m_RemoveAreaWeaponList[i], pOwner->GetCharName() );
		if ( pAreaWeapon )
			pAreaWeapon->DestroyAreaWeapon( pStage );
	}
}