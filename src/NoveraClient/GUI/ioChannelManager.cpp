#include "StdAfx.h"

#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioSoundManager.h"
#include "../io3DEngine/HelpFunc.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3dengine/safesprintf.h"
#include "../NetworkWrappingFunc.h"

#include "../WndID.h"
#include "../ioMyInfo.h"
#include "../EtcHelpFunc.h"
#include "../TextColorTable.h"
#include "../ioBattleRoomMgr.h"
#include "../ioRankBattleManager.h"

#include "ioSP2GUIManager.h"
#include "HelpWnd.h"
#include "MessengerWnd.h"
#include "ioBlackListManager.h"

#include "ioChannelManager.h"
#include <strsafe.h>

ChannelNode::ChannelNode() : m_bUpdate( false )
{
	m_iChannelIdx	    = -1;	
	m_iUserSize         = 0;
	m_dwSentenceIndexer = 0;
}

ChannelNode::~ChannelNode()
{
	m_vUserID.clear();
	m_szManToManID.Clear();

	vChatInfo_iter iter;
	for(iter = m_vChatInfo.begin();iter != m_vChatInfo.end();iter++)
	{
		SAFEDELETE( *iter );
	}
	m_vChatInfo.clear();
}

void ChannelNode::GetUserName( char *szName, int iMaxSize )
{
	char szBuf[MAX_PATH];
	memset( szBuf, 0, MAX_PATH );

	if( GetUserSize() == 2 )
	{
		if( !m_szManToManID.IsEmpty() )
		{
			sprintf( szName, "%s", m_szManToManID.c_str() );
			return;
		}
		else
		{
			vStringNode_iter iter;
			for(iter = m_vUserID.begin();iter != m_vUserID.end();)
			{
				ioHashString &kString = *iter++;
				if( kString == g_MyInfo.GetPublicID() )
					continue;

				sprintf( szName, "%s ", kString.c_str() );
				return;
			}
		}
	}
	
	if( !m_szManToManID.IsEmpty() )
	{
		sprintf_e( szBuf, "%s / ", m_szManToManID.c_str() );
		strcat( szName, szBuf );
	}

	vStringNode_iter iter;
	for(iter = m_vUserID.begin();iter != m_vUserID.end();)
	{
		ioHashString &kString = *iter++;
		if( iter == m_vUserID.end() )
			sprintf( szBuf, "%s ", kString.c_str() );
		else
			sprintf_e( szBuf, "%s / ", kString.c_str() );

		strcat( szName, szBuf );
		if( (int)strlen( szName ) >= iMaxSize )
			break;
	}
}

int ChannelNode::GetUserSize()
{
	int iUserSize = m_iUserSize;
	if( GetManToManID().IsEmpty() )
		return iUserSize;
	return iUserSize + 1;
}

ChatInfo *ChannelNode::GetChatInfo( int iArray )
{
	if( !COMPARE( iArray, 0, (int)m_vChatInfo.size() ) )
		return NULL;
	return m_vChatInfo[iArray];
}


bool ChannelNode::IsChannelUser( ioHashString szName )
{
	vStringNode_iter iter;
	for(iter = m_vUserID.begin();iter != m_vUserID.end();iter++)
	{
		ioHashString &kString = *iter;
		if( szName == kString )
			return true;
	}

	if( GetManToManID() == szName )
		return true;

	return false;
}

void ChannelNode::JoinUser( ioHashString szID )
{
	if( GetManToManID() == szID )
		SetManToManID( "" );

	if( IsChannelUser( szID ) )
		return; //

	m_vUserID.push_back( szID );
	m_bUpdate = true;
	MessengerBtn::sg_bMessenger = true;
}

void ChannelNode::LeaveUser( ioHashString szID )
{
	vStringNode_iter iter;
	for(iter = m_vUserID.begin();iter != m_vUserID.end();iter++)
	{
		ioHashString &kString = *iter;
		if( kString == szID )
		{
			m_vUserID.erase( iter );
			m_bUpdate = true;
			MessengerBtn::sg_bMessenger = true;
			return;
		}
	}
}

void ChannelNode::SetChatInfo( ioComplexStringPrinter &szChat , const char *szID , bool bFirst )
{
	ChatInfo *pChatNode = new ChatInfo;
	if( !pChatNode ) return;

	pChatNode->m_Index  = m_dwSentenceIndexer;
	pChatNode->m_bFirst = bFirst;
	szChat.CopyFullText( pChatNode->m_Printer );
	szChat.CopyFullText( pChatNode->m_OverPrinter );
	pChatNode->m_szID = szID;
	pChatNode->m_OverPrinter.SetFullTextColor( 0xCE4E00 );    //오버는 주황색

	m_vChatInfo.push_back( pChatNode );
	m_bUpdate = true;
	MessengerBtn::sg_bMessenger = true;	
}

void ChannelNode::AddSentenceIndex()
{
	m_dwSentenceIndexer++;
}

bool ChannelNode::IsMannerTrialChatHaveID( const ioHashString &rszID )
{
	enum { MAX_CHECK_CHAT = 20, };

	int iSize = GetChatInfoSize();
	int iStart = 0;
	if( iSize > MAX_CHECK_CHAT )
		iStart = iSize - MAX_CHECK_CHAT;

	for( int i = iStart; i < iSize; i++ )
	{
		ChatInfo *pChatInfo = GetChatInfo( i );
		if( !pChatInfo ) continue;
		if( rszID == pChatInfo->m_szID )
		{
			return true;
		}
	}
	
	return false;
}

void ChannelNode::GetMannerTrialChat( OUT ioHashString &rkChatList )
{
	std::string szMerge;

	enum { MAX_CHECK_CHAT = 20, };

	int iSize = GetChatInfoSize();
	int iStart = 0;
	if( iSize > MAX_CHECK_CHAT )
		iStart = iSize - MAX_CHECK_CHAT;

	for( int i = iStart; i < iSize; i++ )
	{
		ChatInfo *pChatInfo = GetChatInfo( i );
		if( !pChatInfo ) continue;
		if( pChatInfo->m_Printer.IsEmpty() ) continue;
		
		std::string szLine;
		pChatInfo->m_Printer.CopyOnlyText( szLine );
		szMerge += szLine;
		szMerge += "\n";
	}

	if( !szMerge.empty())
		rkChatList = szMerge.c_str();
}

template<> ioChannelManager* Singleton< ioChannelManager >::ms_Singleton = 0;

ioChannelManager::ioChannelManager()
{
	m_vChannelNode.reserve(MAX_CHANNEL);
}

ioChannelManager::~ioChannelManager()
{
	RemoveNode( NULL );
}

ioChannelManager& ioChannelManager::GetSingleton()
{
	return Singleton< ioChannelManager >::GetSingleton();
}

void ioChannelManager::RemoveNode( ChannelNode *node )
{
	if( node == NULL )
	{
		vChannelNode_iter iter;
		for( iter=m_vChannelNode.begin() ; iter!=m_vChannelNode.end() ; ++iter )
		{
			ChannelNode *pItem = *iter;
			SAFEDELETE( pItem );
		}
		m_vChannelNode.clear();
	}
	else
	{
		vChannelNode_iter iter = std::find( m_vChannelNode.begin(), m_vChannelNode.end(), node );
		if( iter != m_vChannelNode.end() )
		{
			ChannelNode *pItem = *iter;
			SAFEDELETE( pItem );
			m_vChannelNode.erase( iter );
			return;
		}
	}	
}

ChannelNode *ioChannelManager::GetNodeArray( int iArray )
{
	int iSize = m_vChannelNode.size();
	if( !COMPARE( iArray, 0, iSize ) )
		return NULL;

	return m_vChannelNode[iArray];
}

int ioChannelManager::GetIndexToArray( int iIndex )
{
	int iArray = -1;
	vChannelNode_iter iter;
	for( iter=m_vChannelNode.begin() ; iter!=m_vChannelNode.end() ; ++iter )
	{
		iArray++;
		ChannelNode *pItem = *iter;
		if( pItem->GetIndex() == iIndex )
		{
			return iArray;
		}
	}

	return iArray;
}

ChannelNode *ioChannelManager::GetNodeIndex( int iIndex )
{
	vChannelNode_iter iter;
	for( iter=m_vChannelNode.begin() ; iter!=m_vChannelNode.end() ; ++iter )
	{
		ChannelNode *pItem = *iter;
		if( pItem->GetIndex() == iIndex )
		{
			return pItem;
		}
	}

	return NULL;
}

ChannelNode *ioChannelManager::GetNodeUser( ioHashString szName, int iUserSize  )
{
	vChannelNode_iter iter;
	for( iter=m_vChannelNode.begin() ; iter!=m_vChannelNode.end() ; ++iter )
	{
		ChannelNode *pItem = *iter;
		if( pItem->IsChannelUser( szName ) )
		{
			if( pItem->GetUserSize() <= iUserSize )
				return pItem;
		}
	}

	return NULL;
}

bool ioChannelManager::IsUpdate( int iIndex )
{
	vChannelNode_iter iter;
	for( iter=m_vChannelNode.begin() ; iter!=m_vChannelNode.end() ; ++iter )
	{
		ChannelNode *pItem = *iter;
		if( pItem->GetIndex() == iIndex )
		{
			return pItem->IsUpdate();
		}
	}
	return false;
}

void ioChannelManager::ClearUpdate( int iIndex )
{
	vChannelNode_iter iter;
	for( iter=m_vChannelNode.begin() ; iter!=m_vChannelNode.end() ; ++iter )
	{
		ChannelNode *pItem = *iter;
		if( pItem->GetIndex() == iIndex )
		{
			pItem->ClearUpdate();
			break;
		}
	}
}

void ioChannelManager::LeaveExceptionChannel( ioHashString szManToManID, int iChangeIndex )
{
	vChannelNode_iter iter;
	for( iter=m_vChannelNode.begin() ; iter!=m_vChannelNode.end() ; ++iter )
	{
		ChannelNode *pItem = *iter;
		if( pItem->GetManToManID() == szManToManID )
		{
			if( pItem->GetUserSize() <= 2 )
			{
				SP2Packet kPacket( CTPK_CHANNEL_LEAVE );
				kPacket << pItem->GetIndex();
				TCPNetwork::SendToServer( kPacket );	
				
				int iPrevIndex = pItem->GetIndex();
				pItem->SetIndex( iChangeIndex );
				pItem->SetManToManID( "" );

				MessengerWnd *pWnd = dynamic_cast<MessengerWnd*>(g_GUIMgr.FindWnd( MESSENGER_WND ));
				if( pWnd && pWnd->IsShow() )
					pWnd->ChangeChannelIndex( iPrevIndex, iChangeIndex );
				return;
			}			
		}
	}
}

void ioChannelManager::ShowChatPopup( int iIndex, ioHashString szID, ioHashString szChat )
{
	MessengerPopup *pPopup = dynamic_cast<MessengerPopup*>(g_GUIMgr.FindWnd( MESSENGER_POPUP ));
	if( !pPopup )	return;
	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		if( g_BattleRoomMgr.IsTournamentModeType() || g_BattleRoomMgr.IsBroadcastModeType() || g_RankBattleMgr.IsRankBattlePlaying()) 
			return;
	}

	pPopup->ShowPopup( MessengerPopup::CHANNEL_POPUP, szID, szChat, iIndex );
}

void ioChannelManager::ShowInvitePopup( int iIndex, ioHashString szID, ioHashString szInvitedID )
{
	MessengerPopup *pPopup = dynamic_cast<MessengerPopup*>(g_GUIMgr.FindWnd( MESSENGER_POPUP ));
	if( !pPopup )	return;
	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		if( g_BattleRoomMgr.IsTournamentModeType() || g_BattleRoomMgr.IsBroadcastModeType() || g_RankBattleMgr.IsRankBattlePlaying()) 
			return;
	}

	pPopup->ShowPopup( MessengerPopup::CHANNEL_POPUP, szID, "", iIndex );
	g_SoundMgr.PlaySound( "interface\\team_invite.wav" );
}

void ioChannelManager::AddAnnounce( int iIndex, ioHashString szAnnounce )
{
	ChannelNode *pNode = GetNodeIndex( iIndex );
	if( pNode == NULL )
		return;

	pNode->AddSentenceIndex();

	MessengerBtn::sg_bMessenger = true;	

	const float fScale = FONT_SIZE_12;
	const float fWidhtSize = 265.0f;
	
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );

	char szSrcChat[MAX_PATH];
	SafeSprintf( szSrcChat, sizeof( szSrcChat ), STR(1), szAnnounce.c_str() );

	// 1라인을 넘어서지 않는다.
	if( g_FontMgr.GetTextWidth( szSrcChat, TS_NORMAL, fScale ) < fWidhtSize )
	{		
		kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( fScale, szSrcChat );
		pNode->SetChatInfo( kPrinter , "", true);
	}	
	else
	{
		int iLen    = strlen( szSrcChat );
		int iResult = 0;
		bool bFirst = true;
        while( iResult < iLen )
		{
			char szDstChat[MAX_PATH];
			memset( szDstChat, 0, sizeof( szDstChat ) );
			iResult += Help::StringCutFun( fScale, fWidhtSize, TS_NORMAL, szDstChat, sizeof(szDstChat), &szSrcChat[iResult] );
			
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );
			kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter.AddTextPiece( fScale, szDstChat );
			pNode->SetChatInfo( kPrinter ,"", bFirst);
			bFirst = false;
			kPrinter.ClearList();
		}
	}	
}

void ioChannelManager::AddChat( int iIndex, const char *szID, ioHashString szChat )
{
	ChannelNode *pNode = GetNodeIndex( iIndex );
	if( pNode == NULL )
		return;

	pNode->AddSentenceIndex();

	MessengerBtn::sg_bMessenger = true;
	if( !g_MyInfo.IsBroadcastUser() )
		g_SoundMgr.PlaySound( "interface\\team_invite.wav" );

	const float fScale = FONT_SIZE_12;
	const float fWidhtSize = 265.0f;
	float fIDWidth = 0.0f;
	
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );

	char szSrcChat[MAX_PATH];
	memset( szSrcChat, 0, sizeof( szSrcChat ) );
	sprintf( szSrcChat, "%s", szChat.c_str() );

	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( fScale, STR(1), szID );
	fIDWidth = kPrinter.GetFullWidth();
	
	// 1라인을 넘어서지 않는다.
	if( g_FontMgr.GetTextWidth( szSrcChat, TS_NORMAL, fScale ) < fWidhtSize - fIDWidth )
	{		
		kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( fScale, szSrcChat );
		pNode->SetChatInfo( kPrinter , szID, true );
	}	
	else
	{
		int iLen    = strlen( szSrcChat );
		int iResult = 0;
		bool bFirst = true;
		while( iResult < iLen )
		{
			char szDstChat[MAX_PATH];
			memset( szDstChat, 0, sizeof( szDstChat ) );
			iResult += Help::StringCutFun( fScale, fWidhtSize - fIDWidth, TS_NORMAL, szDstChat, sizeof(szDstChat), &szSrcChat[iResult] );
			
			fIDWidth = 0.0f;
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );
			kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter.AddTextPiece( fScale, szDstChat );
			pNode->SetChatInfo( kPrinter , szID, bFirst );
			bFirst = false;
			kPrinter.ClearList();			
		}
	}	
}

void ioChannelManager::OnInviteUser( int iIndex, ioHashString szInviteUser, ioHashString szJoinUser )
{
	if(g_BlackListManager.IsBlackList(szInviteUser) && szJoinUser == g_MyInfo.GetPublicID() ) // blacklist유저가 초대하면 바로 나감
	{
		LeaveByBlackList(iIndex);
		return;
	}

	ShowInvitePopup( iIndex, szInviteUser, szJoinUser );

	ChannelNode *pNode = GetNodeIndex( iIndex );
	if( pNode == NULL )	return;

	char szChat[MAX_PATH];
	SafeSprintf( szChat, sizeof( szChat ), STR(1), szInviteUser.c_str(), szJoinUser.c_str() );
    AddAnnounce( iIndex, szChat );
}

void ioChannelManager::OnJoinUser( int iIndex, int iUserSize, ioHashString szJoinUser, ioHashString szManToManID )
{
	ChannelNode *pNode = GetNodeIndex( iIndex );
	if( pNode == NULL )
	{
		ChannelNode *pItem = new ChannelNode;
		pItem->SetIndex( iIndex );
		pItem->SetUserSize( iUserSize );
		pItem->SetManToManID( szManToManID );
		pItem->JoinUser( szJoinUser );
		m_vChannelNode.push_back( pItem );			
	}	
	else
	{
		pNode->SetUserSize( iUserSize );
		pNode->SetManToManID( szManToManID );
		pNode->JoinUser( szJoinUser );
	}
}

void ioChannelManager::OnChannelAllUser( SP2Packet &rkPacket )
{
	int iIndex, iMaxUser;
	rkPacket >> iIndex >> iMaxUser;
	ChannelNode *pNode = GetNodeIndex( iIndex );
	if( pNode == NULL )
		return;

	bool isBlackList = false;
	for(int i = 0;i < iMaxUser;i++)
	{
		ioHashString szName;
		rkPacket >> szName;
		if(g_BlackListManager.IsBlackList(szName)) 
			isBlackList = true;
		pNode->JoinUser( szName );
	}

	if(isBlackList && iMaxUser == 2)// 블랙리스트 유저와 1:1 대화면 바로 나감
	{
		LeaveByBlackList( iIndex );
	}
}

void ioChannelManager::OnLeaveUser( int iIndex, int iUserSize, ioHashString szLeaveUser, ioHashString szManToManID )
{
	ChannelNode *pNode = GetNodeIndex( iIndex );
	if( pNode == NULL )
		return;

	if( g_MyInfo.GetPublicID() == szLeaveUser )
	{
		RemoveNode( pNode );
		MessengerWnd *pWnd = dynamic_cast<MessengerWnd*>(g_GUIMgr.FindWnd( MESSENGER_WND ));
		if( pWnd && pWnd->IsShow() )
			pWnd->Init();
		return;
	}
	pNode->SetUserSize( iUserSize );
	pNode->SetManToManID( szManToManID );	
	pNode->LeaveUser( szLeaveUser );

	if( iUserSize != 1 )
	{
		char szChat[MAX_PATH];
		SafeSprintf( szChat, sizeof( szChat ), STR(1), szLeaveUser.c_str() );
		AddAnnounce( iIndex, szChat );
	}	
}

void ioChannelManager::OnChat( int iIndex, ioHashString szID, ioHashString szChat )
{
	if(g_BlackListManager.IsBlackList(szID)) return;

	AddChat( iIndex, szID.c_str(), szChat );
	if( szID != g_MyInfo.GetPublicID() )
		ShowChatPopup( iIndex, szID, szChat );
}

void ioChannelManager::LeaveByBlackList( int iIndex )
{
	ChannelNode *pNode = GetNodeIndex( iIndex );
	if( !pNode ) return;

	RemoveNode( pNode );
	
	SP2Packet kPacket( CTPK_CHANNEL_LEAVE );
	kPacket << iIndex;
	TCPNetwork::SendToServer( kPacket );		

	MessengerWnd *pMessengerWnd = dynamic_cast<MessengerWnd*>(g_GUIMgr.FindWnd(MESSENGER_WND));
	if( pMessengerWnd )
		pMessengerWnd->Init();	
	MessengerBtn::sg_bMessenger = false;
}