#include "StdAfx.h"
#include "ioRangeFearBuff.h"


ioRangeFearBuff::ioRangeFearBuff(void)
{
}

ioRangeFearBuff::ioRangeFearBuff( const ioRangeFearBuff& rhs )
	: ioBuff( rhs ),
	m_fEffectRange( rhs.m_fEffectRange ),
	m_fFearRange( rhs.m_fFearRange ),
	m_dwFearTerm( rhs.m_dwFearTerm ),
	m_szFearEffect( rhs.m_szFearEffect ),
	m_szFearBuffList( rhs.m_szFearBuffList )
{

}

ioRangeFearBuff::~ioRangeFearBuff(void)
{
}

void	ioRangeFearBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char	szBuf[MAX_PATH];
	char	szKey[MAX_PATH];

	m_fEffectRange	= rkLoader.LoadFloat_e( "effect_range", 0.0f );
	m_fFearRange	= rkLoader.LoadFloat_e( "fear_range", 0.0f );
	m_dwFearTerm	= rkLoader.LoadInt_e( "fear_term", 0 );
	rkLoader.LoadString_e( "fear_effect", "", szBuf, MAX_PATH );
	m_szFearEffect	= szBuf;
	
	int	iBuffCnt	= rkLoader.LoadInt_e( "fear_buff_cnt", 0 );
	for( int i = 0; i < iBuffCnt; i++ )
	{
		ioHashString	szFearBuff;

		wsprintf_e( szKey, "fear_buff%d", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		szFearBuff	= szBuf;

		m_szFearBuffList.push_back( szFearBuff );
	}
}

ioBuff*	ioRangeFearBuff::Clone()
{
	return	new	ioRangeFearBuff( *this );
}

void	ioRangeFearBuff::StartBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_FearEffectID	= -1;
	m_dwFearBuffStartTime	= FRAMEGETTIME();

	ioBuff::StartBuff( pOwner );

	/*ioPlayStage*	pStage	= pOwner->GetCreator();
	ioBaseChar*	pBuffOwner	= GetOwner();
	ioBaseChar* pOwnerChar	= pStage->GetOwnerChar();
	ioBaseChar*	pCreator	= GetCreator();
	if( pStage && pBuffOwner && pOwnerChar && pCreator )
	{
		if( pCreator == pBuffOwner )
		{
			if( m_FearEffectID == -1 )
			{
				ioEffect*	pEffect	= pBuffOwner->AttachEffect( m_szFearEffect );
				if( pEffect )
					m_FearEffectID	= pEffect->GetUniqueID();
			}
		}
	}*/
	if( m_FearEffectID == -1 )
	{
		ioEffect*	pEffect	= pOwner->AttachEffect( m_szFearEffect );
		if( pEffect )
			m_FearEffectID	= pEffect->GetUniqueID();
	}
}

bool	ioRangeFearBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return	true;
}

void	ioRangeFearBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		{
			if( !CheckSwitchType( fTimePerSec ) )
			{
				if( GetOwner() && CheckRestoreStateCheck() )
					GetOwner()->SetState( CS_DELAY, m_bSendDelayNetWork );

				m_pOwner->CheckUseBuffSkill( m_Name, false );
				SetReserveEndBuff();
				return;
			}

			CheckCallingProcess();
		}
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
		{
			CheckCallingAreaWeapon();
		}
		break;
	}

	if( CheckOwnerState() )
		ProcessBuffState();
}

void	ioRangeFearBuff::EndBuff()
{
	ioBaseChar*	pOwner	= GetOwner();

	if( pOwner && m_FearEffectID != -1 )
	{
		pOwner->EndEffect( m_FearEffectID, false );
		m_FearEffectID	= -1;
	}

	ioBuff::EndBuff();
}

void	ioRangeFearBuff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );

}

void	ioRangeFearBuff::ApplyBuffInfo( SP2Packet &rkPacket )
{

}

bool	ioRangeFearBuff::CheckRestoreStateCheck()
{
	if( !m_pOwner ) 
		return	false;

	CharState	eState	= m_pOwner->GetState();

	switch( eState )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		{
			return	true;
		}
		break;
	}

	return	false;
}

bool	ioRangeFearBuff::CheckOwnerState()
{
	ioBaseChar*	pOwner	= GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return	false;
	}

	if( pOwner->IsSystemState() )
	{
		SetReserveEndBuff();
		return	false;
	}

	return	true;
}

void	ioRangeFearBuff::ProcessBuffState()
{
	ioBaseChar*	pOwner	= GetOwner();
	if( !pOwner )
		return;

	ioPlayStage*	pStage	= pOwner->GetCreator();
	if( !pStage )
		return;

	ioBaseChar*	pCreator	= GetCreator();

	if( !pCreator || pCreator == pOwner )
		return;

	/*ioBaseChar* pOwnerChar	= pStage->GetOwnerChar();
	if( !pOwnerChar )
		return;*/

	//if( !pCreator->HasBuff( BT_RANGE_FEAR_BUFF ) )
		//return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	/*if( pCreator == pOwner )	// 공격자
	{
		const	BaseCharList	&charList	= pStage->GetBaseCharList();
		BaseCharList::const_iterator iter;
		bool	bEffectVisible	= false;
		for( iter = charList.begin(); iter != charList.end(); ++iter )
		{
			ioBaseChar*	pChar	= *iter;
			if( !pChar ) 
				continue;

			if( !pChar->HasBuff( BT_RANGE_FEAR_BUFF ) )
				continue;

			if( CheckEffectRange( pChar ) )
			{
				bEffectVisible	= true;

				break;
			}
		}

		if( bEffectVisible && m_FearEffectID == -1 )
		{
			ioEffect*	pEffect	= pCreator->AttachEffect( m_szFearEffect );
			if( pEffect )
				m_FearEffectID	= pEffect->GetUniqueID();
		}
		else if( !bEffectVisible && m_FearEffectID != -1 )
		{
			pCreator->EndEffect( m_FearEffectID, false );
			m_FearEffectID	= -1;
		}
	}
	else*/	// 피격자
	{
		if( CheckFearRange( pCreator ) && m_dwFearBuffStartTime + m_dwFearTerm < dwCurTime )
		{
			for( int i = 0; i < (int)m_szFearBuffList.size(); i++ )
			{
				pOwner->AddNewBuff( m_szFearBuffList[i], pOwner->GetCharName(), "", NULL );
			}

			m_dwFearBuffStartTime	= dwCurTime;
		}
	}

	/*if( pOwnerChar != pCreator )
	{
		if( CheckEffectRange( pCreator ) && m_FearEffectID == -1 )
		{
			ioEffect*	pEffect	= pCreator->AttachEffect( m_szFearEffect );
			if( pEffect )
				m_FearEffectID	= pEffect->GetUniqueID();
		}
		else if( !CheckEffectRange( pCreator ) && m_FearEffectID != -1 )
		{
			pCreator->EndEffect( m_FearEffectID, false );
			m_FearEffectID	= -1;
		}
	}

	if( CheckFearRange( pCreator ) && m_dwFearBuffStartTime + m_dwFearTerm < dwCurTime )
	{
		for( int i = 0; i < (int)m_szFearBuffList.size(); i++ )
		{
			pOwner->AddNewBuff( m_szFearBuffList[i], pOwner->GetCharName(), "", NULL );
		}

		m_dwFearBuffStartTime	= dwCurTime;
	}*/

	/*const	BaseCharList&	rkTargetList	= pStage->GetBaseCharList();
	BaseCharList::const_iterator	iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar*	pTarget	= (*iter);
		if( !pTarget )
			continue;

		if( pTarget->GetCharName() == pOwner->GetCharName() )
			continue;

		if( !pTarget->HasBuff( BT_RANGE_FEAR_BUFF ) )
			continue;

		if( pOwner->GetTeam() == pTarget->GetTeam() )
			continue;
	}*/
}

bool	ioRangeFearBuff::CheckEffectRange( ioBaseChar* pTarget )
{
	ioBaseChar*	pOwner	= GetOwner();
	if( !pOwner )
		return	false;

	D3DXVECTOR3	vPosOwner	= pOwner->GetMidPositionByRate();
	D3DXVECTOR3	vPosTarget	= pTarget->GetMidPositionByRate();
	D3DXVECTOR3	vDist	= vPosTarget - vPosOwner;
	float	fLength	= D3DXVec3Length( &vDist );

	if( fLength < m_fEffectRange )
		return	true;

	return	false;
}

bool	ioRangeFearBuff::CheckFearRange( ioBaseChar* pTarget )
{
	ioBaseChar*	pOwner	= GetOwner();
	if( !pOwner )
		return	false;

	D3DXVECTOR3	vPosOwner	= pOwner->GetMidPositionByRate();
	D3DXVECTOR3	vPosTarget	= pTarget->GetMidPositionByRate();
	D3DXVECTOR3	vDist	= vPosTarget - vPosOwner;
	float	fLength	= D3DXVec3Length( &vDist );

	if( fLength < m_fFearRange )
		return	true;

	return	false;
}