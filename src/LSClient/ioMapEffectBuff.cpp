

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioMapEffectBuff.h"

ioMapEffectBuff::ioMapEffectBuff()
{
	m_UniqueObjID = -1;
}

ioMapEffectBuff::ioMapEffectBuff( const ioMapEffectBuff &rhs )
	: ioBuff( rhs ),
	m_bDisableDownState( rhs.m_bDisableDownState ),
	m_bDisableDropDownState( rhs.m_bDisableDropDownState ),
	m_szMapEffect( rhs.m_szMapEffect ),
	m_fOffset( rhs.m_fOffset ),
	m_fSacleRate( rhs.m_fSacleRate )
{
}

ioMapEffectBuff::~ioMapEffectBuff()
{
}

void ioMapEffectBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bDisableDownState		= rkLoader.LoadBool_e( "disable_down_state", false );
	m_bDisableDropDownState	= rkLoader.LoadBool_e( "disable_drop_down_state", false );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "map_effect", "", szBuf, MAX_PATH );	
	m_szMapEffect = szBuf;

	m_fOffset		= rkLoader.LoadFloat_e( "map_effect_offset", 0.0f );
	m_fSacleRate	= rkLoader.LoadFloat_e( "map_effect_scale", FLOAT1 );
}

ioBuff* ioMapEffectBuff::Clone()
{
	return new ioMapEffectBuff( *this );
}

void ioMapEffectBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	pOwner->SetInvisibleState( true );	
	CreateEffect();
	
}

bool ioMapEffectBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	pOwner->SetInvisibleState( true );
	CreateEffect();

	return true;
}

void ioMapEffectBuff::CreateEffect()
{
	ioBaseChar* pOwner = GetOwner();
	if( pOwner )
	{
		ioPlayStage* pStage = pOwner->GetCreator();
		if( pStage )
		{
			D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
			vPos.y += m_fOffset;

			ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szMapEffect, vPos, D3DXVECTOR3( m_fSacleRate, m_fSacleRate, m_fSacleRate ) );
			if( pMapEffect )
			{
				m_UniqueObjID = pMapEffect->GetUniqueID();
			}
		}
	}
}

void ioMapEffectBuff::UpdateEffect()
{
	ioBaseChar* pOwner = GetOwner();
	if( pOwner )
	{
		ioPlayStage* pStage = pOwner->GetCreator();
		if( pStage )
		{
			ioMapEffect* pMapEffect = pStage->FindMapEffect( m_UniqueObjID );
			if( pMapEffect )
			{
				D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
				vPos.y += m_fOffset;

				pMapEffect->SetWorldPosition( vPos );
			}
		}
	}
}

void ioMapEffectBuff::EndEffect()
{
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pStage )
	{
		pStage->DestroyMapEffect( m_UniqueObjID );
		m_UniqueObjID = -1;
	}
}

void ioMapEffectBuff::ProcessBuff( float fTimePerSec )
{
	CheckOwnerStateCheck();

	if( CheckEnableDelayState() )
		return;

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

	UpdateEffect();
}

bool ioMapEffectBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner )
		return false;

	if( !m_pOwner )
		return false;

	CharState eState = m_pOwner->GetState();

	if( m_bDisableDownState )
	{
		if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
		{
			SetReserveEndBuff();
			return true;
		}
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
		{
			SetReserveEndBuff();
			return true;
		}
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
		{
			SetReserveEndBuff();
			return true;
		}
		else if( eState == CS_DROP_ZONE_DOWN )
		{
			SetReserveEndBuff();
			return true;
		}
	}

	return false;
}

void ioMapEffectBuff::EndBuff()
{
	ioBuff::EndBuff();

	ioBaseChar* pOwner = GetOwner();
	if( pOwner )
		pOwner->SetInvisibleState( false );

	EndEffect();
}