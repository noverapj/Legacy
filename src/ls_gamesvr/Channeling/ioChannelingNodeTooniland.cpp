#include "stdafx.h"
#include "../NodeInfo/User.h"
#include ".\ioChannelingNodeTooniland.h"
#include "..\Local\ioLocalParent.h"
#include "..\EtcHelpFunc.h"
#include "..\BillingRelayServer\BillingRelayServer.h"
#include "../NodeInfo/ioEtcItem.h"

ioChannelingNodeTooniland::ioChannelingNodeTooniland(void)
{
	
}

ioChannelingNodeTooniland::~ioChannelingNodeTooniland(void)
{
}

ChannelingType ioChannelingNodeTooniland::GetType()
{
	return CNT_TOONILAND;
}

bool ioChannelingNodeTooniland::AddRequestGetCashPacket( IN User *pUser, OUT SP2Packet &rkSendPacket, IN DWORD dwErrorPacketID, IN DWORD dwErrorPacketType )
{
	// pUser가 NULL이니 에러 메시지를 보낼 수 없다.
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}

	PACKET_GUARD_bool( rkSendPacket.Write(pUser->GetChannelingUserID()) );
	return true;
}

bool ioChannelingNodeTooniland::OnRecieveGetCash( User *pUser, SP2Packet &rkRecievePacket, DWORD dwErrorPacketID, DWORD dwErrorPacketType )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}


	DWORD        dwUserIndex     = 0;
	ioHashString szBillingGUID;
	bool         bSetUserMouse   = false;
	int          iReturnValue    = 0;
	int          iReturnCash        = 0; 
	int          iReturnPresentCash = 0; 

	rkRecievePacket >> dwUserIndex;
	rkRecievePacket >> szBillingGUID;
	rkRecievePacket >> bSetUserMouse;
	rkRecievePacket >> iReturnValue;

	if( iReturnValue != CASH_RESULT_SUCCESS )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s ReturnValue is Error : %d:%s:%s[%d]", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), pUser->GetBillingGUID().c_str(), iReturnValue );
		pUser->ClearBillingGUID();
		SP2Packet kReturn( dwErrorPacketID );
		kReturn << dwErrorPacketType;
		kReturn << bSetUserMouse;
		pUser->SendMessage( kReturn );
		pUser->SetCash( 0 ); // 초기화
		pUser->SetPurchasedCash( 0 ); // 초기화
		pUser->SetChannelingCash( 0 ); // 초기화
		return false;
	}

	rkRecievePacket >> iReturnCash;
	rkRecievePacket >> iReturnPresentCash;

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][cash]Channeling tooniland get cash success : [%d] [%s] [%d] [%d] [%d] [%d]", pUser->GetUserIndex(), pUser->GetBillingGUID().c_str(), iReturnCash, iReturnPresentCash, pUser->GetCash(), pUser->GetChannelingCash() );

	iReturnCash = iReturnPresentCash = 0;

	pUser->SetCash( iReturnCash );
	pUser->SetPurchasedCash( iReturnPresentCash );
	pUser->SetChannelingCash( 0 );

	SP2Packet kPacket( STPK_GET_CASH );
	kPacket << GET_CASH_SUCCESS;
	kPacket << bSetUserMouse;
	kPacket << iReturnCash;
	kPacket << iReturnPresentCash;
	pUser->SendMessage( kPacket );
	pUser->ClearBillingGUID();

	//Title check
	UserTitleInven* pInven = pUser->GetUserTitleInfo();
	if( pInven )
	{
		pInven->CheckTitleValue(TITLE_HAVE_GOLD, iReturnCash);
	}

	return true;
}

bool ioChannelingNodeTooniland::AddRequestOutputCashPacket( IN User *pUser, OUT SP2Packet &rkSendPacket, IN DWORD dwErrorPacketID, IN DWORD dwErrorPacketType, IN const char *szRecvPrivateID, IN const char *szRecvPublicID, IN DWORD dwRecvUserIndex )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}

	rkSendPacket << pUser->GetChannelingUserID();

	return true;
}

bool ioChannelingNodeTooniland::CheckRequestOutputCash( User *pUser, int& iPayAmt, DWORD dwErrorPacketID, DWORD dwErrorPacketType, DWORD dwItemCode, IntOfTwoVec& vConsumeInfo )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}

	int iCheckCash = pUser->GetCash();

	switch( dwErrorPacketID )
	{
	case STPK_PRESENT_BUY:
		iCheckCash = pUser->GetPurchasedCash();
		break;
	case STPK_ETCITEM_BUY:
		{
			if( ioEtcItem::EIT_ETC_TIME_CASH1 == dwItemCode || ioEtcItem::EIT_ETC_TIME_CASH2 == dwItemCode )
				iCheckCash = pUser->GetPurchasedCash();
		}
		break;
	}

	if( iCheckCash < iPayAmt )
	{
		if( IsPossibleToUseBonusCash(dwErrorPacketID) )
		{
			if( GetBonusCashInfoForOutputCash(pUser, iCheckCash, iPayAmt, vConsumeInfo) )
				return true;
		}

		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal && pLocal->IsBillingTestID( pUser->GetPublicID() ) )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s IsBillingTestID is true : %d:%s:%d [%d:%d]", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), iPayAmt, dwErrorPacketID, iCheckCash );
			return true;
		}

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Want of Cash : %d:%s:%d [%d:%d]", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), iPayAmt, dwErrorPacketID, iCheckCash );
		SP2Packet kReturn( dwErrorPacketID );
		kReturn << dwErrorPacketType;
		pUser->SendMessage( kReturn );
		return false;
	}

	return true;
}

bool ioChannelingNodeTooniland::CheckReciveOutputCash( User *pUser, int iReturnValue, int iPayAmt, bool bBillingError, const ioHashString &rsBillingError, DWORD dwErrorPacketID, DWORD dwErrorPacketType )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}

	if( iReturnValue != CASH_RESULT_SUCCESS )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s ReturnValue is Error : %d:%s:%s:%d", __FUNCTION__, iReturnValue, pUser->GetPublicID().c_str(), pUser->GetBillingGUID().c_str(), dwErrorPacketID );
		pUser->ClearBillingGUID();
		SP2Packet kReturn( dwErrorPacketID );
		kReturn << dwErrorPacketType;
		kReturn << bBillingError;
		if( bBillingError )
			kReturn << rsBillingError;
		pUser->SendMessage( kReturn );
		return false;
	}

	return true;
}

void ioChannelingNodeTooniland::SendCancelCash( IN SP2Packet &rkPacket )
{
	if( rkPacket.GetPacketID() != BSTPK_OUTPUT_CASH_RESULT )
		return;

	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex = 0;
	ioHashString szPrivateID;
	ioHashString szOrderNo;
	int          iPayAmt  = 0;
	int          iType    = 0;
	int          iReturnValue = 0;
	int          iTransactionID = 0;
	int          iCash = 0;
	int          iPurchasedCash = 0;
	int			 iReturnItemPrice = 0;

	rkPacket >> dwUserIndex;
	rkPacket >> szBillingGUID;
	rkPacket >> iReturnValue;
	rkPacket >> iReturnItemPrice;
	if( iReturnValue != CASH_RESULT_SUCCESS )
		return;

	rkPacket >> iType;
	rkPacket >> iPayAmt;
	rkPacket >> iTransactionID;
	int iItemValueList[ioChannelingNodeParent::MAX_ITEM_VALUE];
	for (int i = 0; i < ioChannelingNodeParent::MAX_ITEM_VALUE ; i++)
		iItemValueList[i] = 0;
	ioChannelingNodeParent::GetItemValueList( rkPacket, iType, iItemValueList );

	// Cancel Step 2
	rkPacket >> iChannelingType;
	rkPacket >> iCash;
	rkPacket >> iPurchasedCash; 
	rkPacket >> szPrivateID;
	rkPacket >> szOrderNo;

	//--------------------------------SEND------------------------------------//
	char szTempGUID[USER_GUID_NUM_PLUS_ONE]="";
	Help::GetGUID( szTempGUID, sizeof(szTempGUID) );
	SP2Packet kBillingPacket( BSTPK_CANCEL_CASH );
	// 공통
	kBillingPacket << iChannelingType;
	kBillingPacket << szTempGUID;
	kBillingPacket << dwUserIndex;

	// Cancel Step 3
	kBillingPacket << szPrivateID;
	kBillingPacket << szOrderNo;

	if( !g_BillingRelayServer.SendMessage( kBillingPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send Fail : %d:%s:%s:%s", __FUNCTION__, dwUserIndex, szPrivateID.c_str(), szBillingGUID.c_str(), szTempGUID );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send : %d:%s:%s:%s", __FUNCTION__, dwUserIndex, szPrivateID.c_str(), szBillingGUID.c_str(), szTempGUID );
	}
}

bool ioChannelingNodeTooniland::AddReuestSubscriptionRetractCashPacket( IN User *pUser, OUT SP2Packet & rkSendPacket )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}

	rkSendPacket << pUser->GetChannelingUserID();

	return true;
}

bool ioChannelingNodeTooniland::OnReceiveSubscriptionRetractCash( IN User *pUser, OUT SP2Packet& rReceivePacket, IN DWORD dwErrorPacketID, DWORD dwErrorPacketType )
{
	DWORD        dwUserIndex     = 0;
	ioHashString szBillingGUID;
	DWORD		 dwIndex;
	ioHashString szSubscriptionID;
	int			 iReturnCode = 0;
 
	PACKET_GUARD_bool( rReceivePacket.Read(dwUserIndex) );
	PACKET_GUARD_bool( rReceivePacket.Read(szBillingGUID) );
	PACKET_GUARD_bool( rReceivePacket.Read(dwIndex) );
	PACKET_GUARD_bool( rReceivePacket.Read(szSubscriptionID) );
	PACKET_GUARD_bool( rReceivePacket.Read(iReturnCode) );
 
	if( iReturnCode == BILLING_SUBSCRIPTION_RETRACT_RESULT_SUCCESS )
	{
		//청약 철회 패킷 (성공)
		int iCancelCash = 0;
		SP2Packet kReturnPacket( STPK_SUBSCRIPTION_RETR );
		pUser->SetSubscriptionRetract( dwIndex, szSubscriptionID, iCancelCash, kReturnPacket, true );

		//kyg 여기에 패킷 추가할것은 도형씨와 협의 필요 130704
		pUser->ClearBillingGUID();

		return pUser->SendMessage(kReturnPacket);

		//kyg 성공 로그 남겨야하나 고민 
	}
	else if ( iReturnCode == BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL)
	{
		ioHashString errCode;

		PACKET_GUARD_bool( rReceivePacket.Read(errCode) );

	 	SP2Packet kPacket( STPK_SUBSCRIPTION_RETR_CHECK );

		PACKET_GUARD_bool( kPacket.Write(SUBSCRIPTION_RETR_CHECK_EXCEPTION) );
		PACKET_GUARD_bool( kPacket.Write(9) );

		pUser->ClearBillingGUID();

		pUser->SendMessage( kPacket );

		LOG.PrintTimeAndLog(0,"%s Fail TooniLandBuyServer Rtn ErrCode(%d:%s)",__FUNCTION__,iReturnCode,errCode.c_str());
	}
	else
	{
		ioHashString errCode;

		PACKET_GUARD_bool( rReceivePacket.Read(errCode) );

		SP2Packet kPacket( STPK_SUBSCRIPTION_RETR_CHECK );

		PACKET_GUARD_bool( kPacket.Write(SUBSCRIPTION_RETR_CHECK_EXCEPTION) );
		PACKET_GUARD_bool( kPacket.Write(9) );

		pUser->ClearBillingGUID();

		pUser->SendMessage( kPacket );

		LOG.PrintTimeAndLog(0,"%s Fail BillingRelayServer Rtn WrongCode(%d:%s)",__FUNCTION__,iReturnCode,errCode.c_str());

		return false;
	}

	return true;
}
