

#include "stdafx.h"

#include "ioMarkBuff.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


ioMarkBuff::ioMarkBuff()
{
	m_dwStartTime = 0;
	m_bSetVisible = false;
}

ioMarkBuff::ioMarkBuff( const ioMarkBuff &rhs )
	: ioBuff( rhs ),
	m_stEffectName( rhs.m_stEffectName ),
	m_bUseLength( rhs.m_bUseLength ),
	m_fCheckLength( rhs.m_fCheckLength )
{
	m_dwStartTime = 0;
	m_bSetVisible = false;
	m_effectID = -1;
}

ioMarkBuff::~ioMarkBuff()
{
}

void ioMarkBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "mark_effect", "", szBuf, MAX_PATH );

	m_stEffectName = szBuf;

	m_bUseLength = rkLoader.LoadBool_e( "use_length", false );
	m_fCheckLength = rkLoader.LoadFloat_e ( "use_max_length", 0.f );
}

ioBuff* ioMarkBuff::Clone()
{
	return new ioMarkBuff( *this );
}

void ioMarkBuff::StartBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioBuff::StartBuff( pOwner );
	ioPlayStage* pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioBaseChar* pMyChar;
	pMyChar = pStage->GetOwnerChar();
	if( !pMyChar )
		return;

	m_dwStartTime = FRAMEGETTIME();

	if( pOwner && pOwner->IsNeedProcess() )//당사자
	{
		ioEffect *pEffect = pOwner->AttachEffect( m_stEffectName );
		if( pEffect )
			m_effectID = pEffect->GetUniqueID();
	}
	else if( pOwner && GetCreator() && GetCreator()->GetTeam() == pMyChar->GetTeam() )//생성자와 같은팀
	{
		ioEffect *pEffect = pOwner->AttachEffect( m_stEffectName );
		if( pEffect )
			m_effectID = pEffect->GetUniqueID();
	}

}

bool ioMarkBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_dwStartTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() )
	{

	}
	else if( GetCreator() && GetCreator()->IsNeedProcess() )
	{

	}

	return true;
}

void ioMarkBuff::ProcessBuff( float fTimePerSec )
{
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

	if( m_bUseLength )
	{
		ioBaseChar* pCreator = GetCreator();
		if( pCreator && m_pOwner )
		{
			D3DXVECTOR3 vCreatorPos = pCreator->GetWorldPosition();
			D3DXVECTOR3 vOwnerPos=  m_pOwner->GetWorldPosition();
			D3DXVECTOR3 vPos = vCreatorPos - vOwnerPos;
			float fLength = D3DXVec3Length( &vPos );
			if( fLength > m_fCheckLength )
				SetReserveEndBuff();
		}
		
	}
}

void ioMarkBuff::EndBuff()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	if( m_effectID != -1 )
	{
		ioEffect *pEffect = pOwner->GetEffectByObjID( m_effectID );
		if( pEffect )
			pEffect->EndEffectForce();

		m_effectID = -1;
	}
	
	ioBuff::EndBuff();
}

