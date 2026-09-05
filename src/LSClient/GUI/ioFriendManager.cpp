#include "StdAfx.h"

#include "../ioGuildChatMgr.h"
#include "../ioDateHelp.h"

#include "UserListWnd.h"
#include "MannerValuationWnd.h"
#include "MessengerWnd.h"
#include "ioFriendManager.h"

FriendNode::FriendNode( int iIndex, DWORD dwUserIndex, ioHashString FriendID, bool bOnline, int iUserPos, int iGradeLevel, bool bOtherBF, DWORD dwRegTime,
						int iCloverSendCount, int iCloverSendTime, int iCloverRecvCount, int iCloverRecvTime, int iCloverRecvWaitCount )
{
	m_iIndex		= iIndex;
	m_dwUserIndex   = dwUserIndex;
	m_FriendID		= FriendID;
	m_bOnline		= bOnline;
	m_iUserPos      = iUserPos;
	m_iGradeLevel   = iGradeLevel;
	m_bOtherBF		= bOtherBF;
	m_dwRegTime     = dwRegTime;

	m_iCloverSendCount     = iCloverSendCount;
	m_iCloverSendDate     = iCloverSendTime;
 	m_iCloverRecvCount     = iCloverRecvCount;
 	m_iCloverRecvDate     = iCloverRecvTime;
 	m_iCloverRecvWaitCount = iCloverRecvWaitCount;

	m_iFriendRegSec = 0;
	m_iCloverSendSec = 0;
	m_iCloverRecvSec = 0;
}
FriendNode::~FriendNode(){}

void FriendNode::SetGradeLevel( int iGradeLevel )
{
	m_iGradeLevel = iGradeLevel;
	g_UserInfoMgr.SetGrade( m_FriendID, iGradeLevel );
}

void  FriendNode::DecreaseCloverSendSec()
{
	m_iCloverSendSec--;
	m_iCloverSendSec = max( 0, m_iCloverSendSec );
}

void  FriendNode::DecreaseCloverRecvSec()
{
	m_iCloverRecvSec--;
	m_iCloverRecvSec = max( 0, m_iCloverRecvSec );
}

void  FriendNode::DecreaseFriendRegSec()
{
	m_iFriendRegSec--;
	m_iFriendRegSec = max( 0, m_iFriendRegSec );
}

void FriendNode::ApplyFriendClover( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	// Send, Recv 처리.
	int iType;
	int iCloverSendCount, iCloverSendDate, iCloverRecvCount, iCloverRecvDate, iCloverRecvWaitCount;
	rkPacket >> iType >> iCloverSendCount >> iCloverSendDate >> iCloverRecvCount >> iCloverRecvDate >> iCloverRecvWaitCount;
	m_iCloverSendCount     = iCloverSendCount;
	m_iCloverSendDate      = iCloverSendDate;
	m_iCloverRecvCount     = iCloverRecvCount;
	m_iCloverRecvDate      = iCloverRecvDate;
	m_iCloverRecvWaitCount = iCloverRecvWaitCount;

	ioClover *pClover = g_MyInfo.GetClover();
	if( !pClover ) return;

	switch( iType )
	{
	case ioClover::FRIEND_CLOVER_SEND_SUCCESS:
		m_iCloverSendSec = pClover->GetSendTimeGap() * 60;
		break;
	case ioClover::FRIEND_CLOVER_SEND_FAIL_ALREADY_SEND:
		InitCloverSendSec();
		break;
	case ioClover::FRIEND_CLOVER_SEND_FAIL_FRIEND_REG_TIME:
		InitCloverRegSec();
		break;
	case ioClover::FRIEND_CLOVER_RECV_FAIL_NOT_CLOVER:
		InitCloverRecvSec();
		break;
	case ioClover::FRIEND_CLOVER_COME_TO_FRIEND:
		m_iCloverRecvSec = pClover->GetRecvTimeGap() * 60;
		break;
	}
}

void FriendNode::InitCloverSendSec()
{
	// 서버시간
	int iEnableSendTime = 0;
	ioClover *pClover = g_MyInfo.GetClover();
	if( pClover )
		iEnableSendTime = pClover->GetSendTimeGap();

	time_t kCurServerTime = g_MyInfo.GetServerDate();
	kCurServerTime += ( REALGETTIME()/1000 );

	int iYear, iMonth, iDay, iHour, iMinute;
	Help::GetParsingDate( m_iCloverSendDate, 2010, iYear, iMonth, iDay, iHour, iMinute );
	time_t kSendTime = DateHelp::ConvertSecondTime( iYear, iMonth, iDay, iHour, iMinute, 0 );

	kSendTime += iEnableSendTime * 60;

	int iGapSec = kSendTime - kCurServerTime;
	if( iGapSec < iEnableSendTime * 60 )
	{
		iGapSec = max( 0, iGapSec );
		m_iCloverSendSec = iGapSec;
	}
}

void FriendNode::InitCloverRecvSec()
{
	// 서버시간
	int iEnableRecvTime = 0;
	ioClover *pClover = g_MyInfo.GetClover();
	if( pClover )
		iEnableRecvTime = pClover->GetRecvTimeGap();

	time_t kCurServerTime = g_MyInfo.GetServerDate();
	kCurServerTime += ( REALGETTIME()/1000 );

	int iYear, iMonth, iDay, iHour, iMinute;
	Help::GetParsingDate( m_iCloverRecvDate, 2010, iYear, iMonth, iDay, iHour, iMinute );
	time_t kRecvTime = DateHelp::ConvertSecondTime( iYear, iMonth, iDay, iHour, iMinute, 0 );
	kRecvTime += iEnableRecvTime * 60;

	int iGapSec = kRecvTime - kCurServerTime;
	if( iGapSec < iEnableRecvTime * 60 )
	{
		iGapSec = max( 0, iGapSec );
		m_iCloverRecvSec = iGapSec;
	}
}
void FriendNode::InitCloverRegSec()
{
	// 서버시간
	int iAbusingTime = 0;
	ioClover *pClover = g_MyInfo.GetClover();
	if( pClover )
		iAbusingTime = pClover->GetAbusingTime();

	time_t kCurServerTime = g_MyInfo.GetServerDate();
	kCurServerTime += ( REALGETTIME()/1000 );

	int iYear, iMonth, iDay, iHour, iMinute;
	Help::GetParsingDate( m_dwRegTime, 2000, iYear, iMonth, iDay, iHour, iMinute );
	time_t kRegTime = DateHelp::ConvertSecondTime( iYear, iMonth, iDay, iHour, iMinute, 0 );
	kRegTime += iAbusingTime * 60;

	int iGapSec = kRegTime - kCurServerTime;
	if( iGapSec < iAbusingTime * 60 )
	{
		iGapSec = max( 0, iGapSec );
		m_iFriendRegSec = iGapSec;
	}
}

template<> ioFriendManager* Singleton< ioFriendManager >::ms_Singleton = 0;

ioFriendManager::ioFriendManager()
{
	m_vFriendNode.reserve(1000);
	m_bLoadingCompelte   = false;
	m_dwLastLoadingTimer = 0;
	m_iMessengerState = NEW_START;
	m_iDefaultSlotSize   = 0;
	m_pPlayStage         = NULL;
	m_bBestFriendCheck   = true;
}

ioFriendManager::~ioFriendManager()
{
	RemoveNode( NULL );
	m_vFriendRequestUser.clear();
}

ioFriendManager& ioFriendManager::GetSingleton()
{
	return Singleton< ioFriendManager >::GetSingleton();
}

void ioFriendManager::RemoveNode( FriendNode *node )
{
	if( node == NULL )
	{
		vFriendNode_iter iter;
		for( iter=m_vFriendNode.begin() ; iter!=m_vFriendNode.end() ; ++iter )
		{
			FriendNode *pItem = *iter;
			SAFEDELETE( pItem );
		}
		m_vFriendNode.clear();
		m_bLoadingCompelte   = false;
		m_dwLastLoadingTimer = 0;
	}
	else
	{
		vFriendNode_iter iter = std::find( m_vFriendNode.begin(), m_vFriendNode.end(), node );
		if( iter != m_vFriendNode.end() )
		{
			FriendNode *pItem = *iter;
			SAFEDELETE( pItem );
			m_vFriendNode.erase( iter );
			return;
		}
	}	
}

void ioFriendManager::RemoveBestFriend( DWORD dwUserIndex )
{
	vBestFriend_iter iter;
	for(iter = m_vBestFriend.begin();iter != m_vBestFriend.end();iter++)
	{
		BestFriend &rkBestFriend = *iter;
		if( rkBestFriend.m_dwUserIndex == dwUserIndex )
		{
			m_vBestFriend.erase( iter );
			return;
		}
	}
}

int ioFriendManager::GetOnlineNodeSize()
{
	int iCount = 0;
	vFriendNode_iter iter;
	for( iter=m_vFriendNode.begin() ; iter!=m_vFriendNode.end() ; ++iter )
	{
		FriendNode *pItem = *iter;
		if( pItem->IsOnline() )
			iCount++;
	}
	return iCount;
}

const char *ioFriendManager::GetFriendRequestUser()
{
	if( m_vFriendRequestUser.empty() )
		return "";
	
	return m_vFriendRequestUser[0].m_szUserName.c_str();
}

const char *ioFriendManager::GetFriendRequestUser( DWORD &dwTableIndex, DWORD &dwUserIndex )
{
	dwTableIndex = dwUserIndex = 0;
	if( m_vFriendRequestUser.empty() )
		return "";

	dwTableIndex = m_vFriendRequestUser[0].m_dwTableIndex;
	dwUserIndex  = m_vFriendRequestUser[0].m_dwUserIndex;
	return m_vFriendRequestUser[0].m_szUserName.c_str();
}

DWORD ioFriendManager::GetFriendRequestLastIndex()
{
	DWORD dwLastIndex = MAX_FRIEND_IDX_VALUE;
	int iSize = m_vFriendRequestUser.size();
	for(int i = 0;i < iSize;i++)
	{
		if( m_vFriendRequestUser[i].m_dwTableIndex < dwLastIndex )
		{
			dwLastIndex = m_vFriendRequestUser[i].m_dwTableIndex;
		}
	}
	return dwLastIndex;
}

bool ioFriendManager::IsAlreadyRequestUser( DWORD dwTableIndex )
{
	int iSize = m_vFriendRequestUser.size();
	for(int i = 0;i < iSize;i++)
	{
		if( m_vFriendRequestUser[i].m_dwTableIndex == dwTableIndex )
		{
			return true;
		}
	}
	return false;
}

bool ioFriendManager::IsFriend( ioHashString szName )
{
	vFriendNode_iter iter;
	for( iter=m_vFriendNode.begin() ; iter!=m_vFriendNode.end() ; ++iter )
	{
		FriendNode *pItem = *iter;
		if( pItem->GetName() == szName )
			return true;
	}
	return false;
}

FriendNode *ioFriendManager::GetNode( int iArray )
{
	int iSize = m_vFriendNode.size();
	if( !COMPARE( iArray, 0, iSize ) )
		return NULL;

	return m_vFriendNode[iArray];
}

FriendNode *ioFriendManager::GetNodeOnlineUser( int iArray )
{
	vFriendNode_iter iter;
	for( iter=m_vFriendNode.begin() ; iter!=m_vFriendNode.end() ; ++iter )
	{
		FriendNode *pItem = *iter;
		if( pItem->IsOnline() )
		{
			if( iArray == 0 )
				return pItem;
			else
				iArray--;
		}
	}
	return NULL;
}

FriendNode *ioFriendManager::GetNode( const ioHashString &rkName )
{
	vFriendNode_iter iter;
	for( iter=m_vFriendNode.begin() ; iter!=m_vFriendNode.end() ; ++iter )
	{
		FriendNode *pItem = *iter;
		if( pItem->GetName() == rkName )
			return pItem;
	}

	return NULL;
}

FriendNode *ioFriendManager::GetNodeToUserIndex( const DWORD &dwUserIndex )
{
	vFriendNode_iter iter;
	for( iter=m_vFriendNode.begin() ; iter!=m_vFriendNode.end() ; ++iter )
	{
		FriendNode *pItem = *iter;
		if( pItem->GetUserIndex() == dwUserIndex )
			return pItem;
	}

	return NULL;
}

DWORD ioFriendManager::GetBestFriendState( const ioHashString &rkName )
{
	FriendNode *pNode = GetNode( rkName );
	if( pNode == NULL ) return BFT_NONE;

	vBestFriend_iter iter;
	for(iter = m_vBestFriend.begin();iter != m_vBestFriend.end();iter++)
	{
		BestFriend &rkBestFriend = *iter;
		if( rkBestFriend.m_dwUserIndex == pNode->GetUserIndex() )
			return rkBestFriend.m_dwState;
	}

	return BFT_NONE;
}

int ioFriendManager::GetBestFriendStateCount( DWORD dwState )
{
	int iResult = 0;
	vBestFriend_iter iter;
	for(iter = m_vBestFriend.begin();iter != m_vBestFriend.end();iter++)
	{
		BestFriend &rkBestFriend = *iter;
		if( rkBestFriend.m_dwState == dwState )
			iResult++;
	}
	return iResult;
}

DWORD ioFriendManager::GetBestFriendState( const DWORD dwUserIndex )
{
	vBestFriend_iter iter;
	for(iter = m_vBestFriend.begin();iter != m_vBestFriend.end();iter++)
	{
		BestFriend &rkBestFriend = *iter;
		if( rkBestFriend.m_dwUserIndex == dwUserIndex )
			return rkBestFriend.m_dwState;
	}

	return BFT_NONE;
}

void ioFriendManager::CheckBestFriend()
{
	if( !m_bBestFriendCheck ) 
		return;

	m_bBestFriendCheck = false;

	DWORDVec kUserIndexVec;
	vBestFriend_iter iter;
	for(iter = m_vBestFriend.begin();iter != m_vBestFriend.end();iter++)
	{
		BestFriend &rkBestFriend = *iter;
		if( rkBestFriend.m_dwState == BFT_SET )
		{
			FriendNode *pFriend = GetNodeToUserIndex( rkBestFriend.m_dwUserIndex );
			if( pFriend == NULL )
			{
				// 오프라인 상태일 때 친구 해제되었음
				kUserIndexVec.push_back( rkBestFriend.m_dwUserIndex );
			}
		}
	}

	if( kUserIndexVec.empty() )
		return;

	SP2Packet kPacket( CTPK_BESTFRIEND_EXCEPTION_LIST );
	kPacket << (int)kUserIndexVec.size();
	for(int i = 0;i < (int)kUserIndexVec.size();i++)
	{
		kPacket << kUserIndexVec[i];
	}
	TCPNetwork::SendToServer( kPacket );
	kUserIndexVec.clear();
}

bool ioFriendManager::IsLoadingComplete()
{
	if( m_dwLastLoadingTimer == 0 )
		return false;
	if( m_bLoadingCompelte )
		return true;

	if( REALGETTIME() - m_dwLastLoadingTimer > 5000 )
	{
		m_bLoadingCompelte = true;
	}

	return m_bLoadingCompelte;
}

void ioFriendManager::SetUpdate()
{
	if( !g_GUIMgr.IsShow( MESSENGER_WND ) ) return;

	UserListWnd::sg_bUserListUpdate =  true;
}

void ioFriendManager::OnLogInUser( ioHashString szName, int iGradeLevel )
{
	vFriendNode_iter iter;
	for( iter=m_vFriendNode.begin() ; iter!=m_vFriendNode.end() ; ++iter )
	{
		FriendNode *pItem = *iter;
		if( pItem->GetName() == szName )
		{
			if( !pItem->IsOnline() )
			{
				g_GUIMgr.ShowLoginPopup( szName );
			}
			g_UserInfoMgr.SetSyncTime( szName, 0 );
			pItem->SetOnline( true );
			pItem->SetGradeLevel( iGradeLevel );
			UserListWnd::sg_bUserListUpdate =  true;
			break;
		}
	}
}

void ioFriendManager::OnLogOutUser( ioHashString szName )
{
	vFriendNode_iter iter;
	for( iter=m_vFriendNode.begin() ; iter!=m_vFriendNode.end() ; ++iter )
	{
		FriendNode *pItem = *iter;
		if( pItem->GetName() == szName )
		{
			pItem->SetOnline( false );
			UserListWnd::sg_bUserListUpdate =  true;
			break;
		}
	}
}

void ioFriendManager::OnOtherBestFriend( ioHashString szName, bool bOtherBestFriend )
{
	vFriendNode_iter iter;
	for( iter=m_vFriendNode.begin() ; iter!=m_vFriendNode.end() ; ++iter )
	{
		FriendNode *pItem = *iter;
		if( pItem->GetName() == szName )
		{
			pItem->SetOtherBF( bOtherBestFriend );
			UserListWnd::sg_bUserListUpdate =  true;
			break;
		}
	}
}

void ioFriendManager::OnUserPos( ioHashString szName, int iUserPos, int iUserLevel, DWORD dwGuildIndex, DWORD dwGuildMark )
{
	vFriendNode_iter iter;
	for( iter=m_vFriendNode.begin() ; iter!=m_vFriendNode.end() ; ++iter )
	{
		FriendNode *pItem = *iter;
		if( pItem->GetName() == szName )
		{
			pItem->SetUserPos( iUserPos );
			if( iUserLevel != -1 )
				pItem->SetGradeLevel( iUserLevel );
			UserListWnd::sg_bUserListUpdate =  true;
			break;
		}
	}
}

void ioFriendManager::OnUserPos( ioHashString szName, int iUserPos, int iUserLevel )
{
	vFriendNode_iter iter;
	for( iter=m_vFriendNode.begin() ; iter!=m_vFriendNode.end() ; ++iter )
	{
		FriendNode *pItem = *iter;
		if( pItem->GetName() == szName )
		{
			pItem->SetUserPos( iUserPos );
			if( iUserLevel != -1 )
				pItem->SetGradeLevel( iUserLevel );
			UserListWnd::sg_bUserListUpdate =  true;
			break;
		}
	}
}

void ioFriendManager::SendFriendList()
{
	SP2Packet kPacket( CTPK_FRIEND_LIST_MSG );
	kPacket << SEND_FRIEND_LIST_COUNT;
	TCPNetwork::SendToServer( kPacket );

	m_dwLastLoadingTimer = REALGETTIME();
	m_bLoadingCompelte   = false;
}

void ioFriendManager::SendFriendRequestList()
{
	SP2Packet kPacket( CTPK_FRIEND_REQUEST_LIST );
	kPacket << GetFriendRequestLastIndex() << SEND_FRIEND_REQUEST_LIST;
	TCPNetwork::SendToServer( kPacket );
}

void ioFriendManager::DeleteFriend( ioHashString szName )
{
	vFriendNode_iter iter;
	for( iter=m_vFriendNode.begin() ; iter!=m_vFriendNode.end() ; ++iter )
	{
		FriendNode *pItem = *iter;
		if( pItem->GetName() == szName )
		{
			RemoveNode( pItem );
			UserListWnd::sg_bUserListUpdate =  true;
			MannerValuationExtensionWnd::sg_bUpdateInfo = true;
			return;
		}
	}
}

void ioFriendManager::DeleteRequestFriend( const ioHashString szName )
{
	int iSize = m_vFriendRequestUser.size();
	for(int i = 0;i < iSize;i++)
	{
		if( m_vFriendRequestUser[i].m_szUserName == szName )
		{
			m_vFriendRequestUser.erase( m_vFriendRequestUser.begin() + i );
			return;
		}
	}
}

void ioFriendManager::ApplyFriendList( SP2Packet &rkPacket )
{
	m_InsertedFriend.Clear();

	int iCurSize = 0;
	rkPacket >> iCurSize;

	int iIndex;
	ioHashString szFriendID;
	bool bOnline, bOtherBestFriend;
	int iUserPos, iGradeLevel, iCampPosition;
	DWORD dwUserIndex, dwGuildIndex, dwGuildMark;
	int iCloverSendCount = 0, iCloverSendDate = 0, iCloverRecvCount = 0, iCloverRecvDate = 0, iCloverRecvWaitCount = 0;
	DWORD dwRegTime = 0;
	
	for( int i=0; i<iCurSize; i++ )
	{
		rkPacket >> iIndex >> dwUserIndex >> szFriendID >> bOnline >> iUserPos >> iGradeLevel >> iCampPosition >> dwGuildIndex >> dwGuildMark >> bOtherBestFriend >> dwRegTime;
		rkPacket >> iCloverSendCount >> iCloverSendDate >> iCloverRecvCount >> iCloverRecvDate >> iCloverRecvWaitCount;
		
		if( IsFriend( szFriendID ) ) continue;
		if( g_App.IsAdminID( szFriendID.c_str() ) ) continue;     //개발자는 친구 리스트에서 제외 : 서버에서는 존재하여 동기화된다.

		if( !szFriendID.IsEmpty() )
		{
			FriendNode *pFriend = new FriendNode( iIndex, dwUserIndex, szFriendID, bOnline, iUserPos, iGradeLevel, bOtherBestFriend, dwRegTime,
												  iCloverSendCount, iCloverSendDate, iCloverRecvCount, iCloverRecvDate, iCloverRecvWaitCount );
			if( !pFriend ) continue;
			
			pFriend->InitCloverSendSec();
			pFriend->InitCloverRecvSec();
			pFriend->InitCloverRegSec();

			m_vFriendNode.push_back( pFriend );
			m_InsertedFriend = szFriendID;
			if( bOnline )
			{
				g_GuildInfo.SetGuildMark( szFriendID, dwGuildIndex, dwGuildMark );
				g_UserInfoMgr.SetCampPosition( szFriendID, iCampPosition );
				g_UserInfoMgr.SetGrade( szFriendID, iGradeLevel );

				if( m_iMessengerState == NEW_START )
					m_iMessengerState = MESSENGER_OPEN;				
				else if( m_iMessengerState == NOT_VALUE )
					g_GUIMgr.ShowLoginPopup( szFriendID );               // 친구 추가된 유저 로그인 팝업
			}			
		}
	}

	if( iCurSize >= SEND_FRIEND_LIST_COUNT )
		SendFriendList();
	else
	{
		m_bLoadingCompelte = true;
		if( m_iMessengerState != NOT_VALUE )
		{
			if( g_GuildChatMgr.GetUserSize() > 1 )
			{
				g_GuildChatMgr.AddAnnounce( STR(1) );
			}
		}
		m_iMessengerState = NOT_VALUE; 

		// 절친 체크 - 내가 오프라인 상태일 때 친구 해제된 유저가 있나?
		CheckBestFriend();
	}
	UserListWnd::sg_bUserListUpdate =  true;
	MannerValuationExtensionWnd::sg_bUpdateInfo = true;
	g_QuestMgr.QuestCompleteTerm( QC_FRIEND_MAKE );
}

void ioFriendManager::ApplyFriendRequestUser( SP2Packet &rkPacket )
{
	int iCurSize = 0;
	rkPacket >> iCurSize;
	for(int i = 0;i < iCurSize;i++)
	{
		FriendRequestUser kUser;
		rkPacket >> kUser.m_dwTableIndex >> kUser.m_dwUserIndex >> kUser.m_szUserName;

		if( IsAlreadyRequestUser( kUser.m_dwTableIndex ) )
			continue;

		if( g_BlackListManager.IsBlackList( kUser.m_szUserName ) )
		{
			// 블랙 리스트 유저 자동 거절
			SP2Packet kPacket( CTPK_FRIEND_COMMAND );
			kPacket << FRIEND_COMMAND_BLACKLIST;
			kPacket <<  kUser.m_dwTableIndex << kUser.m_dwUserIndex << kUser.m_szUserName;
			TCPNetwork::SendToServer( kPacket );

			g_ChatMgr.SetSystemMsg( STR(1), kUser.m_szUserName.c_str() );
			continue;
		}
		else if( GetNodeSize() >= GetSlotSize() )
		{
			SP2Packet kPacket( CTPK_FRIEND_COMMAND );
			kPacket << FRIEND_COMMAND_REFUSE;
			kPacket << kUser.m_dwTableIndex << kUser.m_dwUserIndex << kUser.m_szUserName;
			TCPNetwork::SendToServer( kPacket );

			g_ChatMgr.SetSystemMsg( STR(2), kUser.m_szUserName.c_str() );
			continue;
		}

		m_vFriendRequestUser.push_back( kUser );
	}

	if( iCurSize >= SEND_FRIEND_REQUEST_LIST )
		SendFriendRequestList();	

	if( iCurSize == 0 )
	{
		FriendRequestWnd::SetFirstView( false );
	}
}

void ioFriendManager::ClientFriendInfoUpdate( ioHashString szName, int iGradeLevel, int iUserPos )
{
	vFriendNode_iter iter;
	for( iter=m_vFriendNode.begin() ; iter!=m_vFriendNode.end() ; ++iter )
	{
		FriendNode *pItem = *iter;
		if( pItem->GetName() == szName )
		{
			pItem->SetGradeLevel( iGradeLevel );
			pItem->SetUserPos( iUserPos );
			UserListWnd::sg_bUserListUpdate =  true;
			if( !pItem->IsOnline() )
			{
				pItem->SetOnline( true );
			}			
			return;
		}
	}
}

BestFriend &ioFriendManager::GetBestFriendFirstDismiss()
{
	static BestFriend kReturnData;
	
	kReturnData.m_dwMagicDate = 0xFFFFFFFF;
	vBestFriend_iter iter;
	for(iter = m_vBestFriend.begin();iter != m_vBestFriend.end();iter++)
	{
		BestFriend &rkBestFriend = *iter;
		if( rkBestFriend.m_dwState != BFT_DISMISS ) continue;

		if( kReturnData.m_dwMagicDate > rkBestFriend.m_dwMagicDate )
		{
			kReturnData = rkBestFriend;
		}
	}
	return kReturnData;
}

BestFriend &ioFriendManager::GetBestFriend( const ioHashString &rkName )
{
	static BestFriend kReturnData;	
	FriendNode *pFriendNode  = GetNode( rkName );
	if( pFriendNode == NULL ) return kReturnData;

	vBestFriend_iter iter;
	for(iter = m_vBestFriend.begin();iter != m_vBestFriend.end();iter++)
	{
		BestFriend &rkBestFriend = *iter;
		if( rkBestFriend.m_dwUserIndex == pFriendNode->GetUserIndex() )
			return rkBestFriend;
	}
	return kReturnData;
}

BestFriend &ioFriendManager::GetBestFriend( const DWORD dwUserIndex )
{
	static BestFriend kReturnData;
	FriendNode *pFriendNode  = GetNodeToUserIndex( dwUserIndex );
	if( pFriendNode == NULL ) return kReturnData;

	vBestFriend_iter iter;
	for(iter = m_vBestFriend.begin();iter != m_vBestFriend.end();iter++)
	{
		BestFriend &rkBestFriend = *iter;
		if( rkBestFriend.m_dwUserIndex == dwUserIndex )
			return rkBestFriend;
	}
	return kReturnData;
}

bool ioFriendManager::AlreadyBestFriendCheck( DWORD dwTableIndex )
{
	vBestFriend_iter iter;
	for(iter = m_vBestFriend.begin();iter != m_vBestFriend.end();iter++)
	{
		BestFriend &rkBestFriend = *iter;
		if( rkBestFriend.m_dwIndex == dwTableIndex )
			return true;
	}
	return false;
}

void ioFriendManager::ApplyBestFriendList( SP2Packet &rkPacket )
{
	int iCurSize = 0;
	rkPacket >> iCurSize;
	for( int i=0; i<iCurSize; i++ )
	{
		BestFriend kBestFriend;
		rkPacket >> kBestFriend.m_dwIndex >> kBestFriend.m_dwUserIndex >> kBestFriend.m_dwState >> kBestFriend.m_dwMagicDate;

		if( AlreadyBestFriendCheck( kBestFriend.m_dwIndex ) ) continue;
		
		m_vBestFriend.push_back( kBestFriend );
	}
	UserListWnd::sg_bUserListUpdate =  true;
	MannerValuationExtensionWnd::sg_bUpdateInfo = true;
}

void ioFriendManager::InsertBestFriend( DWORD dwTableIndex, DWORD dwFriendIndex )
{
	BestFriend kBestFriend;
	kBestFriend.m_dwIndex = dwTableIndex;
	kBestFriend.m_dwUserIndex = dwFriendIndex;
	kBestFriend.m_dwState = BFT_SET;
	kBestFriend.m_dwMagicDate = 0;

	if( AlreadyBestFriendCheck( kBestFriend.m_dwIndex ) ) return;

	m_vBestFriend.push_back( kBestFriend );
	UserListWnd::sg_bUserListUpdate =  true;
	MannerValuationExtensionWnd::sg_bUpdateInfo = true;
}

void ioFriendManager::ChangeBestFriendData( DWORD dwUserIndex, DWORD dwState, DWORD dwMagicDate )
{
	vBestFriend_iter iter;
	for(iter = m_vBestFriend.begin();iter != m_vBestFriend.end();iter++)
	{
		BestFriend &rkBestFriend = *iter;
		if( rkBestFriend.m_dwUserIndex == dwUserIndex )
		{
			rkBestFriend.m_dwState = dwState;
			rkBestFriend.m_dwMagicDate = dwMagicDate;
			UserListWnd::sg_bUserListUpdate =  true;
			MannerValuationExtensionWnd::sg_bUpdateInfo = true;
			break;
		}
	}
}

void ioFriendManager::LoadINIInfo( ioINILoader &rkLoader )
{
	m_iDefaultSlotSize = rkLoader.LoadInt_e( "friend_manager", "default_slot_size", 100 );
}

int ioFriendManager::GetSlotSize()
{
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem )
		return 	m_iDefaultSlotSize;

	ioUserEtcItem::ETCITEMSLOT kSlot;
	if( !pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_FRIEND_SLOT_EXTEND, kSlot ) )
		return m_iDefaultSlotSize;

	return m_iDefaultSlotSize + kSlot.m_iValue1;
}

void ioFriendManager::ChangeName( const ioHashString &rszName, const ioHashString &rszNewName )
{
	vFriendNode_iter iter;
	for( iter=m_vFriendNode.begin() ; iter!=m_vFriendNode.end() ; ++iter )
	{
		FriendNode *pItem = *iter;
		if( pItem->GetName() != rszName )
			continue;

		pItem->SetName( rszNewName );
		return;
	}
}