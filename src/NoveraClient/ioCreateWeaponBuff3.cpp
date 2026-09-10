
#include "stdafx.h"

#include "ioCreateWeaponBuff3.h"

#include "ioBaseChar.h"

ioCreateWeaponBuff3::ioCreateWeaponBuff3()
{
	Init();
}

ioCreateWeaponBuff3::ioCreateWeaponBuff3( const ioCreateWeaponBuff3 &rhs )
	: ioBuff( rhs ),
	  m_bEnableOutLine( rhs.m_bEnableOutLine ),
	  m_fOutLineR( rhs.m_fOutLineR ),
	  m_fOutLineG( rhs.m_fOutLineG ),
	  m_fOutLineB( rhs.m_fOutLineB ),
	  m_fOutLineAlpha( rhs.m_fOutLineAlpha ),
	  m_fOutLineThickness( rhs.m_fOutLineThickness ),
	  m_szWaitEffect( rhs.m_szWaitEffect ),
	  m_dwWaitTime( rhs.m_dwWaitTime ),
	  m_fFireHeightOffSet( rhs.m_fFireHeightOffSet ),
	  m_fCheckDist( rhs.m_fCheckDist ),
	  m_ShortWeaponInfo( rhs.m_ShortWeaponInfo ),
	  m_LongWeaponInfo( rhs.m_LongWeaponInfo )
{
	Init();
}

ioCreateWeaponBuff3::~ioCreateWeaponBuff3()
{
}

void ioCreateWeaponBuff3::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_bEnableOutLine = rkLoader.LoadBool_e( "outline_enable", false );
	m_fOutLineR = rkLoader.LoadFloat_e( "outline_color_red", FLOAT1 );
	m_fOutLineG = rkLoader.LoadFloat_e( "outline_color_green", FLOAT1 );
	m_fOutLineB = rkLoader.LoadFloat_e( "outline_color_blue", 0.0f );
	m_fOutLineAlpha = rkLoader.LoadFloat_e( "outline_color_alpha", FLOAT05 );
	m_fOutLineThickness = rkLoader.LoadFloat_e( "outline_thickness", 2.0f );

	rkLoader.LoadString_e( "wait_effect", "", szBuf, MAX_PATH );
	m_szWaitEffect = szBuf;

	m_dwWaitTime = rkLoader.LoadInt_e( "wait_time", 0 );

	m_fFireHeightOffSet = rkLoader.LoadFloat_e( "fire_offset_height", 0.0f );	
	m_fCheckDist = rkLoader.LoadFloat_e( "check_dist", 0.0f );	

	m_ShortWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "short_attack_index", NO_WEAPON_ATTRIBUTE );
	m_ShortWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "short_attack_index_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString_e( "short_attack_wounded", "", szBuf, MAX_PATH );
	m_ShortWeaponInfo.m_WoundedAnimation = szBuf;
	m_ShortWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "short_attack_wounded_time", 0 );
	m_ShortWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "short_attack_wounded_loop_ani", false );

	m_LongWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "long_attack_index", NO_WEAPON_ATTRIBUTE );
	m_LongWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "long_attack_index_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString_e( "long_attack_wounded", "", szBuf, MAX_PATH );
	m_LongWeaponInfo.m_WoundedAnimation = szBuf;
	m_LongWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "long_attack_wounded_time", 0 );
	m_LongWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "long_attack_wounded_loop_ani", false );
}

ioBuff* ioCreateWeaponBuff3::Clone()
{
	return new ioCreateWeaponBuff3( *this );
}

void ioCreateWeaponBuff3::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
	
	if( m_bEnableOutLine )
	{
		D3DCOLORVALUE kColor;
		Help::InitColorValue( kColor, m_fOutLineR, m_fOutLineG, m_fOutLineB, m_fOutLineAlpha );
		m_pOwner->ApplyOutLineChange( COT_BUFF_PROTECTION, kColor, m_fOutLineThickness );
	}
	
	Init();
	SetWaitState( pOwner );
}

bool ioCreateWeaponBuff3::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;
	
	if( m_bEnableOutLine )
	{
		D3DCOLORVALUE kColor;
		Help::InitColorValue( kColor, m_fOutLineR, m_fOutLineG, m_fOutLineB, m_fOutLineAlpha );
		m_pOwner->ApplyOutLineChange( COT_BUFF_PROTECTION, kColor, m_fOutLineThickness );
	}

	Init();
	SetWaitState( pOwner );

	return true;
}

void ioCreateWeaponBuff3::ProcessBuff( float fTimePerSec )
{
	ioBaseChar *pCreator = GetCreator();
	if( !m_pOwner || !pCreator || pCreator->GetState() == CS_DIE )
	{
		SetReserveEndBuff();
		return;
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
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_BuffState )
	{
	case BS_WAIT:
		{
			if ( m_dwCheckWaitTime < dwCurTime )
			{
				SetFireState( m_pOwner );
				return;
			}
		}
		break;
	case BS_FIRE:
		{
			if ( m_pOwner->IsNeedProcess() )
				CreateWeapon();
		}
		break;
	}
}

void ioCreateWeaponBuff3::EndBuff()
{
	ioBuff::EndBuff();

	if ( !m_pOwner )
		return;

	m_pOwner->EndEffect( m_szWaitEffect, false );

	if( m_bEnableOutLine )
		m_pOwner->RestoreOutLineChange( COT_BUFF_PROTECTION );
}

void ioCreateWeaponBuff3::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	int nType;
	rkPacket >> nType;

	switch ( nType )
	{
	case BS_FIRE:
		{
			bool bShort;
			rkPacket >> bShort;

			if ( bShort )
				m_CurWeaponInfo = m_ShortWeaponInfo;
			else
				m_CurWeaponInfo = m_LongWeaponInfo;

			ioBaseChar *pCreator = GetCreator();
			if( !pCreator )
				return;

			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
			vPos.y += m_fFireHeightOffSet;

			DWORD dwWeaponIndex = pCreator->IncreaseWeaponIndexBase();
			ioWeapon* pWeapon = pCreator->DummyExplicit( m_CurWeaponInfo, vPos, dwWeaponIndex, false, pOwner->GetCharName(), m_Name, m_bUseCallAttWeaponCol );
			if( pWeapon )
			{
				pWeapon->SetPosition( vPos );
				pWeapon->SetMoveDir( -ioMath::UNIT_Y );
			}

			SetReserveEndBuff();
		}
		break;
	}
}

void ioCreateWeaponBuff3::CreateWeapon()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioBaseChar *pCreator = GetCreator();
	if( !pCreator )
		return;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDist = pCreator->GetMidPositionByRate() - vPos;
	bool bShort = true;
	if ( D3DXVec3LengthSq( &vDist ) <= m_fCheckDist * m_fCheckDist )
		m_CurWeaponInfo = m_ShortWeaponInfo;
	else
	{
		m_CurWeaponInfo = m_LongWeaponInfo;
		bShort = false;
	}

	vPos.y += m_fFireHeightOffSet;
	DWORD dwWeaponIndex = pCreator->IncreaseWeaponIndexBase();

	ioWeapon* pWeapon = pCreator->DummyExplicit( m_CurWeaponInfo, vPos, dwWeaponIndex, false, pOwner->GetCharName(), m_Name, m_bUseCallAttWeaponCol );
	if( pWeapon )
	{
		pWeapon->SetPosition( vPos );
		pWeapon->SetMoveDir( -ioMath::UNIT_Y );
	}

	SetReserveEndBuff();

	if ( m_pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << m_pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_FIRE;
		kPacket << bShort;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCreateWeaponBuff3::Init()
{
	m_BuffState = BS_NONE;
	m_dwCheckWaitTime = 0;
}

void ioCreateWeaponBuff3::SetWaitState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_dwCheckWaitTime = FRAMEGETTIME() + m_dwWaitTime;

	pOwner->AttachEffect( m_szWaitEffect );

	m_BuffState = BS_WAIT;
}

void ioCreateWeaponBuff3::SetFireState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	pOwner->EndEffect( m_szWaitEffect, false );

	m_BuffState = BS_FIRE;
}