
#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"
#include "../EtcHelpFunc.h"
#include "../DataBase/DBClient.h"

#include "UserNodeManager.h"
#include "ioFriend.h"


ioFriend::ioFriend()
{
	Initialize();
}

ioFriend::~ioFriend()
{
	
}

void ioFriend::Initialize()
{
	m_vFriendList.clear();
	m_vBestFriendList.clear();
}

int ioFriend::GetLastFriendIndex()
{
	if( m_vFriendList.empty() )
		return 0;

	CRASH_GUARD();
	int iSize = m_vFriendList.size();
	return m_vFriendList[iSize - 1].m_iIndex;
}

bool ioFriend::IsFriend( const ioHashString &szName )
{
	CRASH_GUARD();
	vFRIEND_iter iter,iEnd;
	iEnd = m_vFriendList.end();
	for(iter = m_vFriendList.begin();iter != iEnd;iter++)
	{
		FRIEND &kFriend = *iter;
		if( kFriend.m_szName == szName )
			return true;
	}
	return false;
}

bool ioFriend::IsFriend( DWORD dwTableIndex )
{
	vFRIEND_iter iter,iEnd;
	iEnd = m_vFriendList.end();
	for(iter = m_vFriendList.begin();iter != iEnd;iter++)
	{
		FRIEND &kFriend = *iter;
		if( (DWORD)kFriend.m_iIndex == dwTableIndex )
			return true;
	}
	return false;
}

bool ioFriend::IsFriendRegHourCheck( const ioHashString &szName, DWORD dw24HourBefore )
{
	CRASH_GUARD();
	vFRIEND_iter iter,iEnd;
	iEnd = m_vFriendList.end();
	for(iter = m_vFriendList.begin();iter != iEnd;iter++)
	{
		FRIEND &kFriend = *iter;
		if( kFriend.m_szName != szName ) continue;
		if( kFriend.m_dwRegTime <= dw24HourBefore )
			return true;
	}
	return false;
}

int ioFriend::GetFriendOnlineUserCount( DWORD dw24HourBefore )
{
	CRASH_GUARD();	
	int iResult = 0;
	vFRIEND_iter iter,iEnd;
	iEnd = m_vFriendList.end();
	for(iter = m_vFriendList.begin();iter != iEnd;iter++)
	{
		FRIEND &kFriend = *iter;
		if( kFriend.m_dwRegTime <= dw24HourBefore )
		{
			if( g_UserNodeManager.GetGlobalUserNode( kFriend.m_szName ) )
				iResult++;
		}
	}
	return iResult;
}

void ioFriend::InsertFriend( int iIndex, DWORD dwUserIndex, const ioHashString &szName, int iGradeLevel, int iCampPos, DWORD dwRegTime
							, int iSendCount, int iSendDate, int iReceiveCount, int iReceiveDate, int iBeforeReceiveCount, bool bSave )
{
	if( IsFriend( szName ) )
		return;

	vFRIEND_iter iter,iEnd;
	iEnd = m_vFriendList.end();
	for(iter = m_vFriendList.begin();iter != iEnd;iter++)
	{
		FRIEND &rFriend = *iter;
		if( iIndex < rFriend.m_iIndex )
		{
			FRIEND kFriend;
			kFriend.m_iIndex = iIndex;
			kFriend.m_dwUserIndex = dwUserIndex;
			kFriend.m_szName = szName;
			kFriend.m_iGradeLevel = iGradeLevel;
			kFriend.m_iCampPosition = iCampPos;
			kFriend.m_dwRegTime   = dwRegTime;
			kFriend.m_iCloverSendCount			= iSendCount;
			kFriend.m_iCloverSendDate			= iSendDate;
			kFriend.m_iCloverReceiveCount		= iReceiveCount;
			kFriend.m_iCloverBeforeReceiveDate	= iReceiveDate;
			kFriend.m_iCloverBeforeReceiveCount	= iBeforeReceiveCount;
			kFriend.m_bCloverChangeState		= bSave;
			m_vFriendList.insert( iter, kFriend );		
			return;
		}
	}

	FRIEND kFriend;
	kFriend.m_iIndex = iIndex;
	kFriend.m_dwUserIndex = dwUserIndex;
	kFriend.m_szName = szName;
	kFriend.m_iGradeLevel = iGradeLevel;
	kFriend.m_iCampPosition = iCampPos;
	kFriend.m_dwRegTime   = dwRegTime;
	kFriend.m_iCloverSendCount			= iSendCount;
	kFriend.m_iCloverSendDate			= iSendDate;
	kFriend.m_iCloverReceiveCount		= iReceiveCount;
	kFriend.m_iCloverBeforeReceiveDate	= iReceiveDate;
	kFriend.m_iCloverBeforeReceiveCount	= iBeforeReceiveCount;
	kFriend.m_bCloverChangeState		= bSave;
	m_vFriendList.push_back( kFriend );
}

//! 친구에게 보낸 나의 send count 증가.
void ioFriend::SetFriendCloverSendData( const DWORD dwUserIndex, const int iCount, const int iDate )
{
	FRIEND& rkFriend = GetFriendToUserIndex( dwUserIndex );

	rkFriend.m_iCloverSendCount				+= iCount;	// 내가 보낸
	rkFriend.m_iCloverSendDate				= iDate;
	rkFriend.m_bCloverChangeState			= true;
}

//! 클로버를 Send하면 친구정보는 B.Receive count 증가.
void ioFriend::SetFriendBeforeReceiveCloverData( const DWORD dwUserIndex, const int iCount, const int iDate )
{
	FRIEND& rkFriend = GetFriendToUserIndex( dwUserIndex );

	rkFriend.m_iCloverBeforeReceiveDate		= iDate;
	rkFriend.m_iCloverBeforeReceiveCount	+= iCount;
	rkFriend.m_bCloverChangeState			= true;

	// 예외처리..
	if( rkFriend.m_iCloverBeforeReceiveCount >= MAX_BEFORE_RECEIVE_CLOVERCOUNT )
		rkFriend.m_iCloverBeforeReceiveCount = Help::GetCloverReceiveCount();
}

//! 친구가 클로버를 Receive 할때,
void ioFriend::SetFriendReceiveCloverData( const DWORD dwUserIndex, const int iCount )
{
	FRIEND& rkFriend = GetFriendToUserIndex( dwUserIndex );

	rkFriend.m_iCloverReceiveCount			+= iCount;
	rkFriend.m_iCloverBeforeReceiveDate		= 0;
	rkFriend.m_iCloverBeforeReceiveCount	-= iCount;
	if( rkFriend.m_iCloverBeforeReceiveCount < 0 )
		rkFriend.m_iCloverBeforeReceiveCount = 0;
	rkFriend.m_bCloverChangeState			= true;
}

void ioFriend::SetFriendBeforeReceiveCloverDelete( const DWORD dwUserIndex )
{
	FRIEND& rkFriend = GetFriendToUserIndex( dwUserIndex );

	rkFriend.m_iCloverBeforeReceiveDate		= 0;
	rkFriend.m_iCloverBeforeReceiveCount	= 0;
	rkFriend.m_bCloverChangeState			= true;
}

void ioFriend::InitFriendBeforeReceiveCloverCount()
{
	CRASH_GUARD();

	vFRIEND_iter iter	= m_vFriendList.begin();
	vFRIEND_iter iEnd	= m_vFriendList.end();

	for( iter ; iter != iEnd ; iter++ )
	{
		FRIEND& rkFriend = *iter;

		if( rkFriend.m_iCloverBeforeReceiveCount == 0 )
			continue;

		// 받을 수 있는 시간, 받을 수 있는 클로버 갯수 초기화.
		rkFriend.m_iCloverBeforeReceiveDate		= 0;
		rkFriend.m_iCloverBeforeReceiveCount	= 0;
		rkFriend.m_bCloverChangeState = true;
	}
}

bool ioFriend::DeleteFriend( const ioHashString &szName )
{
	CRASH_GUARD();
	vFRIEND_iter iter,iEnd;
	iEnd = m_vFriendList.end();
	for(iter = m_vFriendList.begin();iter != iEnd;iter++)
	{
		FRIEND &kFriend = *iter;
		if( kFriend.m_szName == szName )
		{
			m_vFriendList.erase( iter );
			return true;
		}
	}
	return false;
}

int ioFriend::FriendLastIterSize( int iLastIndex )
{
	int iMaxSize = m_vFriendList.size();
	for(int i = 0;i < iMaxSize;i++)
	{
		if( m_vFriendList[i].m_iIndex > iLastIndex )
		{
			return iMaxSize - i;
		}
	}
	return 0;
}

vFRIEND_iter ioFriend::FriendIter( int iLastIndex )
{
	vFRIEND_iter iter,iEnd;
	iEnd = m_vFriendList.end();
	for(iter = m_vFriendList.begin();iter != iEnd;iter++)
	{
		FRIEND &kFriend = *iter;
		if( kFriend.m_iIndex > iLastIndex )
			return iter;
	}
	return iEnd;
}

FRIEND &ioFriend::GetFriend( DWORD dwTableIndex )
{
	vFRIEND_iter iter,iEnd;
	iEnd = m_vFriendList.end();
	for(iter = m_vFriendList.begin();iter != iEnd;iter++)
	{
		FRIEND &kFriend = *iter;
		if( (DWORD)kFriend.m_iIndex == dwTableIndex )
			return *iter;
	}

	static FRIEND kNoneFriend;
	return kNoneFriend;
}

FRIEND &ioFriend::GetFriendToUserIndex( DWORD dwUserIndex )
{
	vFRIEND_iter iter,iEnd;
	iEnd = m_vFriendList.end();
	for(iter = m_vFriendList.begin();iter != iEnd;iter++)
	{
		FRIEND &kFriend = *iter;
		if( kFriend.m_dwUserIndex == dwUserIndex )
			return *iter;
	}

	static FRIEND kNoneFriend;
	return kNoneFriend;
}

FRIEND &ioFriend::GetFriendToUserName( const ioHashString &rkName )
{
	vFRIEND_iter iter,iEnd;
	iEnd = m_vFriendList.end();
	for(iter = m_vFriendList.begin();iter != iEnd;iter++)
	{
		FRIEND &kFriend = *iter;
		if( kFriend.m_szName == rkName )
			return *iter;
	}

	static FRIEND kNoneFriend;
	return kNoneFriend;
}

DWORD ioFriend::GetFriendNameToUserIndex( const ioHashString &rkName )
{
	vFRIEND_iter iter,iEnd;
	iEnd = m_vFriendList.end();
	for(iter = m_vFriendList.begin();iter != iEnd;iter++)
	{
		FRIEND &kFriend = *iter;
		if( kFriend.m_szName == rkName )
			return kFriend.m_dwUserIndex;
	}
	return 0;
}

void ioFriend::FillMoveDataToBestFriend( SP2Packet &rkPacket )
{
	rkPacket << (int)m_vBestFriendList.size();

	vBestFriend::iterator iter, iEnd;
	iEnd = m_vBestFriendList.end();
	for(iter = m_vBestFriendList.begin();iter != iEnd;iter++)
	{
		BestFriend &kBestFriend = *iter;
		rkPacket << kBestFriend.m_dwIndex << kBestFriend.m_dwUserIndex << kBestFriend.m_dwState << kBestFriend.m_dwMagicDate << kBestFriend.m_bChangeData;
	}
}

void ioFriend::ApplyMoveDataToBestFriend( SP2Packet &rkPacket, bool bDummyNode )
{
	int iSize;
	rkPacket >> iSize;
	for(int i = 0;i < iSize;i++)
	{
		BestFriend kBestFriend;
		rkPacket >> kBestFriend.m_dwIndex >> kBestFriend.m_dwUserIndex >> kBestFriend.m_dwState >> kBestFriend.m_dwMagicDate >> kBestFriend.m_bChangeData;
		m_vBestFriendList.push_back( kBestFriend );
	}	
}

int ioFriend::BestFriendLastIterSize( int iLastIndex )
{
	int iMaxSize = m_vBestFriendList.size();
	for(int i = 0;i < iMaxSize;i++)
	{
		if( m_vBestFriendList[i].m_dwIndex > (DWORD)iLastIndex )
		{
			return iMaxSize - i;
		}
	}
	return 0;
}

vBestFriend_iter ioFriend::BestFriendIter( int iLastIndex )
{
	vBestFriend_iter iter,iEnd;
	iEnd = m_vBestFriendList.end();
	for(iter = m_vBestFriendList.begin();iter != iEnd;iter++)
	{
		BestFriend &kBestFriend = *iter;
		if( kBestFriend.m_dwIndex > (DWORD)iLastIndex )
			return iter;
	}
	return iEnd;
}

void ioFriend::InsertBestFriend( User *pOwner, DWORD dwIndex, DWORD dwUserIndex, DWORD dwState, DWORD dwMagicDate )
{
	if( pOwner == NULL ) return;
	if( dwIndex == 0 ) return;

	BestFriend kBestFriend;
	kBestFriend.m_dwIndex = dwIndex;
	kBestFriend.m_dwUserIndex = dwUserIndex;
	kBestFriend.m_dwState = dwState;
	kBestFriend.m_dwMagicDate = dwMagicDate;

	// 시간 체크
	if( kBestFriend.m_dwMagicDate > 0 )
	{
		CTime kCurTime = CTime::GetCurrentTime();
		CTime kCheckTime( Help::GetSafeValueForCTimeConstructor( kBestFriend.GetYear(), kBestFriend.GetMonth(), kBestFriend.GetDay(), kBestFriend.GetHour(), kBestFriend.GetMinute(), 0 ) );
		if( kCurTime >= kCheckTime )
		{
			if( kBestFriend.m_dwState == BFT_DISMISS )
			{
				// 삭제 쿼리
				g_DBClient.OnDeleteBestFirendTable( pOwner->GetUserDBAgentID(), pOwner->GetAgentThreadID(), kBestFriend.m_dwIndex );
				return;
			}

			if( kBestFriend.m_dwState == BFT_SET )
				kBestFriend.m_dwMagicDate = 0;   // 용병 대여 시간 초기화
		}
	}

	// 이미 받은 친구인지 체크
	vBestFriend_iter iter,iEnd;
	iEnd = m_vBestFriendList.end();
	for(iter = m_vBestFriendList.begin();iter != iEnd;iter++)
	{
		BestFriend &rkBestFriend = *iter;
		if( rkBestFriend.m_dwIndex == kBestFriend.m_dwIndex )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "InsertBestFriend : 이미 보유중인 친구 : %d - %d - %d - %d", dwIndex, dwUserIndex, dwState, dwMagicDate );
			return;
		}
	}

	m_vBestFriendList.push_back( kBestFriend );
}

void ioFriend::UpdateBestFriend( User *pOwner, DWORD dwUserIndex, DWORD dwState, DWORD dwMagicDate )
{
	if( pOwner == NULL ) return;

	vBestFriend_iter iter,iEnd;
	iEnd = m_vBestFriendList.end();
	for(iter = m_vBestFriendList.begin();iter != iEnd;iter++)
	{
		BestFriend &rkBestFriend = *iter;
		if( rkBestFriend.m_dwUserIndex == dwUserIndex )
		{
			rkBestFriend.m_dwState = dwState;
			rkBestFriend.m_dwMagicDate = dwMagicDate;
			rkBestFriend.m_bChangeData = true;			
			return;
		}
	}
}

DWORD ioFriend::GetBestFriendState( DWORD dwUserIndex )
{
	vBestFriend_iter iter,iEnd;
	iEnd = m_vBestFriendList.end();
	for(iter = m_vBestFriendList.begin();iter != iEnd;iter++)
	{
		BestFriend &kBestFriend = *iter;
		if( kBestFriend.m_dwUserIndex == dwUserIndex )
			return kBestFriend.m_dwState;
	}
	return BFT_NONE;
}

DWORD ioFriend::GetBestFriendState( const ioHashString &rkFriendID )
{
	FRIEND &rkFriend = GetFriendToUserName( rkFriendID );
	if( rkFriend.m_dwUserIndex == 0 )
		return BFT_NONE;
	return GetBestFriendState( rkFriend.m_dwUserIndex );
}

void ioFriend::CheckBestFriendToDate( DWORD dwCheckState, DWORDVec &rUserIndex )
{
	CTime kCurTime = CTime::GetCurrentTime();
	
	vBestFriend_iter iter,iEnd;
	iEnd = m_vBestFriendList.end();
	for(iter = m_vBestFriendList.begin();iter != iEnd;iter++)
	{
		BestFriend &kBestFriend = *iter;
		if( kBestFriend.m_dwMagicDate == 0 ) continue;

		if( kBestFriend.m_dwState == dwCheckState )
		{
			CTime kCheckTime( Help::GetSafeValueForCTimeConstructor( kBestFriend.GetYear(), kBestFriend.GetMonth(), kBestFriend.GetDay(), kBestFriend.GetHour(), kBestFriend.GetMinute(), 0 ) );
			if( kCurTime >= kCheckTime )
			{
				rUserIndex.push_back( kBestFriend.m_dwUserIndex );
			}
		}
	}
}

void ioFriend::DeleteBestFriendTable( User *pOwner, DWORD dwUserIndex )
{
	if( pOwner == NULL ) return;

	vBestFriend_iter iter,iEnd;
	iEnd = m_vBestFriendList.end();
	for(iter = m_vBestFriendList.begin();iter != iEnd;iter++)
	{
		BestFriend &kBestFriend = *iter;

		if( kBestFriend.m_dwUserIndex == dwUserIndex )
		{
			m_vBestFriendList.erase( iter );
			
			// 삭제 쿼리
			g_DBClient.OnDeleteBestFirendTable( pOwner->GetUserDBAgentID(), pOwner->GetAgentThreadID(), kBestFriend.m_dwIndex );
			return;
		}
	}
}

bool ioFriend::ClearBestFriend( User *pOwner, DWORD dwUserIndex, DWORD &rMagicDate )
{
	if( pOwner == NULL ) return false;

	vBestFriend_iter iter,iEnd;
	iEnd = m_vBestFriendList.end();
	for(iter = m_vBestFriendList.begin();iter != iEnd;iter++)
	{
		BestFriend &kBestFriend = *iter;
		if( kBestFriend.m_dwState != BFT_SET ) continue;

		if( kBestFriend.m_dwUserIndex == dwUserIndex )
		{
			kBestFriend.m_dwState = BFT_DISMISS;			

			CTimeSpan kTimeSpan( 0, Help::GetBestFriendDismissDelayHour(), 0, 0 ); 
			CTime kCurTime = CTime::GetCurrentTime() + kTimeSpan;			
			kBestFriend.SetDate( kCurTime );
			rMagicDate = kBestFriend.m_dwMagicDate;
			g_DBClient.OnUpdateBestFriendList( pOwner->GetUserDBAgentID(), pOwner->GetAgentThreadID(), kBestFriend.m_dwIndex, kBestFriend.m_dwState, kBestFriend.m_dwMagicDate );
			return true;
		}
	}

	return false;
}

void ioFriend::ExceptionClearBestFriend( User *pOwner, DWORD dwUserIndex, CTime &rkTime )
{
	if( pOwner == NULL ) return;

	vBestFriend_iter iter,iEnd;
	iEnd = m_vBestFriendList.end();
	for(iter = m_vBestFriendList.begin();iter != iEnd;iter++)
	{
		BestFriend &kBestFriend = *iter;
		if( kBestFriend.m_dwState != BFT_SET ) continue;

		if( kBestFriend.m_dwUserIndex == dwUserIndex )
		{
			kBestFriend.m_dwState = BFT_DISMISS;			

			CTimeSpan kTimeSpan( 0, Help::GetBestFriendDismissDelayHour(), 0, 0 ); 
			CTime kCurTime = rkTime + kTimeSpan;			
			kBestFriend.SetDate( kCurTime );
			g_DBClient.OnUpdateBestFriendList( pOwner->GetUserDBAgentID(), pOwner->GetAgentThreadID(), kBestFriend.m_dwIndex, kBestFriend.m_dwState, kBestFriend.m_dwMagicDate );
			return;
		}
	}
}

DWORD ioFriend::SetBestFriendTime( const ioHashString &rkFriendID, CTime &rkTime )
{
	FRIEND &rkFriend = GetFriendToUserName( rkFriendID );

	vBestFriend_iter iter,iEnd;
	iEnd = m_vBestFriendList.end();
	for(iter = m_vBestFriendList.begin();iter != iEnd;iter++)
	{
		BestFriend &kBestFriend = *iter;
		if( kBestFriend.m_dwState != BFT_SET ) continue;

		if( kBestFriend.m_dwUserIndex == rkFriend.m_dwUserIndex )
		{
			kBestFriend.SetDate( rkTime );
			kBestFriend.m_bChangeData = true;
			return kBestFriend.m_dwMagicDate;
		}
	}
	return 0;
}

DWORD ioFriend::GetBestFriendTime( DWORD dwUserIndex )
{
	vBestFriend_iter iter,iEnd;
	iEnd = m_vBestFriendList.end();
	for(iter = m_vBestFriendList.begin();iter != iEnd;iter++)
	{
		BestFriend &kBestFriend = *iter;
		if( kBestFriend.m_dwUserIndex == dwUserIndex )
			return kBestFriend.m_dwMagicDate;
	}
	return 0;
}

bool ioFriend::IsRentalAlreadyCheck( DWORD dwUserIndex )
{
	vBestFriend_iter iter,iEnd;
	iEnd = m_vBestFriendList.end();
	for(iter = m_vBestFriendList.begin();iter != iEnd;iter++)
	{
		BestFriend &kBestFriend = *iter;
		if( kBestFriend.m_dwState != BFT_SET ) continue;
		if( kBestFriend.m_dwUserIndex != dwUserIndex ) continue;
		if( kBestFriend.m_dwMagicDate == 0 ) continue;

		BestFriend kTempFriend;
		kTempFriend.SetDate( CTime::GetCurrentTime() );
		if( kTempFriend.m_dwMagicDate > kBestFriend.m_dwMagicDate )
		{
			kBestFriend.m_dwMagicDate = 0;
			kBestFriend.m_bChangeData = true;
			return false;
		}
		else
		{
			return true;
		}
	}
	return false;
}

int ioFriend::GetRentalCharCount()
{
	int iReturn = 0;
	vBestFriend_iter iter,iEnd;
	iEnd = m_vBestFriendList.end();
	for(iter = m_vBestFriendList.begin();iter != iEnd;iter++)
	{
		BestFriend &kBestFriend = *iter;
		if( kBestFriend.m_dwState != BFT_SET ) continue;
		if( kBestFriend.m_dwMagicDate == 0 ) continue;

		BestFriend kTempFriend;
		kTempFriend.SetDate( CTime::GetCurrentTime() );
		if( kTempFriend.m_dwMagicDate > kBestFriend.m_dwMagicDate )
		{
			kBestFriend.m_dwMagicDate = 0;
			kBestFriend.m_bChangeData = true;
		}
		else
		{
			iReturn++;
		}
	}
	return iReturn;
}

void ioFriend::SaveBestFriend( User *pOwner )
{
	if( pOwner == NULL ) return;

	vBestFriend::iterator iter, iEnd;
	iEnd = m_vBestFriendList.end();
	for(iter = m_vBestFriendList.begin();iter != iEnd;iter++)
	{
		BestFriend &kBestFriend = *iter;
		if( kBestFriend.m_bChangeData )
		{
			// 여기서 변경된 베프 내용 저장	
			g_DBClient.OnUpdateBestFriendList( pOwner->GetUserDBAgentID(), pOwner->GetAgentThreadID(), kBestFriend.m_dwIndex, kBestFriend.m_dwState, kBestFriend.m_dwMagicDate );
			kBestFriend.m_bChangeData = false;
		}
	}
}

void ioFriend::SaveFriendClover( User *pOwner )
{
	if( pOwner == NULL )
		return;

	vFRIEND::iterator	iter	= m_vFriendList.begin();
	vFRIEND::iterator	iterEnd	= m_vFriendList.end();

	for( iter ; iter != iterEnd ; ++iter )
	{
		FRIEND& kFriend = *iter;

		if( kFriend.m_bCloverChangeState )
		{
			// DB Update.
			g_DBClient.OnUpdateFriendCloverInfo( pOwner->GetUserDBAgentID(),
												pOwner->GetAgentThreadID(), 
												kFriend.m_iIndex,
												kFriend.m_iCloverSendCount,
												kFriend.m_iCloverSendDate,
												kFriend.m_iCloverReceiveCount,
												kFriend.m_iCloverBeforeReceiveDate,
												kFriend.m_iCloverBeforeReceiveCount );

			kFriend.m_bCloverChangeState = false;
		}
	}
}
