
#include "stdafx.h"

#include "ioCheckChainBuff2.h"

#include "WeaponDefine.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioCheckChainBuff2::ioCheckChainBuff2()
{
	m_dwChainWeaponIndex = -1;
	m_dwCreatorEffectID = -1;
}

ioCheckChainBuff2::ioCheckChainBuff2( const ioCheckChainBuff2 &rhs )
	: ioBuff( rhs ),
	  m_nChainWeapon( rhs.m_nChainWeapon ),
	  m_OwnerBuffList( rhs.m_OwnerBuffList ),
	  m_szCreatorEffect( rhs.m_szCreatorEffect ),
	  m_vCreatorEffectOffset( rhs.m_vCreatorEffectOffset ),
	  m_fCheckDist( rhs.m_fCheckDist )
{	
	m_dwChainWeaponIndex = -1;
	m_dwCreatorEffectID = -1;
}

ioCheckChainBuff2::~ioCheckChainBuff2()
{	
}

ioBuff* ioCheckChainBuff2::Clone()
{
	return new ioCheckChainBuff2( *this );
}

void ioCheckChainBuff2::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];
	m_nChainWeapon = rkLoader.LoadInt_e( "chain_weapon", NO_WEAPON_ATTRIBUTE );

	rkLoader.LoadString( "creator_effect", "", szBuf, MAX_PATH );
	m_szCreatorEffect = szBuf;

	m_vCreatorEffectOffset.x = rkLoader.LoadFloat_e( "creator_effect_offset_x", 0.0f );
	m_vCreatorEffectOffset.y = rkLoader.LoadFloat_e( "creator_effect_offset_y", 0.0f );
	m_vCreatorEffectOffset.z = rkLoader.LoadFloat_e( "creator_effect_offset_z", 0.0f );

	int nBuffCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();
	for ( int i=0; i<nBuffCnt; i++ )
	{
		wsprintf_e( szKey, "owner_buff%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_OwnerBuffList.push_back( szBuf );
	}

	m_fCheckDist = rkLoader.LoadFloat_e( "check_dist", 0.0f );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioCheckChainBuff2::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_dwChainWeaponIndex = -1;
	m_BuffState = BS_START;		
}

bool ioCheckChainBuff2::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_dwChainWeaponIndex = -1;
	m_BuffState = BS_START;

	return true;
}

void ioCheckChainBuff2::ProcessBuff( float fTimePerSec )
{	
	ioBaseChar *pCreator = GetCreator();
	if ( !m_pOwner || !pCreator )
	{
		SetReserveEndBuff();
		return;
	}

	if ( m_fCheckDist > 0.0f && !CheckDist( pCreator, m_pOwner ) )
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
	
	switch( m_BuffState )
	{
	case BS_START:
		{
			CreateChain( m_pOwner );
			AddOwnerBuff( m_pOwner );

			D3DXVECTOR3 vCreatorPos = pCreator->GetMidPositionByRate() + pCreator->GetWorldOrientation() * m_vCreatorEffectOffset;
			m_dwCreatorEffectID = CreateEffect( m_pOwner->GetCreator(), m_szCreatorEffect, vCreatorPos, D3DXVECTOR3(1.0f,1.0f,1.0f) );
		}
		break;
	case BS_WAIT:
		SetChainPos( m_pOwner );
		break;
	}
}

void ioCheckChainBuff2::EndBuff()
{
	SetEndState( m_pOwner );

	ioBuff::EndBuff();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioCheckChainBuff2::CreateChain( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioBaseChar *pCreator = GetCreator();
	if ( !pCreator )
		return;

	m_dwChainWeaponIndex = pOwner->IncreaseWeaponIndexBase();

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx = m_nChainWeapon;
	kFireTime.dwWeaponIdx = m_dwChainWeaponIndex++;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );

	ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );	
	if ( pWeapon )
	{
		ioUroborusWeapon4 *pChainWeapon = ToUroborusWeapon4( pWeapon );
		if ( pChainWeapon )
		{
			pChainWeapon->SetVisible( false );
			m_dwChainWeaponIndex = pWeapon->GetWeaponIndex();
		}
	}

	m_BuffState = BS_WAIT;
}

void ioCheckChainBuff2::SetChainPos( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;
	
	ioBaseChar *pCreator = GetCreator();
	if ( !pCreator )
	{
		SetReserveEndBuff();
		return;
	}	

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwChainWeaponIndex );
	ioUroborusWeapon4 *pChainWeapon = ToUroborusWeapon4( pWeapon );
	if ( pChainWeapon )
	{
		if ( pChainWeapon->IsHasArrow() )
			pChainWeapon->SetVisible( true );

		if ( m_dwCreatorEffectID != -1 )
		{
			ioMapEffect *pCreatorEffect = pStage->FindMapEffect( m_dwCreatorEffectID );
			if ( pCreatorEffect )
			{
				D3DXVECTOR3 vCreatorPos = pCreator->GetMidPositionByRate() + pCreator->GetWorldOrientation() * m_vCreatorEffectOffset;
				pCreatorEffect->SetWorldPosition( vCreatorPos );
				pCreatorEffect->SetWorldOrientation( pCreator->GetWorldOrientation() );

				pChainWeapon->SetPosition( vCreatorPos );
			}
		}
		else
			pChainWeapon->SetPosition( pCreator->GetMidPositionByRate() );
	}	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioCheckChainBuff2::SetEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( m_dwChainWeaponIndex != -1)
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwChainWeaponIndex );
		if ( pWeapon )
			pWeapon->SetWeaponDead();
	}
	
	DestroyEffect( pOwner->GetCreator(), m_dwCreatorEffectID );
	RemoveOwnerBuff( pOwner );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioCheckChainBuff2::AddOwnerBuff( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	int nBuffCnt = (int)m_OwnerBuffList.size();
	for ( int i=0; i<nBuffCnt; i++ )
		pOwner->AddNewBuff( m_OwnerBuffList[i], GetCreatorName(), "", NULL );
}

void ioCheckChainBuff2::RemoveOwnerBuff( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	int nBuffCnt = (int)m_OwnerBuffList.size();
	for ( int i=0; i<nBuffCnt; i++ )
	{
		ioBuff *pRemoveBuff = pOwner->GetBuff( m_OwnerBuffList[i], GetCreatorName() );
		if ( pRemoveBuff )
			pRemoveBuff->SetReserveEndBuff();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD ioCheckChainBuff2::CreateEffect( ioPlayStage *pStage, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale )
{
	if ( !pStage || szEffect.IsEmpty() )
		return -1;

	ioMapEffect *pMaker = pStage->CreateMapEffect( szEffect, vPos, vScale );
	if ( pMaker )
		return pMaker->GetUniqueID();

	return -1;
}

void ioCheckChainBuff2::DestroyEffect( ioPlayStage *pStage, DWORD& dwEffectID )
{
	if ( !pStage || dwEffectID == -1 )
		return;

	pStage->DestroyMapEffect( dwEffectID );
	dwEffectID = -1;
}

bool ioCheckChainBuff2::CheckDist( ioBaseChar *pCreator, ioBaseChar *pOwner )
{
	if ( !pCreator || !pOwner )
		return false;

	D3DXVECTOR3 vDiff = pCreator->GetWorldPosition() - pOwner->GetWorldPosition();
	if ( D3DXVec3LengthSq( &vDiff ) >= m_fCheckDist * m_fCheckDist )
		return false;

	return true;
}