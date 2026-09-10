#include "StdAfx.h"

#include "../io3DEngine/ioSoundManager.h"
#include "../io3DEngine/HelpFunc.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3dengine/safesprintf.h"
#include "../NetworkWrappingFunc.h"

#include "../WndID.h"
#include "../ioSP2TCPSocket.h"
#include "../Setting.h"
#include "../SlangUtil.h"
#include "../ioMyInfo.h"
#include "../ChatMessageFunc.h"
#include "../EtcHelpFunc.h"
#include "../ioBattleRoomMgr.h"
#include "../ioRankBattleManager.h"

#include "ioSP2GUIManager.h"
#include "HelpWnd.h"
#include "MessengerWnd.h"
#include "MemoListWnd.h"
#include "ioBlackListManager.h"
#include "MannerTrialChat.h"

#include "ioMemoManager.h"

#include <strsafe.h>

MemoNode::MemoNode( ioHashString UserID, ioHashString Memo, DWORD dwTime, bool bOnline, bool bFromMemo )
{
	m_UserID	= UserID;
	m_Memo		= Memo;	
	m_dwMemoTime= dwTime;
	m_bOnline   = bOnline;
	m_bViewMemo = false;
	m_bFromMemo = bFromMemo;
}
MemoNode::~MemoNode(){}

UserLoginCheck::UserLoginCheck()
{
	
}

UserLoginCheck::~UserLoginCheck()
{
	m_vList.clear();
}

void UserLoginCheck::SendUserLogin( ioHashString szUserID )
{
	vUserLogin_iter iter = m_vList.begin();
	for( ; iter!=m_vList.end(); iter++ )
	{
		UserLogin &kInfo = *iter;

		if( kInfo.m_szUserID == szUserID )
		{
			if( FRAMEGETTIME() - kInfo.m_dwSendTimer > 5000 )        //
			{
				kInfo.m_dwSendTimer = FRAMEGETTIME();
				SP2Packet kPacket( CTPK_USER_LOGIN );
				kPacket << 1 << szUserID;
				TCPNetwork::SendToServer( kPacket );
			}
			return;
		}
	}

	UserLogin kInfo;
	kInfo.m_szUserID    = szUserID;
	kInfo.m_dwSendTimer = FRAMEGETTIME();
	m_vList.push_back( kInfo );

	SP2Packet kPacket( CTPK_USER_LOGIN );
	kPacket << 1 << szUserID;
	TCPNetwork::SendToServer( kPacket );
}

void UserLoginCheck::RecvUserLogin( ioHashString szUserID )
{
	vUserLogin_iter iter = m_vList.begin();
	for( ; iter!=m_vList.end(); iter++ )
	{
		if( iter->m_szUserID == szUserID )
		{
			iter->m_dwSendTimer = FRAMEGETTIME();				
			return;
		}
	}

	UserLogin kInfo;
	kInfo.m_szUserID    = szUserID;
	kInfo.m_dwSendTimer = FRAMEGETTIME();
	m_vList.push_back( kInfo );
}

void UserLoginCheck::ChangeName( const ioHashString &rszUserID, const ioHashString &rszNewUserID )
{
	vUserLogin_iter iter = m_vList.begin();
	for( ; iter!=m_vList.end(); iter++ )
	{
		if( iter->m_szUserID != rszUserID )
			continue;

		iter->m_szUserID = rszNewUserID;
		return;
	}
}
//////////////////////////////////////////////////////////////////////////
template<> ioMemoManager* Singleton< ioMemoManager >::ms_Singleton = 0;

ioMemoManager::ioMemoManager() : m_bUpdate( false )
{
	m_vMemoNode.reserve(1000);
	m_dwProcessOfflineSendTime = 0;
	m_iServerArray = 0;
}

ioMemoManager::~ioMemoManager()
{
	vMemoNode_iter iter;
	for( iter=m_vMemoNode.begin() ; iter!=m_vMemoNode.end() ; ++iter )
	{
		MemoNode *pItem = *iter;
		SAFEDELETE( pItem );
	}
	m_vMemoNode.clear();
}

ioMemoManager& ioMemoManager::GetSingleton()
{
	return Singleton< ioMemoManager >::GetSingleton();
}

void ioMemoManager::RemoveNode( MemoNode *node )
{
	vMemoNode_iter iter = std::find( m_vMemoNode.begin(), m_vMemoNode.end(), node );
	if( iter != m_vMemoNode.end() )
	{
		MemoNode *pItem = *iter;
		SAFEDELETE( pItem );
		m_vMemoNode.erase( iter );
		m_bUpdate = true;
		return;
	}
}

int ioMemoManager::GetOnlineMemoCount()
{
	int iCount = 0;
	vMemoNode_iter iter;
	for( iter=m_vMemoNode.begin() ; iter!=m_vMemoNode.end() ; ++iter )
	{
		MemoNode *pItem = *iter;
		if( pItem->IsOnline() )
		{
			iCount++;
		}
	}
	return iCount;
}

int ioMemoManager::GetNotViewMemoCount()
{
	int iCount = 0;
	vMemoNode_iter iter;
	for( iter=m_vMemoNode.begin() ; iter!=m_vMemoNode.end() ; ++iter )
	{
		MemoNode *pItem = *iter;
		if( !pItem->IsViewMemo() )
		{
			iCount++;
		}
	}
	return iCount;
}

MemoNode *ioMemoManager::GetNode( int iArray )
{
	int iSize = m_vMemoNode.size();
	if( !COMPARE( iArray, 0, iSize ) )
		return NULL;

	return m_vMemoNode[iArray];
}

bool ioMemoManager::IsLogIn( ioHashString szName )
{
	vMemoNode_iter iter;
	for( iter=m_vMemoNode.begin() ; iter!=m_vMemoNode.end() ; ++iter )
	{
		MemoNode *pItem = *iter;
		if( pItem->GetUserID() == szName )
		{
			return pItem->IsOnline();
		}
	}

	return false;
}

void ioMemoManager::OnLogInUser( ioHashString szName )
{
	vMemoNode_iter iter;
	for( iter=m_vMemoNode.begin() ; iter!=m_vMemoNode.end() ; ++iter )
	{
		MemoNode *pItem = *iter;
		if( pItem->GetUserID() == szName )
		{
			pItem->SetOnline( true );
			m_UserLoginList.RecvUserLogin( szName );
			break;
		}
	}
}

void ioMemoManager::OnLogOutUser( ioHashString szName )
{
	vMemoNode_iter iter;
	for( iter=m_vMemoNode.begin() ; iter!=m_vMemoNode.end() ; ++iter )
	{
		MemoNode *pItem = *iter;
		if( pItem->GetUserID() == szName )
		{
			pItem->SetOnline( false );
			m_UserLoginList.RecvUserLogin( szName );
			break;
		}
	}
}

void ioMemoManager::ShowMemoPopup( MemoNode *node )
{
	if( node == NULL ) return;
	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		if( g_BattleRoomMgr.IsTournamentModeType() || g_BattleRoomMgr.IsBroadcastModeType() || g_RankBattleMgr.IsRankBattlePlaying()) 
			return;
	}

	//
	MessengerPopup *pPopup = dynamic_cast<MessengerPopup*>(g_GUIMgr.FindWnd( MESSENGER_POPUP ));
	if( pPopup )
	{
		pPopup->ShowPopup( MessengerPopup::MEMO_POPUP, node->GetUserID(), "" );
	}
}

void ioMemoManager::SendOfflineMemo()
{
	if( g_TCPSocketMgr.IsChangeServer() )
		return;

	//쪽지 요청.
	SP2Packet kPacket( CTPK_OFFLINE_MEMO_MSG );
	kPacket << m_iServerArray << MAX_RECV_MEMO_SIZE;
	m_SendOfflineMemoList.push_back( kPacket );         // 리스트에 담아놓고 0.5초에 한번씩 콜.
}

void ioMemoManager::SendUserLogin( ioHashString szName )
{
	m_UserLoginList.SendUserLogin( szName );
}

void ioMemoManager::SendMemo( ioHashString szFromID, ioHashString szMemo )
{
	char szMemoBuf[MAX_PATH] = "";
	ioHashString szOwnerMemo = szMemo;
	if( !Setting::IsSlangFilterOther() )
	{
		StringCbCopy( szMemoBuf, MAX_PATH, g_SlangUtil.ConvertStringWithPreLine( szMemo.c_str() ) );
		szOwnerMemo = szMemoBuf;
	}

	SP2Packet kPacket( CTPK_MEMO_SEND_MSG );
	kPacket << szFromID << szOwnerMemo;
	TCPNetwork::SendToServer( kPacket );

	g_ChatMgr.SendChatLog( ioMannerTrialChatManager::TT_MEMO, g_MyInfo.GetPublicID().c_str(), szOwnerMemo.c_str(), MT_NONE, -1 );

	DWORD dwTime;
	SYSTEMTIME st;
	GetLocalTime( &st );
	dwTime = (st.wMonth * 1000000) + (st.wDay * 10000) + (st.wHour * 100) + st.wMinute;

	MemoNode *pNode = new MemoNode( szFromID, szOwnerMemo, dwTime, false, false );
	pNode->SetViewMemo( true );
	m_vMemoNode.insert( m_vMemoNode.begin(), pNode );
	std::sort( m_vMemoNode.begin(), m_vMemoNode.end(), MemoNodeSort() );	
	m_bUpdate = true;
}

void ioMemoManager::OnlineMemo( SP2Packet &rkPacket )
{
	DWORD dwTime;
	bool bOnline;
	ioHashString szFromID, szMsg;	
	rkPacket >> szFromID >> szMsg >> dwTime >> bOnline;

	if( szFromID.IsEmpty() ) return;
	if( g_BlackListManager.IsBlackList(szFromID) )
		return;

	g_MannerTrialChatMgr.Add( ioMannerTrialChatManager::TT_MEMO, szFromID.c_str(), szMsg.c_str() ); // 욕필터전에 할것

	MemoNode *pNode = new MemoNode( szFromID, g_SlangUtil.ConvertString( szMsg.c_str() ), dwTime, bOnline, true );
	m_vMemoNode.insert( m_vMemoNode.begin(), pNode );
	std::sort( m_vMemoNode.begin(), m_vMemoNode.end(), MemoNodeSort() );	
	m_UserLoginList.RecvUserLogin( szFromID );
	
	if( !g_MyInfo.IsBroadcastUser() )         //방송 유저에게는 팝업 띄우지 않음.
	{
		ShowMemoPopup( pNode );	
		g_SoundMgr.PlaySound( "interface\\team_invite.wav" );
	}
	MessengerBtn::sg_bMessenger = true;
	m_bUpdate = true;
}

void ioMemoManager::OfflineMemo( SP2Packet &rkPacket )
{
	int iCurSize;
	rkPacket >> iCurSize;

	for(int i = 0;i < iCurSize;i++)
	{
		DWORD dwTime;
		bool bOnline;
		ioHashString szFromID, szMsg;	
		rkPacket >> szFromID >> szMsg >> dwTime >> bOnline;
		if( szFromID.IsEmpty() ) continue;
		if( g_BlackListManager.IsBlackList(szFromID) ) continue;

		g_MannerTrialChatMgr.Add( ioMannerTrialChatManager::TT_MEMO, szFromID.c_str(), szMsg.c_str() ); // 욕필터전에 할것

		m_vMemoNode.push_back( new MemoNode( szFromID, szMsg, dwTime, bOnline, true ) );	
		std::sort( m_vMemoNode.begin(), m_vMemoNode.end(), MemoNodeSort() );
		m_UserLoginList.RecvUserLogin( szFromID );
	}	
	if( iCurSize < MAX_RECV_MEMO_SIZE )
		m_iServerArray++;
	SendOfflineMemo();
	
	if( iCurSize == 0 )
		return;
	MessengerBtn::sg_bMessenger = true;
	m_bUpdate = true;
}

void ioMemoManager::ChangeName( const ioHashString &rszUserID, const ioHashString &rszNewUserID )
{
	m_UserLoginList.ChangeName( rszUserID, rszNewUserID );
}

void ioMemoManager::ProcessMemo()
{
	if( FRAMEGETTIME() - m_dwProcessOfflineSendTime > OFFLINE_MEMO_SEND_TIME )
	{
		m_dwProcessOfflineSendTime = FRAMEGETTIME();

		if( !m_SendOfflineMemoList.empty() )
		{
			SP2Packet &rkPacket = *m_SendOfflineMemoList.begin();
			TCPNetwork::SendToServer( rkPacket );

			m_SendOfflineMemoList.erase( m_SendOfflineMemoList.begin() );
		}
	}

}