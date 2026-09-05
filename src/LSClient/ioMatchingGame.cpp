#include "StdAfx.h"
#include "ioMatchingGame.h"
#include "./GUI/MatchingGameWnd.h"
#include "ioEtcItem.h"


ioMatchingGame::ioMatchingGame(void)
{
	CellInfo	kCellInfo;

	for( int i = 0; i < MAX_NUMOF_CELL; i++ )
	{
		kCellInfo.m_bIsOpened	= false;
		kCellInfo.m_Status		= CARD_NONE;
		kCellInfo.m_iCardNum	= CARD_NUMBER_NOT_OPENED;

		m_CellInfoMap.insert( CellInfoMap::value_type( i, kCellInfo ) );
	}

	LoadProperty();
	Reset();
}


ioMatchingGame::~ioMatchingGame(void)
{
}

void	ioMatchingGame::LoadProperty()
{
	ioINILoader_e rkLoader( "config/sp2_matchinggame.ini" );

	rkLoader.SetTitle_e( "mission_info" );
	
	char	szBuf[MAX_PATH];
	ioEtcItem*	pEtcItem	= NULL;
	MissionInfo	missionInfo;
	int	iCount	= rkLoader.LoadFloat_e( "max_mission_info", 0 );

	for( int i = 0; i < iCount; i++ )
	{
		wsprintf( szBuf, "mission_type%d", i + 1 );
		missionInfo.m_iMissionType	= rkLoader.LoadInt( szBuf, 0 );

		wsprintf( szBuf, "clover_normal%d", i + 1 );
		missionInfo.m_iLucky1Node	= rkLoader.LoadInt( szBuf, 0 );

		wsprintf( szBuf, "clover_gold%d", i + 1 );
		missionInfo.m_iLucky2Node	= rkLoader.LoadInt( szBuf, 0 );

		wsprintf( szBuf, "reward_item_type%d", i + 1 );
		missionInfo.m_iRewardItemType	= rkLoader.LoadInt( szBuf, 0 );
		wsprintf( szBuf, "reward_item_value%d", i + 1 );
		missionInfo.m_dwRewardItemValue	= (DWORD)( rkLoader.LoadInt( szBuf, 0 ) );
		missionInfo.m_szRewardInfo.Clear();
		missionInfo.m_szRewardName.Clear();

		m_MissionVector.push_back( missionInfo );
	}

	rkLoader.SetTitle_e( "reward_info" );

	iCount	= rkLoader.LoadInt_e( "max_reward_info", 0 );

	for( int i = 0; i < iCount; i++ )
	{
		wsprintf( szBuf, "reward_item_type%d", i + 1 );
		m_iRewardType[i]	= rkLoader.LoadInt( szBuf, 0 );

		wsprintf( szBuf, "reward_item_value%d", i + 1 );
		m_iRewardValue[i]	= rkLoader.LoadInt( szBuf, 0 );
	}
}

void	ioMatchingGame::SetCellInfo( int iCell, int iCardNum )
{
	m_CellInfoMap[iCell].m_iCardNum	= iCardNum;
}

void	ioMatchingGame::OpenCard( int iCell )
{
	m_CellInfoMap[iCell].m_bIsOpened	= true;
}

void	ioMatchingGame::CloseCard()
{
	m_CellInfoMap[m_iFirstSelect].m_bIsOpened	= false;
	m_CellInfoMap[m_iSecondSelect].m_bIsOpened	= false;
	m_iFirstSelect	= m_iSecondSelect	= -1;
}

bool	ioMatchingGame::SelectCell( int iCell )
{
	bool	bSelectImmediately	= true;

	if( m_iFirstSelect == -1 )
	{
		SetFirstSelect( iCell );
		
		if( m_CellInfoMap[m_iFirstSelect].m_iCardNum == CARD_NUMBER_NOT_OPENED )
		{
			SendCellSelect( m_iFirstSelect );

			bSelectImmediately	= false;
		}
	}
	else if( m_iSecondSelect == -1 && iCell != m_iFirstSelect )
	{
		SetSecondSelect( iCell );

		SendCellSelect( m_iSecondSelect );

		if( m_CellInfoMap[m_iSecondSelect].m_iCardNum == CARD_NUMBER_NOT_OPENED )
			bSelectImmediately	= false;
	}

	return	bSelectImmediately;
}

void	ioMatchingGame::SetMissionInfo( int iType, int iLucky1Node, int iLucky2Node, int iLucky1CompleteNode, int iLucky2CompleteNode )
{
	m_CurrentMissionInfo.m_iMissionType	= iType;
	m_CurrentMissionInfo.m_iLucky1Node		= iLucky1Node;
	m_CurrentMissionInfo.m_iLucky2Node		= iLucky2Node;
	m_CurrentMissionInfo.m_iLucky1CompletedNode	= iLucky1CompleteNode;
	m_CurrentMissionInfo.m_iLucky2CompletedNode	= iLucky2CompleteNode;

	MissionVector::iterator	iter	= m_MissionVector.begin();
	for( ; iter != m_MissionVector.end(); iter++ )
	{
		if( m_CurrentMissionInfo.m_iMissionType == iter->m_iMissionType && m_CurrentMissionInfo.m_iLucky1Node == iter->m_iLucky1Node )
		{
			m_CurrentMissionInfo.m_iRewardItemType	= iter->m_iRewardItemType;
			m_CurrentMissionInfo.m_dwRewardItemValue	= iter->m_dwRewardItemValue;

			if( m_CurrentMissionInfo.m_iRewardItemType == PRESENT_ETC_ITEM )
			{
				ioEtcItem*	pEtcItem	= g_EtcItemMgr.FindEtcItem( m_CurrentMissionInfo.m_dwRewardItemValue );
				if( pEtcItem )
				{
					m_CurrentMissionInfo.m_szRewardName	= pEtcItem->GetName();
					m_CurrentMissionInfo.m_szRewardInfo	= pEtcItem->GetIconName();
				}
				else
				{
					m_CurrentMissionInfo.m_szRewardName.Clear();
					m_CurrentMissionInfo.m_szRewardInfo.Clear();
				}
			}
		}
	}
}

void	ioMatchingGame::UpdateMissionInfo( int iLucky1, int iLucky2 )
{
	m_CurrentMissionInfo.m_iLucky1CompletedNode	= iLucky1;
	m_CurrentMissionInfo.m_iLucky2CompletedNode	= iLucky2;
}

bool	ioMatchingGame::IsCellOpened( int iCell )
{
	bool	bOpened	= false;

	if( COMPARE( iCell, 0, MAX_NUMOF_CELL ) )
		bOpened	= m_CellInfoMap[iCell].m_bIsOpened;

	return	bOpened;
}

int		ioMatchingGame::GetCellStatus( int iCell )
{
	int	iStatus	= CARD_NONE;

	if( COMPARE( iCell, 0, MAX_NUMOF_CELL ) )
		iStatus	= m_CellInfoMap[iCell].m_Status;

	return	iStatus;
}

int		ioMatchingGame::GetCellCardNum( int iCell )
{
	int	iCardNum	= CARD_NUMBER_NOT_OPENED;

	if( COMPARE( iCell, 0, MAX_NUMOF_CELL ) )
		iCardNum	= m_CellInfoMap[iCell].m_iCardNum;

	return	iCardNum;
}

void	ioMatchingGame::SetFirstSelect( int iSelect )
{
	if( COMPARE( iSelect, 0, MAX_NUMOF_CELL ) )
		m_iFirstSelect	= iSelect;
}

void	ioMatchingGame::SetSecondSelect( int iSelect )
{
	if( COMPARE( iSelect, 0, MAX_NUMOF_CELL ) )
		m_iSecondSelect	= iSelect;
}

void	ioMatchingGame::Reset()
{
	m_dwTotalTime	= 0;
	m_iTicket	= 0;
	m_iCardMatchCount	= 0;
	m_Status	= MGS_GAME_PREPARE;
	m_iFirstSelect	= -1;
	m_iSecondSelect	= -1;

	m_CurrentMissionInfo.m_iMissionType	= MISSION_TYPE_NONE;
	m_CurrentMissionInfo.m_iLucky1Node		= 0;
	m_CurrentMissionInfo.m_iLucky2Node		= 0;
	m_CurrentMissionInfo.m_iLucky1CompletedNode	= 0;
	m_CurrentMissionInfo.m_iLucky2CompletedNode	= 0;
	m_CurrentMissionInfo.m_szRewardInfo.Clear();

	for( int i = 0; i < MAX_NUMOF_CELL; i++ )
	{
		m_CellInfoMap[i].m_bIsOpened	= false;
		m_CellInfoMap[i].m_Status		= CARD_NONE;
		m_CellInfoMap[i].m_iCardNum		= CARD_NUMBER_NOT_OPENED;
	}
	
}

void	ioMatchingGame::ApplyMatchingGamePacket( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	switch( rkPacket.GetPacketID() )
	{
	case STPK_CARD_MATCHING_GET_INFO:
		{
			int iCommand;
			rkPacket >> iCommand;

			if( iCommand == CARD_MATCHING_GET_INFO_SUCCESS )
			{
				Reset();

				DWORD	dwTotalTime;
				BYTE	 missionType, lucky1Node, lucky2Node, lucki1CompleteNode, lucky2CompleteNode;
				rkPacket >> missionType >> lucky1Node >> lucky2Node >> lucki1CompleteNode >> lucky2CompleteNode >> dwTotalTime;

				/*m_CurrentMissionInfo.m_iMissionType	= missionType;
				m_CurrentMissionInfo.m_iLucky1Node	= lucky1Node;
				m_CurrentMissionInfo.m_iLucky2Node	= lucky2Node;
				m_CurrentMissionInfo.m_iLucky1CompletedNode	= lucki1CompleteNode;
				m_CurrentMissionInfo.m_iLucky2CompletedNode	= lucky2CompleteNode;*/
				SetMissionInfo( missionType, lucky1Node, lucky2Node, lucki1CompleteNode, lucky2CompleteNode );

				m_dwTotalTime	= dwTotalTime * 1000;

				m_Status = MGS_GAME_INIT;

				MatchingGameWnd*	pWnd	= dynamic_cast<MatchingGameWnd*>( g_GUIMgr.FindWnd( MATCHING_GAME_WND ) );
				if( pWnd )
				{
					if( !pWnd->IsShow() )
						pWnd->ShowWnd();
				}
			}
		}
		break;

	case STPK_CARD_MATCHING_START_SET:
		{
			int	iCommand;
			rkPacket >> iCommand;

			if( iCommand == CARD_MATCHING_START_SET_SUCCESS )
			{
				int	iType;
				DWORD	dwTime;
				rkPacket >> iType >> dwTime;

				m_Status = MGS_GAME_PLAYING;
			}
			else if( iCommand == CARD_MATCHING_ETCITEM_USE_DONT_HAVE )
			{

			}
			else if( iCommand == CARD_MATCHING_ETCITEM_USE_EXCEPTION )
			{

			}
		}
		break;

	case STPK_CARD_MATCHING_CONFIRM_CARD_SINGLE:
		{
			int	iCommand;
			rkPacket >> iCommand;

			BYTE	confirmCard;
			int		iCardNum, iCardState, iMatchCount;

			if( iCommand == CARD_MATCHING_CONFIRM_CARD_SINGLE )
			{
				rkPacket >> confirmCard >> iCardNum >> iCardState >> iMatchCount;

				if( COMPARE( confirmCard, 0, m_CellInfoMap.size() ) )
				{
					m_CellInfoMap[confirmCard].m_iCardNum	= iCardNum;
					m_CellInfoMap[confirmCard].m_Status		= CARD_CONFIRMED;
					m_CellInfoMap[confirmCard].m_bIsOpened	= true;
				}

				m_iCardMatchCount	= iMatchCount;
			}
			else if( iCommand == CARD_MATCHING_CONFIRM_ERROR  )
			{
				rkPacket >> confirmCard;
			}
		}
		break;

	case STPK_CARD_MATCHING_CONFIRM_CARD_RESULT:
		{
			int	iCommand;
			rkPacket >> iCommand;

			BYTE	confirmCard1, confirmCard2;
			int		iCardNum1, iCardState1, iCardNum2, iCardState2, iMatchCount;

			if( iCommand == CARD_MATCHING_CONFIRM_CARD || iCommand == CARD_MATCHING_CARD_MATCHED_DONE )
			{
				rkPacket >> confirmCard1 >> iCardNum1 >> iCardState1 >> confirmCard2 >> iCardNum2 >> iCardState2 >> iMatchCount;

				if( COMPARE( confirmCard1, 0, m_CellInfoMap.size() ) )
				{
					m_CellInfoMap[confirmCard1].m_iCardNum	= iCardNum1;
					m_CellInfoMap[confirmCard1].m_bIsOpened	= true;
				}

				if( COMPARE( confirmCard2, 0, m_CellInfoMap.size() ) )
				{
					m_CellInfoMap[confirmCard2].m_iCardNum	= iCardNum2;
					m_CellInfoMap[confirmCard2].m_bIsOpened	= true;
				}

				m_iCardMatchCount	= iMatchCount;

				if( iCommand == CARD_MATCHING_CONFIRM_CARD )
				{
					m_CellInfoMap[confirmCard1].m_Status	= CARD_CONFIRMED;
					m_CellInfoMap[confirmCard2].m_Status	= CARD_CONFIRMED;
					m_CellInfoMap[confirmCard1].m_bIsOpened	= false;
					m_CellInfoMap[confirmCard2].m_bIsOpened	= false;
					m_iFirstSelect	= m_iSecondSelect	= -1;
				}
				else if( iCommand == CARD_MATCHING_CARD_MATCHED_DONE )
				{
					m_CellInfoMap[confirmCard1].m_Status	= CARD_MATCHED_DONE;
					m_CellInfoMap[confirmCard2].m_Status	= CARD_MATCHED_DONE;
					m_iFirstSelect	= m_iSecondSelect	= -1;
				}
			}
			else if( iCommand == CARD_MATCHING_CONFIRM_NONE )
			{
				rkPacket >> confirmCard1 >> iCardNum1 >> iCardState1 >> confirmCard2 >> iCardNum2 >> iCardState2 >> iMatchCount;

				if( COMPARE( confirmCard1, 0, m_CellInfoMap.size() ) )
				{
					m_CellInfoMap[confirmCard1].m_iCardNum	= iCardNum1;
					m_CellInfoMap[confirmCard1].m_bIsOpened	= false;
					m_CellInfoMap[confirmCard1].m_Status	= CARD_CONFIRMED;
				}

				if( COMPARE( confirmCard2, 0, m_CellInfoMap.size() ) )
				{
					m_CellInfoMap[confirmCard2].m_iCardNum	= iCardNum2;
					m_CellInfoMap[confirmCard2].m_bIsOpened	= false;
					m_CellInfoMap[confirmCard2].m_Status	= CARD_CONFIRMED;
				}

				m_iFirstSelect	= m_iSecondSelect	= -1;
				m_iCardMatchCount	= iMatchCount;
			}
			else if( iCommand == CARD_MATCHING_CONFIRM_ERROR )
			{
				rkPacket >> confirmCard1 >> confirmCard2;
			}
		}
		break;

	case STPK_CARD_MATCHING_END_RESULT:
		{
			int	iCommand, iMatchCount;
			BYTE	missionType, lucky1, lucky2, luck1Complete, luck2Complete;
			rkPacket >> iCommand >> missionType >> lucky1 >> lucky2 >> luck1Complete >> luck2Complete >> iMatchCount;

			if( iCommand == CARD_MATCHING_RESULT_COMPLETE_DONE )
			{
				UpdateMissionInfo( luck1Complete, luck2Complete );
			}
			else if( iCommand == CARD_MATCHING_RESULT_TIME_OUT_SERVER )
			{
				UpdateMissionInfo( luck1Complete, luck2Complete );
			}

			m_Status	= MGS_GAME_CLOSING;
		}
		break;
	}
}

void	ioMatchingGame::SendOpenGame()
{
	if( m_Status != MGS_GAME_PREPARE && m_Status != MGS_GAME_CLOSING )
		return;

	m_Status = MGS_GAME_PREPARE;

	TCPNetwork::MouseBusy( true );
	SP2Packet	kPacket( CTPK_CARD_MATCHING_GET_INFO );
	TCPNetwork::SendToServer( kPacket );
}

void	ioMatchingGame::SendGameStart( bool bPremium )
{
	if( m_Status != MGS_GAME_INIT )
		return;

	TCPNetwork::MouseBusy( true );
	SP2Packet	kPacket( CTPK_CARD_MATCHING_START_SET );

	if( bPremium )
		kPacket << ioEtcItem::EIT_ETC_CARD_MATCH_PREMIUM_TICKET;
	else
		kPacket << ioEtcItem::EIT_ETC_CARD_MATCH_TICKET;

	kPacket << REALGETTIME();

	TCPNetwork::SendToServer( kPacket );
}

void	ioMatchingGame::SendCellSelect( int iCell )
{
	if( m_Status != MGS_GAME_PLAYING )
		return;

	TCPNetwork::MouseBusy( true );
	if( m_iSecondSelect == -1 )
	{
		SP2Packet	kPacket( CTPK_CARD_MATCHING_CONFIRM_SINGLE );
		kPacket << (BYTE)( m_iFirstSelect );
		TCPNetwork::SendToServer( kPacket );
	}
	else
	{
		SP2Packet	kPacket( CTPK_CARD_MATCHING_CONFIRM_CARD );
		kPacket << (BYTE)( m_iFirstSelect ) << (BYTE)( m_iSecondSelect );
		TCPNetwork::SendToServer( kPacket );
	}
}

void	ioMatchingGame::SendEndGame()
{
	m_Status	= MGS_GAME_PREPARE;

	SP2Packet	kPacket( CTPK_CARD_MATCHING_END_REQUEST );
	kPacket << g_MyInfo.GetUserIndex();
	TCPNetwork::SendToServer( kPacket );
}

int		ioMatchingGame::GetTicketCount( bool bPremium )
{
	int	irtnValue	= 0;

	ioUserEtcItem*	pUserEct	= g_MyInfo.GetUserEtcItem();
	ioUserEtcItem::ETCITEMSLOT kSlot;
	if( pUserEct )
	{
		if( bPremium )
		{
			if( pUserEct->GetEtcItem( ioEtcItem::EIT_ETC_CARD_MATCH_PREMIUM_TICKET, kSlot ) )
			{
				irtnValue	= kSlot.GetUse();
			}
		}
		else
		{
			if( pUserEct->GetEtcItem( ioEtcItem::EIT_ETC_CARD_MATCH_TICKET, kSlot ) )
			{
				irtnValue	= kSlot.GetUse();
			}
		}
	}

	return	irtnValue;
}

DWORD	ioMatchingGame::GetTotalTime()
{
	return	m_dwTotalTime;
}

bool	ioMatchingGame::IsMissionComplete()
{
	bool	bRtnValue	= false;

	if( m_CurrentMissionInfo.m_iLucky1Node == m_CurrentMissionInfo.m_iLucky1CompletedNode &&
		m_CurrentMissionInfo.m_iLucky2Node == m_CurrentMissionInfo.m_iLucky2CompletedNode )
		bRtnValue	= true;

	return	bRtnValue;
}

const	ioHashString	ioMatchingGame::GetRewardIconName( int iReward )
{
	ioHashString	szReward;
	szReward.Clear();

	if( COMPARE( iReward, 0, MAX_REWARD ) )
	{
		if( m_iRewardType[iReward] == PRESENT_ETC_ITEM )
		{
			ioEtcItem*	pEtcItem	= g_EtcItemMgr.FindEtcItem( m_iRewardValue[iReward] );
			if( pEtcItem )
			{
				szReward	= pEtcItem->GetIconName();
			}
		}
	}

	return	szReward;
}