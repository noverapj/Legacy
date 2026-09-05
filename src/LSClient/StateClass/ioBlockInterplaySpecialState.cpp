
#include "stdafx.h"

#include "ioBlockInterplaySpecialState.h"
#include "../ioBaseChar.h"
#include "../ioPlayStage.h"
#include "../Housing/ioBlock.h"
#include "../Housing/ioBlockManager.h"

ioBlockInterplaySpecialState::ioBlockInterplaySpecialState()
{
}

ioBlockInterplaySpecialState::~ioBlockInterplaySpecialState()
{
}

void ioBlockInterplaySpecialState::LoadProperty( ioINILoader &rkLoader )
{
	ioSpecialStateBase::LoadProperty( rkLoader );
}

void ioBlockInterplaySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );

	m_State				= IPS_NONE;
	m_pCurrInterPlay	= NULL;

	m_nBlockUniqueIndex	= 0;
	m_nCollIndex		= 0;

	m_eInterPlayType	= 0;
	m_dwPreEndTime		= 0;
	m_dwActionEndTime	= 0;
	m_dwEndStateEndTime	= 0;

	m_vPos				= ioMath::VEC3_ZERO;
	m_qtRot				= ioMath::QUAT_IDENTITY;
}

void ioBlockInterplaySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioBlock* pBlock = g_BlockManager.GetBlock( m_nBlockUniqueIndex );
	if( pBlock )
		pBlock->SetInterPlayChar( "" );
}

bool ioBlockInterplaySpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{	
	return true;
}

void ioBlockInterplaySpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
}

bool ioBlockInterplaySpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	return true;
}

void ioBlockInterplaySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	CheckBlock( pOwner, pOwner->GetCreator() );
	switch( m_State )
	{
	case IPS_PRE:
		{
			CheckPre( pOwner, pOwner->GetCreator() );
		}
		break;
	case IPS_ACTION:
		{
			CheckAction( pOwner, pOwner->GetCreator() );
		}
		break;
	case IPS_LOOP:
		{
			CheckLoop( pOwner, pOwner->GetCreator() );
		}
		break;
	case IPS_END:
		{
		}
		break;
	}
}

void ioBlockInterplaySpecialState::Start( ioBaseChar* pOwner, const D3DXVECTOR3& vPos, const D3DXQUATERNION& qtRot, __int64 nUniqueIndex, int nCollIndex )
{
	ioBlock* pBlock = g_BlockManager.GetBlock( nUniqueIndex );
	if( pBlock == NULL || !pBlock->GetProperty() )
	{
		m_State = IPS_END;
		return;
	}

	const ioBlockProperty::InterPlayInfo* pInfo = pBlock->GetProperty()->GetInterPlayInfo( nCollIndex );
	if( pInfo == NULL )
	{
		m_State = IPS_END;
		return;
	}

	pOwner->EndUnLimitedEmoticon();
	m_nBlockUniqueIndex = nUniqueIndex;
	m_nCollIndex		= nCollIndex;

	pBlock->SetInterPlayChar( pOwner->GetCharName() );

	D3DXQUATERNION qtCharRot;
	D3DXQuaternionRotationYawPitchRoll( &qtCharRot, DEGtoRAD( pInfo->m_fCharRotate ), 0.0f, 0.0f );

	m_pCurrInterPlay = pInfo;
	pOwner->SetWorldPosition( vPos );
	pOwner->SetTargetRot( qtRot * qtCharRot );
	ChangeToPre( pOwner, pOwner->GetCreator() );
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SPECIAL_STATE_SYNC );
		kPacket << m_pOwner->GetCharName();
		kPacket << (byte)SS_PRE;
		kPacket	<< vPos;
		kPacket	<< qtRot;
		kPacket << nUniqueIndex;
		kPacket << nCollIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBlockInterplaySpecialState::CheckBlock( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	ioBlock* pBlock = g_BlockManager.GetBlock( m_nBlockUniqueIndex );
	if( pBlock == NULL || !pBlock->GetProperty() )
	{
		m_State = IPS_END;
		return;
	}
}

void ioBlockInterplaySpecialState::ChangeToPre( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	if( m_pCurrInterPlay == NULL )
	{
		m_State = IPS_END;
		return;
	}

	m_State			= IPS_PRE;
	m_dwPreEndTime	= FRAMEGETTIME();

	ioHashString szAniamtion	= m_pCurrInterPlay->m_PreAnimation;
	float fAnimationRate		= m_pCurrInterPlay->m_fPreAnimationRate;
	m_eInterPlayType			= m_pCurrInterPlay->m_eInterPlayType;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		int iAniID = pGrp->GetAnimationIdx( szAniamtion );
		if( iAniID != -1 )
		{
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fAnimationRate, 0.0f, 0, true );
			m_dwPreEndTime += pGrp->GetAnimationFullTime(iAniID) * fAnimationRate;
		}
	}
}

void ioBlockInterplaySpecialState::CheckPre( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	if( m_dwPreEndTime < FRAMEGETTIME() )
	{
		if( m_pCurrInterPlay )
		{
			switch( m_pCurrInterPlay->m_eInterPlayType )
			{
			case ioBlockProperty::IPT_ACTION:
				{
					ChangeToAction( pOwner, pStage );
				}
				break;
			case ioBlockProperty::IPT_LOOP:
				{
					ChangeToLoop( pOwner, pStage );
				}
				break;
			}
		}
	}
}

void ioBlockInterplaySpecialState::ChangeToLoop( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	if( m_pCurrInterPlay == NULL )
	{
		m_State = IPS_END;
		return;
	}

	m_State			= IPS_LOOP;
	m_dwPreEndTime	= FRAMEGETTIME();

	ioHashString szAniamtion	= m_pCurrInterPlay->m_LoopAnimation;
	float fAnimationRate		= m_pCurrInterPlay->m_fLoopAnimationRate;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		int iAniID = pGrp->GetAnimationIdx( szAniamtion );
		if( iAniID != -1 )
		{
			pGrp->ClearAllActionAni( FLOAT10, true );
			pGrp->ClearAllLoopAni( FLOAT10, true );
			pGrp->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAnimationRate );
		}
	}
}

void ioBlockInterplaySpecialState::CheckLoop( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	if( pOwner->IsDefenseKey() )
	{
		ChangeToEnd( pOwner, pStage );
	}
}

void ioBlockInterplaySpecialState::ChangeToAction( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	if( m_pCurrInterPlay == NULL )
	{
		m_State = IPS_END;
		return;
	}

	m_State				= IPS_ACTION;
	m_dwActionEndTime	= FRAMEGETTIME();

	ioHashString szAniamtion	= m_pCurrInterPlay->m_ActionAnimation;
	float fAnimationRate		= m_pCurrInterPlay->m_fActionAnimationRate;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		int iAniID = pGrp->GetAnimationIdx( szAniamtion );
		if( iAniID != -1 )
		{
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fAnimationRate, 0.0f, 0, true );
			m_dwActionEndTime += pGrp->GetAnimationFullTime(iAniID) * fAnimationRate;
		}
	}
}

void ioBlockInterplaySpecialState::CheckAction( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	if( m_dwActionEndTime < FRAMEGETTIME() )
	{
		ChangeToEnd( pOwner, pStage );
	}
}

void ioBlockInterplaySpecialState::ChangeToEnd( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	m_State				= IPS_END;
	m_dwEndStateEndTime	= FRAMEGETTIME();
	
	if( m_pCurrInterPlay == NULL )
		return;

	ioHashString szAniamtion	= m_pCurrInterPlay->m_EndAnimation;
	float fAnimationRate		= m_pCurrInterPlay->m_fEndAnimationRate;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		int iAniID = pGrp->GetAnimationIdx( szAniamtion );
		if( iAniID != -1 )
		{
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fAnimationRate, 0.0f, 0, true );
			m_dwEndStateEndTime += pGrp->GetAnimationFullTime(iAniID) * fAnimationRate;
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SPECIAL_STATE_SYNC );
		kPacket << m_pOwner->GetCharName();
		kPacket << (byte)SS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioBlockInterplaySpecialState::IsStateEnd( ioBaseChar* pOwner )
{
	if( m_State == IPS_END && m_dwEndStateEndTime < FRAMEGETTIME() )
		return true;

	return false;
}

void ioBlockInterplaySpecialState::FillJoinSync( SP2Packet &rkPacket, ioBaseChar* pOwner )
{
	if( m_State == IPS_LOOP )
	{
		rkPacket << m_nBlockUniqueIndex;
		rkPacket << m_nCollIndex;
		rkPacket << m_vPos;
		rkPacket << m_qtRot;
	}
}

void ioBlockInterplaySpecialState::ApplyJoinSync( SP2Packet &rkPacket, ioBaseChar* pOwner )
{
	__int64			nBlockUniqueIndex	= 0;
	int				nCollIndex			= 0;
	D3DXVECTOR3		vPos				= ioMath::VEC3_ZERO;
	D3DXQUATERNION	qtRot				= ioMath::QUAT_IDENTITY;

	rkPacket >> nBlockUniqueIndex;
	rkPacket >> nCollIndex;
	rkPacket >> vPos;
	rkPacket >> qtRot;

	Start( pOwner, vPos, qtRot, nBlockUniqueIndex, nCollIndex );
	ChangeToLoop( pOwner, pOwner->GetCreator() );
}

void ioBlockInterplaySpecialState::ApplyStateSync( SP2Packet &rkPacket, ioBaseChar* pOwner )
{
	ioSpecialStateBase::ApplyStateSync( rkPacket, pOwner );

	byte eState;
	rkPacket >> eState;

	switch( eState )
	{
	case SS_PRE:
		{
			D3DXVECTOR3 vPos		= ioMath::VEC3_ZERO;
			D3DXQUATERNION qtRot	= ioMath::QUAT_IDENTITY;
			__int64 nUniqueIndex	= 0;
			int nCollIndex			= 0;

			rkPacket >> vPos >> qtRot >> nUniqueIndex >> nCollIndex;
			Start( pOwner, vPos, qtRot, nUniqueIndex, nCollIndex );
		}
		break;
	case SS_END:
		{
			ChangeToEnd( pOwner, pOwner->GetCreator() );
		}
		break;
	}
}