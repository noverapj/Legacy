

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioExtraCallBuff.h"

ioExtraCallBuff::ioExtraCallBuff()
{
	m_bReserveExtraCall = false;
	m_dwCheckCallBuffTime = 0;
	m_dwCheckCallAttackTime = 0;
}

ioExtraCallBuff::ioExtraCallBuff( const ioExtraCallBuff &rhs )
: ioBuff( rhs ),
m_bDisableDownState( rhs.m_bDisableDownState ),
m_bUseWeaponEquipMesh( rhs.m_bUseWeaponEquipMesh ),
m_bEscapeCatchState( rhs.m_bEscapeCatchState ),
m_bEnableOutLine( rhs.m_bEnableOutLine ),
m_fOutLineR( rhs.m_fOutLineR ),
m_fOutLineG( rhs.m_fOutLineG ),
m_fOutLineB( rhs.m_fOutLineB ),
m_fOutLineAlpha( rhs.m_fOutLineAlpha ),
m_fOutLineThickness( rhs.m_fOutLineThickness ),
m_ExtraCallBuffList( rhs.m_ExtraCallBuffList ),
m_ExtraCallWeaponInfoList( rhs.m_ExtraCallWeaponInfoList ),
m_dwEnableCallBuffTime( rhs.m_dwEnableCallBuffTime ),
m_dwEnableCallAttackTime( rhs.m_dwEnableCallAttackTime )
{
	m_bReserveExtraCall = false;
	m_dwCheckCallBuffTime = 0;
	m_dwCheckCallAttackTime = 0;
}

ioExtraCallBuff::~ioExtraCallBuff()
{
}

void ioExtraCallBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bUseWeaponEquipMesh = rkLoader.LoadBool_e( "use_weapon_equip_mesh", false );
	m_bEscapeCatchState = rkLoader.LoadBool_e( "escape_catch_state", false );

	m_bEnableOutLine = rkLoader.LoadBool_e( "outline_enable", false );
	m_fOutLineR = rkLoader.LoadFloat_e( "outline_color_red", FLOAT1 );
	m_fOutLineG = rkLoader.LoadFloat_e( "outline_color_green", FLOAT1 );
	m_fOutLineB = rkLoader.LoadFloat_e( "outline_color_blue", 0.0f );
	m_fOutLineAlpha = rkLoader.LoadFloat_e( "outline_color_alpha", FLOAT05 );
	m_fOutLineThickness = rkLoader.LoadFloat_e( "outline_thickness", 2.0f );

	//
	m_ExtraCallBuffList.clear();
	m_ExtraCallWeaponInfoList.clear();

	int iBuffCnt = rkLoader.LoadInt_e( "extra_call_buff_cnt", 0 );
	for( int i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "extra_call_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_ExtraCallBuffList.push_back( szBuf );
	}

	int iWeaponInfoCnt = rkLoader.LoadInt_e( "extra_call_attack_cnt", 0 );
	for( int i=0; i < iWeaponInfoCnt; ++i )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "extra_call_attribute%d_index", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "extra_call_attribute%d_resistance", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "extra_call_attribute%d_wound_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "extra_call_attribute%d_wound_duration", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "extra_call_attribute%d_wound_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_ExtraCallWeaponInfoList.push_back( kInfo );
	}

	m_dwEnableCallBuffTime = (DWORD)rkLoader.LoadInt_e( "enable_call_buff_time", 0 );
	m_dwEnableCallAttackTime = (DWORD)rkLoader.LoadInt_e( "enable_call_attack_time", 0 );
}

ioBuff* ioExtraCallBuff::Clone()
{
	return new ioExtraCallBuff( *this );
}

void ioExtraCallBuff::StartBuff( ioBaseChar *pOwner )
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

	if( m_bEnableOutLine )
	{
		D3DCOLORVALUE kColor;
		Help::InitColorValue( kColor, m_fOutLineR, m_fOutLineG, m_fOutLineB, m_fOutLineAlpha );
		m_pOwner->ApplyOutLineChange( COT_BUFF_PROTECTION, kColor, m_fOutLineThickness );
	}

	DWORD dwCurTime = FRAMEGETTIME();

	m_bReserveExtraCall = false;
	m_dwCheckCallBuffTime = dwCurTime;
	m_dwCheckCallAttackTime = dwCurTime;
}

bool ioExtraCallBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
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

	if( m_bEnableOutLine )
	{
		D3DCOLORVALUE kColor;
		Help::InitColorValue( kColor, m_fOutLineR, m_fOutLineG, m_fOutLineB, m_fOutLineAlpha );
		m_pOwner->ApplyOutLineChange( COT_BUFF_PROTECTION, kColor, m_fOutLineThickness );
	}

	DWORD dwCurTime = FRAMEGETTIME();

	m_bReserveExtraCall = false;
	m_dwCheckCallBuffTime = dwCurTime;
	m_dwCheckCallAttackTime = dwCurTime;

	return true;
}

void ioExtraCallBuff::ProcessBuff( float fTimePerSec )
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
		CheckExtraCall();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		{
			if( CheckEnableDelayState() )
				return;

			CheckCallingProcess();
			CheckExtraCall();

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
		CheckExtraCall();
		CheckCallingAreaWeapon();
		break;
	}
}

bool ioExtraCallBuff::CheckOwnerStateCheck()
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

	return false;
}

bool ioExtraCallBuff::CheckRestoreStateCheck()
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

void ioExtraCallBuff::EndBuff()
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
}

void ioExtraCallBuff::SetReserveExtraCall()
{
	m_bReserveExtraCall = true;
}

void ioExtraCallBuff::CheckExtraCall()
{
	if( !m_bReserveExtraCall )
		return;

	m_bReserveExtraCall = false;

	CheckExtraCallBuff();
	CheckExtraCallAttack();
}

void ioExtraCallBuff::CheckExtraCallBuff()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( m_ExtraCallBuffList.empty() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCheckCallBuffTime <  dwCurTime )
	{
		for( int i=0; i < (int)m_ExtraCallBuffList.size(); ++i )
		{
			pOwner->ReserveAddNewBuff( m_ExtraCallBuffList[i], m_CreateChar, m_ItemName, NULL );
		}

		m_dwCheckCallBuffTime = dwCurTime + m_dwEnableCallBuffTime;
	}
}

void ioExtraCallBuff::CheckExtraCallAttack()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( m_ExtraCallWeaponInfoList.empty() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCheckCallAttackTime <  dwCurTime )
	{
		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();

		for( int i=0; i < (int)m_ExtraCallWeaponInfoList.size(); ++i )
		{
			DWORD dwWeaponIndex = pOwner->IncreaseWeaponIndexBase() + 1;

			m_vWeaponIndexList.push_back( dwWeaponIndex );
			pOwner->DummyExplicit( m_ExtraCallWeaponInfoList[i],
								   vPos,
								   dwWeaponIndex,
								   false,
								   m_OwnerName,
								   m_Name,
								   m_bUseCallAttWeaponCol );
		}
		
		m_dwCheckCallAttackTime = dwCurTime + m_dwEnableCallAttackTime;
	}
}
