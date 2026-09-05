#include "StdAfx.h"
#include "ioDiceGame.h"
#include "./GUI/DiceGameWnd.h"


ioDiceGame::ioDiceGame(void)
{
	m_pMoveManager		= NULL;
	m_pRewardManager	= NULL;

	m_byBoardIndex		= 0;
	m_iCurrentPosition	= 1;

	m_iFinalRewardCount		= 0;
	m_iNormalRewardCount	= 0;

	m_TraceMap.clear();

	LoadData();
}


ioDiceGame::~ioDiceGame(void)
{
	SAFEDELETE( m_pMoveManager );
	SAFEDELETE( m_pRewardManager );
}

void	ioDiceGame::LoadINI()
{
	ioINILoader_e kLoader( "config/sp2_snakeladder_info.ini" );

	kLoader.SetTitle_e( "common" );
}

void	ioDiceGame::LoadData()
{
	m_pMoveManager	= new LSC_SnakeLadders_Move_Manager;
	m_pMoveManager->LoadData( SNAKELADDER_MOVE_TABLE );

	m_pRewardManager	= new LSC_SnakeLadders_Reward_Manager;
	m_pRewardManager->LoadData( SNAKELADDER_REWARD_TABLE );

	for( int i = 0; i < m_pRewardManager->GetTotal(); i++ )
	{
		if( m_pRewardManager->GetAt( i )->RewardGroup == 0 )
			m_iFinalRewardCount++;
		else
			m_iNormalRewardCount++;
	}
}

void	ioDiceGame::Load()
{
	//LoadINI();
	LoadData();
}

void	ioDiceGame::ApplyDiceGamePacket( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	switch( rkPacket.GetPacketID() )
	{
	case STPK_DICE_GAME_GET_INFO:
		{
			int		iCommand;
			rkPacket >> iCommand;

			if( iCommand == DICE_GAME_GET_INFO_LIST )
			{
				int		iTraceCount;

				rkPacket >> m_iCurrentPosition >> iTraceCount;

				for( int i = 0; i <= MAX_SLOT; i++ )
				{
					m_TraceMap[i]	= false;
				}

				int	iTrace;
				for( int i = 0; i < iTraceCount; i++ )
				{
					rkPacket >> iTrace;
					m_TraceMap[iTrace]	= true;
				}

				rkPacket >> m_byBoardIndex >> m_iFinalReward;

				for( int i = 0; i < MAX_REWARD_INDEX; i++ )
				{
					rkPacket >> m_iRewardIndex[i];
				}

				DiceGameWnd*	pWnd	= dynamic_cast<DiceGameWnd*>( g_GUIMgr.FindWnd( DICE_GAME_WND ) );
				if( pWnd )
					pWnd->ShowWnd();
			}
		}
		break;

	case STPK_DICE_GAME_THROW_DICE:
		{
			int		iCommand;
			rkPacket >> iCommand;

			int	iMidPosition, iEndPosition;
			int	iRewardIndex, iRewardType, iRewardValue1, iRewardValue2;
			if( iCommand == DICE_GAME_THROW_DICE_OK )
			{
				rkPacket >> m_iCurrentPosition >> m_iCurrentDice >> iMidPosition >> iEndPosition >> iRewardIndex >> iRewardType >> iRewardValue1 >> iRewardValue2;

				if( iEndPosition == 0 )
					m_iCurrentPosition	= iMidPosition;
				else
					m_iCurrentPosition	= iEndPosition;
				m_TraceMap[iMidPosition]	= true;

				DiceGameWnd*	pWnd	= dynamic_cast<DiceGameWnd*>( g_GUIMgr.FindWnd( DICE_GAME_WND ) );
				if( pWnd )
					pWnd->SetDiceRoll( m_iCurrentDice, iMidPosition, iEndPosition );
			}
			else if( iCommand == DICE_GAME_ETCITEM_USE_DONT_HAVE )
			{
				DiceGameWnd*	pWnd	= dynamic_cast<DiceGameWnd*>( g_GUIMgr.FindWnd( DICE_GAME_WND ) );
				if( pWnd )
					pWnd->SetErrorMsg( DiceGameWnd::DICE_GAME_ERROR_DICE );
			}
		}
		break;

	case STPK_DICE_GAME_EXCHANGE_REWARD_LIST:
		{
			int		iCommand;
			rkPacket >> iCommand;

			if( iCommand == DICE_GAME_EXCHANGE_REWARD_LIST_OK )
			{
				rkPacket >> m_iFinalReward;

				for( int i = 0; i < MAX_REWARD_INDEX; i++ )
				{
					rkPacket >> m_iRewardIndex[i];

					DiceGameWnd*	pWnd	= dynamic_cast<DiceGameWnd*>( g_GUIMgr.FindWnd( DICE_GAME_WND ) );
					if( pWnd )
						pWnd->ChangeRewardList();
				}
			}
			else if( iCommand == DICE_GAME_ETCITEM_USE_DONT_HAVE )
			{
				DiceGameWnd*	pWnd	= dynamic_cast<DiceGameWnd*>( g_GUIMgr.FindWnd( DICE_GAME_WND ) );
				if( pWnd )
					pWnd->SetErrorMsg( DiceGameWnd::DICE_GAME_ERROR_REWARD );
			}
		}
		break;

	case STPK_DICE_GAME_EXCHANGE_BOARD:
		{
			int		iCommand;
			rkPacket >> iCommand;

			BYTE	byPrevBoardIndex;
			if( iCommand == DICE_GAME_EXCHANGE_BOARD_OK )
			{
				rkPacket >> byPrevBoardIndex >> m_byBoardIndex;

				DiceGameWnd*	pWnd	= dynamic_cast<DiceGameWnd*>( g_GUIMgr.FindWnd( DICE_GAME_WND ) );
				if( pWnd )
					pWnd->ChangeBoard();
			}
			else if( iCommand == DICE_GAME_ETCITEM_USE_DONT_HAVE )
			{
				DiceGameWnd*	pWnd	= dynamic_cast<DiceGameWnd*>( g_GUIMgr.FindWnd( DICE_GAME_WND ) );
				if( pWnd )
					pWnd->SetErrorMsg( DiceGameWnd::DICE_GAME_ERROR_BOARD );
			}
		}
		break;

	case STPK_DICE_GAME_COMPLETE_DONE:
		{
			
		}
		break;

	case STPK_DICE_GAME_END:
		{

		}
		break;

	case STPK_DICE_GAME_RESTART_GET_INFO:
		{
			int		iCommand;
			rkPacket >> iCommand;

			if( iCommand == DICE_GAME_GET_INFO_LIST )
			{
				int		iTraceCount;

				rkPacket >> m_iCurrentPosition >> iTraceCount;

				for( int i = 0; i <= MAX_SLOT; i++ )
				{
					m_TraceMap[i]	= false;
				}

				int	iTrace;
				for( int i = 0; i < iTraceCount; i++ )
				{
					rkPacket >> iTrace;
					m_TraceMap[iTrace]	= true;
				}

				rkPacket >> m_byBoardIndex >> m_iFinalReward;

				for( int i = 0; i < MAX_REWARD_INDEX; i++ )
				{
					rkPacket >> m_iRewardIndex[i];
				}

				DiceGameWnd*	pWnd	= dynamic_cast<DiceGameWnd*>( g_GUIMgr.FindWnd( DICE_GAME_WND ) );
				if( pWnd )
					pWnd->ShowWnd();
			}
		}
		break;

	default:
		break;
	}
}

void	ioDiceGame::SendOpenGame()
{
	TCPNetwork::MouseBusy( true );
	SP2Packet	kPacket( CTPK_DICE_GAME_START_GET_NFO );
	TCPNetwork::SendToServer( kPacket );
}

void	ioDiceGame::SendDiceRoll()
{
	ioUserEtcItem*	pUserEct	= g_MyInfo.GetUserEtcItem();
	ioUserEtcItem::ETCITEMSLOT kSlot;
	if( pUserEct )
	{
		if( pUserEct->GetEtcItem( ioEtcItem::EIT_ETC_DICE_GAME_DICE, kSlot ) )
		{
			if( kSlot.GetUse() )
			{
				TCPNetwork::MouseBusy( true );
				SP2Packet	kPacket( CTPK_DICE_GAME_THROW_DICE );
				kPacket << ioEtcItem::EIT_ETC_DICE_GAME_DICE;
				kPacket << m_iCurrentPosition;
				TCPNetwork::SendToServer( kPacket );
			}
		}
		else
		{
			DiceGameWnd*	pWnd	= dynamic_cast<DiceGameWnd*>( g_GUIMgr.FindWnd( DICE_GAME_WND ) );
			if( pWnd )
				pWnd->SetErrorMsg( DiceGameWnd::DICE_GAME_ERROR_DICE );
		}
	}
}

void	ioDiceGame::SendRewardChange()
{
	ioUserEtcItem*	pUserEct	= g_MyInfo.GetUserEtcItem();
	ioUserEtcItem::ETCITEMSLOT kSlot;
	if( pUserEct )
	{
		if( pUserEct->GetEtcItem( ioEtcItem::EIT_ETC_DICE_GAME_REWARD_CHANGE, kSlot ) )
		{
			if( kSlot.GetUse() )
			{
				TCPNetwork::MouseBusy( true );
				SP2Packet	kPacket( CTPK_DICE_GAME_EXCHANGE_REWARD_LIST );
				kPacket << ioEtcItem::EIT_ETC_DICE_GAME_REWARD_CHANGE;
				TCPNetwork::SendToServer( kPacket );
			}
		}
		else
		{
			DiceGameWnd*	pWnd	= dynamic_cast<DiceGameWnd*>( g_GUIMgr.FindWnd( DICE_GAME_WND ) );
			if( pWnd )
				pWnd->SetErrorMsg( DiceGameWnd::DICE_GAME_ERROR_REWARD );
		}
	}
}

void	ioDiceGame::SendBoardChange()
{
	ioUserEtcItem*	pUserEct	= g_MyInfo.GetUserEtcItem();
	ioUserEtcItem::ETCITEMSLOT kSlot;
	if( pUserEct )
	{
		if( pUserEct->GetEtcItem( ioEtcItem::EIT_ETC_DICE_GAME_BOARD_CHANGE, kSlot ) )
		{
			if( kSlot.GetUse() )
			{
				TCPNetwork::MouseBusy( true );
				SP2Packet	kPacket( CTPK_DICE_GAME_EXCHANGE_BOARD );
				kPacket << ioEtcItem::EIT_ETC_DICE_GAME_BOARD_CHANGE;
				kPacket << m_byBoardIndex;
				TCPNetwork::SendToServer( kPacket );
			}
		}
		else
		{
			DiceGameWnd*	pWnd	= dynamic_cast<DiceGameWnd*>( g_GUIMgr.FindWnd( DICE_GAME_WND ) );
			if( pWnd )
				pWnd->SetErrorMsg( DiceGameWnd::DICE_GAME_ERROR_BOARD );
		}
	}
}

void	ioDiceGame::SendGameEnd()
{
	//TCPNetwork::MouseBusy( true );
	SP2Packet	kPacket( CTPK_DICE_GAME_END );
	TCPNetwork::SendToServer( kPacket );
}

void	ioDiceGame::SendRestartGame()
{
	TCPNetwork::MouseBusy( true );
	SP2Packet	kPacket( CTPK_DICE_GAME_RESTART_GET_INFO );
	TCPNetwork::SendToServer( kPacket );
}

SHORT	ioDiceGame::GetSnakeLadderEndPoint( SHORT sStartPoint )
{
	if( !m_pMoveManager )
		return 0 ;

	for( int i = 0; i < m_pMoveManager->GetTotal(); i++ )
	{
		LSC_SnakeLadders_Move*	pMove	= m_pMoveManager->GetAt( i );

		if( pMove && pMove->SnakeGroup == m_byBoardIndex && pMove->SnakeStart == sStartPoint )
			return	pMove->SnakeEnd;
	}

	return	0;
}

bool	ioDiceGame::IsSnakeLadderPoint( SHORT sPoint )
{
	for( int i = 0; i < m_pMoveManager->GetTotal(); i++ )
	{
		LSC_SnakeLadders_Move*	pMove	= m_pMoveManager->GetAt( i );

		if( pMove && pMove->SnakeGroup == m_byBoardIndex && ( pMove->SnakeStart == sPoint || pMove->SnakeEnd == sPoint ) )
			return	true;
	}

	return	false;
}

int		ioDiceGame::GetDiceCount()
{
	int		iRtnValue;
	ioUserEtcItem*	pUserEct	= g_MyInfo.GetUserEtcItem();
	ioUserEtcItem::ETCITEMSLOT kSlot;
	if( pUserEct )
	{
		if( pUserEct->GetEtcItem( ioEtcItem::EIT_ETC_DICE_GAME_DICE, kSlot ) )
		{
			iRtnValue	= kSlot.GetUse();
		}
	}

	return	iRtnValue;
}

int		ioDiceGame::GetBoardChangeCount()
{
	int		iRtnValue;
	ioUserEtcItem*	pUserEct	= g_MyInfo.GetUserEtcItem();
	ioUserEtcItem::ETCITEMSLOT kSlot;
	if( pUserEct )
	{
		if( pUserEct->GetEtcItem( ioEtcItem::EIT_ETC_DICE_GAME_BOARD_CHANGE, kSlot ) )
		{
			iRtnValue	= kSlot.GetUse();
		}
	}

	return	iRtnValue;
}

int		ioDiceGame::GetRewardChangeCount()
{
	int		iRtnValue;
	ioUserEtcItem*	pUserEct	= g_MyInfo.GetUserEtcItem();
	ioUserEtcItem::ETCITEMSLOT kSlot;
	if( pUserEct )
	{
		if( pUserEct->GetEtcItem( ioEtcItem::EIT_ETC_DICE_GAME_REWARD_CHANGE, kSlot ) )
		{
			iRtnValue	= kSlot.GetUse();
		}
	}

	return	iRtnValue;
}

LSC_SnakeLadders_Reward*	ioDiceGame::GetRandomReward( bool bFianlReward )
{
	int	iIndex;

	if( bFianlReward )
		iIndex	= ( rand() % m_iFinalRewardCount ) + m_iNormalRewardCount;
	else
		iIndex	= rand() % m_iNormalRewardCount;

	return	m_pRewardManager->GetAt( iIndex );
}

LSC_SnakeLadders_Reward*	ioDiceGame::GetReward( int iIndex )
{
	if( iIndex == MAX_SLOT )
		return	m_pRewardManager->GetData( m_iFinalReward );
	else
		return	m_pRewardManager->GetData( m_iRewardIndex[iIndex] );
}

bool	ioDiceGame::GetTraceInfo( int iIndex )
{
	if( COMPARE( iIndex, 1, MAX_SLOT + 1 ) )
		return	m_TraceMap[iIndex];

	return	false;
}

bool	ioDiceGame::IsFinalRewardReceipt()
{
	if( m_TraceMap[MAX_SLOT] )
		return	true;

	return	false;
}