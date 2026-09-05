
#include "stdafx.h"
#include "ioClover.h"

ioClover::ioClover()
{
	Initialize();
}

ioClover::~ioClover()
{
	Initialize();
}

void ioClover::Initialize()
{
	m_iGiftCloverCount = 0;
	m_iRemainChargeTime = 1800;
	m_dwCheckTime = 0;
	m_bRequestServer = true;
}

void ioClover::LoadINI()
{
	ioINILoader_e kLoader( "config/sp2_clover.ini" );
	kLoader.SetTitle_e( "clover" );

	m_iChargingTime = kLoader.LoadInt_e( "charging_time", 30 );
	m_iChargingCount = kLoader.LoadInt_e( "charging_count", 1 );
	m_iEnableSendTime = kLoader.LoadInt_e( "send_time_gap", 1440 );
	m_iEnableRecvTime = kLoader.LoadInt_e( "recieve_time_gap", 1440 );
	m_iAbusingTime    = kLoader.LoadInt_e( "abusing_time", 1440 );
}

void ioClover::UpdateClover()
{
	if( m_dwCheckTime + PROCESS_TIME > FRAMEGETTIME() )
		return;
	m_dwCheckTime = FRAMEGETTIME();

	int iSize = g_FriendManager.GetNodeSize();
	for( int i=0; i<iSize; ++i )
	{
		FriendNode *pNode = g_FriendManager.GetNode( i );
		if( pNode )
		{
			pNode->DecreaseCloverSendSec();
			pNode->DecreaseCloverRecvSec();
			pNode->DecreaseFriendRegSec();
		}
	}

	if( IsMaxCharge() )
		return;

	m_iRemainChargeTime--;
	if( m_iRemainChargeTime < 0 )
	{
		m_iRemainChargeTime = m_iChargingTime*60;
	}

	if( m_iRemainChargeTime == 0 )
	{
		SendChargeCloverReqeust();
	}
}

void ioClover::ApplyCloverInfo( SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType >> m_iGiftCloverCount >> m_iRemainChargeTime;
	m_bRequestServer = true;

	if( g_App.IsMouseBusy() )
		TCPNetwork::MouseBusy( false );
}

void ioClover::ApplyEtcItemClover( SP2Packet &rkPacket )
{
	int iType, iValue1, iValue2;
	rkPacket >> iType >> iValue1 >> iValue2;

	ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
	if( pUserEtcItem )
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		kSlot.m_iType   = iType;
		kSlot.m_iValue1 = iValue1;
		kSlot.m_iValue2 = iValue2;
		pUserEtcItem->AddEtcItem( kSlot );
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->UpdateTab( true, true );
	}
}

bool ioClover::CheckSendRemainTime( const ioHashString &szUserName )
{
	FriendNode *pFriend = g_FriendManager.GetNode( szUserName );
	if( !pFriend )
		return false;

	if( pFriend->GetCloverSendSec() <= 0 )
		return false;

	return true;
}

bool ioClover::CheckFriendRegTime( const ioHashString &szUserName )
{
	FriendNode *pFriend = g_FriendManager.GetNode( szUserName );
	if( !pFriend )
		return false;

	if( pFriend->GetFriendRegSec() > 0  )
		return true;

	return false;
}

bool ioClover::EnableSendClover( const ioHashString &szUserName )
{
	FriendNode *pFriend = g_FriendManager.GetNode( szUserName );
	if( !pFriend )
		return false;
	
	if( GetCurGiftCloverCount() <= 0 )
		return false;

	if( CheckFriendRegTime( szUserName ) )
		return false;
	
	if( CheckSendRemainTime( szUserName ) )
		return false;
	
	return true;
}

bool ioClover::EnableRecvClover( const ioHashString &szUserName )
{
	FriendNode *pFriend = g_FriendManager.GetNode( szUserName );
	if( !pFriend )
		return false;

	if( pFriend->GetCloverRecvWaitCount() <= 0 )
		return false;

	if( pFriend->GetCloverRecvSec() <= 0 )
	{
		// 임시로 클라이언트의 값을 셋팅해준다. ( 서버로부터 응답을 받으면 서버 기준 값으로 변경된다. )
		pFriend->SetCloverRecvWaitCount( 0 );
		
		SP2Packet kPacket( CTPK_CLOVER_RECEIVE_DELETE );
		kPacket << pFriend->GetUserIndex();
		TCPNetwork::SendToServer( kPacket );

		return false;
	}

	return true;
}

int ioClover::GetRemainChargeMinute()
{
	return m_iRemainChargeTime / 60;
}

int ioClover::GetRemainChargeSecond()
{
	return m_iRemainChargeTime % 60;
}

void ioClover::GetSendRemainTime( IN const ioHashString &szUserName, OUT ioHashString &szRemainTime )
{
	FriendNode *pFriend = g_FriendManager.GetNode( szUserName );
	if( pFriend )
		szRemainTime = GetRemainString( pFriend->GetCloverSendSec() );
}

void ioClover::GetRecvRemainTime( IN const ioHashString &szUserName, OUT ioHashString &szRemainTime )
{
	FriendNode *pFriend = g_FriendManager.GetNode( szUserName );
	if( pFriend )
		szRemainTime = GetRemainString( pFriend->GetCloverRecvSec() );
}

void ioClover::GetFriendRegRemainTime( IN const ioHashString &szUserName, OUT ioHashString &szRemainTime )
{
	FriendNode *pFriend = g_FriendManager.GetNode( szUserName );
	if( pFriend )
		szRemainTime = GetRemainString( pFriend->GetFriendRegSec() );
}

int ioClover::GetRecvCloverCount()
{
	int iCloverCount = 0;
	int iSize = g_FriendManager.GetNodeSize();
	for( int i=0; i<iSize; ++i )
	{
		FriendNode *pFriend = g_FriendManager.GetNode( i );
		if( pFriend )
		{
			iCloverCount += pFriend->GetCloverRecvWaitCount();
		}
	}
	return iCloverCount;
}

const ioHashString &ioClover::GetRemainString( int iRemainSec )
{
	static ioHashString szReturn = "";

	int iHour      = iRemainSec / HOUR_SEC;
	int iMinuteSec = iRemainSec-(iHour*HOUR_SEC);
	int iMinute    = iMinuteSec / MINUTE_SEC;
	int iSec       = iMinuteSec-(iMinute*MINUTE_SEC);

	char szBuf[MAX_PATH] = "";
	if( iHour > 0 )
		wsprintf_e( szBuf, "%d시간 %02d분", iHour, iMinute );
	else
		wsprintf_e( szBuf, "%d분 %02d초", iMinute, iSec );
	szReturn = szBuf;

	return szReturn;
}

void ioClover::SendChargeCloverReqeust()
{
	if( !m_bRequestServer )
		return;

	// 임시로 클라이언트의 값을 셋팅해준다. ( 서버로부터 응답을 받으면 서버 기준 값으로 변경된다. )
	m_iGiftCloverCount += m_iChargingCount;
	m_iGiftCloverCount = min( m_iGiftCloverCount, GIFT_CLOVER_MAX );

	SP2Packet kPacket( CTPK_GIFTCLOVER_CHARGE );
	TCPNetwork::SendToServer( kPacket );

	m_bRequestServer = false;
}

const bool ioClover::IsMaxCharge()
{
	if( m_iGiftCloverCount >= GIFT_CLOVER_MAX )
		return true;

	return false;
}